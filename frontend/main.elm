import Url
import Url.Builder
import Http
import Regex
import Debug
import Browser
import Browser.Navigation as Nav
import Json.Encode as JE
import Json.Decode as JD

import Css exposing (..)
import Html.Styled as Html exposing (..)
import Html.Styled.Events exposing (onInput, onSubmit)
import Html.Styled.Attributes exposing (..)

import Api
import Ports

-- Main

main : Program () Model Msg
main =
    Browser.application
        { init = init,
          view = view,
          update = update,
          subscriptions = subscriptions,
          onUrlChange = UrlChanged,
          onUrlRequest = LinkClicked
        }

-- Model

type LoginState = LoggedIn
                | LoginFailed String -- This holds the login error
                | LoggingIn
                | LoggedOut
                | NeedsNewPassword

type alias Model =
    { key : Nav.Key,
      url : Url.Url,
      email : String,
      password : String,
      passwordConfirm : String,
      loginState : LoginState
    }

init : () -> Url.Url -> Nav.Key -> (Model, Cmd Msg)
init flags url key =
    (Model key url "" "" "" LoggedOut, Cmd.none)

validateEmail : String -> Bool
validateEmail email =
    let
        re = Maybe.withDefault Regex.never <| Regex.fromString "^\\w+@\\w+\\.\\w+$"
    in
        Regex.contains re email

-- Update

type Credentials = Email
                 | Password
                 | PasswordConfirm

type Msg = LinkClicked Browser.UrlRequest
         | UrlChanged Url.Url
         | Login
         | LoginResult JE.Value
         | SetCredentials Credentials String
         | RequestNewUserPassword
         | SetNewUserPassword

update : Msg -> Model -> (Model, Cmd Msg)
update msg model =
    case msg of
        LinkClicked urlRequest ->
            case urlRequest of
                Browser.Internal url -> (model, Nav.pushUrl model.key (Url.toString url))
                Browser.External href -> (model, Nav.load href)
        UrlChanged url ->
            ({ model | url = url }, Cmd.none)
        Login ->
            let
                params = { email = model.email, password = model.password }
            in
                ({ model | loginState = LoggingIn }, Ports.login params)
        LoginResult json ->
            let
                successResult = JD.decodeValue (JD.field "success" JD.bool) json
                errorResult = JD.decodeValue (JD.maybe <| JD.field "error" JD.string) json
                errorMsg = case errorResult of
                               Ok (Just error) ->
                                   error
                               _ -> ""
            in
                case successResult of
                    Ok True ->
                        ({ model | loginState = LoggedIn }, Cmd.none)
                    Ok False ->
                        ({ model | loginState = LoginFailed errorMsg }, Cmd.none)
                    _ ->
                        ({ model | loginState = LoginFailed "Invalid JSON"}, Cmd.none)
        RequestNewUserPassword ->
            ({ model | password = "", loginState = NeedsNewPassword }, Cmd.none)
        SetNewUserPassword ->
            ({ model | loginState = LoggingIn }, Ports.completeUserSignup model.password)
        SetCredentials Email email ->
            ({ model | email = email }, Cmd.none)
        SetCredentials Password password ->
            ({ model | password = password }, Cmd.none)
        SetCredentials PasswordConfirm passwordConfirm ->
            ({ model | passwordConfirm = passwordConfirm}, Cmd.none)

getEndpoint : String -> String
getEndpoint endpoint = Url.Builder.crossOrigin Api.url [ endpoint ] []

-- Subscriptions

subscriptions : Model -> Sub Msg
subscriptions _ = Sub.batch [ Ports.loginResult LoginResult,
                              Ports.requestNewUserPassword (always RequestNewUserPassword) ]

-- View

theme = { fgColor = hex "55AF6A",
          bgColor = hex "DBDBDB",
          errColor = hex "B84949",
          textColor = hex "494949",
          overlayColor = hex "000000D0",
          defaultMargin = px 5
        }

getOverlayCss : Display compatible -> Attribute Msg
getOverlayCss overlayDisplay =
    css [ position fixed,
          display overlayDisplay,
          Css.width (pct 100),
          Css.height (pct 100),
          backgroundColor theme.overlayColor,
          zIndex (int 1),
          top (px 0),
          bottom (px 0),
          left (px 0),
          right (px 0) ]

getDisplayFromLoginState : Bool -> Display {}
getDisplayFromLoginState state =
    case state of
        True ->
            block
        False ->
            { value = none.value, display = none.display }

passwordDiv : List (Attribute Msg) -> Html Msg
passwordDiv attributes =
    input ([ type_ "password"] ++ attributes) []

view : Model -> Browser.Document Msg
view model =
    let
        -- Settings
        validEmail = validateEmail model.email
        loginDisabled =
            let
                baseCondition = not validEmail || String.isEmpty model.password
            in
                case model.loginState of
                    NeedsNewPassword ->
                        baseCondition || model.password /= model.passwordConfirm
                    _ ->
                        baseCondition

        -- CSS styles
        loginCss = css [ textAlign center,
                         fontFamily sansSerif,
                         color theme.fgColor,
                         fontSize (px 50) ]
        formCss = css [ textAlign center ]
        emailCss = css [ color (if validEmail || String.isEmpty model.email
                                then theme.textColor
                                else theme.errColor) ]
        confirmPasswordCss = css [ color (if model.password == model.passwordConfirm
                                          then theme.textColor
                                          else theme.errColor) ]
        marginCss = css [ marginTop theme.defaultMargin ]

        loginMsgDisplay = getDisplayFromLoginState (model.loginState == LoggingIn)
        loginMsgCss = css [ position absolute,
                            fontFamily sansSerif,
                            fontSize (px 25),
                            color theme.fgColor,
                            top (pct 50),
                            left (pct 50),
                            transform <| translate2 (pct -50) (pct -50) ]

        passwordField =
            let
                passwordHelper str = passwordDiv [ value model.password,
                                                   onInput (SetCredentials Password),
                                                   placeholder str,
                                                   marginCss ]
            in
                case model.loginState of
                    NeedsNewPassword ->
                        passwordHelper "New password"
                    _ ->
                        passwordHelper "Password"
        confirmPasswordField = case model.loginState of
                                   NeedsNewPassword ->
                                       div [ marginCss ]
                                           [ passwordDiv [ value model.passwordConfirm,
                                                           placeholder "Confirm password",
                                                           onInput (SetCredentials PasswordConfirm),
                                                           confirmPasswordCss ],
                                             br [] []
                                           ]
                                   _ ->
                                       div [] []
        submitCommand = if model.loginState == NeedsNewPassword
                        then SetNewUserPassword
                        else Login

        -- Page views
        login = div []
                [ div [ loginCss ]
                      [ text "Eolach V2" ],
                  br [] [],
                  Html.form [ onSubmit submitCommand, formCss ]
                      [ input [ type_ "text",
                                placeholder "Email",
                                onInput (SetCredentials Email),
                                emailCss ] [],
                        br [] [],
                        passwordField,
                        br [] [],
                        confirmPasswordField,
                        button [ Html.Styled.Attributes.disabled loginDisabled,
                                 marginCss]
                            [ text "Login" ]
                      ],
                  div [ getOverlayCss loginMsgDisplay ]
                      [ div [ loginMsgCss ]
                            [ text "Logging in..." ]
                      ]
                ]
        home = div [] [ text "Welcome!" ]

        body = case model.loginState of
                   LoggedOut ->
                       login
                   LoggingIn ->
                       login
                   NeedsNewPassword ->
                       login
                   LoggedIn ->
                       home
                   LoginFailed error ->
                       text error
    in
        { title = "Eolach V2",
          body = [ Html.toUnstyled body ]
        }

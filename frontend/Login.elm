module Login exposing (Model, Msg, update, view, init, subscriptions)

import Http
import Regex

import Css exposing (..)
import Html.Styled as Html exposing (..)
import Html.Styled.Events exposing (onClick, onInput, onSubmit)
import Html.Styled.Attributes exposing (..)

import Ports

{- Model -}

type LoginState = LoggedIn String    -- ID token
                | LoginFailed String -- This holds the login error
                | LoggingIn
                | LoggedOut
                | NeedsNewPassword

type alias Model =
    { email : String,
      password : String,
      passwordConfirm : String,
      loginState : LoginState
    }

init : Model
init =
    Model "" "" "" LoggedOut

{- Update -}

type Credentials = Email
                 | Password
                 | PasswordConfirm

type Msg = Login
         | LoginFailedMsg String    -- This holds the error message
         | CloseErrorOverlay
         | LoginSucceeded String    -- This holds the ID token
         | SetCredentials Credentials String
         | RequestNewUserPassword
         | SetNewUserPassword

update : Msg -> Model -> (Model, Cmd Msg)
update msg model =
    case msg of
        CloseErrorOverlay ->
            ({ model | loginState = LoggedOut }, Cmd.none)
        Login ->
            let
                creds = Ports.Credentials model.email model.password
            in
                ({ model | loginState = LoggingIn }, Ports.login creds)
        LoginFailedMsg errorMsg ->
            ({ model | loginState = LoginFailed errorMsg }, Cmd.none)
        LoginSucceeded idToken ->
            ({ model | loginState = LoggedIn idToken }, Cmd.none)
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

{- Subscriptions -}
subscriptions : Model -> Sub Msg
subscriptions _ =
    Sub.batch [ Ports.onLoginFailed LoginFailedMsg,
                Ports.onLoginSucceeded LoginSucceeded,
                Ports.onRequestNewUserPassword (always RequestNewUserPassword)
              ]

{- View -}

validateEmail : String -> Bool
validateEmail email =
    let
        re = Maybe.withDefault Regex.never <| Regex.fromString "^\\w+@\\w+\\.\\w+$"
    in
        Regex.contains re email

getOverlayCss : Display compatible -> Cursor {} -> Attribute Msg
getOverlayCss overlayDisplay overlayCursor =
    css [ position fixed,
          display overlayDisplay,
          Css.width (pct 100),
          Css.height (pct 100),
          backgroundColor theme.overlayColor,
          zIndex (int 1),
          top (px 0),
          bottom (px 0),
          left (px 0),
          right (px 0),
          cursor overlayCursor ]

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

theme = { fgColor = hex "55AF6A",
          bgColor = hex "DBDBDB",
          errColor = hex "B84949",
          textColor = hex "494949",
          overlayColor = hex "000000D0",
          defaultMargin = px 5
        }

view : Model -> Html Msg
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
        overlayMsgCss = css [ position absolute,
                              fontFamily sansSerif,
                              fontSize (px 25),
                              color theme.fgColor,
                              top (pct 50),
                              left (pct 50),
                              transform <| translate2 (pct -50) (pct -50) ]

        loginMsgDisplay = getDisplayFromLoginState (model.loginState == LoggingIn)

        {- If this is a normal login, then we just need the one password field,
        but if this is the first time, we need another to confirm the new
        password.
         -}
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

        -- In case login failed
        errorMsg = case model.loginState of
                       LoginFailed msg ->
                           msg
                       _ ->
                           ""
        errorMsgDisplay = getDisplayFromLoginState (case model.loginState of
                                                        LoginFailed _ -> True
                                                        _ -> False)

        -- If this is the first time logging in, then we'll need to get a new
        -- password from the user.
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
                  div [ getOverlayCss loginMsgDisplay wait ]
                      [ div [ overlayMsgCss ]
                            [ text "Logging in..." ]
                      ],
                  div [ getOverlayCss errorMsgDisplay pointer,
                        onClick CloseErrorOverlay ]
                      [ div [ overlayMsgCss ]
                            [ text errorMsg ]
                      ]
                ]
        home = text "Welcome!"
    in
        case model.loginState of
            LoggedOut ->
                login
            LoggingIn ->
                login
            NeedsNewPassword ->
                login
            LoginFailed error ->
                login
            LoggedIn _ ->
                home

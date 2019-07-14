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

import FontAwesome.Icon as FAIcon
import FontAwesome.Solid as FASolid
import FontAwesome.Styles as FAStyles

import Api

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

type LoginState = LoggedIn String    -- This holds the token
                | LogInFailed String -- This holds the login error
                | LoggingIn
                | LoggedOut

type alias Model =
    { key : Nav.Key,
      url : Url.Url,
      email : String,
      password : String,
      loginState : LoginState
    }

init : () -> Url.Url -> Nav.Key -> (Model, Cmd Msg)
init flags url key =
    (Model key url "" "" LoggedOut, Cmd.none)

validateEmail : String -> Bool
validateEmail email =
    let
        re = Maybe.withDefault Regex.never <| Regex.fromString "^\\w+@\\w+\\.\\w+$"
    in
        Regex.contains re email

-- Update

type Credentials = Email
                 | Password

type Msg = LinkClicked Browser.UrlRequest
         | UrlChanged Url.Url
         | Login
         | LoginResponse (Result Http.Error String)
         | SetCredentials Credentials String

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
                authCmd = Http.post
                          { url = getEndpoint "auth",
                            body = Http.jsonBody (JE.object
                                   [ ("email", JE.string model.email),
                                     ("password", JE.string model.password)
                                   ]),
                            expect = Http.expectJson LoginResponse (decodeStrField "token")
                          }
            in
                ({ model | loginState = LoggingIn }, authCmd)
        LoginResponse result ->
            case result of
                Ok token ->
                    ({ model | loginState = LoggedIn token }, Cmd.none)
                Err error ->
                    let failMessage = case error of
                                          Http.BadUrl url ->
                                              "Bad URL: " ++ url
                                          Http.Timeout ->
                                              "Request timed out"
                                          Http.NetworkError ->
                                              "Network error"
                                          Http.BadStatus status ->
                                              "Request failed: " ++ (String.fromInt status)
                                          Http.BadBody bodyError ->
                                              "Unexpected body: " ++ bodyError
                    in
                        ({ model | loginState = LogInFailed failMessage }, Cmd.none)
        SetCredentials Email email ->
            ({ model | email = email }, Cmd.none)
        SetCredentials Password password ->
            ({ model | password = password }, Cmd.none)

getEndpoint : String -> String
getEndpoint endpoint = Url.Builder.crossOrigin Api.url [ endpoint ] []

decodeStrField : String -> JD.Decoder String
decodeStrField fieldName = JD.field fieldName JD.string

-- Subscriptions

subscriptions : Model -> Sub Msg
subscriptions _ = Sub.none
                  
-- View

theme = { fgColor = hex "55AF6A",
          bgColor = hex "DBDBDB",
          errColor = hex "B84949",
          textColor = hex "494949",
          defaultMargin = px 5
        }

view : Model -> Browser.Document Msg
view model =
    let
        -- Settings
        validEmail = validateEmail model.email
        loginDisabled = not validEmail || String.isEmpty model.password

        -- CSS styles
        loginCss = css [ textAlign center,
                         fontFamily sansSerif,
                         color theme.fgColor,
                         fontSize (px 50) ]
        formCss = css [ textAlign center ]
        emailCss = css [ color (if validEmail || String.isEmpty model.email then theme.textColor else theme.errColor) ]
        marginCss = css [ marginTop theme.defaultMargin ]

        -- Page views
        login = div []
                [ div [ loginCss ]
                      [ text "Eolach V2" ],
                  br [] [],
                  Html.form [ onSubmit Login, formCss ]
                      [ input [ type_ "text", placeholder "Email", onInput (SetCredentials Email), emailCss ] [],
                        br [] [],
                        input [ type_ "password", placeholder "Password", onInput (SetCredentials Password), marginCss ] [],
                        br [] [],
                        button [ Html.Styled.Attributes.disabled loginDisabled, marginCss ]
                            [ text "Login" ]
                      ]
                ]
        loading = div []
                  [ Html.fromUnstyled FAStyles.css,
                    Html.fromUnstyled (FASolid.spinner |> FAIcon.present |> FAIcon.view) ]
        home = div [] [ text "Welcome!" ]

        body = case model.loginState of
                   LoggedOut ->
                       login
                   LoggingIn ->
                       loading
                   LoggedIn _ ->
                       home
                   LogInFailed error ->
                       text error
    in
        { title = "Eolach V2",
          body = [ Html.toUnstyled body ]
        }

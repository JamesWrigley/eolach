import Url
import Regex
import Browser
import Browser.Navigation as Nav

import Css exposing (..)
import Html.Styled exposing (..)
import Html.Styled.Events exposing (onInput, onSubmit)
import Html.Styled.Attributes exposing (..)

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

type alias Model =
    { key : Nav.Key,
      url : Url.Url,
      email : String,
      password : String
    }

init : () -> Url.Url -> Nav.Key -> (Model, Cmd Msg)
init flags url key =
    (Model key url "" "", Cmd.none)

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
            -- Call API to login
            (model, Cmd.none)
        SetCredentials Email email ->
            ({ model | email = email }, Cmd.none)
        SetCredentials Password password ->
            ({ model | password = password }, Cmd.none)

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
        validEmail = validateEmail model.email
        emailColor = if validEmail || String.isEmpty model.email then theme.textColor else theme.errColor
        loginDisabled = not validEmail || String.isEmpty model.password

        login = div []
                [ div [ css [ textAlign center,
                              fontFamily sansSerif,
                              color theme.fgColor,
                              fontSize (px 50) ] ]
                      [ text "Eolach V2" ],
                  br [] [],
                  Html.Styled.form [ onSubmit Login, css [ textAlign center ] ]
                      [ input [ type_ "text", placeholder "Email", onInput (SetCredentials Email),
                                css [ color emailColor ] ] [],
                        br [] [],
                        input [ type_ "password", placeholder "Password", onInput (SetCredentials Password),
                                css [ marginTop theme.defaultMargin ] ] [],
                        br [] [],
                        button [ Html.Styled.Attributes.disabled loginDisabled,
                                 css [ marginTop theme.defaultMargin ] ]
                            [ text "Login" ]
                      ]
                ]
        home = div [] [ text "Welcome!" ]
    in
        { title = "Eolach V2",
          body = [ Html.Styled.toUnstyled login ]
        }

import Url
import Http
import Browser
import Html exposing (Html)
import Browser.Navigation as Nav

import Element as E
import Element.Font as Font
import Element.Input as Input

import Api
import Login
import Admin
import Kiosk
import Routing exposing (Route(..), Session, pathToRoute)

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

type Page = LoginPage Login.Model
          | KioskPage Kiosk.Model
          | AdminPage Admin.Model
          | FourOhFour

type alias Model =
    { key : Nav.Key,
      url : Url.Url,
      page : Page
    }

init : () -> Url.Url -> Nav.Key -> (Model, Cmd Msg)
init flags url key =
    (Model key url (LoginPage <| Login.init key), Cmd.none)

-- Update

type Msg = LinkClicked Browser.UrlRequest
         | UrlChanged Url.Url
         | LoginMsg Login.Msg
         | AdminMsg Admin.Msg

getSession : Model -> Session
getSession model =
    case model.page of
        LoginPage loginModel ->
            loginModel.session
        AdminPage adminModel ->
            adminModel.session
        KioskPage kioskModel ->
            kioskModel.session
        FourOhFour ->
            Session model.key Nothing

update : Msg -> Model -> (Model, Cmd Msg)
update msg model =
    case (msg, model.page) of
        (LinkClicked urlRequest, _) ->
            case urlRequest of
                Browser.Internal url -> (model, Nav.pushUrl model.key (Url.toString url))
                Browser.External href -> (model, Nav.load href)
        (UrlChanged url, _) ->
            case pathToRoute url of
                Routing.Login ->
                    ({ model | page = LoginPage (Login.init model.key) }, Cmd.none)
                Routing.Admin ->
                    ({ model | page = AdminPage (Admin.init <| getSession model) }, Cmd.none)
                Routing.Kiosk ->
                    ({ model | page = KioskPage (Kiosk.init <| getSession model) }, Cmd.none)
                Routing.None ->
                    ({ model | page = FourOhFour }, Cmd.none)
        (LoginMsg loginMsg, LoginPage loginModel) ->
            let
                (newModel, newCmd) = Login.update loginMsg loginModel
            in
                ({ model | page = LoginPage newModel }, Cmd.map LoginMsg newCmd)
        (AdminMsg adminMsg, AdminPage adminModel) ->
            let
                (newModel, newCmd) = Admin.update adminMsg adminModel
            in
                ({ model | page = AdminPage newModel }, Cmd.map AdminMsg newCmd)
        (_, _) ->
            (model, Cmd.none)

-- Subscriptions

subscriptions : Model -> Sub Msg
subscriptions model =
    case model.page of
        LoginPage loginModel ->
            Sub.map LoginMsg (Login.subscriptions loginModel)
        _ ->
            Sub.none

-- View

centerMessage : E.Element msg -> Int -> E.Element msg
centerMessage content fontSize =
    E.el [ E.centerX,
           E.centerY,
           Font.size fontSize,
           Font.family [ Font.monospace ]
         ]
        content

view : Model -> Browser.Document Msg
view model =
    let
        (body, msg) = case model.page of
                          LoginPage loginModel ->
                              (Login.view loginModel, Html.map LoginMsg)
                          AdminPage adminModel ->
                              (Admin.view adminModel, Html.map AdminMsg)
                          KioskPage kioskModel ->
                              (centerMessage (E.text "Kiosk: not implemented yet") 25, Html.map LoginMsg)
                          FourOhFour ->
                              (centerMessage (E.text "404") 100, Html.map LoginMsg)
    in
        { title = "Eolach V2",
          body = List.map msg [ E.layout [] body ]
        }

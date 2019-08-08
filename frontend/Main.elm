import Url
import Http
import Browser
import Html exposing (Html)
import Browser.Navigation as Nav

import Element as E
import Element.Font as EF

import Api
import Login
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
          | ItemsPage
          | KioskPage
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

getSession : Model -> Session
getSession model =
    case model.page of
        LoginPage loginModel ->
            loginModel.session
        _ ->
            Session model.key ""

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
                Routing.Items ->
                    ({ model | page = ItemsPage (Items.init <| getSession model) }, Cmd.none)
                Routing.None ->
                    ({ model | page = FourOhFour }, Cmd.none)
        (LoginMsg loginMsg, LoginPage loginModel) ->
            let
                (newModel, newCmd) = Login.update loginMsg loginModel
            in
                ({ model | page = LoginPage newModel}, Cmd.map LoginMsg newCmd)
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

view : Model -> Browser.Document Msg
view model =
    let
        body = case model.page of
                   LoginPage loginModel ->
                       Login.view loginModel
                   ItemsPage _ ->
                       E.text "Items: not implemented yet"
                   KioskPage ->
                       E.text "Kiosk: not implemented yet"
                   FourOhFour ->
                       E.el [ E.centerX,
                              E.centerY,
                              EF.size 100
                            ]
                           (E.text "404")
    in
        { title = "Eolach V2",
          body = List.map (Html.map LoginMsg) [ E.layout [] body ]
        }

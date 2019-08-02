import Url
import Http
import Browser
import Html exposing (Html)
import Browser.Navigation as Nav

import Html.Styled

import Api
import Login

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
          | BooksPage -- String (Book -> Html msg) (List Book)
          | KioskPage

type alias Model =
    { key : Nav.Key,
      url : Url.Url,
      page : Page
    }

init : () -> Url.Url -> Nav.Key -> (Model, Cmd Msg)
init flags url key =
    (Model key url (LoginPage Login.init), Cmd.none)

-- Update

type Msg = LinkClicked Browser.UrlRequest
         | UrlChanged Url.Url
         | LoginMsg Login.Msg
         | GetBooks
         | ReceivedBooks (Result Http.Error (List Api.Book))

update : Msg -> Model -> (Model, Cmd Msg)
update msg model =
    case (msg, model.page) of
        (LinkClicked urlRequest, _) ->
            case urlRequest of
                Browser.Internal url -> (model, Nav.pushUrl model.key (Url.toString url))
                Browser.External href -> (model, Nav.load href)
        (UrlChanged url, _) ->
            ({ model | url = url }, Cmd.none)
        (LoginMsg loginMsg, LoginPage loginModel) ->
            let
                (newModel, newCmd) = Login.update loginMsg loginModel
            in
                ({ model | page = LoginPage newModel}, Cmd.map LoginMsg newCmd)
        (GetBooks, _) ->
            (model, Api.getBooks ReceivedBooks)
        (ReceivedBooks _, _) ->
            (model, Cmd.none)
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
                       Html.Styled.toUnstyled <| Login.view loginModel
                   _ ->
                       Html.Styled.toUnstyled <| Login.view Login.init
    in
        { title = "Eolach V2",
          body = List.map (Html.map LoginMsg) [ body ]
        }

module Routing exposing (Route(..), Session, routeToPath, pathToRoute, replaceUrl)

import Url
import Url.Parser as UP
import Url.Builder as UB
import Dict exposing (Dict)
import Browser.Navigation as Nav

type Route = Login
           | Items
           | None

type alias Session =
    { key : Nav.Key,
      idToken : String
    }

routeToPath : Route -> String
routeToPath route =
    case route of
        Login ->
            "login"
        Items ->
            "items"
        None ->
            ""

pathToRoute : Url.Url -> Route
pathToRoute path =
    case UP.parse UP.string path of
        Just "login" ->
            Login
        Just "items" ->
            Items
        _ ->
            None

replaceUrl : Nav.Key -> String -> Cmd msg
replaceUrl key path =
    Nav.replaceUrl key path

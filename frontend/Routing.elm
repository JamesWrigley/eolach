module Routing exposing (Route(..), Session, routeToPath, pathToRoute, changeRoute)

import Url
import Url.Parser as UP
import Url.Builder as UB
import Dict exposing (Dict)
import Browser.Navigation as Nav

type Route = Login
           | Admin
           | Kiosk
           | None

type alias Session =
    { key : Nav.Key,
      idToken : Maybe String
    }

routeToPath : Route -> String
routeToPath route =
    case route of
        Login -> "login"
        Admin -> "admin"
        Kiosk -> "kiosk"
        None  -> ""

pathToRoute : Url.Url -> Route
pathToRoute path =
    case UP.parse UP.string path of
        Just "login" -> Login
        Just "admin" -> Admin
        Just "kiosk" -> Kiosk
        _            -> None

changeRoute : Nav.Key -> Route -> Cmd msg
changeRoute key route =
    Nav.replaceUrl key (routeToPath route)

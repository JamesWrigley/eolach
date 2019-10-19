module Kiosk exposing (Model, init)

import Set exposing (Set)
import Dict exposing (Dict)
import Browser.Navigation as Nav

import Routing exposing (Session)

{- Model -}

type Filter = Book
            | Dvd
            | Cd

type alias FilterList = List (Filter, Bool)

type alias Model =
    { session : Session,
      page : Dict Int String,
      filters : FilterList,
      lastEvaluatedKeys : Maybe String
    }

init : Session -> Model
init session =
    Model session Dict.empty [ (Book, True), (Dvd, True), (Cd, True) ] Nothing

{- Update -}

{- Subscriptions -}

{- View -}

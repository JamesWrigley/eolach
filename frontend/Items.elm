module Items exposing (..)

import Set exposing (Set)
import Dict exposing (Dict)
import Browser.Navigation as Nav

import Routing exposing (Session)

{- Model -}

type Filter = Book
            | Dvd
            | Cd

type alias Model =
    { session : Session,
      page : Dict Int String,
      filters : List Filter
    }

init : Session -> Model
init session =
    Model session Dict.empty [ Book, Dvd, Cd ]

{- Update -}

{- Subscriptions -}

{- View -}

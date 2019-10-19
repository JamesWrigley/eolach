module Admin exposing (Model, Msg, init, update, view)

import Element exposing (..)
import Element.Font as Font
import Element.Input as Input

import Routing exposing (Session, changeRoute)

{- Model -}

type alias Model =
    { session : Session
    }

init : Session -> Model
init session =
    Model session

{- Update -}

type Msg = SwitchToKiosk

update : Msg -> Model -> (Model, Cmd Msg)
update msg model =
    case msg of
        SwitchToKiosk ->
            (model, changeRoute model.session.key Routing.Kiosk)

{- View -}

view : Model -> Element Msg
view model =
    el [ centerX,
         centerY,
         Font.size 25,
         Font.family [ Font.monospace ]
       ]
    (Input.button []
         { onPress = Just SwitchToKiosk,
           label = text "Kiosk"
         }
    )

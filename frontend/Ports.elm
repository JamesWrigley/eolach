port module Ports exposing (..)

import Json.Encode as JE

port login : { email : String, password : String } -> Cmd msg
port loginResult : (JE.Value -> msg) -> Sub msg

port completeUserSignup : String -> Cmd msg
port requestNewUserPassword : (() -> msg) -> Sub msg

port module Ports exposing (..)

type alias Credentials =
    { email : String,
      password : String
    }

port login : Credentials -> Cmd msg
port onLoginFailed : (String -> msg) -> Sub msg
port onLoginSucceeded : (String -> msg) -> Sub msg

port completeUserSignup : String -> Cmd msg
port onRequestNewUserPassword : (() -> msg) -> Sub msg

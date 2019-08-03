module Login exposing (Model, Msg, update, view, init, subscriptions)

import Regex
import Html.Events
import Json.Decode as JD

import Element exposing (..)
import Element.Font as Font
import Element.Input as Input
import Element.Border as Border
import Element.Background as Background

import Ports

{- Model -}

type LoginState = LoggedIn String    -- ID token
                | LoginFailed String -- This holds the login error
                | LoggingIn
                | LoggedOut
                | NeedsNewPassword

type alias Model =
    { email : String,
      password : String,
      passwordConfirm : String,
      loginState : LoginState
    }

init : Model
init =
    Model "" "" "" LoggedOut

{- Update -}

type Credentials = Email
                 | Password
                 | PasswordConfirm

type Msg = Login
         | LoginFailedMsg String    -- This holds the error message
         | LoginSucceeded String    -- This holds the ID token
         | SetCredentials Credentials String
         | RequestNewUserPassword
         | SetNewUserPassword

type CredentialValidity = ValidCredentials
                        | InvalidCredentials String

validateCredentials : String -> String -> CredentialValidity
validateCredentials email password =
    case (email, password) of
        ("", _) ->
            InvalidCredentials "Please enter your email address."
        (_, "") ->
            InvalidCredentials "Please enter your password."
        (_, _) ->
            if validateEmail email
            then ValidCredentials
            else InvalidCredentials "Invalid email address."

update : Msg -> Model -> (Model, Cmd Msg)
update msg model =
    case msg of
        Login ->
            case validateCredentials model.email model.password of
                InvalidCredentials error ->
                    ({ model | loginState = LoginFailed error }, Cmd.none)
                ValidCredentials ->
                    let
                        creds = Ports.Credentials model.email model.password
                    in
                        ({ model | loginState = LoggingIn }, Ports.login creds)
        LoginFailedMsg error ->
            ({ model | loginState = LoginFailed error }, Cmd.none)
        LoginSucceeded idToken ->
            ({ model | loginState = LoggedIn idToken }, Cmd.none)
        RequestNewUserPassword ->
            ({ model | password = "", passwordConfirm = "", loginState = NeedsNewPassword }, Cmd.none)
        SetNewUserPassword ->
            ({ model | loginState = LoggingIn }, Ports.completeUserSignup model.password)
        SetCredentials Email email ->
            ({ model | email = email }, Cmd.none)
        SetCredentials Password password ->
            ({ model | password = password }, Cmd.none)
        SetCredentials PasswordConfirm passwordConfirm ->
            ({ model | passwordConfirm = passwordConfirm}, Cmd.none)

{- Subscriptions -}

subscriptions : Model -> Sub Msg
subscriptions _ =
    Sub.batch [ Ports.onLoginFailed LoginFailedMsg,
                Ports.onLoginSucceeded LoginSucceeded,
                Ports.onRequestNewUserPassword (always RequestNewUserPassword)
              ]

{- View -}

colorTheme = { logo = rgb255 85 175 106,
               bg = rgb255 214 213 218,
               error = rgb255 213 30 52,
               darkGrey = rgb255 100 100 100,
               black = rgb255 0 0 0
             }

styleMessage : String -> Element Msg
styleMessage msg =
    el [ Font.family [ Font.monospace ],
         Font.size 15
       ] (text msg)

styleError : String -> Element Msg
styleError error =
    el [ Font.family [ Font.monospace ],
         Font.size 15,
         Font.bold,
         Font.color colorTheme.error
       ] (text error)

validateEmail : String -> Bool
validateEmail email =
    let
        re = Maybe.withDefault Regex.never <| Regex.fromString "^\\w+@\\w+\\.\\w+$"
    in
        Regex.contains re email

onEnter : Msg -> Attribute Msg
onEnter msg =
    htmlAttribute (Html.Events.on "keyup"
                       (JD.field "key" JD.string
                       |> JD.andThen
                            (\key -> if key == "Enter"
                                     then JD.succeed msg
                                     else JD.fail "Not the enter key"
                            )
                       )
                  )

baseLayout : Model -> Element Msg -> Element Msg -> Element Msg -> Element Msg
baseLayout model aboveLogin loginElement belowLogin =
    let
        emailColor = if String.isEmpty model.email || validateEmail model.email
                     then colorTheme.black
                     else colorTheme.error
    in
        row [ width fill,
              height fill,
              Background.color colorTheme.bg
            ]
            [ el [ width (fillPortion 2) ] none,
              --Spacer
        
              column [ centerX, centerY, spacing 10, width (fillPortion 1) ]
                  [ el [ centerX,
                         Font.size 50,
                         Font.color colorTheme.logo
                       ] (text "Eolach V2"),
                    Input.text [ Input.focusedOnLoad,
                                 Font.color emailColor,
                                 onEnter Login
                               ]
                        { onChange = (SetCredentials Email),
                          text = model.email,
                          placeholder = Just <| Input.placeholder [] (text "Email"),
                          label = Input.labelHidden "Email"
                        },
                    Input.newPassword [ onEnter Login ]
                        { onChange = (SetCredentials Password),
                          text = model.password,
                          placeholder = Just <| Input.placeholder [] (text "Password"),
                          label = Input.labelHidden "Password",
                          show = False
                        },
                    aboveLogin,
                    loginElement,
                    el [ paddingXY 0 3] none,
                    paragraph [] [ belowLogin ]
                  ],
        
              -- Spacer
              el [ width (fillPortion 2) ] none
            ]

view : Model -> Element Msg
view model =
    let
        makeLoginButton label loginMsg = Input.button [ centerX,
                                                        centerY,
                                                        paddingXY 30 7,
                                                        Border.solid,
                                                        Border.width 2,
                                                        Border.color colorTheme.darkGrey
                                                      ] { onPress = loginMsg,
                                                          label = text label
                                                        }
        defaultLoginButton = makeLoginButton "Login" (Just Login)
        layoutHelper = baseLayout model
    in
        case model.loginState of
            LoggedOut ->
                layoutHelper none defaultLoginButton none
            LoginFailed error ->
                layoutHelper none defaultLoginButton (styleError error)
            NeedsNewPassword ->
                let
                    confirmationColor = if model.password == model.passwordConfirm
                                        then colorTheme.black
                                        else colorTheme.error
                    confirmPassword = Input.newPassword [ Font.color confirmationColor ]
                                          { onChange = (SetCredentials PasswordConfirm),
                                            text = model.passwordConfirm,
                                            placeholder = Just <| Input.placeholder [] (text "Confirm password"),
                                            label = Input.labelHidden "Confirm password",
                                            show = False
                                          }
                    welcomeMessage = styleMessage "Welcome to Eolach! You're a new user, so you'll have to set a new password."
                in
                    layoutHelper confirmPassword (makeLoginButton "Set password" (Just SetNewUserPassword)) welcomeMessage
            LoggingIn ->
                layoutHelper none (el [ centerX ] <| styleMessage "Logging in...") none
            LoggedIn token ->
                text ("Welcome! Your token is: " ++ token)

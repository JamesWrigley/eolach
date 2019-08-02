// AWS setup
var AWS = require('aws-sdk/global');
var cognitoConfig = require('./cognito_config');
var AmazonCognitoIdentity = require('amazon-cognito-identity-js');

AWS.config.region = cognitoConfig.region;
var poolData = {
    UserPoolId : cognitoConfig.userPoolId,
    ClientId : cognitoConfig.clientId
};
var userPool = new AmazonCognitoIdentity.CognitoUserPool(poolData)

// Variables to use later
var user = null;
var sessionUserAttributes = null;

// Elm setup
var app = Elm.Main.init({
    node: document.getElementById("elm")
});

function handleLoginFailure(error) {
    app.ports.onLoginFailed.send(error.message);
}

function handleLoginSuccess(result) {
    var idToken = result.getIdToken().getJwtToken()
    app.ports.onLoginSucceeded.send(idToken);
}

app.ports.login.subscribe(function (data) {
    var userData = {
        Username : data.email,
        Pool : userPool
    };
    var authData = {
        Username : data.email,
        Password : data.password
    };

    user = new AmazonCognitoIdentity.CognitoUser(userData);
    var authDetails = new AmazonCognitoIdentity.AuthenticationDetails(authData);

    user.authenticateUser(authDetails, {
        onSuccess: handleLoginSuccess,
        onFailure: handleLoginFailure,

        newPasswordRequired: function (userAttributes, requiredAttributes) {
            delete userAttributes.email_verified;
            sessionUserAttributes = userAttributes;
            app.ports.onRequestNewUserPassword.send(null);
        },
    });
});

app.ports.completeUserSignup.subscribe(function (password) {
    user.completeNewPasswordChallenge(password, sessionUserAttributes, {
        onSuccess: handleLoginSuccess,
        onFailure: handleLoginFailure
    });
});

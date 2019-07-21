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
    app.ports.loginResult.send({"success" : false,
                                "error" : error})
}

function handleLoginSuccess(result) {
    // var accessToken = result.getAccessToken().getJwtToken();
    // var pool_url = `cognito-idp.${cognitoConfig.region}.amazonaws.com/${cognitoConfig.userPoolId}`

    // AWS.config.credentials = new AWS.CognitoIdentityCredentials({
    //     IdentityPoolId : cognitoConfig.identityPoolId,
    //     Logins : {
    //         pool_url : result.getIdToken().getJwtToken()
    //     }
    // });
    // AWS.config.credentials.refresh((error) => {
    //     if (error) { }
    // });

    app.ports.loginResult.send({ "success" : true });
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
            app.ports.requestNewUserPassword.send(null);
        },
    });
});

app.ports.completeUserSignup.subscribe(function (password) {
    user.completeNewPasswordChallenge(password, sessionUserAttributes, {
        onSuccess: handleLoginSuccess,
        onFailure: handleLoginFailure
    });
});

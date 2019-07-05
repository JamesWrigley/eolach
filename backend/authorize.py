import json

def authorize(event: dict, context: dict) -> dict:
    token = event["authorizationToken"]
    method_arn = event["methodArn"]

    authorization = "Allow" if token == "yes" else "Deny"
    return {
        "principalId": "user",
        "policyDocument": {
            "Version": "2012-10-17",
            "Statement": [
                {
                    "Action": "execute-api:Invoke",
                    "Effect": authorization,
                    "Resource": method_arn
                }
            ]
        }
    }

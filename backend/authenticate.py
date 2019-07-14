import json

def login(event: dict, context: dict) -> dict:
    body = json.loads(event["body"])
    authenticated = body["email"] == "james@puiterwijk.org"
    return {
        "statusCode": 200,
        "isBase64Encoded": False,
        "body": json.dumps({ "token": "yes" if authenticated else "no" }),
        "headers": {
            "Access-Control-Allow-Origin": "*"
        }
    }

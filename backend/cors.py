def allow(*_) -> dict:
    return {
        "statusCode": 200,
        "headers": {
            "Access-Control-Allow-Origin": "*",
            "Access-Control-Allow-Headers": "Content-Type",
            "Access-Content-Allow-Methods": "OPTIONS,POST"
        }
    }

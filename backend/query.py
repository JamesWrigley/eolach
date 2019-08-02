import json
import functools
from typing import Callable, TypeVar

import boto3

QueryResult = TypeVar("T", str, list)

def dynamodb_metadecorator(table_name: str) -> Callable:
    def dynamodb_decorator(func: Callable) -> Callable:
        @functools.wraps(func)
        def wrapper(event: dict, context: dict) -> dict:
            dynamodb = boto3.resource("dynamodb")
            table = dynamodb.Table(table_name)

            (status, response) = func(table, event["queryStringParameters"], context)
            return {
                "statusCode": status,
                "isBase64Encoded": False,
                "body": json.dumps(response),
                "headers": {
                    "Access-Control-Allow-Origin": "*"
                }
            }

        return wrapper

    return dynamodb_decorator

@dynamodb_metadecorator("Books")
def get_books(books_table, parameters: str, _) -> QueryResult:
    attributes = None
    if parameters:
        try:
            attributes = parameters["attributes"].split(",")
        except KeyError:
            pass

        if any(len(attr) == 0 for attr in attributes):
            return (400, "Invalid attributes format")

    if attributes:
        response = books_table.scan(AttributesToGet=attributes)
    else:
        response = books_table.scan()

    return (200, response["Items"])

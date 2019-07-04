provider "aws" {
  profile = "default"
  region = "us-east-1"
  version = "~> 2.17"
}

output "public_api_url" {
  value = aws_api_gateway_deployment.eolach_public_api_deployment.invoke_url
}

// Lambda's

resource "aws_iam_role" "eolach_lambda_role" {
  name = "EolachLambdaRole"
  assume_role_policy = <<EOF
{
  "Version": "2012-10-17",
  "Statement": [
    {
      "Action": "sts:AssumeRole",
      "Principal": {
        "Service": "lambda.amazonaws.com"
      },
      "Effect": "Allow",
      "Sid": ""
    }
  ]
}
EOF
}

resource "aws_lambda_function" "eolach_auth" {
  runtime = "python3.7"
  handler = "auth.login"
  function_name = "EolachAuth"
  role = "${aws_iam_role.eolach_lambda_role.arn}"

  filename = "deploy/auth.zip"
  source_code_hash = "${filebase64sha256("deploy/auth.zip")}"
}

// API's

resource "aws_api_gateway_rest_api" "eolach_public_api" {
  name = "EolachPublic"
}

resource "aws_api_gateway_resource" "auth_resource" {
  rest_api_id = "${aws_api_gateway_rest_api.eolach_public_api.id}"
  parent_id = "${aws_api_gateway_rest_api.eolach_public_api.root_resource_id}"
  path_part = "auth"
}

resource "aws_api_gateway_method" "auth_method" {
  rest_api_id = "${aws_api_gateway_rest_api.eolach_public_api.id}"
  resource_id = "${aws_api_gateway_resource.auth_resource.id}"
  http_method = "POST"
  authorization = "NONE"
}

resource "aws_api_gateway_integration" "auth_integration" {
  rest_api_id = "${aws_api_gateway_rest_api.eolach_public_api.id}"
  resource_id = "${aws_api_gateway_resource.auth_resource.id}"
  http_method = "${aws_api_gateway_method.auth_method.http_method}"
  integration_http_method = "POST"
  type = "AWS_PROXY"
  uri = "${aws_lambda_function.eolach_auth.invoke_arn}"
}

resource "aws_lambda_permission" "login_from_gateway" {
  statement_id = "AllowEolachPublicInvoke"
  action = "lambda:InvokeFunction"
  function_name = "${aws_lambda_function.eolach_auth.function_name}"
  principal = "apigateway.amazonaws.com"
  source_arn = "${aws_api_gateway_rest_api.eolach_public_api.execution_arn}/*"
}

resource "aws_api_gateway_deployment" "eolach_public_api_deployment" {
  depends_on = ["aws_api_gateway_integration.auth_integration"]

  rest_api_id = "${aws_api_gateway_rest_api.eolach_public_api.id}"
  stage_name = "prod"
}
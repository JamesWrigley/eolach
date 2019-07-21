provider "aws" {
  profile = "default"
  region = "us-east-1"
  version = "~> 2.17"
}

data "aws_region" "current" {}

output "public_api_url" {
  value = aws_api_gateway_deployment.eolach_public_api_deployment.invoke_url
}

output "cognito_client_id" {
  value = aws_cognito_user_pool_client.eolach.id
}

output "cognito_user_pool_id" {
  value = aws_cognito_user_pool.librarians.id
}

output "cognito_identity_pool_id" {
  value = aws_cognito_identity_pool.eolach.id
}

output "aws_region" {
  value = data.aws_region.current.name
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

resource "aws_iam_role_policy_attachment" "basic_execution" {
  role = "${aws_iam_role.eolach_lambda_role.name}"
  policy_arn = "arn:aws:iam::aws:policy/service-role/AWSLambdaBasicExecutionRole"
}

resource "aws_lambda_function" "eolach_login" {
  runtime = "python3.7"
  handler = "authenticate.login"
  function_name = "EolachAuthenticate"
  role = "${aws_iam_role.eolach_lambda_role.arn}"

  filename = "deploy/authenticate.zip"
  source_code_hash = "${filebase64sha256("deploy/authenticate.zip")}"

  depends_on = ["aws_iam_role_policy_attachment.basic_execution"]
}

resource "aws_lambda_function" "eolach_authorize" {
  runtime = "python3.7"
  handler = "authorize.authorize"
  function_name = "EolachAuthorize"
  role = "${aws_iam_role.eolach_lambda_role.arn}"

  filename = "deploy/authorize.zip"
  source_code_hash = "${filebase64sha256("deploy/authorize.zip")}"
}

resource "aws_lambda_function" "eolach_cors" {
  runtime = "python3.7"
  handler = "cors.allow"
  function_name = "EolachCORS"
  role = "${aws_iam_role.eolach_lambda_role.arn}"

  filename = "deploy/cors.zip"
  source_code_hash = "${filebase64sha256("deploy/cors.zip")}"
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

resource "aws_api_gateway_method" "auth_options" {
  rest_api_id = "${aws_api_gateway_rest_api.eolach_public_api.id}"
  resource_id = "${aws_api_gateway_resource.auth_resource.id}"
  http_method = "OPTIONS"
  authorization = "NONE"
}

resource "aws_api_gateway_integration" "auth_integration" {
  rest_api_id = "${aws_api_gateway_rest_api.eolach_public_api.id}"
  resource_id = "${aws_api_gateway_resource.auth_resource.id}"
  http_method = "${aws_api_gateway_method.auth_method.http_method}"
  integration_http_method = "POST"
  type = "AWS_PROXY"
  uri = "${aws_lambda_function.eolach_login.invoke_arn}"
}

resource "aws_api_gateway_integration" "auth_options_integration" {
  rest_api_id = "${aws_api_gateway_rest_api.eolach_public_api.id}"
  resource_id = "${aws_api_gateway_resource.auth_resource.id}"
  http_method = "${aws_api_gateway_method.auth_options.http_method}"
  integration_http_method = "POST"
  type = "AWS_PROXY"
  uri = "${aws_lambda_function.eolach_cors.invoke_arn}"
}

resource "aws_lambda_permission" "login_from_gateway" {
  statement_id = "AllowEolachPublicInvoke"
  action = "lambda:InvokeFunction"
  function_name = "${aws_lambda_function.eolach_login.function_name}"
  principal = "apigateway.amazonaws.com"
  source_arn = "${aws_api_gateway_rest_api.eolach_public_api.execution_arn}/*"
}

resource "aws_lambda_permission" "cors_from_gateway" {
  statement_id = "AllowEolachPublicCORS"
  action = "lambda:InvokeFunction"
  function_name = "${aws_lambda_function.eolach_cors.function_name}"
  principal = "apigateway.amazonaws.com"
  source_arn = "${aws_api_gateway_rest_api.eolach_public_api.execution_arn}/*"
}

resource "aws_api_gateway_deployment" "eolach_public_api_deployment" {
  depends_on = ["aws_api_gateway_integration.auth_integration"]

  rest_api_id = "${aws_api_gateway_rest_api.eolach_public_api.id}"
  stage_name = "prod"
}

// DynamoDB tables


// Cognito

resource "aws_cognito_user_pool" "librarians" {
  name = "EolachLibrarians"
  mfa_configuration = "OFF"

  device_configuration {
    challenge_required_on_new_device = true
  }

  password_policy {
    minimum_length = 10
    require_lowercase = false
    require_numbers = false
    require_symbols = false
    require_uppercase = false
  }

  verification_message_template {
    default_email_option = "CONFIRM_WITH_CODE"
    email_message = "Your verification code is: {####}"
    email_subject = "Eolach verification code"
  }
}

resource "aws_cognito_identity_pool" "eolach" {
  identity_pool_name = "Eolach"
  allow_unauthenticated_identities = false

  cognito_identity_providers {
    client_id = "${aws_cognito_user_pool_client.eolach.id}"
    provider_name = "cognito-idp.${data.aws_region.current.name}.amazonaws.com/${aws_cognito_user_pool.librarians.id}"
  }
}

resource "aws_cognito_user_pool_client" "eolach" {
  name = "Eolach"
  user_pool_id = "${aws_cognito_user_pool.librarians.id}"
  read_attributes = ["email", "email_verified"]
  write_attributes = ["email"]
  generate_secret = false
  refresh_token_validity = 1
}

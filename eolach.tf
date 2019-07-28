provider "aws" {
  profile = "default"
  region = "us-east-1"
  version = "~> 2.17"
}

data "aws_region" "current" {}

output "api_url" {
  value = aws_api_gateway_deployment.eolach_internal_api_deployment.invoke_url
}

output "cognito_client_id" {
  value = aws_cognito_user_pool_client.eolach.id
}

output "cognito_user_pool_id" {
  value = aws_cognito_user_pool.librarians.id
}

output "aws_region" {
  value = data.aws_region.current.name
}

output "frontend_s3_bucket" {
  value = aws_s3_bucket.eolach_frontend.bucket
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

resource "aws_iam_role_policy_attachment" "dynamodb_readonly" {
  role = "${aws_iam_role.eolach_lambda_role.name}"
  policy_arn = "arn:aws:iam::aws:policy/AmazonDynamoDBReadOnlyAccess"
}

resource "aws_lambda_function" "eolach_query_books" {
  runtime = "python3.7"
  handler = "query.get_books"
  function_name = "EolachQueryBooks"
  role = "${aws_iam_role.eolach_lambda_role.arn}"

  filename = "deploy/query.zip"
  source_code_hash = "${filebase64sha256("deploy/query.zip")}"

  depends_on = ["aws_iam_role_policy_attachment.basic_execution"]
}

// API's

resource "aws_api_gateway_rest_api" "eolach_internal_api" {
  name = "EolachInternal"
}

resource "aws_api_gateway_authorizer" "librarian" {
  name = "LibrarianAuthorizer"
  rest_api_id = "${aws_api_gateway_rest_api.eolach_internal_api.id}"
  identity_source = "method.request.header.Authorization"
  type = "COGNITO_USER_POOLS"
  provider_arns = ["${aws_cognito_user_pool.librarians.arn}"]
}

resource "aws_api_gateway_resource" "books" {
  rest_api_id = "${aws_api_gateway_rest_api.eolach_internal_api.id}"
  parent_id = "${aws_api_gateway_rest_api.eolach_internal_api.root_resource_id}"
  path_part = "books"
}

resource "aws_api_gateway_method" "books_get" {
  rest_api_id = "${aws_api_gateway_rest_api.eolach_internal_api.id}"
  resource_id = "${aws_api_gateway_resource.books.id}"
  http_method = "GET"
  authorization = "COGNITO_USER_POOLS"
  authorizer_id = "${aws_api_gateway_authorizer.librarian.id}"
}

resource "aws_api_gateway_integration" "books_get" {
  rest_api_id = "${aws_api_gateway_rest_api.eolach_internal_api.id}"
  resource_id = "${aws_api_gateway_resource.books.id}"
  http_method = "${aws_api_gateway_method.books_get.http_method}"
  integration_http_method = "POST"
  type = "AWS_PROXY"
  uri = "${aws_lambda_function.eolach_query_books.invoke_arn}"
}

resource "aws_lambda_permission" "invoke_from_gateway" {
  statement_id = "AllowEolachPublicInvoke"
  action = "lambda:InvokeFunction"
  function_name = "${aws_lambda_function.eolach_query_books.function_name}"
  principal = "apigateway.amazonaws.com"
  source_arn = "${aws_api_gateway_rest_api.eolach_internal_api.execution_arn}/*"
}

resource "aws_api_gateway_deployment" "eolach_internal_api_deployment" {
  depends_on = ["aws_api_gateway_integration.books_get"]

  rest_api_id = "${aws_api_gateway_rest_api.eolach_internal_api.id}"
  stage_name = "prod"
}

// DynamoDB tables

resource "aws_dynamodb_table" "books" {
  name = "Books"
  billing_mode = "PAY_PER_REQUEST"
  hash_key = "author"
  range_key = "title"

  attribute {
    name = "author"
    type = "S"
  }

  attribute {
    name = "title"
    type = "S"
  }
}

// Testing items

resource "aws_dynamodb_table_item" "mere_christianity" {
  table_name = "${aws_dynamodb_table.books.name}"
  hash_key = "${aws_dynamodb_table.books.hash_key}"
  range_key = "${aws_dynamodb_table.books.range_key}"

  item = <<ITEM
{
    "author": {"S": "C.S. Lewis"},
    "title": {"S": "Mere Christianity"},
    "subjects": {"L": [{"S": "Apologetics"}, {"S": "Theology"}]},
    "section": {"S": "204"}
}
ITEM
}

resource "aws_dynamodb_table_item" "screwtape" {
  table_name = "${aws_dynamodb_table.books.name}"
  hash_key = "${aws_dynamodb_table.books.hash_key}"
  range_key = "${aws_dynamodb_table.books.range_key}"

  item = <<ITEM
{
    "author": {"S": "C.S. Lewis"},
    "title": {"S": "The Screwtape Letters"},
    "subjects": {"L": [{"S": "Theology"}, {"S": "Fiction"}, {"S": "Satire"}]},
    "section": {"S": "248.2"}
}
ITEM
}

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

// resource "aws_cognito_identity_pool" "eolach" {
//   identity_pool_name = "Eolach"
//   allow_unauthenticated_identities = false
// 
//   cognito_identity_providers {
//     client_id = "${aws_cognito_user_pool_client.eolach.id}"
//     provider_name = "cognito-idp.${data.aws_region.current.name}.amazonaws.com/${aws_cognito_user_pool.librarians.id}"
//   }
// }

resource "aws_cognito_user_pool_client" "eolach" {
  name = "Eolach"
  user_pool_id = "${aws_cognito_user_pool.librarians.id}"
  read_attributes = ["email", "email_verified"]
  write_attributes = ["email"]
  generate_secret = false
  refresh_token_validity = 1
}

// Route53

resource "aws_route53_zone" "eolach_website_zone" {
  name = "eolach.co."
  force_destroy = true
}

resource "aws_route53_record" "eolach_cert_validation" {
  zone_id = "${aws_route53_zone.eolach_website_zone.zone_id}"
  name = "${aws_acm_certificate.website.domain_validation_options.0.resource_record_name}"
  type = "${aws_acm_certificate.website.domain_validation_options.0.resource_record_type}"
  records = ["${aws_acm_certificate.website.domain_validation_options.0.resource_record_value}"]
  ttl = 60
}

resource "aws_route53_record" "cloudfront_redirect" {
  zone_id = "${aws_route53_zone.eolach_website_zone.zone_id}"
  name = "eolach.co"
  type = "A"

  alias {
    name = "${aws_cloudfront_distribution.eolach.domain_name}"
    zone_id = "${aws_cloudfront_distribution.eolach.hosted_zone_id}"
    evaluate_target_health = false
  }
}

// Certificate Manager

resource "aws_acm_certificate" "website" {
  domain_name = "eolach.co"
  validation_method = "DNS"

  lifecycle {
    create_before_destroy = true
  }
}

resource "aws_acm_certificate_validation" "website" {
  certificate_arn = "${aws_acm_certificate.website.arn}"
  validation_record_fqdns = ["${aws_route53_record.eolach_cert_validation.fqdn}"]
}

// S3

locals {
  eolach_s3_origin_id = "eolachS3"
}

resource "aws_s3_bucket" "eolach_frontend" {
  bucket_prefix = "eolach-frontend"
  acl = "public-read"

  cors_rule {
    allowed_headers = ["*"]
    allowed_methods = ["GET", "HEAD"]
    allowed_origins = ["*"]
  }
}

// Cloudfront

resource "aws_cloudfront_distribution" "eolach" {
  enabled = true
  is_ipv6_enabled = true
  default_root_object = "index.html"
  price_class = "PriceClass_100"
  aliases = ["eolach.co"]

  origin {
    domain_name = "${aws_s3_bucket.eolach_frontend.bucket_regional_domain_name}"
    origin_id = "${local.eolach_s3_origin_id}"
  }

  default_cache_behavior {
    allowed_methods = ["GET", "HEAD"]
    cached_methods = ["GET", "HEAD"]
    compress = true
    target_origin_id = "${local.eolach_s3_origin_id}"
    viewer_protocol_policy = "redirect-to-https"

    forwarded_values {
      query_string = false

      cookies {
        forward = "none"
      }
    }
  }

  restrictions {
    geo_restriction {
      restriction_type = "none"
    }
  }

  viewer_certificate {
    acm_certificate_arn = "${aws_acm_certificate.website.arn}"
    minimum_protocol_version = "TLSv1.2_2018"
    ssl_support_method = "sni-only"
  }
}

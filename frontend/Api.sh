#! /bin/bash

URL=$(terraform output api_url)

cat <<EOF
module Api exposing (url)

url : String
url = "$URL"
EOF

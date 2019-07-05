#! /bin/bash

URL=$(terraform output public_api_url)

cat <<EOF
module Api exposing (url)

url : String
url = "$URL"
EOF

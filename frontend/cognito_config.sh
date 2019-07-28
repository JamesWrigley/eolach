#! /bin/bash

REGION=$(terraform output aws_region)
CLIENT_ID=$(terraform output cognito_client_id)
USER_POOL_ID=$(terraform output cognito_user_pool_id)

cat <<EOF
module.exports = {
    region: "$REGION",
    clientId: "$CLIENT_ID",
    userPoolId: "$USER_POOL_ID",
};
EOF

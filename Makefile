elm-dev:
	sed "s@API_URL@`terraform output api_url`@" ./frontend/Api.template.elm > ./frontend/Api.elm
	elm make frontend/Main.elm --optimize --output=frontend/elm.js

elm: elm-dev
	uglifyjs frontend/elm.js --compress 'pure_funcs="F2,F3,F4,F5,F6,F7,F8,F9,A2,A3,A4,A5,A6,A7,A8,A9",pure_getters,keep_fargs=false,unsafe_comps,unsafe' | uglifyjs --mangle --output=frontend/elm.min.js

frontend: elm
	npm run build

infra: bundle eolach.tf
	terraform apply

deploy: infra frontend/index.html frontend/cognito.js frontend/Api.template.elm
	make frontend # Need to do this after terraform runs so we get the right API URL
	./frontend/cognito_config.sh > ./frontend/cognito_config.js
	aws s3 cp ./frontend/index.html s3://`terraform output frontend_s3_bucket` --content-type 'text/html'
	aws s3 cp ./frontend/elm.min.js s3://`terraform output frontend_s3_bucket` --content-type 'text/javascript'
	aws s3 cp ./frontend/eolach.js s3://`terraform output frontend_s3_bucket` --content-type 'text/javascript'

bundle: backend/query.py
	-zip -ju deploy/query.zip backend/query.py

clean:
	rm -rf deploy/*

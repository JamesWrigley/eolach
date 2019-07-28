build: elm
	npm run build

elm-dev: frontend/main.elm
	elm make frontend/main.elm --optimize --output=frontend/elm.js

elm: elm-dev
	uglifyjs frontend/elm.js --compress 'pure_funcs="F2,F3,F4,F5,F6,F7,F8,F9,A2,A3,A4,A5,A6,A7,A8,A9",pure_getters,keep_fargs=false,unsafe_comps,unsafe' | uglifyjs --mangle --output=frontend/elm.min.js

deploy: bundle
	terraform apply
	./frontend/Api.sh > ./frontend/Api.elm
	./frontend/cognito_config.sh > ./frontend/cognito_config.js

bundle: backend/query.py backend/cors.py
	-zip -ju backend/query.zip backend/query.py
	-zip -ju backend/cors.zip backend/cors.py

clean:
	rm -rf deploy/*

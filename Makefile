build: elm
	npm run build

elm: frontend/main.elm
	elm make frontend/main.elm --optimize --output=deploy/elm.js
	uglifyjs deploy/elm.js --compress 'pure_funcs="F2,F3,F4,F5,F6,F7,F8,F9,A2,A3,A4,A5,A6,A7,A8,A9",pure_getters,keep_fargs=false,unsafe_comps,unsafe' | uglifyjs --mangle --output=frontend/elm.min.js

deploy:
	terraform apply
	./frontend/Api.sh > ./frontend/Api.elm
	./frontend/cognito_config.sh > ./frontend/cognito_config.js

bundle: backend/authenticate.py backend/authorize.py
	mkdir -p deploy
	-zip -ju deploy/authenticate.zip backend/authenticate.py
	-zip -ju deploy/authorize.zip backend/authorize.py
	-zip -ju deploy/cors.zip backend/cors.py

clean:
	rm -rf deploy/*

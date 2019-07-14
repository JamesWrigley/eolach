deploy: eolach.tf
	terraform apply
	./frontend/Api.sh > ./frontend/Api.elm

bundle: backend/authenticate.py backend/authorize.py
	mkdir -p deploy
	-zip -ju deploy/authenticate.zip backend/authenticate.py
	-zip -ju deploy/authorize.zip backend/authorize.py
	-zip -ju deploy/cors.zip backend/cors.py

clean:
	rm -rf deploy/*

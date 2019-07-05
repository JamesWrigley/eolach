bundle: backend/auth.py
bundle: backend/authenticate.py backend/authorize.py
	mkdir -p deploy
	zip -ju deploy/authenticate.zip backend/authenticate.py
	zip -ju deploy/authorize.zip backend/authorize.py

clean:
	rm -rf deploy/*

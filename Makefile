bundle: backend/auth.py
	mkdir -p deploy
	zip -ju deploy/auth.zip backend/auth.py

clean:
	rm -rf deploy/*

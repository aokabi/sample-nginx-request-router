build:
	docker compose rm -fsv nginx && docker compose up nginx

build-so:
	docker buildx build --output type=local,dest=. .
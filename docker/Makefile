



all:
	docker-compose up --build
	docker-compose exec web python manage.py makemigrations
	docker-compose exec web python manage.py migrate


stop: 
	docker-compose down


clean: stop
	docker volume rm $(docker volume ls -qf dangling=true)

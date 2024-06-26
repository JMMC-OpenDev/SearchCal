# Build a docker image for MCS/SearchCal environment
#
NAME=searchcal
VERSION=6.0.0
TAR_FILE=$(NAME)-$(VERSION).tar

all:	build

build:	Dockerfile
	docker build -t $(NAME):$(VERSION) --rm .

tag_latest:
	docker tag -f $(NAME):$(VERSION) $(NAME):latest

kill:
	docker kill $(NAME)

run:
	docker run -d -p 8079:8079 --name=$(NAME) --rm $(NAME):$(VERSION)

run_vol:
	docker run -d -v /data/VOL_SCL/:/root/MCSTOP/data/tmp/ -p 8079:8079 --name=$(NAME) --rm $(NAME):$(VERSION)

# You can edit the sources using your IDE on the host machine and compile inside the container (using make shell)
run_dev:
	docker run -d -v $$PWD/SearchCal/:/root/SearchCal/ -p 8079:8079 --name=$(NAME) --rm $(NAME):$(VERSION)

shell:
	docker exec -it $(NAME) bash

logs:
	docker logs $(NAME)

logs-f:
	docker logs --since 10m -f $(NAME)


save:
	docker save -o $(TAR_FILE) $(NAME):$(VERSION)

clean:
	rm -rf $(TAR_FILE)


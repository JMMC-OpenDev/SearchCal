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
	docker run -p 8079:8079 --name=$(NAME) --rm $(NAME):$(VERSION)

run_vol:
	docker run -v /data/VOL_SCL/:/root/MCSTOP/data/tmp/ -p 8079:8079 --name=$(NAME) --rm $(NAME):$(VERSION)

shell:
	docker run -it $(NAME):$(VERSION) /bin/bash

save:
	docker save -o $(TAR_FILE) $(NAME):$(VERSION) 

clean:
	rm -rf $(TAR_FILE)

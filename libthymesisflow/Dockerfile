FROM ubuntu:18.04

MAINTAINER michele.gazzetti1@ibm.com

RUN apt-get update && \
	apt-get install -y libocxl-dev

COPY ./bin/thymesisf-agent /usr/bin/
	
ENTRYPOINT ["/usr/bin/thymesisf-agent"]

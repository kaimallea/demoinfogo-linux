FROM ubuntu:14.04
MAINTAINER Kai Mallea "kmallea@gmail.com"
ENV REFRESHED_AT 2016-05-05
RUN apt-get -qq update && apt-get install -y curl build-essential g++-multilib
RUN mkdir /demoinfogo
WORKDIR /demoinfogo
ADD Makefile /demoinfogo/
RUN make get_protobuf
RUN make protobuf
ADD src/ /demoinfogo/src/
RUN make
ENTRYPOINT ["/bin/bash"]
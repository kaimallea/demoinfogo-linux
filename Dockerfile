FROM ubuntu:16.04

ENV LAST_UPDATED_AT 2017-12-22

RUN DEBIAN_FRONTEND=noninteractive apt-get update \
# install dependencies
    && DEBIAN_FRONTEND=noninteractive apt-get install -y build-essential g++-multilib curl \
# delete apt cache to save space
    && rm -rf /var/lib/apt/lists/*

WORKDIR /demoinfogo

ENTRYPOINT ["/bin/bash"] 
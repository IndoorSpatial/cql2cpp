# docker buildx build -t cql2cpp .
# docker save cql2cpp:latest -o cql2cpp.tar
# docker load -i cql2cpp.tar
# docker tag cql2cpp kunlinyu/cql2cpp:latest
# docker push kunlinyu/cql2cpp:latest

FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update

# install basic tools
RUN apt-get install -y sudo
RUN apt-get install -y curl wget iputils-ping
RUN apt-get install -y build-essential pkg-config
RUN apt-get install -y file gdb
RUN apt-get install -y git cmake repo
RUN apt-get install -y vim tree silversearcher-ag

# install toolchain
RUN apt-get install -y gcc-aarch64-linux-gnu
RUN apt-get install -y g++-aarch64-linux-gnu

# install dependencies
RUN apt-get install -y flex bison
RUN apt-get install -y libgflags-dev libgoogle-glog-dev libgtest-dev
RUN apt-get install -y libgeos++-dev
RUN apt-get install -y sqlite3 libsqlite3-mod-spatialite

USER ubuntu
WORKDIR /home/ubuntu/

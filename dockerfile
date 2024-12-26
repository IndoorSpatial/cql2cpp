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
RUN apt-get install -y flex bison++
RUN apt-get install -y libgflags-dev libgoogle-glog-dev libgtest-dev
RUN apt-get install -y libgeos++-dev

USER ubuntu
WORKDIR /home/ubuntu/

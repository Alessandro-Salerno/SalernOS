FROM ubuntu

RUN apt-get update 
RUN apt-get upgrade -y
RUN apt-get install -y nasm --fix-missing
RUN apt-get install -y binutils-x86-64-linux-gnu --fix-missing
RUN apt-get install -y build-essential --fix-missing
RUN apt-get install -y gcc-x86-64-linux-gnu --fix-missing
RUN apt-get install -y xorriso --fix-missing
RUN apt-get install -y git --fix-missing
RUN apt-get install -y curl --fix-missing


VOLUME /root/env
WORKDIR /root/env

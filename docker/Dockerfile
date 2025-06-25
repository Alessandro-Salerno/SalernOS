FROM --platform=linux/amd64 ubuntu

RUN apt-get update 
RUN apt-get upgrade -y
RUN apt-get install -y nasm --fix-missing
RUN apt-get install -y make --fix-missing
RUN apt-get install -y build-essential --fix-missing
RUN apt-get install -y xorriso --fix-missing
RUN apt-get install -y git --fix-missing
RUN apt-get install -y curl --fix-missing

RUN useradd -ms /bin/bash user
WORKDIR /workspace
USER user

VOLUME /workspace

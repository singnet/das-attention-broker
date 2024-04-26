FROM ubuntu:latest

RUN apt-get update -y
RUN apt-get install -y build-essential
RUN apt-get install -y pkg-config

RUN mkdir /opt/data
RUN mkdir /opt/das-attention-broker
VOLUME /opt/das-attention-broker
WORKDIR /opt/attention-broker

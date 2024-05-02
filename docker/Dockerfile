FROM ubuntu:latest

ARG BASE_DIR="/opt"
ARG TMP_DIR="/tmp"

ARG ATTENTION_BROKER_DIR="${BASE_DIR}/das-attention-broker"
ARG DATA_DIR="${BASE_DIR}/data"
ARG GRPC_DIR="${BASE_DIR}/grpc"
ARG PROTO_DIR="${BASE_DIR}/proto"
ARG BAZEL_DIR="${BASE_DIR}/bazel"

RUN mkdir -p ${ATTENTION_BROKER_DIR}
RUN mkdir -p ${DATA_DIR}
RUN mkdir -p ${GRPC_DIR}
RUN mkdir -p ${BAZEL_DIR}
RUN mkdir -p ${PROTO_DIR}
VOLUME ${ATTENTION_BROKER_DIR}

RUN apt-get update -y
RUN apt-get install -y git

RUN cd ${GRPC_DIR} &&\
    git clone https://github.com/grpc/grpc &&\
    cd grpc &&\
    git submodule update --init

RUN apt-get install -y build-essential
RUN apt-get install -y autoconf
RUN apt-get install -y libtool
RUN apt-get install -y pkg-config
RUN apt-get install -y curl
RUN apt-get install -y gcc
RUN apt-get install -y protobuf-compiler

ENV CC=/usr/bin/gcc
COPY assets/bazelisk ${BAZEL_DIR}
RUN ln -s ${BAZEL_DIR}/bazelisk /usr/bin/bazel
RUN cd ${GRPC_DIR}/grpc &&\
    ${BAZEL_DIR}/bazelisk build :all

ADD https://raw.githubusercontent.com/singnet/das-query-engine/master/proto/attention_broker.proto ${PROTO_DIR}
ADD https://raw.githubusercontent.com/singnet/das-query-engine/master/proto/common.proto ${PROTO_DIR}
ADD https://raw.githubusercontent.com/singnet/das-query-engine/master/proto/echo.proto ${PROTO_DIR}

WORKDIR /opt/das-attention-broker

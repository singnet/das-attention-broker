#!/bin/bash

mkdir -p bin
docker run \
    --volume .:/opt/das-attention-broker \
    --workdir /opt/das-attention-broker/src \
    das-attention-broker-builder \
    make

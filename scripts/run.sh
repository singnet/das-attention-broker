#!/bin/bash

DIR=`dirname $2`
FILE=`basename $2`
CONTAINER_NAME="das-attention-broker-run"

docker run \
    --net="host" \
    --name=$CONTAINER_NAME \
    --env DAS_REDIS_HOSTNAME=$DAS_REDIS_HOSTNAME \
    --env DAS_REDIS_PORT=$DAS_REDIS_PORT \
    --env DAS_MONGODB_HOSTNAME=$DAS_MONGODB_HOSTNAME \
    --env DAS_MONGODB_PORT=$DAS_MONGODB_PORT \
    --env DAS_MONGODB_USERNAME=$DAS_MONGODB_USERNAME \
    --env DAS_MONGODB_PASSWORD=$DAS_MONGODB_PASSWORD \
    --volume $DIR:/opt/data \
    --volume .:/opt/das-attention-broker \
    -it das-attention-broker-builder \
    bin/$1 /opt/data/$FILE

sleep 1
docker rm $CONTAINER_NAME >& /dev/null

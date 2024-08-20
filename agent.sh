+#!/bin/bash

export WORKSPACE_BASE=$(pwd)

docker run \
    -it \
    --pull=always \
    --add-host host.docker.internal:host-gateway \
    -e SANDBOX_USER_ID=$(id -u) \
    -e LLM_OLLAMA_BASE_URL="http://host.docker.internal:11434" \
    -e WORKSPACE_MOUNT_PATH=$WORKSPACE_BASE \
    -v $WORKSPACE_BASE:/opt/workspace_base \
    -v /var/run/docker.sock:/var/run/docker.sock \
    -p 3000:3000 \
    ghcr.io/opendevin/opendevin:main

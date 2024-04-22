#!/usr/bin/bash
podman build -t nginx-proxy .
podman run --network=host --name bestcontainer -d nginx-proxy


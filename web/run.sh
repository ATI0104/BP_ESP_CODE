#!/usr/bin/bash
podman build -t nginx-proxy .
podman run -p 8081:8081  --name bestcontainer -d nginx-proxy


#!/bin/zsh

cd ../client
docker build -t client:release --build-arg BUILD_TYPE=release .

cd ../worker
docker build -t worker:release --build-arg BUILD_TYPE=release .

cd ../scenarios

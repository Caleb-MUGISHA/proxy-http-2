#!/bin/bash
set -eo pipefail

# Build proxy
mkdir -p build && cd build
conan install .. --build=missing
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)

# AWS Deployment
terraform -chdir=../infra/environments/aws/ apply -auto-approve

# Azure Deployment
terraform -chdir=../infra/environments/azure/ apply -auto-approve

# Performance Benchmarking
wrk -t12 -c400 -d30s https://proxy-endpoint/benchmark

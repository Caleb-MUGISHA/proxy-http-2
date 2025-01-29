# Proxy Application

## Overview
This project is a high-performance proxy server that includes core networking functionalities, load balancing mechanisms, configuration parsing, and metric collection. The system is designed for extensibility and scalability, with modular components for handling HTTP/2 traffic, managing connections, and exporting metrics.

## Project Structure
```
├── proxy/
│   ├── src/
│   │   ├── core/
│   │   │   ├── listener.cc         # Network listener implementation
│   │   │   ├── connection_handler.cc
│   │   │   └── http2_codec.cc      # HTTP/2 codec using nghttp2
│   │   ├── config/
│   │   │   ├── yaml_parser.cc      # Configuration parsing
│   │   │   └── route_table.cc      # Routing rules
│   │   ├── load_balancing/
│   │   │   ├── round_robin.cc
│   │   │   └── least_conn.cc
│   │   ├── metrics/
│   │   │   ├── prometheus_exporter.cc
│   │   │   └── stats_collector.cc
│   │   ├── main.cc
│   │   └── proxy.h
│   ├── include/                   # Header files
│   ├── third_party/               # Custom dependencies
│   ├── CMakeLists.txt
│   └── conanfile.txt
├── infra/
│   ├── modules/
│   │   ├── aws/
│   │   │   ├── network/
│   │   │   ├── compute/
│   │   │   └── lb/
│   │   └── azure/
│   │       ├── network/
│   │       ├── compute/
│   │       └── lb/
│   ├── environments/
│   │   ├── prod/
│   │   └── staging/
└── scripts/
    ├── deploy.sh
    └── healthcheck.sh
```

## Features
- **Network Listener:** Manages incoming and outgoing traffic.
- **HTTP/2 Support:** Uses `nghttp2` for efficient HTTP/2 communication.
- **Configuration Parsing:** YAML-based configuration for flexibility.
- **Load Balancing:** Supports Round Robin and Least Connection strategies.
- **Metrics Exporting:** Integrates with Prometheus for observability.
- **Infrastructure as Code:** Modular infrastructure setup for AWS and Azure.

## Installation

### Prerequisites
- C++ compiler (GCC/Clang/MSVC)
- CMake
- Conan (dependency manager)
- Docker (optional for running infrastructure)

### Steps
1. Clone the repository:
   ```bash
   git clone <repository-url>
   cd proxy_project
   ```
2. Install dependencies using Conan:
   ```bash
   conan install . --build=missing
   ```
3. Build the project:
   ```bash
   cmake -B build
   cmake --build build
   ```
4. Run the proxy:
   ```bash
   ./build/proxy
   ```

## Deployment
Use the deployment script to deploy the proxy:
```bash
./scripts/deploy.sh
```
To check the health status:
```bash
./scripts/healthcheck.sh
```


# Multi-Stage Build for C++ Application
FROM ubuntu:22.04 AS builder

# Install build dependencies
RUN apt-get update && apt-get install -y \
    cmake \
    g++ \
    git \
    make \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /build

# Copy source code
COPY CMakeLists.txt .
COPY main.cpp .
COPY jsondb.h .
COPY jsondb.cpp .
COPY Models.h .
# COPY algo.cpp .

# Build the application
RUN cmake -B build -G "Unix Makefiles" \
    && cmake --build build --config Release

# Runtime Stage - Minimal image
FROM ubuntu:22.04

# Install only runtime dependencies
RUN apt-get update && apt-get install -y \
    ca-certificates \
    curl \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy compiled binary from builder
COPY --from=builder /build/build/server_app /app/server_app

# Database file will be created on first run by the application

# Expose the port (Cloud deployments use PORT env var)
EXPOSE 8080

# Health check
HEALTHCHECK --interval=30s --timeout=3s --start-period=5s --retries=3 \
    CMD curl -f http://localhost:8080/health || exit 1

# Run the application
CMD ["/app/server_app"]

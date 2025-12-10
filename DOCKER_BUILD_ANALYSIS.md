# Docker Build & Repository Publishing Analysis

**Date:** 2025-12-10  
**Project:** DAA Flight Booking Application  
**Analysis Status:** ⚠️ Issues Found

---

## Executive Summary

This document outlines the analysis of the Docker build process and identifies potential errors that could occur when publishing this repository. Several critical and minor issues have been identified that need to be addressed before production deployment.

---

## 🔴 Critical Issues

### 1. **Missing `algo.cpp` File in Dockerfile**

**Severity:** HIGH  
**Impact:** Docker build will fail

**Problem:**
- The `Dockerfile` (line 20) copies `algo.cpp` to the build directory
- However, `algo.cpp` exists in the root directory but appears to be empty or minimal
- The `CMakeLists.txt` does NOT include `algo.cpp` in the build (line 59: only `main.cpp` and `jsondb.cpp`)

**Evidence:**
```dockerfile
# Dockerfile line 20
COPY algo.cpp .
```

```cmake
# CMakeLists.txt line 59
add_executable(server_app main.cpp jsondb.cpp)
```

**Solution:**
Either:
- **Option A:** Remove `COPY algo.cpp .` from Dockerfile if it's not needed
- **Option B:** Add `algo.cpp` to CMakeLists.txt if it contains required code

---

### 2. **Port Mismatch in main.cpp**

**Severity:** MEDIUM-HIGH  
**Impact:** Application won't be accessible on expected port

**Problem:**
- `main.cpp` line 177 sets default port to `18080` instead of `8080`
- Dockerfile exposes port `8080` (line 44)
- This mismatch will cause connection issues

**Evidence:**
```cpp
// main.cpp line 177
int port = 18080;  // Default to 8080 for cloud deployment
```

```dockerfile
# Dockerfile line 44
EXPOSE 8080
```

**Solution:**
Change line 177 in `main.cpp` to:
```cpp
int port = 8080;  // Default to 8080 for cloud deployment
```

---

### 3. **Missing Database File Handling**

**Severity:** MEDIUM  
**Impact:** Runtime errors on first container startup

**Problem:**
- Dockerfile line 41 attempts to copy `flight_database.json` but this file may not exist in the build context
- The RUN command will fail if the file doesn't exist during build

**Evidence:**
```dockerfile
# Dockerfile line 41
RUN if [ -f flight_database.json ]; then cp flight_database.json /app/; else echo "Database will be created on first run"; fi
```

**Issue:** This command runs in the builder stage but the file is never copied from the host. The condition will always be false.

**Solution:**
Replace lines 40-41 in Dockerfile with:
```dockerfile
# Copy database file if it exists (optional, will be created on first run)
COPY flight_database.json* /app/ || echo "Database will be created on first run"
```

Or better yet, remove this entirely and ensure the application creates the database on startup.

---

## 🟡 Medium Priority Issues

### 4. **Incomplete .dockerignore**

**Severity:** MEDIUM  
**Impact:** Larger image size, potential security issues

**Problem:**
- `.dockerignore` excludes `backend/` and `frontend/` directories
- These directories exist in the project but are not needed for the C++ server
- However, sensitive files might not be excluded

**Current .dockerignore:**
```
build/
.git/
.gitignore
README.md
*.md
.vscode/
.idea/
CMakeCache.txt
cmake_install.cmake
Makefile
backend/
frontend/
```

**Missing entries:**
- `*.log`
- `.env` and `.env.*`
- `*.json` (database files that shouldn't be in the image)
- `node_modules/` (if any)

**Recommended .dockerignore:**
```
# Build artifacts
build/
out/
bin/
cmake-build-debug/
_deps/

# Version control
.git/
.gitignore

# Documentation
README.md
*.md
DOCS/

# IDE
.vscode/
.idea/
.vs/
*.code-workspace

# CMake artifacts
CMakeCache.txt
cmake_install.cmake
Makefile

# Application directories not needed
backend/
frontend/

# Database and logs
*.json
*.log
*.db

# Environment files
.env
.env.*

# Dependencies
node_modules/
```

---

### 5. **No Health Check Endpoint Validation**

**Severity:** MEDIUM  
**Impact:** Health checks may fail silently

**Problem:**
- Dockerfile defines a health check using `/health` endpoint (line 47-48)
- The endpoint exists in `main.cpp` (line 62-65) but returns plain text "OK"
- Health check uses `curl` which is installed, but the endpoint should return proper HTTP status

**Current Implementation:**
```cpp
CROW_ROUTE(app, "/health")
([](){
    return crow::response("OK");
});
```

**Recommendation:**
Add proper JSON response:
```cpp
CROW_ROUTE(app, "/health")
([](){
    json health = {
        {"status", "healthy"},
        {"timestamp", std::time(nullptr)},
        {"service", "flight-booking-api"}
    };
    return crow::response(health.dump());
});
```

---

## 🟢 Low Priority Issues

### 6. **Missing Build Optimization Flags**

**Severity:** LOW  
**Impact:** Larger binary size, slower performance

**Problem:**
- CMake build uses Release config but no explicit optimization flags
- Could benefit from additional compiler optimizations

**Recommendation:**
Add to CMakeLists.txt:
```cmake
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(MSVC)
        add_compile_options(/O2 /GL)
    else()
        add_compile_options(-O3 -march=native)
    endif()
endif()
```

---

### 7. **No Multi-Architecture Support**

**Severity:** LOW  
**Impact:** Limited deployment options

**Problem:**
- Dockerfile doesn't specify platform
- May have issues on ARM-based systems (e.g., Apple Silicon, AWS Graviton)

**Recommendation:**
Use buildx for multi-platform builds:
```bash
docker buildx build --platform linux/amd64,linux/arm64 -t daa-flight-app:latest .
```

---

## 📋 Pre-Publishing Checklist

### Required Actions Before Publishing:

- [ ] **Fix port mismatch** (Change 18080 to 8080 in main.cpp)
- [ ] **Resolve algo.cpp issue** (Remove from Dockerfile or add to CMake)
- [ ] **Fix database file handling** in Dockerfile
- [ ] **Update .dockerignore** with recommended entries
- [ ] **Test Docker build** locally
- [ ] **Test container startup** and verify all endpoints
- [ ] **Add docker-compose.yml** for easier deployment
- [ ] **Document environment variables** in README
- [ ] **Add CI/CD configuration** (.github/workflows/docker-build.yml)
- [ ] **Security scan** the Docker image

### Recommended Actions:

- [ ] Improve health check endpoint
- [ ] Add build optimization flags
- [ ] Create multi-architecture builds
- [ ] Add volume mounts for database persistence
- [ ] Add logging configuration
- [ ] Create .env.example file

---

## 🐳 Corrected Docker Build Commands

### To build the Docker image (after fixes):

```bash
# Start Docker Desktop first
docker build -t daa-flight-app:latest .

# Or with build arguments
docker build --build-arg PORT=8080 -t daa-flight-app:latest .
```

### To run the container:

```bash
# Basic run
docker run -p 8080:8080 daa-flight-app:latest

# With environment variables
docker run -p 8080:8080 -e PORT=8080 daa-flight-app:latest

# With volume for database persistence
docker run -p 8080:8080 -v $(pwd)/data:/app/data daa-flight-app:latest
```

### To test the container:

```bash
# Check health
curl http://localhost:8080/health

# Check API root
curl http://localhost:8080/

# Check airports
curl http://localhost:8080/api/airports
```

---

## 🚀 Recommended Docker Compose Configuration

Create `docker-compose.yml`:

```yaml
version: '3.8'

services:
  flight-api:
    build:
      context: .
      dockerfile: Dockerfile
    ports:
      - "8080:8080"
    environment:
      - PORT=8080
    volumes:
      - ./data:/app/data
    restart: unless-stopped
    healthcheck:
      test: ["CMD", "curl", "-f", "http://localhost:8080/health"]
      interval: 30s
      timeout: 3s
      retries: 3
      start_period: 5s
```

---

## 📦 Repository Publishing Considerations

### Files to Include:
- ✅ Source code (*.cpp, *.h)
- ✅ CMakeLists.txt
- ✅ Dockerfile
- ✅ .dockerignore
- ✅ .gitignore
- ✅ README.md
- ✅ Documentation files (*.md)
- ⚠️ Sample database (optional, for testing)

### Files to EXCLUDE:
- ❌ build/ directory
- ❌ .vscode/, .idea/ directories
- ❌ Compiled binaries (*.exe, *.dll, *.o)
- ❌ Personal database files with real data
- ❌ .env files with secrets
- ❌ IDE-specific files

### Sensitive Information Check:
- [ ] No hardcoded passwords or API keys
- [ ] No personal data in sample database
- [ ] No absolute file paths
- [ ] No internal IP addresses or hostnames

---

## 🔧 Quick Fix Script

Here's a PowerShell script to apply the critical fixes:

```powershell
# fix-docker-issues.ps1

# Fix 1: Update port in main.cpp
(Get-Content main.cpp) -replace 'int port = 18080;', 'int port = 8080;' | Set-Content main.cpp

# Fix 2: Remove algo.cpp from Dockerfile if not needed
(Get-Content Dockerfile) -replace 'COPY algo.cpp .', '# COPY algo.cpp .' | Set-Content Dockerfile

# Fix 3: Update .dockerignore
@"
# Build artifacts
build/
out/
bin/
cmake-build-debug/
_deps/

# Version control
.git/
.gitignore

# Documentation
*.md
!README.md

# IDE
.vscode/
.idea/
.vs/
*.code-workspace

# CMake artifacts
CMakeCache.txt
cmake_install.cmake
Makefile

# Application directories
backend/
frontend/

# Database and logs
*.json
*.log
*.db

# Environment files
.env
.env.*
"@ | Set-Content .dockerignore

Write-Host "Fixes applied! Review changes before committing."
```

---

## 📊 Summary

| Issue | Severity | Status | Action Required |
|-------|----------|--------|-----------------|
| algo.cpp mismatch | HIGH | ❌ Not Fixed | Remove from Dockerfile or add to CMake |
| Port mismatch | HIGH | ❌ Not Fixed | Change 18080 to 8080 |
| Database file handling | MEDIUM | ❌ Not Fixed | Update Dockerfile |
| Incomplete .dockerignore | MEDIUM | ❌ Not Fixed | Add missing entries |
| Health check | MEDIUM | ⚠️ Works but suboptimal | Improve response format |
| Build optimization | LOW | ⚠️ Optional | Add compiler flags |
| Multi-arch support | LOW | ⚠️ Optional | Use buildx |

---

## 🎯 Next Steps

1. **Apply critical fixes** (Issues #1, #2, #3)
2. **Start Docker Desktop**
3. **Test Docker build**: `docker build -t daa-flight-app:latest .`
4. **Test container run**: `docker run -p 8080:8080 daa-flight-app:latest`
5. **Verify endpoints** work correctly
6. **Update documentation** with deployment instructions
7. **Commit and push** to repository

---

**Analysis completed successfully. Please address the critical issues before publishing.**



# 🚀 C++ Backend API - Setup Guide

This is the C++ Backend for our project. It uses **Crow** (Framework) and **CMake** (Build System).

## 🛠️ 1. Prerequisites (Install these first!)

Before you clone the code, ensure you have the following installed.

### A. VS Code Extensions
Install these two extensions in VS Code:
1.  **C/C++** (by Microsoft)
2.  **CMake Tools** (by Microsoft)

### B. The Compiler (Windows Users - Important!)
You cannot just use MinGW. You need the **Microsoft Visual C++ Compiler**.
1.  Download **[Visual Studio Build Tools](https://visualstudio.microsoft.com/visual-cpp-build-tools/)**.
2.  Run the installer.
3.  **Check the box:** `Desktop development with C++`.
4.  Click **Install** (approx. 2-5GB).
5.  **Restart your computer.**

*(Mac/Linux users: Just install `cmake` and `g++` via terminal).*

---

## 📥 2. Initial Setup

1.  **Clone the Repository:**
    ```bash
    git clone <your-repo-url>
    cd <your-folder-name>
    ```

2.  **Open in VS Code:**
    ```bash
    code .
    ```

3.  **Configure CMake:**
    *   Open VS Code.
    *   Press `Ctrl + Shift + P`.
    *   Type **`CMake: Scan for Kits`** -> Select **Visual Studio Build Tools 2022 Release**.
    *   The output terminal should start doing some work.

---

## 🔨 3. How to Build (Compile)

Since this is C++, we don't use `npm start`. We must compile the code into an `.exe` file.

1.  **The First Build:**
    *   Press **`F7`** (or click the "Build" gear icon in the bottom blue bar).
    *   *Note:* The first time will take 1-3 minutes because it downloads the `Crow` and `JSON` libraries automatically.
    *   **Wait** until you see: `[build] Build finished with exit code 0`.

2.  **Troubleshooting Build Errors:**
    *   If CMake fails, delete the `build/` folder entirely.
    *   Press `Ctrl + Shift + P` -> **`CMake: Delete Cache and Reconfigure`**.

---

## ▶️ 4. How to Run

Do **NOT** use the "Play" button in the top right corner (Code Runner). It will not work.

**Option A: VS Code Interface**
1.  Look at the bottom blue bar.
2.  Click the **Play Icon** (▶️) next to the Build Gear.

**Option B: Terminal**
Run the executable manually:
```powershell
.\build\Debug\server_app.exe
```

---

## 🧪 5. How to Test

The server runs on **Port 18080** (to avoid conflicts with Oracle/System apps).

**1. Check Health (Browser):**
Visit: [http://localhost:18080/health](http://localhost:18080/health)
*   *Expected:* `{"port":18080,"status":"running"}`

**2. Test POST Request (PowerShell):**
```powershell
curl -X POST http://localhost:18080/api/user `
     -H "Content-Type: application/json" `
     -d '{"username": "Teammate", "age": 21}'
```

---

## 📂 Project Structure

*   `main.cpp` - The entry point and API Routes.
*   `CMakeLists.txt` - The build configuration (Do not touch unless adding libraries).
*   `build/` - (Auto-generated) Contains the executable. **Do not push this to Git.**
*   `.gitignore` - Ensures `build/` files are ignored.
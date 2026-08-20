# Locale: en-us (English)
# MSG_ID format: ST_{Level}{Component}{Number}
#   Level    = I | D | W | E (Info / (Doc/Debug) / Warning / Error)
#   Component= full component name (Functions, CommonSettings, Node, Dependencies,
#                         Pybind11, Fbjni, ...) — no single-letter abbreviations
#   Number   = two-digit decimal sequence
# --------------------------------------------------------------

# CMakeLists.txt

## code/CMakeLists.txt
set(ST_DC0 "Enable ccache/sccache")
set(ST_DC1 "Enable parallel builds")
set(ST_DC2 "Enable artifact caching for components")
set(ST_DC3 "Enable third-party dependency caching")

# ---- Functions ----
set(ST_EFunctions0 "The source code directory '{0}' does not exist")
set(ST_EFunctions1 "Parent '{0}' of '{1}' not found in feature paths")
set(ST_EFunctions2 "No root found in feature paths")
set(ST_IFunctions3 "Target '{0}' cache hit (hash '{1}')")
set(ST_IFunctions4 "Target '{0}' cache miss, building from source")
set(ST_DFunctions5 "Update cache for '{0}' to '{1}'")
set(ST_EFunctions6 "File '{0}' does not exist")

# ---- CommonSettings ----
set(ST_ICommonSettings0 "Using '{0}'")

# ---- Node (FindNode + FindNodeAddonApi) ----
# Doc strings (D prefix) — option / cache variable docs
set(ST_DNode0 "Node.js executable")
set(ST_DNode1 "Node.js executable not found")
set(ST_DNode2 "Failed to get Node.js version")
set(ST_DNode3 "Automatically install missing Node.js development files and node-addon-api")
set(ST_DNode8 "Node.js headers not found after auto-install attempt.")
set(ST_DNode9 "Node.js import library not found (Windows)")
set(ST_DNode18 "Package manager command")
set(ST_DNode19 "Install command for package manager")
set(ST_DNode20 "Exec command for package manager")
set(ST_DNode21 "DLX command for package manager")
set(ST_DNode22 "Whether package manager is pnpm")
set(ST_DNode23 "Package manager lookup done")
set(ST_DNode24 "Package manager save mode command")
set(ST_DNode25 "Cached node-addon-api include dir")
# Info strings (I prefix) — st_l_info / st_l_info_fmt
set(ST_INode4 "Node.js headers not found in cache. Trying to install via node-gyp...")
set(ST_INode7 "node-gyp install completed successfully.")
set(ST_INode10 "node-addon-api not found. Trying to install...")
set(ST_INode13 "FindNode: Version '{0}'")
set(ST_INode14 "FindNode: Include dirs '{0}'")
set(ST_INode15 "FindNode: Library '{0}'")
set(ST_INode16 "FindNode: node-addon-api found at '{0}'")
set(ST_INode17 "FindNode: node-addon-api not found (optional)")
# Warning strings (W prefix) — st_l_warning / st_l_warning_fmt
set(ST_WNode5 "Neither pnpm nor npm found. Cannot auto-install dependencies.")
set(ST_WNode6 "'node-gyp install' failed (error: {0}). You may need to install manually.")
set(ST_WNode11 "Failed to install node-addon-api (error: {0}). Proceeding without it.")
set(ST_WNode12 "No package manager available to install node-addon-api.")

# ---- Dependencies ----
set(ST_EDependencies0 "ensure_package requires NAME, REPO, TAG, INSTALL_PREFIX")
set(ST_IDependencies1 "Found '{0}' (system or {1})")
set(ST_IDependencies2 "'{0}' not found. Fetching from '{1}'...")

# ---- Pybind11 ----
# Doc strings (D prefix)
set(ST_DPybind110 "Automatically install pybind11 via pip if not found")
set(ST_DPybind112 "Path to pybind11 CMake config")
set(ST_DPybind1112 "pybind11 include dirs")
# Info strings (I prefix)
set(ST_IPybind111 "FindPybind11: Attempting to detect pybind11 via Python...")
set(ST_IPybind113 "FindPybind11: Auto-detecting pybind11_DIR = '{0}'")
set(ST_IPybind114 "FindPybind11: Auto-detecting failed (result='{0}', error='{1}')")
set(ST_IPybind115 "FindPybind11: pybind11 not found, attempting to install via pip...")
set(ST_IPybind116 "FindPybind11: pip install succeeded, re-detecting...")
set(ST_IPybind117 "FindPybind11: Auto-detecting pybind11_DIR after install = '{0}'")
set(ST_IPybind118 "FindPybind11: After installation, still failed to get pybind11 CMake dir.")
set(ST_IPybind119 "FindPybind11: pip install failed (error: {0}). Please install pybind11 manually.")
set(ST_IPybind1110 "FindPybind11: pybind11 not found. Please install it via `pip install pybind11` or set pybind11_DIR.")
set(ST_IPybind1111 "FindPybind11: Target pybind11::pybind11 is missing. Check your pybind11 installation.")
set(ST_IPybind1113 "FindPybind11: Python Executable '{0}'")
set(ST_IPybind1114 "FindPybind11: Pybind11 DIR '{0}'")
set(ST_IPybind1115 "FindPybind11: Include dirs '{0}'")
set(ST_IPybind1116 "FindPybind11: Target 'pybind11::pybind11' automatically provides Python headers and libraries.")

# ---- Fbjni ----
# Doc strings (D prefix)
set(ST_DFbjni0 "JDK root")
set(ST_DFbjni1 "JDK installation root")
set(ST_DFbjni3 "Automatically download and build fbjni if not found.")
set(ST_DFbjni4 "Git tag for fbjni auto-install")
set(ST_DFbjni5 "Install prefix for auto-built fbjni")
set(ST_DFbjni6 "fbjni source directory path")
# Info strings (I prefix)
set(ST_IFbjni0 "FindFbjni: Found existing fbjni source at '{0}' (skip download)")
set(ST_IFbjni1 "FindFbjni: fbjni source not found, downloading from {0}...")
set(ST_IFbjni2 "FindFbjni: Patching fbjni CMakeLists.txt (SHARED -> STATIC, removing PDB install rules)")
set(ST_IFbjni3 "FindFbjni: fbjni built successfully (static library)")
set(ST_IFbjni4 "FindFbjni: fbjni_INCLUDE_DIRS = '{0}'")
set(ST_IFbjni5 "FindFbjni: fbjni_LIBRARIES = '{0}'")
set(ST_IFbjni6 "FindFbjni: Version = '{0}'")
set(ST_IFbjni7 "FindFbjni: fbjni not found. To enable automatic download, set -DFBJNI_AUTO_INSTALL=ON")
# Warning strings (W prefix)
set(ST_WFbjni2 "JAVA_HOME not found. fbjni may fail to build.")
set(ST_WFbjni3 "FindFbjni: Failed to create target fbjni.")

# ---- GoogleTest ----
# Doc strings (D prefix)
set(ST_DGoogleTest0 "Automatically download and build GoogleTest if not found.")
set(ST_DGoogleTest1 "Git tag for GoogleTest auto-install")
set(ST_DGoogleTest2 "Install prefix for auto-built GoogleTest")
set(ST_DGoogleTest3 "GoogleTest source directory path")
# Info strings (I prefix)
set(ST_IGoogleTest0 "FindGoogleTest: Found existing GoogleTest source at '{0}' (skip download)")
set(ST_IGoogleTest1 "FindGoogleTest: GoogleTest source not found, downloading from {0}...")
set(ST_IGoogleTest2 "FindGoogleTest: GoogleTest built successfully")
set(ST_IGoogleTest3 "FindGoogleTest: GoogleTest not found. To enable automatic download, set -DGOOGLETEST_AUTO_INSTALL=ON")
set(ST_IGoogleTest4 "FindGoogleTest: GoogleTest_INCLUDE_DIRS = '{0}'")
set(ST_IGoogleTest5 "FindGoogleTest: GoogleTest_LIBRARIES = '{0}'")
set(ST_IGoogleTest6 "FindGoogleTest: Version = '{0}'")
# Warning strings (W prefix)
set(ST_WGoogleTest0 "FindGoogleTest: Failed to create target gtest.")

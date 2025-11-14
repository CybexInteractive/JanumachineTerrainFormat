# 🧾Changelog
All notable changes to this project will be dosumented in this file.

## 🚧 Unreleased
Work in progress

**Added**  
**Fixed**  
**Changed**  
**Deprecated**  
**Removed**  
**Security**  

## ⭐ [JTF 1.0.0](https://github.com/CybexInteractive/JanumachineTerrainFormat/releases/tag/v1.0.0) ─ 14-11-2025
Initial stable release of **JTF ─ Janumachine Terrain Format**.

**Implemented**
- **C++ core library** with reading/writing support.
- **C API** `jtf_c_api.h` exposing:
  - `Create()`
  - `Read()`
  - `Write()`
  - `GetVersion()`
  - `Destroy()`
  - Return value `Log`, status reporting structure.

**Added**
- **Logging system** with consistent error codes.
- **File header format** including:
  - Signature (magic number)
  - Version (major, minor, patch)
  - Dimensions (width, height)
  - Bit depth (sample precision)
  - Bounds (lower, upper)
- **Versioning system** integrated with CMake:
  - `PROJECT_VERSION`
  - auto-generated `jtf_version.h`
  - DLL/SO/DYLIB version metadata
- Very basic **unit test** for native functionality.

**Fixed**

**Changed**

**Deprecated**

**Removed**

**Security**

## ⌛ [JTF 0.1.0](https://youtu.be/dQw4w9WgXcQ?si=pqGzY85-3mrTXyxb&t=43) ─ 04-11-2025
First working prototype.

**Added**
- Basic file structure and serializer/deserializer.

<br>

🛈 The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/) and adheres to [Semantic Versioning](https://semver.org/).

<!--
## 🚧 Unreleased
SUBTITLE

**Implemented**
- Foo
- Bar

**Added**

**Fixed**

**Changed**

**Deprecated**

**Removed**

**Security**
-->
# 🧾Changelog
All notable changes to this project will be dosumented in this file.

<!--
## 🚧 Unreleased
Work in progress

**Added**  
**Fixed**  
**Changed**  
**Deprecated**  
**Removed**  
**Security**  
-->

## ⭐ [JTF 1.0.1] Unreleased ─ 15-11-2025
<!--(https://github.com/CybexInteractive/JanumachineTerrainFormat/releases/tag/v1.1.0)-->
Work in progress

**Added**  
**Fixed**  
- Writer writing wrong version (copy paste error, writing minor in place of patch version number).
- Ensuring chunk ids are written (human readable) as little-endian to file.
- Reader not ensuring correct endianness of HEAD chunk payload.
- Reader not ensuring little-endianness of height samples.

**Changed**  
- Log string in jtf_reader.cpp function FileReadError from [JTF Import Error] to [JTF Read Error].

**Deprecated**  
**Removed**  
**Security** 

## ⭐ [JTF 1.0.0](https://github.com/CybexInteractive/JanumachineTerrainFormat/releases/tag/v1.0.0) ─ 15-11-2025
Initial stable release of **JTF ─ Janumachine Terrain Format**.

**Added**
- **C++ core library** with reading/writing support.
- **C API** `jtf_c_api.h` exposing:
  - `Create()`,
  - `Read()`,
  - `Write()`,
  - `GetVersion()`,
  - `Destroy()`,
  - Return value `JTF_Log`, status reporting structure.
- **New `JTF_Log` system** providing:
  - `success` flag,  
  - human-readable error message (stack-allocated, no ownership transfer),  
  - consistent error handling across read/write operations.  
- **File header format** including:
  - Signature (magic number),
  - Version (major, minor, patch),
  - Dimensions (width, height),
  - Bit depth (sample precision),
  - Bounds (lower, upper).
- **Versioning system** integrated with CMake:
  - `PROJECT_VERSION`,
  - auto-generated `jtf_version.h`,
  - DLL/SO/DYLIB version metadata.
- **Binary compatibility guarantees**:
  - fixed-size header,  
  - defined endianness,  
  - stable magic number,  
  - clearly versioned layout.
- Very basic **unit test** for native functionality.

**Fixed**
- Corrected handling of height buffer sizes.
- Fixed rare case where file reads could overrun if metadata was malformed.

**Changed**
- Refactored `sampleCount` from `uint64_t` to `uint32_t` (max 4097 × 4097 fits safely).
- Simplified the internal `JTF` struct (no `std::vector<double>` in the C interface).
- Updated serialization code for:
  - correct ordering of metadata,  
  - deterministic header layout,  
  - stable cross-platform binary representation.
- Reworked error propagation (exceptions in C++ → structured logs in C).
- Unified version definition location (`jtf_version.h.in` → generated header).
- Cleaned up include structure and removed redundant declarations.

## 🗄️ [JTF 0.1.0](https://youtu.be/dQw4w9WgXcQ?si=pqGzY85-3mrTXyxb&t=43) ─ 04-11-2025 (Internal Unreleased Prototype)
First working prototype.

**Added**
- First working prototype of file format and serializer/deserializer.
- Basic read/write functionality without structured logging or version metadata.
- Early C API draft for Unity interop testing.

<br>

🛈 The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/) and adheres to [Semantic Versioning](https://semver.org/).

<!--
## 🚧 Unreleased
SUBTITLE

**Added**
- Foo
- Bar

**Fixed**

**Changed**

**Deprecated**

**Removed**

**Security**
-->

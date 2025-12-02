# 🧾Changelog
All notable changes to this project will be dosumented in this file.

<!--
## 🚧 Unreleased ─ dd-mm-yyyy
Work in progress

**Added**  
**Fixed**  
**Changed**  
**Deprecated**  
**Removed**  
**Security**  
-->

## 🚧 [JTF 1.1.0] Unreleased ─ dd-mm-yyyy
Work in progress

**Added**  
- **C_API** `ReadRequested()` function to read chunks selectively:
    - always reads "HEAD" chunk, holding relevant flags for reading process,
    - no case sensitivity because of native `::toupper` conversion,
    - `bool` flag for skipping file CRC verification.
- `ReadChunks()` selective file chunk readout:
    - requestable by chunk type as string,
    - optional file level `CRC` verification.
- `RequestableChunkNames` compile time array.
- `GetRequestableChunkNamesMap()` map builder.
- `LookupChunkID()` function.

**Fixed**  
- Potentially UB (undefined behaviour) in writing height samples.

**Changed**  
- File `CRC` calculation to encapsulate chunk `CRC`s only:
    - fast to compute / verify,
    - allows chunk skipping / selective reading whilst `CRC` verifying read,
    - allows streaming read,
    - file `CRC` does **not** include:
        - `Signature`,
        - self,
        - chunk payloads,
        - chunk lengths
        - chunk types.
- Layout of `JTF` struct, saparating `Header` and `Heights` data into separate structs.
- Extracted `ReadValidateSignature()`function.

**Deprecated**  
**Removed**  
**Security**  

## ⭐ [JTF 1.0.1](https://github.com/CybexInteractive/JanumachineTerrainFormat/releases/tag/v1.0.1) ─ 01-12-2025
Fix little-endianness, versioning, and minor optimization.

**Fixed**  
- **Reader** not ensuring correct endianness of `HEAD` chunk payload.
- **Reader** not ensuring little-endianness of height samples.
- **Reader** not ensuring little-endianness of `CRC` values.
- **Reader** not ensuring `CRC` values are addet to file `CRC` as is in raw file.
- **Writer** writing wrong version (copy paste error, writing `Minor` in place of `Patch` version number).
- **Writer** not ensuring chunk ids are written as little-endian to file (human readable).
- **Writer** not ensuring little-endianness of chunk length and type, including value added to `CRC`.
- **Writer** not ensuring little-endianness of height samples.
- **Writer** having inconsistent `CRC` handling in `FEND` chunk.
- Remove unnecessary `byteswap` in writer helper function `WriteUInt8_LittleEndian()`.

**Changed**  
- `Log` string in `jtf_reader.cpp` function `FileReadError` from `[JTF Import Error]` to `[JTF Read Error]`.
- **Writer** helper functions return written value so it can consistently be added to `CRC` as little-endian.

## 🗃️ [JTF 1.0.0](https://github.com/CybexInteractive/JanumachineTerrainFormat/releases/tag/v1.0.0) ─ 15-11-2025
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

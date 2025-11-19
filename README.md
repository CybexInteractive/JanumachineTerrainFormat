**SPDX:** `LicenseRef-JTF-Spec AND MIT`

# Janumachine Terrain Format
    
<a href="https://github.com/CybexInteractive/JanumachineTerrainFormat/tree/Development?tab=License-1-ov-file" alt="MIT License"><img src="https://img.shields.io/badge/JTF%20Code-MIT-2ECC71"/></a>
<a href="https://github.com/CybexInteractive/JanumachineTerrainFormat/tree/Development?tab=License-1-ov-file" alt="JTF spec custom License"><img src="https://img.shields.io/badge/JTF%20Spec-Custom%20License-%23A020F0"/></a>
![SPDX](https://img.shields.io/badge/SPDX-LicenseRef--JTF--Spec%20AND%20MIT-blue)
<br><br>
Compact binary terrain heightmap format for 3D applications and game engines.  

## ⭐ Key Features
▹ Magic number signature for guaranteed binary identity  
▹ Fixed-size header with versioning  
▹ Normalized height storage <code>(<span style="color: #abc8a8;">0.0</span> - <span style="color: #abc8a8;">1.0</span>)</code>  
▹ Integer bounds (floor / ceiling)  
▹ Supports <code>32-bit <span style="color: #5798d9;">float</span></code> or <code>64-bit <span style="color: #5798d9;">double</span></code> height sample precision  
▹ Row-major grid data  
▹ CRC-32 validation per chunk and full file CRC  

## 📄 File Overview
╔═════════════╗  
║&emsp; Signature &emsp;&emsp;&emsp;&nbsp;&thinsp;║ &emsp;(8 bytes)  
╟─────────────╢  
║&emsp; HEAD Chunk &emsp;&emsp13;&emsp14;&emsp14;&thinsp;║ &emsp;Header with metadata  
╟─────────────╢  
║&emsp; HMAP Chunk &emsp;&emsp13;&emsp14;&thinsp;║ &emsp;Height samples  
╟─────────────╢  
║&emsp; FEND Chunk &emsp;&emsp;&hairsp;║ &emsp;File end marker  
╟─────────────╢  
║&emsp; File CRC-32 &emsp;&emsp;&emsp14;&puncsp;&hairsp;║ &emsp;Whole-file checksum  
╚═════════════╝  

### 🔄 Endianness
<table>
  <tr>
    <td>Signature</td>
    <td>Big-endian</td>
  </tr>
  <tr>
    <td>Chunk IDs (Typename)</td>
    <td>Big-endian</td>
  </tr>
  <tr>
    <td>all integers</td>
    <td>Little-endian</td>
  </tr>
  <tr>
    <td>float/double samples&emsp;</td>
    <td>Little-endian&emsp;&emsp;</td>
  </tr>
</table>

### 📏 Limits
| Parameter | Value |
| :--- | :--- |
| Max Width/Height | <code><span style="color: #abc8a8;">4097</span></code> |
| Allowed bitDepth | <code><span style="color: #abc8a8;">32</span></code> , <code><span style="color: #abc8a8;">64</span></code> |
| Reserved bytes | Must be <code><span style="color: #abc8a8;">0</span></code> until future spec |


> [!NOTE] 
> __<ins> Why <span style="color: #abc8a8;">4097</span>?</ins>__  
> An (unreasonable but less limiting) <code><span style="color: #abc8a8;">4096</span></code> grid cells plus <code><span style="color: #abc8a8;">1</span></code> extra point per axis for height-fiels mesh topology.

### 📐 Row-Major Ordering
Sample index: <code><span style="color: #9cdcfe;">index</span> = <span style="color: #9cdcfe;">y</span> * <span style="color: #9cdcfe;">width</span> + <span style="color: #9cdcfe;">x</span></code>  
Origin: **Bottom-Left**&hairsp; <code>(<span style="color: #9cdcfe;">x</span>, <span style="color: #9cdcfe;">y</span>) = (<span style="color: #abc8a8;">0</span>, <span style="color: #abc8a8;">0</span>)</code>  

### ✅ CRC Definition
All CRCs are **CRC-32 IEEE**.  

| CRC | Covers |
| :--- | :--- |
| Chunk CRC | Chunk type & payload |
| File CRC | Entire file up to and including FEND CRC |

## 🚨 File Invalid Conditions
A loader **must reject** the file if **any** of the following conditions is true:
- Signature mismatch
- Unknown major version (>1)
- Chunk CRC mismatch
- File CRC mismatch
- BitDepth **not** <code><span style="color: #abc8a8;">32</span></code> or <code><span style="color: #abc8a8;">64</span></code>
- Non-zero reserved bytes

### ⌛ Future Extension Plan (v2.0+)
Reserved header bytes are/may be intended for:
- Compression flag
- Streaming support
- Metadata blocks (seed, biome, etc.)
- Additional data channels (normals, splat map, vegetation mask, etc.)

## 🔎 File Details

### ✨ Signature (Magic Number)
ASCII <code><span style="color: #abc8a8;">0x8A 4A 54 46 0D 0A 1B 0A</span></code> (big-endian)  
String form &hairsp;<code><span style="color: #bfbf00;">"\x8AJTF\r\n\x1B\n"</span></code>

| Byte | Meaning |
| ---: | :--- |
| <code><span style="color: #abc8a8;">8A</span></code>| Non-ASCII start byte, ensures binary identity. |
| <code><span style="color: #abc8a8;">4A 54 46</span></code> | <code><span style="color: #bfbf00;">"JTF"</span></code> |
| <code><span style="color: #abc8a8;">0D 0A</span></code><br>&emsp; | <code><span style="color: orange;">CR</span></code> <code><span style="color: orange;">LF</span></code> - Carriage return & Line feed,<br>&emsp;&emsp;&emsp;&emsp;&ensp;standard line ending pair helps detect text mode corruption. |
| <code><span style="color: #abc8a8;">1B</span></code> | Escape character |
| <code><span style="color: #abc8a8;">0A</span></code> | <code><span style="color: orange;">LF</span></code> - Line Feed, a common safe end byte. |

### &#8505; Header Chunk (HEAD)
| Field | Size | Type | Description |
| :--- | ---: | :--- | :--- |
| Chunk Length | 4 | <code><span style="color: #5c9064;">UInt32</span></code> | Always <code><span style="color: #abc8a8;">32</span></code> |
| Chunk Type | 4 | `ASCII` | <code><span style="color: #bfbf00;">"HEAD"</span></code> |
| Version Major | 1 | <code><span style="color: #5798d9;">byte</span></code> ||
| Version Minor | 1 | <code><span style="color: #5798d9;">byte</span></code> ||
| Version Patch | 1 | <code><span style="color: #5798d9;">byte</span></code> ||
| Width | 2 | <code><span style="color: #5c9064;">UInt16</span></code> | Grid width (limited to <code><span style="color: #abc8a8;">4097</span></code>) |
| Height | 2 | <code><span style="color: #5c9064;">UInt16</span></code> | Grid height (limited to <code><span style="color: #abc8a8;">4097</span></code>) |
| Bit Depth | 1 | <code><span style="color: #5798d9;">byte</span></code> | Bits per Sample (<code><span style="color: #abc8a8;">32</span></code> = <code><span style="color: #5798d9;">float</span></code>, <code><span style="color: #abc8a8;">64</span></code> = <code><span style="color: #5798d9;">double</span></code>) |
| Reserved | 8 | <code><span style="color: #5c9064;">UInt64</span></code> | Padding / unused / reserved for future use. Must be zero.|
| Bounds Lower | 4 | <code><span style="color: #5c9064;">Int32</span></code> | Floor of lowest elevation. |
| Bounds Upper | 4 | <code><span style="color: #5c9064;">Int32</span></code> | Ceiling of highest elevation. |
| Reserved | 8 | <code><span style="color: #5c9064;">UInt64</span></code> | Padding / unused / reserved for future use. Must be zero.|
| CRC-32 | 4 | <code><span style="color: #5c9064;">UInt32</span></code> | CRC for HEAD chunk, includes chunk type & data.|


### 🌄 Height Map Chunk (HMAP)
Height data byte count: <code><span style="color: #9cdcfe;">n</span> = <span style="color: #9cdcfe;">width</span> * <span style="color: #9cdcfe;">height</span> * (<span style="color: #9cdcfe;">bitDepth</span> / <span style="color: #abc8a8;">8</span>)</code>

<table>
  <tr>
    <td>Height data sample format:</td>
    <td><code>32-bit</code></td>
    <td><code><span style="color: #5798d9;">float</span></code></td>
    <td>IEEE 754 little-endian</td>
  </tr>
  <tr>
    <td></td>
    <td><code>64-bit</code></td>
    <td><code><span style="color: #5798d9;">double</span></code></td>
    <td>IEEE 754 little-endian</td>
  </tr>
</table>

<table>
  <tr>
    <td>Height value range:</td>
    <td><code>BoundsLower</code></td>
    <td><code><span style="color: #abc8a8;">0.0</span></code></td>
  </tr>
  <tr>
    <td></td>
    <td><code>BoundsUpper</code></td>
    <td><code><span style="color: #abc8a8;">1.0</span></code></td>
  </tr>
</table>

The intended way to reconstruct real height is: <code><span style="color: #5798d9;">var</span> <span style="color: #9cdcfe;">height</span> = BoundsLower + <span style="color: #9cdcfe;">sample</span> * (BoundsUpper - BoundsLower);</code>

> [!IMPORTANT]
> The file format does not enforce samples to be within the bounds, 
> this is ensured through intermediate utility methods and can be ignored entirely / hacked to achieve 
> out of bounds results by writing a custom utility method as plugin for the respective software.
<!--
`❗` The file format does not enforce samples to be within the bounds, 
this is ensured through intermediate utility methods and can be ignored entirely / hacked to achieve 
out of bounds results by writing a custom utility method as plugin for the respective software.
-->

| Field | Size | Type | Description |
| :--- | ---: | :--- | :--- |
| Chunk Length | 4 | <code><span style="color: #5c9064;">UInt32</span></code> | Number of payload bytes |
| Chunk Type | 4 | `ASCII` | <code><span style="color: #bfbf00;">"HMAP"</span></code> |
| Height Data | <code><span style="color: #9cdcfe;">n</span></code> | <code><span style="color: #5798d9;">byte</span>[]</code> | Heights ordered in row-major order. |
| CRC-32 | 4 | <code><span style="color: #5c9064;">UInt32</span></code> | CRC for HMAP chunk, includes chunk type & data.|

### 🛑 File End Chunk (FEND)
As file end marker a consistent block is used.

| Field | Size | Type | Description |
| :--- | ---: | :--- | :--- |
| Chunk Length | 4 | <code><span style="color: #5c9064;">UInt32</span></code> | Always <code><span style="color: #abc8a8;">0</span></code> |
| Chunk Type | 4 | `ASCII` | <code><span style="color: #bfbf00;">"FEND"</span></code> |
| CRC-32 | 4 | <code><span style="color: #5c9064;">UInt32</span></code> | CRC for <code><span style="color: #bfbf00;">"FEND"</span></code> |

### 🛡️ File CRC-32
After the `FEND` chunk comes one final CRC-32 of the entire file up to and including `FEND` CRC.

## <span style="color: green;">&lt;/&gt;</span> Implementation

<table>
  <tr>
    <td>Write to file</td>
    <td><code><span style="color: #4ec9b0;">JanumachineTerrainFormat</span>.<span style="color: #dcdcaa;">WriteToJTF</span>(<span style="color: #bfbf00;">"...\filename.jtf"</span>, <span style="color: #9cdcfe;">width</span>, <span style="color: #9cdcfe;">height</span>, <span style="color: #9cdcfe;">min</span>, <span style="color: #9cdcfe;">max</span>, <span style="color: #9cdcfe;">heights</span>);</code></td>
  </tr>
  <tr>
    <td>Read from file</td>
    <td><code><span style="color: #5798d9;">JTF</span><span style="color: #9cdcfe;">data</span> = <span style="color: #4ec9b0;">JanumachineTerrainFormat</span>.<span style="color: #dcdcaa;">ReadJTF</span>(<span style="color: #bfbf00;">"...\filename.jtf"</span>);</code></td>
  </tr>
</table>


## <span style="color: crimson;">(§)</span> License
© <span style="color: gold;">2025</span> Cybex Interactive & Matthias Simon Gut (aka <span style="color: khaki;">Cybex</span>)  
&emsp;&emsp;&emsp;&emsp13;&emsp14;All rights reserved.  
&emsp;&emsp;&emsp;&emsp13;&emsp14;Use is granted under the terms specified in the accompanying LICENSE.md file.  

This file format specification is <span style="color: lime;">royalty-free and open</span> to implement and use in <span style="color: lime;">commercial</span> and <span style="color: lime;">non-commercial</span> applications, tools, plugins, engines, and pipelines.  

Attribution example: **<span style="color: green;">"Supports JTF - © Cybex Interactive & Matthias Simon Gut"</span>**  
More examples see: [ATTRIBUTION.md](docs/ATTRIBUTION.md)

### Reference Libraries / Code (.dll, scripts, etc.)
The provided reference libraries are licensed under the MIT license (see [LICENSE.md](LICENSE.md) for details) and may be used, linked, embedded, modified, and redistributed under those terms.

### JTF Specification / Documentation
The JTF (Janumachine Terrain Format) specification is royalty-free for both commercial and non-commercial use in applications, tools, plugins, and pipelines.  
Attribution is required when distributing software, derivatives, plugins, tools, etc. using this format (see [LICENSE.md](LICENSE.md) for details).

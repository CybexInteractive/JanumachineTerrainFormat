// MIT License
// © 2025 Cybex Interactive & Matthias Simon Gut (aka Cybex)
// See LICENSE.md for full license text (https://raw.githubusercontent.com/CybexInteractive/JanumachineTerrainFormat/main/LICENSE.md).

#pragma once

#include "jtf_version.h"
#include "jtf_types.h"
#include "jtf_crc32.h"
#include <string>
#include <fstream>
#include <bit>
#include <unordered_map>

namespace cybex_interactive::jtf
{
	constexpr uint64_t JTF_SIGNATURE = 0x8A4A54460D0A1B0A;

	constexpr uint32_t MAP_AXIS_SIZE_LIMIT = 4097;

	// ensure chunk IDs are built big-endian
	constexpr inline uint32_t BuildChunkID_LittleEndian(char a, char b, char c, char d) noexcept
	{
		return
			(static_cast<uint32_t>(a)) |
			(static_cast<uint32_t>(b) << 8) |
			(static_cast<uint32_t>(c) << 16) |
			(static_cast<uint32_t>(d) << 24);
	}

	constexpr uint32_t CHUNK_ID_HEAD = BuildChunkID_LittleEndian('H','E','A','D');
	constexpr uint32_t CHUNK_ID_HMAP = BuildChunkID_LittleEndian('H','M','A','P');
	constexpr uint32_t CHUNK_ID_FEND = BuildChunkID_LittleEndian('F','E','N','D');

	constexpr inline std::string DecodeChunkID(uint32_t chunkID) noexcept
	{
		char name[5] = {
			static_cast<char>(chunkID & 0xFF),
			static_cast<char>((chunkID >> 8) & 0xFF),
			static_cast<char>((chunkID >> 16) & 0xFF),
			static_cast<char>((chunkID >> 24) & 0xFF),
			'\0' // null terminator
		};
		return std::string(name);
	}

	//constexpr std::unordered_map<std::string, uint32_t> RequestableChunkNames = {
	//	{"HEAD", CHUNK_ID_HEAD},
	//	{"HMAP", CHUNK_ID_HMAP},

	//	{"FEND", CHUNK_ID_FEND}
	//};


	class JTFFile
	{
	public:
		/// <summary>Write to .jtf file.</summary>
		/// <param name="path">File path.</param>
		/// <param name="width">Terrain width. Max value = 4097.</param>
		/// <param name="height">Terrain height. Max value = 4097.</param>
		/// <param name="boundsLower">Lowest Elevation floored to next lesser int32_t.</param>
		/// <param name="boundsUpper">Highest Elevation ceiled to next greater int32_t.</param>
		/// <param name="heights">Terrain heights stored in row-major order.</param>
		template<typename T> static void Write(const std::string& filePath, uint16_t width, uint16_t height, int32_t boundsLower, int32_t boundsUpper, const std::vector<T>& heights);

		/// <summary>Read terrain data from .jtf file.</summary>
		/// <param name="path">File path.</param>
		/// <returns>Returns JTF data struct.</returns>
		static JTF Read(const std::string& filePath);

		///// <summary>Read specified data from .jtf file.</summary>
		///// <param name="path">File path.</param>
		///// <param name="requestedChunks">Requested chunk names. "HEAD", "HMAP", etc.</param>
		///// <param name="verifyFileCrc">Read all chunk CRCs to verify file CRC.</param>
		///// <returns>Returns JTF data struct with selectively populated chunks.</returns>
		//static JTF Read(const std::string& filePath, const std::vector<std::string>& requestedChunks, bool verifyFileCrc);

	private:
		/// <summary>Write the JTF signature (magic number).</summary>
		/// <param name="file">File</param>
		/// <param name="fileCrc">Computing file CRC reference.</param>
		inline static void WriteSignature(std::ofstream& file);

		/// <summary>Write the header chunk 'HEAD'.</summary>
		/// <param name="file">File</param>
		/// <param name="width">Terrain width.</param>
		/// <param name="height">Terrain height.</param>
		/// <param name="bitDepth">Bit depth: 32 = 32bit single precision, 64 = 64bit double precision.</param>
		/// <param name="boundsLower">Lowest Elevation floored to next lesser int32_t.</param>
		/// <param name="boundsUpper">Highest Elevation ceiled to next greater int32_t.</param>
		/// <param name="fileCrc">Computing file CRC reference.</param>
		inline static void WriteHeadChunk(std::ofstream& file, uint16_t width, uint16_t height, uint8_t bitDepth, int32_t boundsLower, int32_t boundsUpper, Crc32& fileCrc);

		/// <summary>Write the height map chunk 'HMAP'.</summary>
		/// <param name="file">File</param>
		/// <param name="heights">Heights, normalized with bounds as extents.</param>
		/// <param name="fileCrc">Computing file CRC reference.</param>
		template<typename T> inline static void WriteHmapChunk(std::ofstream& file, uint8_t bitDepth, const std::vector<T>& heights, Crc32& fileCrc);

		/// <summary>Write the file end chunk 'FEND'.</summary>
		/// <param name="file">File</param>
		/// <param name="fileCrc">Computing file CRC reference.</param>
		inline static void WriteFendChunk(std::ofstream& file, Crc32& fileCrc);

		/// <summary>Write the file CRC32.</summary>
		/// <param name="file">File</param>
		/// <param name="fileCrc">Computing file CRC reference.</param>
		inline static void WriteFileCrc(std::ofstream& file, Crc32& fileCrc);


		/// <summary>Read the chunk type ASCII.</summary>
		/// <param name="filePath">File path (for exception log purpose).</param>
		/// <param name="file">File</param>
		/// <returns>uint32_t of ASCII</returns>
		inline static uint32_t ReadChunkType(const std::string& filePath, std::ifstream& file);

		/// <summary>Read the head chunk 'HEAD'.</summary>
		/// <param name="filePath">File path (for exception log purpose).</param>
		/// <param name="file">File</param>
		/// <param name="payloadSize">Payload size as written in file.</param>
		/// <param name="fileCrc">Computed file CRC reference.</param>
		/// <param name="jtf">JTF reference.</param>
		inline static void ReadHeadChunk(const std::string& filePath, std::ifstream& file, uint32_t payloadSize, Crc32& fileCrc, JTF& jtf);

		/// <summary>Read the height map chunk 'HMAP'.</summary>
		/// <param name="filePath">File path (for exception log purpose).</param>
		/// <param name="file">File</param>
		/// <param name="payloadSize">Payload size as written in file.</param>
		/// <param name="fileCrc">Computed file CRC reference.</param>
		/// <param name="jtf">JTF reference.</param>
		inline static void ReadHmapChunk(const std::string& filePath, std::ifstream& file, uint32_t payloadSize, Crc32& fileCrc, JTF& jtf);

		/// <summary>Read the file end chunk 'FEND'.</summary>
		/// <param name="filePath">File path (for exception log purpose).</param>
		/// <param name="file">File</param>
		/// <param name="payloadSize">Payload size as written in file.</param>
		/// <param name="fileCrc">Computed file CRC reference.</param>
		inline static void ReadFendChunk(const std::string& filePath, std::ifstream& file, uint32_t payloadSize, Crc32& fileCrc);

		/// <summary>Read the file CRC32.</summary>
		/// <param name="filePath">File path (for exception log purpose).</param>
		/// <param name="file">File</param>
		/// <param name="fileCrc">Computed file CRC reference.</param>
		inline static void ReadFileCrc(const std::string& filePath, std::ifstream& file, Crc32& fileCrc);
	};
}

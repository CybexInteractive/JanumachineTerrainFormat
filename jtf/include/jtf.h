// MIT License
// © 2025 Cybex Interactive & Matthias Simon Gut (aka Cybex)
// See LICENSE.md for full license text (https://raw.githubusercontent.com/CybexInteractive/JanumachineTerrainFormat/main/LICENSE.md).

#pragma once

#include "jtf_export.h"
#include "jtf_types.h"
#include "jtf_crc32.h"
#include <string>

namespace cybex_interactive::jtf
{
	class JTF_API JTFFile
	{
	public:
		/// <summary>Write terrain data to .jtf file.</summary>
		/// <param name="path">File path.</param>
		/// <param name="width">Terrain width. Max value = 8193.</param>
		/// <param name="height">Terrain height. Max value = 8193.</param>
		/// <param name="boundsLower">Lowest Elevation floored to next lesser int32_t.</param>
		/// <param name="boundsUpper">Highest Elevation ceiled to next greater int32_t.</param>
		/// <param name="heights">Terrain heights stored in row-major order.</param>
		template<typename T> static void Write(const std::string& filePath, uint16_t width, uint16_t height, int32_t boundsLower, int32_t boundsUpper, const std::vector<T>& heights);

		/// <summary>Read terrain data from .jtf file.</summary>
		/// <param name="path">File path.</param>
		/// <returns>Returns JTF data struct.</returns>
		static JTF Read(const std::string& filePath);

	private:
		/// <summary>Write the JTF signature (magic number).</summary>
		inline static void WriteSignature(std::ofstream& file, Crc32& fileCrc);

		/// <summary>Write the header chunk 'HEAD'.</summary>
		/// <param name="file">File</param>
		/// <param name="width">Terrain width.</param>
		/// <param name="height">Terrain height.</param>
		/// <param name="bitDepth">Bit depth: 32 = 32bit single precision, 64 = 64bit double precision.</param>
		/// <param name="boundsLower">Lowest Elevation floored to next lesser int32_t.</param>
		/// <param name="boundsUpper">Highest Elevation ceiled to next greater int32_t.</param>
		inline static void WriteHeadChunk(std::ofstream& file, uint16_t width, uint16_t height, uint16_t bitDepth, int32_t boundsLower, int32_t boundsUpper, Crc32& fileCrc);

		/// <summary>Write the height map chunk 'HMAP'.</summary>
		/// <param name="file">File</param>
		/// <param name="heights">Heights, normalized with bounds as extents.</param>
		template<typename T> inline static void WriteHmapChunk(std::ofstream& file, uint16_t bitDepth, const std::vector<T>& heights, Crc32& fileCrc);

		/// <summary>Write the file end chunk ?FEND'.</summary>
		/// <param name="file">File</param>
		inline static void WriteFendChunk(std::ofstream& file, Crc32& fileCrc);

		/// <summary>Write the file CRC32.</summary>
		/// <param name="file">File</param>
		inline static void WriteFileCrc(std::ofstream& file, Crc32& fileCrc);
	};
}

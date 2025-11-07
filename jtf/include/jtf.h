// MIT License
// © 2025 Cybex Interactive & Matthias Simon Gut (aka Cybex)
// See LICENSE.md for full license text (https://raw.githubusercontent.com/CybexInteractive/JanumachineTerrainFormat/refs/heads/develop/LICENSE.md).

#pragma once

#include "jtf_export.h"
#include "jtf_types.h"
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
		static void Write(const std::string& filePath, uint16_t width, uint16_t height, int32_t boundsLower, int32_t boundsUpper, const std::vector<double>& heights);

		/// <summary>Read terrain data from .jtf file.</summary>
		/// <param name="path">File path.</param>
		/// <returns>Returns JTF data struct.</returns>
		static JTF Read(const std::string& filePath);
	};
}

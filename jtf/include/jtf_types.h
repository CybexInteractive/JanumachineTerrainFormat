// MIT License
// © 2025 Cybex Interactive & Matthias Simon Gut (aka Cybex)
// See LICENSE.md for full license text (https://raw.githubusercontent.com/CybexInteractive/JanumachineTerrainFormat/main/LICENSE.md).

#pragma once

#include <cstdint>
#include <vector>

namespace cybex_interactive::jtf
{
	struct JTF_Head
	{
		uint8_t VersionMajor = 0;
		uint8_t VersionMinor = 0;
		uint8_t VersionPatch = 0;

		uint16_t Width = 0;
		uint16_t Height = 0;

		uint8_t BitDepth = 0;

		int32_t BoundsLower = 0;
		int32_t BoundsUpper = 0;
		int32_t BoundsRange() const { return BoundsUpper - BoundsLower; }
	};

	struct JTF_Heights
	{
		std::vector<double> HeightSamples;
	};

	struct JTF
	{
		JTF_Head Header;
		JTF_Heights Heights;
	};
}
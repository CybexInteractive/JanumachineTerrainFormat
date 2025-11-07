// MIT License
// © 2025 Cybex Interactive & Matthias Simon Gut (aka Cybex)
// See LICENSE.md for full license text (https://raw.githubusercontent.com/CybexInteractive/JanumachineTerrainFormat/refs/heads/develop/LICENSE.md).

#include "jtf.h"
#include <fstream>
#include <vector>
#include <cstring>
#include <stdexcept>
#include <format>

#include <iostream>

namespace cybex_interactive::jtf
{
	static std::string fileWriteError(const std::string& filePath, const std::string& message)
	{
		return std::format("[JTF Write Error] '{}' {} => File could not be generated.\n", filePath, message);
	}

	void JTFFile::Write(const std::string& filePath, uint16_t width, uint16_t height, int32_t boundsLower, int32_t boundsUpper, const std::vector<double>& heights)
	{
		std::cout << std::format("Write to {} -> {}, {}, {}, {}, {}\n", filePath, width, height, boundsLower, boundsUpper, heights.capacity());
		std::cout << fileWriteError(filePath, "Just a test WRITE error");
	}
}
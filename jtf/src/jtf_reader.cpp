// MIT License
// © 2025 Cybex Interactive & Matthias Simon Gut (aka Cybex)
// See LICENSE.md for full license text (https://raw.githubusercontent.com/CybexInteractive/JanumachineTerrainFormat/main/LICENSE.md).

#include "jtf.h"
#include <format>

#include <iostream>

namespace cybex_interactive::jtf
{
	static std::string fileReadError(const std::string& filePath, const std::string& message)
	{
		return std::format("[JTF Export Error] '{}' {} => File corrupted or not saved correctly.\n", filePath, message);
	}

	JTF JTFFile::Read(const std::string& filePath)
	{
		std::cout << std::format("Read from {}\n", filePath);
		std::cout << fileReadError(filePath, "Just a test READ error");
		return JTF();
	}
}
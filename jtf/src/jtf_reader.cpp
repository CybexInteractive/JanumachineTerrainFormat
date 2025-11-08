// MIT License
// © 2025 Cybex Interactive & Matthias Simon Gut (aka Cybex)
// See LICENSE.md for full license text (https://raw.githubusercontent.com/CybexInteractive/JanumachineTerrainFormat/main/LICENSE.md).

#include <format>
#include "jtf.h"


namespace cybex_interactive::jtf
{
	static std::string fileReadError(const std::string& filePath, const std::string& message)
	{
		return std::format("[JTF Export Error] '{}' {} => File corrupted or not saved correctly.\n", filePath, message);
	}

	JTF JTFFile::Read(const std::string& filePath)
	{
		return JTF();
	}
}
// MIT License
// © 2025 Cybex Interactive & Matthias Simon Gut (aka Cybex)
// See LICENSE.md for full license text (https://raw.githubusercontent.com/CybexInteractive/JanumachineTerrainFormat/main/LICENSE.md).

#include "jtf.h"
#include <iostream>

int main()
{
    std::cout << "Hello JTF!\n";

	using namespace cybex_interactive::jtf; // shortcut for JTFFile

	std::string filePath = "D:/Users/Matthias/Documents/Throwaway/JTF Tests/CppJTFTest.jtf";
	std::vector<float> heights(16, 1.23f); // example data (4x4 heightmap)
	try
	{
		JTFFile::Write(
			filePath,
			4,       // width
			4,       // height
			-50,     // bounds lower
			150,     // bounds upper
			heights
		);

		std::cout << "JTF write test succeeded.\n";
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << "\n";
	}

	/*try
	{
		JTFFile::Read(filePath);

		std::cout << "JTF read test succeeded.\n";
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << "\n";
	}*/

	return 0;
}

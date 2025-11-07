// JTF_Testing.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "jtf.h"
#include <iostream>

int main()
{
    std::cout << "Hello JTF!\n";

	using namespace cybex_interactive::jtf; // shortcut for JTFFile

	std::vector<double> heights(16, 1.23); // example data (4x4 heightmap)
	try
	{
		JTFFile::Write(
			"test_output.jtf",
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

	try
	{
		JTFFile::Read("test_output.jtf");

		std::cout << "JTF read test succeeded.\n";
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << "\n";
	}

	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

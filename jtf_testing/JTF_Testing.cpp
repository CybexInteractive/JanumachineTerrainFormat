// MIT License
// © 2025 Cybex Interactive & Matthias Simon Gut (aka Cybex)
// See LICENSE.md for full license text (https://raw.githubusercontent.com/CybexInteractive/JanumachineTerrainFormat/main/LICENSE.md).

#include "jtf_c_api.h"
#include <iostream>
#include <format>
#include <filesystem>
#include <vector>

using namespace std;

static string ResultCompare(JTF_Result expected, JTF_Result result)
{
	return expected == result ? "Match" : "Mismatch";
}

static string PrintResult(JTF_Result result)
{
	switch (result)
	{
	case JTF_SUCCESS:
		return "Success";
	case JTF_INVALID_ARGUMENT:
		return "Invalid Argument";
	case JTF_FILE_NOT_FOUND:
		return "File not found";
	case JTF_CRC_MISMATCH:
		return "CRC mismatch";
	case JTF_UNSUPPORTED_FORMAT:
		return "Unsupported format";
	case JTF_EXCEPTION:
		return "Exception";
	default:
		return "Unknown Result";
	}
}

void RunWriteReadTest(const char* filePath, uint16_t width, uint16_t height, int32_t boundsLower, int32_t boundsUpper, const vector<double>& heights,
	JTF_Result expectedWriteResult, JTF_Result expectedReadResult, string testDesciption, string expectedWriteError, string expectedReadError)
{
	if (testDesciption != "") cout << format("Descritption:\t\t {}", testDesciption) << endl << endl;

	cout << format("File path:\t\t {}", filePath) << endl;
	
	cout << format("Expected Result W/R:\t {} / {}", PrintResult(expectedWriteResult), PrintResult(expectedReadResult)) << endl;
	
	if (expectedWriteError == "") expectedWriteError = "None";
	if (expectedReadError == "") expectedReadError = "None";
	cout << format("Expected Error W/R:\t {} / {}", expectedWriteError, expectedReadError) << endl << endl;

	JTF* file = Create();
	if (!file)
	{
		std::cerr << "Failed to create JTF handle." << std::endl;
		return;
	}

	// write test
	JTF_Log writeResult = Write(filePath, width, height, boundsLower, boundsUpper, heights.data(), heights.size());
	cout << format("Write result:\t\t {} {}", ResultCompare(expectedWriteResult, writeResult.result), PrintResult(writeResult.result)) << endl;
	file = nullptr;
	Destroy(file);

	// read test
	JTF_Log readResult = Read(filePath, &file);
	cout << format("Read result:\t\t {} {}", ResultCompare(expectedReadResult, readResult.result), PrintResult(readResult.result)) << endl;
	file = nullptr;
	Destroy(file);

	if (filesystem::exists(filePath)) filesystem::remove(filePath);

	cout << "----------------------------------------------------------------------------------------------------" << endl << endl;
}

int main()
{
	cout << "Testing JTF" << endl << endl;

	cout << "Choose procedure:\n1. Default\n2. Read file at path" << endl;

	cout << "Awaiting input: ";

	int choice;
	cin >> choice;

	// Move cursor up 1 line and clear it
	cout << "\033[1A\033[2K";
	cout << "\033[1A\033[2K";
	cout << "\033[1A\033[2K";
	cout << "\033[1A\033[2K";
	cout << "\033[1A\033[2K";
	cout << "\033[1A\033[2K";

	cout << "\r\rTesting JTF (" << (choice == 1 ? "Default" : "Read file at path") << ")" << endl;

	cout << "----------------------------------------------------------------------------------------------------" << endl << endl;

	string filePath = (filesystem::temp_directory_path() / "CppJTFTest.jtf").string();

	vector<float> heights_float_positive(16, 1.23f); // example data (4x4 value positive float heightmap)
	vector<double> heights_double_negative(32, -0.5); // example data (4x8 value negative double heightmap)

	string testDescription = "Expecting sucessful write and read.";
	JTF_Result expectedWriteResult = JTF_SUCCESS;
	JTF_Result expectedReadResult = JTF_SUCCESS;
	string expectedWriteError = "";
	string expectedReadError = "";



	RunWriteReadTest(filePath.c_str(), 4, 8, -50, 150, heights_double_negative, 
		expectedWriteResult, expectedReadResult, testDescription, expectedWriteError, expectedReadError);



	testDescription =
		"HeightSamples size mismatch. Width 4 Height 8, HeightSamples size 16.";
	expectedWriteError =
		format(R"([JTF Write Error] '{}' heights size mismatch with map size (width * height). => File could not be generated.)", filePath);
	expectedReadError =
		format(R"([JTF Import Error] '{}' Cannot open file for reading. => File corrupted or not saved correctly.)", filePath);


	return 0;
}
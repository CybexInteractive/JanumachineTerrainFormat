// MIT License
// © 2025 Cybex Interactive & Matthias Simon Gut (aka Cybex)
// See LICENSE.md for full license text (https://raw.githubusercontent.com/CybexInteractive/JanumachineTerrainFormat/main/LICENSE.md).

#include "jtf.h"
#include <iostream>
#include <format>
#include <filesystem>

using namespace std;
using namespace cybex_interactive::jtf;

string NormalizeString(string s)
{
	// Remove \r characters and trim spaces
	s.erase(std::remove(s.begin(), s.end(), '\r'), s.end());
	while (!s.empty() && isspace(s.back())) s.pop_back();
	while (!s.empty() && isspace(s.front())) s.erase(s.begin());
	return s;
}

template<typename T> void RunWriteReadTest(string filePath, uint16_t width, uint16_t height, int32_t boundsLower, int32_t boundsUpper, const vector<T> heights,
	string testDesciption, string expectedWriteError, string expectedReadError)
{
	if (testDesciption != "") cout << format("Descritption: {}", testDesciption) << endl << endl;

	cout << format("Path: {}", filePath) << endl;
	cout << "Expected Errors:" << endl;
	if (expectedWriteError == "") expectedWriteError = "None";
	cout << format("Write: {}", expectedWriteError) << endl;
	if (expectedReadError == "") expectedReadError = "None";
	cout << format("Read:  {}", expectedReadError) << endl << endl;

	try
	{
		using value_type = typename decltype(heights)::value_type;

		cout << "> JTF write test." << endl;

		JTF data;
		data.VersionMajor = 1; // will be written by the writer, here just to prevent confusion in the output
		data.VersionMinor = 0; // will be written by the writer, here just to prevent confusion in the output
		data.Width = width;
		data.Height = height;
		data.BitDepth = is_same_v<value_type, float> ? 32 : 64;
		data.BoundsLower = boundsLower;
		data.BoundsUpper = boundsUpper;

		cout << "JTF data:" << endl;
		cout << format("  Version:\t{}.{}", data.VersionMajor, data.VersionMinor) << endl;
		cout << format("  Dimension:\t({}, {})", data.Width, data.Height) << endl;
		cout << format("  Depth:\t{}bit", data.BitDepth) << endl;
		cout << format("  Bounds:\t{}..{} ({})", data.BoundsLower, data.BoundsUpper, data.BoundsRange()) << endl;
		cout << format("  Heights n:\t{}", sizeof(heights)) << endl;

		JTFFile::Write(
			filePath,
			data.Width,
			data.Height,
			data.BoundsLower,
			data.BoundsUpper,
			heights
		);

		cout << "> JTF write success." << endl << endl;
	}
	catch (const exception& e)
	{
		string error = e.what();
		cout << "> JTF write fail." << endl;
		cerr << "Expect : " << expectedWriteError << endl;
		cerr << "Thrown : " << error;
		if (NormalizeString(error) == NormalizeString(expectedWriteError)) cout << "Error result match." << endl << endl;
		else cout << "Error result DO NOT match!" << endl << endl;
	}

	try
	{
		cout << "> JTF read test." << endl;

		JTF data = JTFFile::Read(filePath);

		cout << "JTF data:" << endl;
		cout << format("  Version:\t{}.{}", data.VersionMajor, data.VersionMinor) << endl;
		cout << format("  Dimension:\t({}, {})", data.Width, data.Height) << endl;
		cout << format("  Depth:\t{}bit", data.BitDepth) << endl;
		cout << format("  Bounds:\t{}..{} ({})", data.BoundsLower, data.BoundsUpper, data.BoundsRange()) << endl;
		cout << format("  Heights n:\t{}", sizeof(data.Heights)) << endl;
		cout << "> JTF read sucess." << endl << endl;
	}
	catch (const exception& e)
	{
		string error = e.what();
		cout << "> JTF read fail." << endl;
		cerr << "Expect : " << expectedReadError << endl;
		cerr << "Thrown : " << error;
		if (NormalizeString(error) == NormalizeString(expectedReadError)) cout << "Error result match." << endl << endl;
		else cout << "Error result DO NOT match!" << endl << endl;
	}

	cout << "----------------------------------------------------------------------------------------------------" << endl << endl;
	if (filesystem::exists(filePath)) filesystem::remove(filePath);
}

int main()
{

	cout << "Testing JTF!" << endl;
	cout << "----------------------------------------------------------------------------------------------------" << endl << endl;

	string filePath = filesystem::temp_directory_path().string() + "CppJTFTest.jtf";

	vector<float> heights_fp(16, 1.23f); // example data (4x4 value positive float heightmap)
	vector<double> heights_dn(32, -0.5); // example data (4x8 value negative double heightmap)

	string testDescription = "Expecting sucessful write and read.";
	string expectedWriteError = "";
	string expectedReadError = "";

	RunWriteReadTest(filePath, 4, 4, -50, 150, heights_fp, testDescription, expectedWriteError, expectedReadError);

	RunWriteReadTest(filePath, 4, 8, -50, 150, heights_dn, testDescription, expectedWriteError, expectedReadError);

	testDescription =
		"Heights size mismatch. Width 4 Height 8, Heights size 16.";
	expectedWriteError =
		format(R"([JTF Write Error] '{}' heights size mismatch with map size (width * height). => File could not be generated.)", filePath);
	expectedReadError =
		format(R"([JTF Import Error] '{}' Cannot open file for reading. => File corrupted or not saved correctly.)", filePath);
	RunWriteReadTest(filePath, 4, 8, -50, 150, heights_fp, testDescription, expectedWriteError, expectedReadError);


	return 0;
}
// MIT License
// © 2025 Cybex Interactive & Matthias Simon Gut (aka Cybex)
// See LICENSE.md for full license text (https://raw.githubusercontent.com/CybexInteractive/JanumachineTerrainFormat/main/LICENSE.md).

#include "jtf_c_api.h"
#include "jtf_types.h"
#include "jtf.h"
#include <memory>
#include <string>


struct JTF
{
	cybex_interactive::jtf::JTF data;
};

extern "C"
{
	JTF_API JTF* Create(void)
	{
		return new JTF();
	}

	JTF_API void Destroy(JTF* file)
	{
		delete file;
	}

	JTF_API JTF_Result Write(const char* filePath, uint16_t width, uint16_t height, int32_t boundsLower, int32_t boundsUpper, const double* heights, uint64_t sampleCount)
	{
		if (!filePath || !heights || sampleCount == 0) return JTF_INVALID_ARGUMENT;

		try
		{
			std::vector<double> map(heights, heights + sampleCount);
			cybex_interactive::jtf::JTFFile::Write(std::string(filePath), width, height, boundsLower, boundsUpper, map);
			return JTF_SUCCESS;
		}
		catch (...)
		{
			return JTF_EXCEPTION;
		}
	}

	JTF_API JTF_Result Read(const char* filePath, JTF** out_file)
	{
		if (!filePath || !out_file) return JTF_INVALID_ARGUMENT;

		try
		{
			std::unique_ptr<JTF> file(new JTF());
			file->data = cybex_interactive::jtf::JTFFile::Read(filePath);
			*out_file = file.release();
			return JTF_SUCCESS;
		}
		catch (...)
		{
			return JTF_EXCEPTION;
		}

	}

	JTF_API const char* GetVersion(void)
	{
		return "JTF v1.0";
	}
}
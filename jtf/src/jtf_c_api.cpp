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
	uint8_t VersionMajor = 0;
	uint8_t VersionMinor = 0;

	uint16_t Width = 0;
	uint16_t Height = 0;

	uint16_t BitDepth = 0;

	int32_t BoundsLower = 0;
	int32_t BoundsUpper = 0;

	double* HeightSamples;
	uint32_t HeightSampleCount;
};

extern "C"
{
	JTF_API JTF* Create(void)
	{
		return new JTF();
	}

	JTF_API void Destroy(JTF* file)
	{
		if (!file) return;
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

	JTF_API JTF_Result Read(const char* filePath, JTF** out_data)
	{
		if (!filePath || !out_data) return JTF_INVALID_ARGUMENT;

		try
		{
			std::unique_ptr<JTF> data(new JTF());

			cybex_interactive::jtf::JTF jtf = cybex_interactive::jtf::JTFFile::Read(filePath);

			data->VersionMajor = jtf.VersionMajor;
			data->VersionMinor = jtf.VersionMinor;
			data->Width = jtf.Width;
			data->Height = jtf.Height;
			data->BitDepth = jtf.BitDepth;
			data->BoundsLower = jtf.BoundsLower;
			data->BoundsUpper = jtf.BoundsUpper;

			uint32_t heightSampleCount = static_cast<uint32_t>(jtf.HeightSamples.size());
			data->HeightSampleCount = heightSampleCount;

			if (heightSampleCount > 0)
			{
				data->HeightSamples = new double[heightSampleCount];
				memcpy(data->HeightSamples, jtf.HeightSamples.data(), heightSampleCount * sizeof(double));
			}
			else data->HeightSamples = nullptr;

			*out_data = data.release();
			
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
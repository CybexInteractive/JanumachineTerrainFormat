// MIT License
// © 2025 Cybex Interactive & Matthias Simon Gut (aka Cybex)
// See LICENSE.md for full license text (https://raw.githubusercontent.com/CybexInteractive/JanumachineTerrainFormat/main/LICENSE.md).

#include "jtf_c_api.h"
#include <memory>
#include <string>
#include <format>
#include <cstring>
#include <cstdio>

struct JTF
{
	uint8_t VersionMajor = 0;
	uint8_t VersionMinor = 0;

	uint16_t Width = 0;
	uint16_t Height = 0;

	uint16_t BitDepth = 0;

	int32_t BoundsLower = 0;
	int32_t BoundsUpper = 0;

	double* HeightSamples = nullptr;
	uint32_t HeightSampleCount = 0;
};

static inline JTF_Log BuildLog(JTF_Result result, const char* message)
{
	JTF_Log log{};
	log.result = result;
	//strncpy_s(log.message, message ? message : "", JTF_LOG_MESSAGE_LIMIT); // MSVC‐only (Windows)
	snprintf(log.message, JTF_LOG_MESSAGE_LIMIT, "%s", message ? message : ""); // all platforms
	return log;
}

extern "C"
{
	JTF_API JTF* Create(void)
	{
		return new JTF();
	}

	JTF_API void Destroy(JTF* data)
	{
		if (!data) return;
		delete[] data->HeightSamples;
		delete data;
	}

	JTF_API JTF_Log Write(const char* filePath, uint16_t width, uint16_t height, int32_t boundsLower, int32_t boundsUpper, const double* heightSamples, uint64_t heightSampleCount)
	{
		if (!filePath) return BuildLog(JTF_INVALID_ARGUMENT, "[JTF Write Error] Missing file path. File could not be generated.\n");
		if (!heightSamples) return BuildLog(JTF_INVALID_ARGUMENT, "[JTF Write Error] Missing height samples. File could not be generated.\n");
		if (heightSampleCount == 0) return BuildLog(JTF_INVALID_ARGUMENT, "[JTF Write Error] Invalid height sample count [0]. File could not be generated.\n");

		try
		{
			std::vector<double> map(heightSamples, heightSamples + heightSampleCount);
			cybex_interactive::jtf::JTFFile::Write(std::string(filePath), width, height, boundsLower, boundsUpper, map);
			return BuildLog(JTF_SUCCESS, std::format("[JTF Write] Wrote JTF successfully to '{}'.", filePath).c_str());
		}
		catch (const std::runtime_error& e)
		{
			return BuildLog(JTF_EXCEPTION, e.what());
		}
		catch (...)
		{
			return BuildLog(JTF_EXCEPTION, "[JTF Write Error] Unknown native exception during write. File could not be generated.");
		}
	}

	JTF_API JTF_Log Read(const char* filePath, JTF** out_data)
	{
		if (!filePath) return BuildLog(JTF_INVALID_ARGUMENT, "[JTF Read Error] Missing file path. File could not be read.\n");
		if (!out_data) return BuildLog(JTF_INVALID_ARGUMENT, "[JTF Read Error] Missing out parameter. File could not be read.\n");

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
			
			return BuildLog(JTF_SUCCESS, std::format("[JTF Read] Read JTF successfully from '{}'.", filePath).c_str());
		}
		catch (const std::exception& e)
		{
			return BuildLog(JTF_EXCEPTION, e.what());
		}
		catch (...)
		{
			return BuildLog(JTF_EXCEPTION, "[JTF Read Error] Unknown native exception during read. File could not be read.");
		}
	}

	JTF_API const char* GetVersion(void)
	{
		static thread_local std::string buffer = std::format("v{}.{}.{}", JTF_VERSION_MAJOR, JTF_VERSION_MINOR, JTF_VERSION_PATCH);
		return buffer.c_str();
	}
}
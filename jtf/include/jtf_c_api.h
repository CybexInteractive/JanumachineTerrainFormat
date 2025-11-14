// MIT License
// © 2025 Cybex Interactive & Matthias Simon Gut (aka Cybex)
// See LICENSE.md for full license text (https://raw.githubusercontent.com/CybexInteractive/JanumachineTerrainFormat/main/LICENSE.md).

#pragma once

#include "jtf.h"
#include "jtf_types.h"
#include "jtf_version.h"
#include "jtf_export.h"
#include <cstdint>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

	#define JTF_LOG_MESSAGE_LIMIT 256

	typedef enum JTF_Result
	{
		JTF_SUCCESS = 0,
		JTF_INVALID_ARGUMENT = 1,
		JTF_FILE_NOT_FOUND = 2,
		JTF_CRC_MISMATCH = 3,
		JTF_UNSUPPORTED_FORMAT = 4,
		JTF_EXCEPTION = 100
	} JTF_Result;

	struct JTF_Log
	{
		JTF_Result result;
		char message[JTF_LOG_MESSAGE_LIMIT];
	};

	/// <summary>Opaque handle representing an in memory JTF file.</summary>
	struct JTF;

	/// <summary>Create a new JTF file.</summary>
	/// <returns>Pointer to new JTF file in memory.</returns>
	JTF_API JTF* Create(void);

	/// <summary>Write .jtf file.</summary>
	/// <param name="path">File path.</param>
	/// <param name="file">JTF handle to write.</param>
	/// <returns>JTF_Result code.</returns>
	JTF_API JTF_Log Write(const char* filePath, uint16_t width, uint16_t height, int32_t boundsLower, int32_t boundsUpper, const double* heights, uint64_t sampleCount);

	/// <summary>Read .jtf file.</summary>
	/// <param name="path">File path.</param>
	/// <param name="out_file">Pointer to new JTF handle.</param>
	/// <returns>JTF_Result code.</returns>
	JTF_API JTF_Log Read(const char* filePath, JTF** out_file);

	/// <summary>Destroy a JTF file handle and free memory.</summary>
	JTF_API void Destroy(JTF* file);

	/// <summary>Get version string of the JTF library.</summary>
	JTF_API const char* GetVersion(void);

#ifdef __cplusplus
}
#endif // __cplusplus
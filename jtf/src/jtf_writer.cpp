// MIT License
// © 2025 Cybex Interactive & Matthias Simon Gut (aka Cybex)
// See LICENSE.md for full license text (https://raw.githubusercontent.com/CybexInteractive/JanumachineTerrainFormat/main/LICENSE.md).

#include "jtf.h"
#include "jtf_utility.h"
#include <vector>
#include <cstring>
#include <format>

namespace cybex_interactive::jtf
{
	inline static std::string FileWriteError(const std::string& filePath, const std::string& message)
	{
		return std::format("[JTF Write Error] '{}' {} File could not be generated.\n", filePath, message);
	}


	inline static int32_t WriteInt32_LittleEndian(std::ofstream& file, int32_t value) {
		if constexpr (std::endian::native == std::endian::big)
			value = byteswap(value);
		file.write(reinterpret_cast<const char*>(&value), sizeof(value));
		return value;
	}

	inline static uint8_t WriteUInt8_LittleEndian(std::ofstream& file, uint8_t value) {
		file.write(reinterpret_cast<const char*>(&value), sizeof(value));
		return value;
	}

	inline static uint16_t WriteUInt16_LittleEndian(std::ofstream& file, uint16_t value) {
		if constexpr (std::endian::native == std::endian::big)
			value = byteswap(value);
		file.write(reinterpret_cast<const char*>(&value), sizeof(value));
		return value;
	}

	inline static uint32_t WriteUInt32_LittleEndian(std::ofstream& file, uint32_t value){
		if constexpr (std::endian::native == std::endian::big)
			value = byteswap(value);
		file.write(reinterpret_cast<const char*>(&value), sizeof(value));
		return value;
	}
	
	inline static uint64_t WriteUInt64_LittleEndian(std::ofstream& file, uint64_t value) {
		if constexpr (std::endian::native == std::endian::big)
			value = byteswap(value);
		file.write(reinterpret_cast<const char*>(&value), sizeof(value));
		return value;
	}


	inline static void UInt64_BigEndian(uint64_t value, uint8_t* out)
	{
		for (int i = 7; i >= 0; --i)
		{
			out[i] = static_cast<uint8_t>(value & 0xFF);
			value >>= 8;
		}
	}
	

	template<typename T> void JTFFile::Write(const std::string& filePath, uint16_t width, uint16_t height, int32_t boundsLower, int32_t boundsUpper, const std::vector<T>& heights)
	{
		// type compatibility check
		static_assert(std::is_same_v<T, float> || std::is_same_v<T, double>, "JTF supports only float or double for T.");

		// size constraint check
		if (width > MAP_AXIS_SIZE_LIMIT || height > MAP_AXIS_SIZE_LIMIT)
			throw std::invalid_argument(FileWriteError(filePath, std::format("width [{}] and/or height [{}] exceeds limit of [{}].", width, height, MAP_AXIS_SIZE_LIMIT)));
		if (width == 0 || height == 0)
			throw std::invalid_argument(FileWriteError(filePath, std::format("width [{}] and/or height [{}] subceeds limit of 1.", width, height)));

		// heights to map size check
		if (heights.size() != size_t(width) * size_t(height))
			throw std::invalid_argument(FileWriteError(filePath, "heights size mismatch with map size (width * height)."));

		// file existance check
		std::ofstream file(filePath, std::ios::binary | std::ios::trunc);
		if (!file)
			throw std::runtime_error(FileWriteError(filePath, "Cannot open file for writing."));

		uint8_t bitDepth = std::is_same_v<T, float> ? 32 : 64;

		// heights payload size limit check
		if (heights.size() * (bitDepth / 8) > std::numeric_limits<uint32_t>::max())
			throw std::overflow_error(FileWriteError(filePath, "Payload size exceeds 4 GB limit."));

		Crc32 fileCrc;

		WriteSignature(file, fileCrc);
		WriteHeadChunk(file, width, height, bitDepth, boundsLower, boundsUpper, fileCrc);
		WriteHmapChunk(file, bitDepth, heights, fileCrc);
		WriteFendChunk(file, fileCrc);
		WriteFileCrc(file, fileCrc);
	}

	void JTFFile::WriteSignature(std::ofstream& file, Crc32& fileCrc)
	{
		uint8_t signatureBE[8];
		UInt64_BigEndian(JTF_SIGNATURE, signatureBE);
		file.write(reinterpret_cast<const char*>(signatureBE), sizeof(signatureBE));
	}

	void JTFFile::WriteHeadChunk(std::ofstream& file, uint16_t width, uint16_t height, uint8_t bitDepth, int32_t boundsLower, int32_t boundsUpper, Crc32& fileCrc)
	{
		constexpr uint64_t zero64 = 0;

		// chunk length
		const uint32_t payloadSize = 32;
		WriteUInt32_LittleEndian(file, payloadSize);

		Crc32 chunkCrc;

		// chunk type
		constexpr uint32_t chunkTypeName = CHUNK_ID_HEAD;
		uint32_t written_uint32 = WriteUInt32_LittleEndian(file, chunkTypeName);
		AppendToCrc(reinterpret_cast<const uint8_t*>(&written_uint32), sizeof(written_uint32), { &chunkCrc });

		// version major
		const uint8_t versionMajor = JTF_VERSION_MAJOR;
		uint8_t written_uint8 = WriteUInt8_LittleEndian(file, versionMajor);
		AppendToCrc(reinterpret_cast<const uint8_t*>(&written_uint8), sizeof(written_uint8), { &chunkCrc });
		// version minor
		const uint8_t versionMinor = JTF_VERSION_MINOR;
		written_uint8 = WriteUInt8_LittleEndian(file, versionMinor);
		AppendToCrc(reinterpret_cast<const uint8_t*>(&written_uint8), sizeof(written_uint8), { &chunkCrc });
		// version patch
		const uint8_t versionPatch = JTF_VERSION_PATCH;
		written_uint8 = WriteUInt8_LittleEndian(file, versionPatch);
		AppendToCrc(reinterpret_cast<const uint8_t*>(&written_uint8), sizeof(written_uint8), { &chunkCrc });

		// dimensions
		uint16_t written_uint16 = WriteUInt16_LittleEndian(file, width);
		AppendToCrc(reinterpret_cast<const uint8_t*>(&written_uint16), sizeof(written_uint16), { &chunkCrc });
		written_uint16 = WriteUInt16_LittleEndian(file, height);
		AppendToCrc(reinterpret_cast<const uint8_t*>(&written_uint16), sizeof(written_uint16), { &chunkCrc });

		// bit depth
		written_uint8 = WriteUInt8_LittleEndian(file, bitDepth);
		AppendToCrc(reinterpret_cast<const uint8_t*>(&written_uint8), sizeof(written_uint8), { &chunkCrc });

		// RESERVED 8 BYTES ([8..16] = 0 by default)
		uint64_t written_uint64 = WriteUInt64_LittleEndian(file, zero64);
		AppendToCrc(reinterpret_cast<const uint8_t*>(&written_uint64), sizeof(written_uint64), { &chunkCrc });

		// bounds
		int32_t written_int32 = WriteInt32_LittleEndian(file, boundsLower);
		AppendToCrc(reinterpret_cast<const uint8_t*>(&written_int32), sizeof(written_int32), { &chunkCrc });
		written_int32 = WriteInt32_LittleEndian(file, boundsUpper);
		AppendToCrc(reinterpret_cast<const uint8_t*>(&written_int32), sizeof(written_int32), { &chunkCrc });

		// RESERVED 8 BYTES ([24..32] = 0 by default)
		written_uint64 = WriteUInt64_LittleEndian(file, zero64);
		AppendToCrc(reinterpret_cast<const uint8_t*>(&written_uint64), sizeof(written_uint64), { &chunkCrc });

		// chunk crc
		uint32_t crcValue = chunkCrc.GetCurrentHashAsUInt32();
		written_uint32 = WriteUInt32_LittleEndian(file, crcValue);
		AppendToCrc(reinterpret_cast<const uint8_t*>(&written_uint32), sizeof(written_uint32), { &fileCrc });
	}

	template<typename T> void JTFFile::WriteHmapChunk(std::ofstream& file, uint8_t bitDepth, const std::vector<T>& heights, Crc32& fileCrc)
	{
		// chunk length
		uint32_t sampleSize = bitDepth / 8;
		uint32_t payloadSize = static_cast<uint32_t>(heights.size() * sampleSize); // size limit checked in JTFFile::Write
		WriteUInt32_LittleEndian(file, payloadSize);

		Crc32 chunkCrc;

		// chunk type
		constexpr uint32_t chunkTypeName = CHUNK_ID_HMAP;
		uint32_t written_uint32 = WriteUInt32_LittleEndian(file, chunkTypeName);
		AppendToCrc(reinterpret_cast<const uint8_t*>(&written_uint32), sizeof(written_uint32), { &chunkCrc });

		// height data
		if (std::endian::native == std::endian::big)
		{
			std::vector<uint8_t> encoded(payloadSize);

			if (bitDepth == 32)
			{
				uint32_t* out = reinterpret_cast<uint32_t*>(encoded.data());
				for (size_t i = 0; i < heights.size(); ++i)
				{
					uint32_t v;
					std::memcpy(&v, &heights[i], 4);
					v = byteswap(v);
					out[i] = v;
				}
			}
			else
			{
				uint64_t* out = reinterpret_cast<uint64_t*>(encoded.data());
				for (size_t i = 0; i < heights.size(); ++i)
				{
					uint64_t v;
					std::memcpy(&v, &heights[i], 8);
					v = byteswap(v);
					out[i] = v;
				}
			}

			file.write(reinterpret_cast<const char*>(encoded.data()), payloadSize);
			AppendToCrc(encoded.data(), payloadSize, { &chunkCrc });
		}
		else
		{
			const uint8_t* heightsData = reinterpret_cast<const uint8_t*>(heights.data());
			file.write(reinterpret_cast<const char*>(heightsData), payloadSize);
			AppendToCrc(heightsData, payloadSize, { &chunkCrc });
		}

		// chunk crc
		uint32_t crcValue = chunkCrc.GetCurrentHashAsUInt32();
		written_uint32 = WriteUInt32_LittleEndian(file, crcValue);
		AppendToCrc(reinterpret_cast<const uint8_t*>(&written_uint32), sizeof(written_uint32), { &fileCrc });
	}

	void JTFFile::WriteFendChunk(std::ofstream& file, Crc32& fileCrc)
	{
		// chunk length
		const uint32_t payloadSize = 0;
		WriteUInt32_LittleEndian(file, payloadSize);

		Crc32 chunkCrc;

		// chunk type
		constexpr uint32_t chunkTypeName = CHUNK_ID_FEND;
		uint32_t written_uint32 = WriteUInt32_LittleEndian(file, chunkTypeName);
		AppendToCrc(reinterpret_cast<const uint8_t*>(&written_uint32), sizeof(written_uint32), { &chunkCrc });

		// chunk crc
		uint32_t crcValue = chunkCrc.GetCurrentHashAsUInt32();
		written_uint32 = WriteUInt32_LittleEndian(file, crcValue);
		AppendToCrc(reinterpret_cast<const uint8_t*>(&written_uint32), sizeof(written_uint32), { &fileCrc });
	}

	void JTFFile::WriteFileCrc(std::ofstream& file, Crc32& fileCrc)
	{
		uint32_t crcValue = fileCrc.GetCurrentHashAsUInt32();
		WriteUInt32_LittleEndian(file, crcValue);
	}


	// Explicit template instantiations
	template void JTFFile::Write<float>(const std::string&, uint16_t, uint16_t, int32_t, int32_t, const std::vector<float>&);
	template void JTFFile::Write<double>(const std::string&, uint16_t, uint16_t, int32_t, int32_t, const std::vector<double>&);

}
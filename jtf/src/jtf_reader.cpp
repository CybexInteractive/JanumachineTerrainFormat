// MIT License
// © 2025 Cybex Interactive & Matthias Simon Gut (aka Cybex)
// See LICENSE.md for full license text (https://raw.githubusercontent.com/CybexInteractive/JanumachineTerrainFormat/main/LICENSE.md).

#include "jtf.h"
#include "jtf_utility.h"
#include <cstring>
#include <cstdint>
#include <format>

namespace cybex_interactive::jtf
{
	inline static std::string FileReadError(const std::string& filePath, const std::string& message)
	{
		return std::format("[JTF Read Error] '{}' {} File corrupted or not saved correctly.\n", filePath, message);
	}


	inline static void ReadToBuffer(const std::string& filePath, std::ifstream& file, void* buffer, size_t size)
	{
		file.read(reinterpret_cast<char*>(buffer), size);
		if (file.gcount() != static_cast<std::streamsize>(size))
			throw std::runtime_error(FileReadError(filePath, "Unexpected EOF."));
	}


	inline static int32_t ReadInt32_LittleEndian(const uint8_t* pointer)
	{
		uint32_t raw = (static_cast<uint32_t>(pointer[0]))
					 | (static_cast<uint32_t>(pointer[1]) << 8)
					 | (static_cast<uint32_t>(pointer[2]) << 16)
					 | (static_cast<uint32_t>(pointer[2]) << 24);
		return static_cast<int32_t>(raw);
	}

	inline static uint8_t ReadUInt8_LittleEndian(const uint8_t* pointer)
	{
		return (static_cast<uint8_t>(pointer[0]));
	}

	inline static uint16_t ReadUInt16_LittleEndian(const uint8_t* pointer)
	{
		return (static_cast<uint16_t>(pointer[0]))
			 | (static_cast<uint16_t>(pointer[1]) << 8);
	}

	inline static uint32_t ReadUInt32_LittleEndian(const uint8_t* pointer)
	{
		return (static_cast<uint32_t>(pointer[0]))
			 | (static_cast<uint32_t>(pointer[1]) << 8)
			 | (static_cast<uint32_t>(pointer[2]) << 16)
			 | (static_cast<uint32_t>(pointer[2]) << 24);
	}

	inline static uint64_t ReadUInt64_LittleEndian(const uint8_t* pointer)
	{
		return (static_cast<uint64_t>(pointer[0]))
			 | (static_cast<uint64_t>(pointer[1]) << 8)
			 | (static_cast<uint64_t>(pointer[2]) << 16)
			 | (static_cast<uint64_t>(pointer[2]) << 24)
			 | (static_cast<uint64_t>(pointer[2]) << 32)
			 | (static_cast<uint64_t>(pointer[2]) << 40)
			 | (static_cast<uint64_t>(pointer[2]) << 48)
			 | (static_cast<uint64_t>(pointer[3]) << 56);
	}

	inline static float ReadFloat_LittleEndian(const uint8_t* pointer)
	{
		uint32_t raw = (static_cast<uint32_t>(pointer[0]))
					 | (static_cast<uint32_t>(pointer[1]) << 8)
					 | (static_cast<uint32_t>(pointer[2]) << 16)
					 | (static_cast<uint32_t>(pointer[2]) << 24);
		float value;
		static_assert(sizeof(value) == sizeof(raw));
		std::memcpy(&value, &raw, sizeof(value));
		return value;
	}

	inline static double ReadDouble_LittleEndian(const uint8_t* pointer)
	{
		uint64_t raw = (static_cast<uint64_t>(pointer[0]))
					 | (static_cast<uint64_t>(pointer[1]) << 8)
					 | (static_cast<uint64_t>(pointer[2]) << 16)
					 | (static_cast<uint64_t>(pointer[2]) << 24)
					 | (static_cast<uint64_t>(pointer[2]) << 32)
					 | (static_cast<uint64_t>(pointer[2]) << 40)
					 | (static_cast<uint64_t>(pointer[2]) << 48)
					 | (static_cast<uint64_t>(pointer[3]) << 56);
		double value;
		static_assert(sizeof(value) == sizeof(raw));
		std::memcpy(&value, &raw, sizeof(value));
		return value;
	}


	uint32_t JTFFile::ReadChunkType(const std::string& filePath, std::ifstream& file)
	{
		uint8_t bytes[4];
		ReadToBuffer(filePath, file, bytes, 4);

		uint32_t chunkType =
			(static_cast<uint32_t>(bytes[0])) |
			(static_cast<uint32_t>(bytes[1]) << 8) |
			(static_cast<uint32_t>(bytes[2]) << 16) |
			(static_cast<uint32_t>(bytes[3]) << 24);

		return chunkType;
	}



	inline static bool VerifySignature(const uint8_t* bytes)
	{
		uint64_t signature = JTF_SIGNATURE;
		uint8_t compare[8]{};
		for (int i = 7; i >= 0; --i)
		{
			compare[i] = static_cast<uint8_t>(signature & 0xFF);
			signature >>= 8;
		}
		return std::memcmp(compare, bytes, 8) == 0;
	}


	JTF JTFFile::Read(const std::string& filePath)
	{
		JTF jtf;

		// file existance check
		std::ifstream file(filePath, std::ios::binary);
		if (!file)
			throw std::runtime_error(FileReadError(filePath, "Cannot open file for reading."));

		Crc32 fileCrc;

		// read and verify signature
		uint8_t signature[8];
		ReadToBuffer(filePath, file, signature, 8/*sizeof(signature)*/);
		if (!VerifySignature(signature))
			throw std::runtime_error(FileReadError(filePath, "Invalid file signature."));
		AppendToCrc(signature, sizeof(signature), { &fileCrc });

		// read chunks
		bool fendReached = false;
		while (file && !fendReached)
		{
			// read chunk length
			uint32_t payloadSize{};
			ReadToBuffer(filePath, file, &payloadSize, sizeof(payloadSize));
			AppendToCrc(reinterpret_cast<uint8_t*>(&payloadSize), sizeof(payloadSize), { &fileCrc });

			// read chunk type
			uint32_t chunkType = ReadChunkType(filePath, file);

			// dispatch
			switch (chunkType)
			{
				case CHUNK_ID_BIG_ENDIAN_HEAD:
					ReadHeadChunk(filePath, file, payloadSize, fileCrc, jtf);
					break;

				case CHUNK_ID_BIG_ENDIAN_HMAP:
					ReadHmapChunk(filePath, file, payloadSize, fileCrc, jtf);
					break;

				case CHUNK_ID_BIG_ENDIAN_FEND:
					ReadFendChunk(filePath, file, payloadSize, fileCrc);
					fendReached = true;
					break;

				default:
					throw std::runtime_error(FileReadError(filePath, std::format("Unknown chunk type '{}'.", DecodeChunkID(chunkType))));
			}
		}

		ReadFileCrc(filePath, file, fileCrc);

		return jtf;
	}

	void JTFFile::ReadHeadChunk(const std::string& filePath, std::ifstream& file, uint32_t payloadSize, Crc32& fileCrc, JTF& jtf)
	{
		if (payloadSize != 32)
			throw std::runtime_error(FileReadError(filePath, std::format("Invalid HEAD payload size, expected [32] got [{}].", payloadSize)));

		std::vector<uint8_t> payload(payloadSize);
		ReadToBuffer(filePath, file, payload.data(), payloadSize);

		Crc32 chunkCrc;

		constexpr char expectedChunkTypeName[4] = { 'H','E','A','D' };
		AppendToCrc(reinterpret_cast<const uint8_t*>(expectedChunkTypeName), 4, { &chunkCrc, &fileCrc });
		AppendToCrc(payload.data(), payloadSize, { &chunkCrc, &fileCrc });

		// read expected chunk crc
		uint32_t expectedCrc;
		ReadToBuffer(filePath, file, &expectedCrc, sizeof(expectedCrc));
		AppendToCrc(reinterpret_cast<const uint8_t*>(&expectedCrc), sizeof(expectedCrc), { &fileCrc });

		// crc compare
		uint32_t computedCrc = chunkCrc.GetCurrentHashAsUInt32();
		if (expectedCrc != computedCrc)
			throw std::runtime_error(FileReadError(filePath, "HEAD CRC mismatch."));

		// decode fields
		size_t offset = 0;

		// version major
		jtf.VersionMajor = ReadUInt8_LittleEndian(payload.data() + offset);
		offset++;
		// version minor
		jtf.VersionMinor = ReadUInt8_LittleEndian(payload.data() + offset);
		offset++;
		// version patch
		jtf.VersionPatch = ReadUInt8_LittleEndian(payload.data() + offset);
		offset++;

		// dimensions
		jtf.Width = ReadUInt16_LittleEndian(payload.data() + offset);
		offset += 2;
		jtf.Width = ReadUInt16_LittleEndian(payload.data() + offset);
		offset += 2;

		// bit depth
		jtf.BitDepth = ReadUInt8_LittleEndian(payload.data() + offset);
		offset++;

		// RESERVED 8 BYTES ([8..16] = 0 by default)
		offset += 8;

		// bounds
		jtf.BoundsLower = ReadInt32_LittleEndian(payload.data() + offset);
		offset += 4;
		jtf.BoundsUpper = ReadInt32_LittleEndian(payload.data() + offset);
		offset += 4;

		// RESERVED 8 BYTES ([24..32] = 0 by default)
		offset += 8;
	}

	void JTFFile::ReadHmapChunk(const std::string& filePath, std::ifstream& file, uint32_t payloadSize, Crc32& fileCrc, JTF& jtf)
	{
		Crc32 chunkCrc;

		constexpr char expectedChunkTypeName[4] = { 'H','M','A','P' };
		AppendToCrc(reinterpret_cast<const uint8_t*>(expectedChunkTypeName), 4, { &chunkCrc, &fileCrc });

		std::vector<uint8_t> payload(payloadSize);
		ReadToBuffer(filePath, file, payload.data(), payloadSize);
		AppendToCrc(payload.data(), payloadSize, { &chunkCrc, &fileCrc });

		// read expected chunk crc
		uint32_t expectedCrc;
		ReadToBuffer(filePath, file, &expectedCrc, sizeof(expectedCrc));
		AppendToCrc(reinterpret_cast<const uint8_t*>(&expectedCrc), sizeof(expectedCrc), { &fileCrc });

		// crc compare
		uint32_t computedCrc = chunkCrc.GetCurrentHashAsUInt32();
		if (expectedCrc != computedCrc)
			throw std::runtime_error(FileReadError(filePath, "HMAP CRC mismatch."));

		size_t sampleCount = payloadSize / (jtf.BitDepth / 8);
		jtf.HeightSamples.resize(sampleCount);

		if (jtf.BitDepth == 32)
		{
			for (size_t i = 0; i < sampleCount; ++i)
			{
				const uint8_t* pointer = payload.data() + i * 4;
				jtf.HeightSamples[i] = static_cast<double>(ReadFloat_LittleEndian(pointer));
			}
		}
		else if (jtf.BitDepth == 64)
		{
			for (size_t i = 0; i < sampleCount; ++i)
			{
				const uint8_t* pointer = payload.data() + i * 8;
				jtf.HeightSamples[i] = ReadDouble_LittleEndian(pointer);
			}
		}
		else throw std::runtime_error(FileReadError(filePath, std::format("Unsupported bit depth in HMAP chunk, expected [32] or [64] got [{}].", jtf.BitDepth)));
	}

	void JTFFile::ReadFendChunk(const std::string& filePath, std::ifstream& file, uint32_t payloadSize, Crc32& fileCrc)
	{
		if (payloadSize != 0)
			throw std::runtime_error(FileReadError(filePath, std::format("Invalid FEND payload size, expected [0] got [{}].", payloadSize)));

		Crc32 chunkCrc;

		constexpr char expectedChunkTypeName[4] = { 'F','E','N','D' };
		AppendToCrc(reinterpret_cast<const uint8_t*>(expectedChunkTypeName), 4, { &chunkCrc, &fileCrc });

		// read expected chunk crc
		uint32_t expectedCrc;
		ReadToBuffer(filePath, file, &expectedCrc, sizeof(expectedCrc));
		AppendToCrc(reinterpret_cast<const uint8_t*>(&expectedCrc), sizeof(expectedCrc), { &fileCrc });

		uint32_t computedCrc = chunkCrc.GetCurrentHashAsUInt32();
		if (expectedCrc != computedCrc)
			throw std::runtime_error(FileReadError(filePath, "FEND CRC mismatch."));
	}

	void JTFFile::ReadFileCrc(const std::string& filePath, std::ifstream& file, Crc32& fileCrc)
	{
		uint32_t expectedCrc;
		ReadToBuffer(filePath, file, &expectedCrc, sizeof(expectedCrc));

		uint32_t computedCrc = fileCrc.GetCurrentHashAsUInt32();
		if (expectedCrc != computedCrc)
			throw std::runtime_error(FileReadError(filePath, "File CRC mismatch."));
	}
}
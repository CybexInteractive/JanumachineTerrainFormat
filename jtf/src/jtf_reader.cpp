// MIT License
// © 2025 Cybex Interactive & Matthias Simon Gut (aka Cybex)
// See LICENSE.md for full license text (https://raw.githubusercontent.com/CybexInteractive/JanumachineTerrainFormat/main/LICENSE.md).

#include "jtf.h"
#include "jtf_utility.h"
#include <cstring>
#include <cstdint>
#include <format>
#include <optional>
#include <algorithm>

namespace cybex_interactive::jtf
{
	inline static std::string FileReadError(const std::string& filePath, const std::string& message)
	{
		return std::format("[JTF Read Error] '{}' {} File corrupted or not saved correctly.\n", filePath, message);
	}


	inline static void ReadToBuffer(const std::string& filePath, std::ifstream& file, void* buffer, size_t size)
	{
		file.read(reinterpret_cast<char*>(buffer), size);
		if (!file || static_cast<std::size_t>(file.gcount()) != size)
			throw std::runtime_error(FileReadError(filePath, "Unexpected EOF."));
	}


	inline static int32_t ReadInt32_LittleEndian(const uint8_t* pointer)
	{
		uint32_t raw = (static_cast<uint32_t>(pointer[0]))
					 | (static_cast<uint32_t>(pointer[1]) << 8)
					 | (static_cast<uint32_t>(pointer[2]) << 16)
					 | (static_cast<uint32_t>(pointer[3]) << 24);
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
			 | (static_cast<uint32_t>(pointer[3]) << 24);
	}

	inline static uint64_t ReadUInt64_LittleEndian(const uint8_t* pointer)
	{
		return (static_cast<uint64_t>(pointer[0]))
			 | (static_cast<uint64_t>(pointer[1]) << 8)
			 | (static_cast<uint64_t>(pointer[2]) << 16)
			 | (static_cast<uint64_t>(pointer[3]) << 24)
			 | (static_cast<uint64_t>(pointer[4]) << 32)
			 | (static_cast<uint64_t>(pointer[5]) << 40)
			 | (static_cast<uint64_t>(pointer[6]) << 48)
			 | (static_cast<uint64_t>(pointer[7]) << 56);
	}

	inline static float ReadFloat_LittleEndian(const uint8_t* pointer)
	{
		uint32_t raw = (static_cast<uint32_t>(pointer[0]))
					 | (static_cast<uint32_t>(pointer[1]) << 8)
					 | (static_cast<uint32_t>(pointer[2]) << 16)
					 | (static_cast<uint32_t>(pointer[3]) << 24);
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
					 | (static_cast<uint64_t>(pointer[3]) << 24)
					 | (static_cast<uint64_t>(pointer[4]) << 32)
					 | (static_cast<uint64_t>(pointer[5]) << 40)
					 | (static_cast<uint64_t>(pointer[6]) << 48)
					 | (static_cast<uint64_t>(pointer[7]) << 56);
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


	static const std::unordered_map<std::string, uint32_t>& GetRequestableChunkNamesMap()
	{
		static const std::unordered_map<std::string, uint32_t> map = []()
			{
				std::unordered_map<std::string, uint32_t> m;
				for (const RequestableChunkName& entry : RequestableChunkNames)
					m.emplace(std::string(entry.name), entry.id);
				return m;
			}();
		return map;
	}

	std::optional<uint32_t> LookupChunkID(std::string_view name)
	{
		// convert to uppercase
		std::string key{ name };
		std::transform(key.begin(), key.end(), key.begin(),
			[](unsigned char c) -> char
			{
				return static_cast<char>(std::toupper(c));
			});

		const std::unordered_map<std::string, uint32_t>& map = GetRequestableChunkNamesMap();
		std::unordered_map<std::string, uint32_t>::const_iterator iterator = map.find(std::string(key));
		if (iterator != map.end())
			return iterator->second;
		return std::nullopt;
	}


	JTF JTFFile::Read(const std::string& filePath)
	{
		JTF jtf;

		// file existance check
		std::ifstream file(filePath, std::ios::binary);
		if (!file)
			throw std::runtime_error(FileReadError(filePath, "Cannot open file for reading."));

		ReadValidateSignature(filePath, file);

		// read chunks
		Crc32 fileCrc;
		bool fendReached = false;
		while (file && !fendReached)
		{
			// read chunk length
			uint8_t payloadSizeBytes[4];
			ReadToBuffer(filePath, file, &payloadSizeBytes, sizeof(payloadSizeBytes));
			uint32_t payloadSize = ReadUInt32_LittleEndian(payloadSizeBytes);

			// read chunk type
			uint32_t chunkType = ReadChunkType(filePath, file);

			// dispatch
			switch (chunkType)
			{
				case CHUNK_ID_HEAD:
					ReadHeadChunk(filePath, file, payloadSize, fileCrc, jtf);
					break;

				case CHUNK_ID_HMAP:
					ReadHmapChunk(filePath, file, payloadSize, fileCrc, jtf);
					break;

				case CHUNK_ID_FEND:
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

	JTF JTFFile::Read(const std::string& filePath, const std::vector<std::string>& requestedChunks, bool verifyFileCrc)
	{
		JTF jtf;

		// file existance check
		std::ifstream file(filePath, std::ios::binary);
		if (!file)
			throw std::runtime_error(FileReadError(filePath, "Cannot open file for reading."));

		// analyze requested chunks
		std::vector<uint32_t> requestedChunkIds;
		requestedChunkIds.reserve(requestedChunks.size());
		// we always need the header, holding relevant flags
		requestedChunkIds.push_back(CHUNK_ID_HEAD);
		for (auto& name : requestedChunks)
		{
			std::optional<uint32_t> id = LookupChunkID(name);
			if (!id)
				throw std::runtime_error(FileReadError(filePath, std::format("Requested unsupported chunk name '{}'. Allowed names are: HEAD, HMAP, FEND.", name)));
			if (*id == CHUNK_ID_HEAD)
				continue;
			requestedChunkIds.push_back(*id);
		}

		ReadValidateSignature(filePath, file);

		// read chunks
		Crc32 fileCrc;
		bool fendReached = false;
		size_t chunksRemaining = requestedChunkIds.size();
		while (file && !fendReached)
		{
			// read chunk length
			uint8_t payloadSizeBytes[4];
			ReadToBuffer(filePath, file, &payloadSizeBytes, sizeof(payloadSizeBytes));
			uint32_t payloadSize = ReadUInt32_LittleEndian(payloadSizeBytes);

			// read chunk type
			uint32_t chunkType = ReadChunkType(filePath, file);

			// dispatch
			bool requested = std::find(requestedChunkIds.begin(), requestedChunkIds.end(), chunkType) != requestedChunkIds.end();
			if (requested)
			{
				switch (chunkType)
				{
					case CHUNK_ID_HEAD:
						ReadHeadChunk(filePath, file, payloadSize, fileCrc, jtf);
						break;

					case CHUNK_ID_HMAP:
						ReadHmapChunk(filePath, file, payloadSize, fileCrc, jtf);
						break;

					case CHUNK_ID_FEND:
						ReadFendChunk(filePath, file, payloadSize, fileCrc);
						fendReached = true;
						break;

					default:
						throw std::runtime_error(FileReadError(filePath, std::format("Unknown chunk type '{}'.", DecodeChunkID(chunkType))));
				}

				chunksRemaining--;

				// if not verifying file CRC break out early
				if (chunksRemaining == 0 && !verifyFileCrc) break;
			}
			else
			{
				// skip payload
				if (payloadSize > 0)
				{
					file.seekg(static_cast<std::streamoff>(payloadSize), std::ios::cur);
					if (!file)
						throw std::runtime_error(FileReadError(filePath, "Unexpected EOF while skipping payload."));
				}

				// read expected chunk crc
				uint8_t expectedCrcBytes[4];
				ReadToBuffer(filePath, file, &expectedCrcBytes, sizeof(expectedCrcBytes));
				AppendToCrc(expectedCrcBytes, sizeof(expectedCrcBytes), { &fileCrc });

				if (chunkType == CHUNK_ID_FEND) fendReached = true;
			}
		}

		if (verifyFileCrc) ReadFileCrc(filePath, file, fileCrc);

		return jtf;
	}

	void JTFFile::ReadValidateSignature(const std::string& filePath, std::ifstream& file)
	{
		// read and verify signature
		uint8_t signature[8];
		ReadToBuffer(filePath, file, signature, 8/*sizeof(signature)*/);
		if (!VerifySignature(signature))
			throw std::runtime_error(FileReadError(filePath, "Invalid file signature."));
	}

	void JTFFile::ReadHeadChunk(const std::string& filePath, std::ifstream& file, uint32_t payloadSize, Crc32& fileCrc, JTF& jtf)
	{
		if (payloadSize != 32)
			throw std::runtime_error(FileReadError(filePath, std::format("Invalid HEAD payload size, expected [32] got [{}].", payloadSize)));

		std::vector<uint8_t> payload(payloadSize);
		ReadToBuffer(filePath, file, payload.data(), payloadSize);

		Crc32 chunkCrc;

		constexpr char expectedChunkTypeName[4] = { 'H','E','A','D' };
		AppendToCrc(reinterpret_cast<const uint8_t*>(expectedChunkTypeName), 4, { &chunkCrc });
		AppendToCrc(payload.data(), payloadSize, { &chunkCrc });

		// read expected chunk crc
		uint8_t expectedCrcBytes[4];
		ReadToBuffer(filePath, file, &expectedCrcBytes, sizeof(expectedCrcBytes));
		AppendToCrc(expectedCrcBytes, sizeof(expectedCrcBytes), { &fileCrc });
		uint32_t expectedCrc = ReadUInt32_LittleEndian(expectedCrcBytes);

		// crc compare
		uint32_t computedCrc = chunkCrc.GetCurrentHashAsUInt32();
		if (expectedCrc != computedCrc)
			throw std::runtime_error(FileReadError(filePath, "HEAD CRC mismatch."));

		// decode fields
		size_t offset = 0;

		// version major
		jtf.Header.VersionMajor = ReadUInt8_LittleEndian(payload.data() + offset);
		offset++;
		// version minor
		jtf.Header.VersionMinor = ReadUInt8_LittleEndian(payload.data() + offset);
		offset++;
		// version patch
		jtf.Header.VersionPatch = ReadUInt8_LittleEndian(payload.data() + offset);
		offset++;

		// dimensions
		jtf.Header.Width = ReadUInt16_LittleEndian(payload.data() + offset);
		offset += 2;
		jtf.Header.Height = ReadUInt16_LittleEndian(payload.data() + offset);
		offset += 2;

		// bit depth
		jtf.Header.BitDepth = ReadUInt8_LittleEndian(payload.data() + offset);
		offset++;

		// RESERVED 8 BYTES ([8..16] = 0 by default)
		offset += 8;

		// bounds
		jtf.Header.BoundsLower = ReadInt32_LittleEndian(payload.data() + offset);
		offset += 4;
		jtf.Header.BoundsUpper = ReadInt32_LittleEndian(payload.data() + offset);
		offset += 4;

		// RESERVED 8 BYTES ([24..32] = 0 by default)
		offset += 8;
	}

	void JTFFile::ReadHmapChunk(const std::string& filePath, std::ifstream& file, uint32_t payloadSize, Crc32& fileCrc, JTF& jtf)
	{
		Crc32 chunkCrc;

		constexpr char expectedChunkTypeName[4] = { 'H','M','A','P' };
		AppendToCrc(reinterpret_cast<const uint8_t*>(expectedChunkTypeName), 4, { &chunkCrc });

		std::vector<uint8_t> payload(payloadSize);
		ReadToBuffer(filePath, file, payload.data(), payloadSize);
		AppendToCrc(payload.data(), payloadSize, { &chunkCrc });

		// read expected chunk crc
		uint8_t expectedCrcBytes[4];
		ReadToBuffer(filePath, file, &expectedCrcBytes, sizeof(expectedCrcBytes));
		AppendToCrc(expectedCrcBytes, sizeof(expectedCrcBytes), { &fileCrc });
		uint32_t expectedCrc = ReadUInt32_LittleEndian(expectedCrcBytes);

		// crc compare
		uint32_t computedCrc = chunkCrc.GetCurrentHashAsUInt32();
		if (expectedCrc != computedCrc)
			throw std::runtime_error(FileReadError(filePath, "HMAP CRC mismatch."));

		if (payloadSize % (jtf.Header.BitDepth / 8) != 0)
			throw std::runtime_error(FileReadError(filePath, "HMAP payload size does not match bit depth requirement."));

		size_t sampleCount = payloadSize / (jtf.Header.BitDepth / 8);
		jtf.Heights.HeightSamples.resize(sampleCount);

		if (jtf.Header.BitDepth == 32)
		{
			for (size_t i = 0; i < sampleCount; ++i)
			{
				const uint8_t* pointer = payload.data() + i * 4;
				jtf.Heights.HeightSamples[i] = static_cast<double>(ReadFloat_LittleEndian(pointer));
			}
		}
		else if (jtf.Header.BitDepth == 64)
		{
			for (size_t i = 0; i < sampleCount; ++i)
			{
				const uint8_t* pointer = payload.data() + i * 8;
				jtf.Heights.HeightSamples[i] = ReadDouble_LittleEndian(pointer);
			}
		}
		else throw std::runtime_error(FileReadError(filePath, std::format("Unsupported bit depth in HMAP chunk, expected [32] or [64] got [{}].", jtf.Header.BitDepth)));
	}

	void JTFFile::ReadFendChunk(const std::string& filePath, std::ifstream& file, uint32_t payloadSize, Crc32& fileCrc)
	{
		if (payloadSize != 0)
			throw std::runtime_error(FileReadError(filePath, std::format("Invalid FEND payload size, expected [0] got [{}].", payloadSize)));

		Crc32 chunkCrc;

		constexpr char expectedChunkTypeName[4] = { 'F','E','N','D' };
		AppendToCrc(reinterpret_cast<const uint8_t*>(expectedChunkTypeName), 4, { &chunkCrc });

		// read expected chunk crc
		uint8_t expectedCrcBytes[4];
		ReadToBuffer(filePath, file, &expectedCrcBytes, sizeof(expectedCrcBytes));
		AppendToCrc(expectedCrcBytes, sizeof(expectedCrcBytes), { &fileCrc });
		uint32_t expectedCrc = ReadUInt32_LittleEndian(expectedCrcBytes);

		uint32_t computedCrc = chunkCrc.GetCurrentHashAsUInt32();
		if (expectedCrc != computedCrc)
			throw std::runtime_error(FileReadError(filePath, "FEND CRC mismatch."));
	}

	void JTFFile::ReadFileCrc(const std::string& filePath, std::ifstream& file, Crc32& fileCrc)
	{
		// read expected chunk crc
		uint8_t expectedCrcBytes[4];
		ReadToBuffer(filePath, file, &expectedCrcBytes, sizeof(expectedCrcBytes));
		uint32_t expectedCrc = ReadUInt32_LittleEndian(expectedCrcBytes);

		uint32_t computedCrc = fileCrc.GetCurrentHashAsUInt32();
		if (expectedCrc != computedCrc)
			throw std::runtime_error(FileReadError(filePath, "File CRC mismatch."));
	}
}
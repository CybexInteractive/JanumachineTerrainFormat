// MIT License
// © 2025 Cybex Interactive & Matthias Simon Gut (aka Cybex)
// See LICENSE.md for full license text (https://raw.githubusercontent.com/CybexInteractive/JanumachineTerrainFormat/refs/heads/develop/LICENSE.md).

#include "jtf_crc32.h"

namespace cybex_interactive::jtf
{
	void Crc32::Append(const uint8_t* data, size_t length) noexcept
	{
		const uint8_t* bytes = static_cast<const uint8_t*>(data);
		uint32_t crc = m_value;

		for (size_t i = 0; i < length; ++i)
			crc = (crc >> 8) ^ m_table[(crc ^ bytes[i]) & 0xFF];

		m_value = crc;
	}

	uint32_t Crc32::Hash(const uint8_t* data, size_t length) noexcept
	{
		Crc32 crc;
		crc.Append(data, length);
		return crc.GetCurrentHashAsUInt32();
	}
}
// MIT License
// © 2025 Cybex Interactive & Matthias Simon Gut (aka Cybex)
// See LICENSE.md for full license text (https://raw.githubusercontent.com/CybexInteractive/JanumachineTerrainFormat/main/LICENSE.md).

#pragma once

#include <cstdint>
#include <type_traits>

namespace cybex_interactive::jtf
{
	/// <summary>Reverses the order of bytes in an integer.</summary>
	template<typename T> constexpr T byteswap(T value) noexcept
	{
		static_assert(std::is_integral_v<T>, "[JTF Utility Error] Function byteswap(T) requires an integral type.\n");

		switch (sizeof(T))
		{
		case 1:
			return value;

		case 2:
			return static_cast<T>(
				((value & 0x00FF) << 8) |
				((value & 0xFF00) >> 8)
				);

		case 4:
			return static_cast<T>(
				((value & 0x000000FFu) << 24) |
				((value & 0x0000FF00u) << 8) |
				((value & 0x00FF0000u) >> 8) |
				((value & 0xFF000000u) >> 24)
				);

		case 8:
			return static_cast<T>(
				((value & 0x00000000000000FFull) << 56) |
				((value & 0x000000000000FF00ull) << 40) |
				((value & 0x0000000000FF0000ull) << 24) |
				((value & 0x00000000FF000000ull) << 8) |
				((value & 0x000000FF00000000ull) >> 8) |
				((value & 0x0000FF0000000000ull) >> 24) |
				((value & 0x00FF000000000000ull) >> 40) |
				((value & 0xFF00000000000000ull) >> 56)
				);

		default:
			static_assert(sizeof(T) <= 8, "[JTF Utility Error] Unsupported type size for byteswap(T).\n");
			return value;
		}
	}
}
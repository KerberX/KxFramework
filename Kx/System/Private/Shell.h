#pragma once
#include "../Common.h"

namespace KxFramework::Shell::Private
{
	std::optional<uint32_t> MapSHWindowCommand(FlagSet<SHWindowCommand> option) noexcept;
	FlagSet<SHWindowCommand> MapSHWindowCommand(uint32_t option) noexcept;
}

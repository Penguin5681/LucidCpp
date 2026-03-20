#pragma once
#include <iostream>
#include <string>

namespace vis
{
	inline void explore(const std::string &message)
	{
		std::cout << "Lucid Engine Initialized: " << message << std::endl;
	}
} // namespace vis
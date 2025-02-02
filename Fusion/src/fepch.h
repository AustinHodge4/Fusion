#pragma once

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

#include <chrono>
#include <stdexcept>

#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>

#include "fusion/core/core.h"
#include "fusion/core/log.h"

#ifdef FE_PLATFORM_WINDOWS
	#include <Windows.h>
#elif defined(FE_PLATFORM_LINUX)
	#include <unistd.h>
#endif

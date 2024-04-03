
#pragma once

/* Engine macros */
#include "Definitions.h"

/* STD */
#include <string>
#include <algorithm>
#include <type_traits>
#include <memory>
#include <fstream>
#include <sstream>
#include <limits>
#include <cassert>
#include <cstdint>
#include <atomic>
#include <map>
#include <unordered_map>
#include <cstdio>
#include <filesystem>
#include <regex>
#include <locale>
#include <codecvt>
#include <array>
#include <deque>
#include <vector>
#include <iostream>
#include <cstdarg>
#include <thread>
#include <condition_variable>
#include <set>
#include <variant>
#include <cstring>
#include <unordered_set>
//===========================

/* Runtime */
#include "Runtime/Core/Time/Time.h"

#include "Runtime/Core/Tools/Eventing/Event.h"

#include "Runtime/Core/Tools/FileSystem/Buffer.h"
#include "Runtime/Core/Tools/FileSystem/FileSystem.h"
#include "Runtime/Core/Tools/FileSystem/IniFile.h"

#include "Runtime/Core/Tools/Utils/Stopwatch.h"
#include "Runtime/Core/Tools/Utils/hash.h"
#include "Runtime/Core/Tools/Utils/PathParser.h"
#include "Runtime/Core/Tools/Utils/ProgressTracker.h"
#include "Runtime/Core/Tools/Utils/Random.h"
#include "Runtime/Core/Tools/Utils/ReferenceOrValue.h"
#include "Runtime/Core/Tools/Utils/SizeConverter.h"
#include "Runtime/Core/Tools/Utils/Stopwatch.h"
#include "Runtime/Core/Tools/Utils/String.h"
#include "Runtime/Core/Tools/Utils/SystemCalls.h"

#include "Runtime/Core/Log/debug.h"
//===========================

/* Math */
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Ray.h"
#include "Math/RayHit.h"
#include "Math/Rectangle.h"
#include "Math/BoundingBox.h"
#include "Math/Sphere.h"
#include "Math/Matrix.h"
#include "Math/Frustum.h"
#include "Math/Plane.h"
#include "Math/MathHelper.h"
//===========================

#if !defined(_MSC_VER)
    #define FFX_GCC
#endif

/* PROFILER */
#define USING_EASY_PROFILER

#include <easy/details/profiler_colors.h>
#include <easy/profiler.h>
//===========================




// Engine macros
#include "Definitions.h"

//= STD =====================
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

//= RUNTIME ====================
// Core
//#include "Engine.h"
//#include "Event.h"
//#include "Settings.h"
//#include "Timer.h"
//#include "FileSystem.h"
//#include "Stopwatch.h"
//
//// Logging
//#include "../Logging/Log.h"

// Math
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
//==============================

#if !defined(_MSC_VER)
    #define FFX_GCC
#endif

#include <memory>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>

#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RADIANS
#define GLM_SWIZZLE_XYZW 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Sleet/Core/Base.h"
#include "Sleet/Core/Log.h"
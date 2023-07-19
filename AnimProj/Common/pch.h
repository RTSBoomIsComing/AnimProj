// author: Changwan Yu
#pragma once

#include <stdint.h>
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <algorithm>
#include <array>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

#include <d3d11.h>
#include <dxgi.h>
#include <d3dcompiler.h>

#include <wrl.h>
using Microsoft::WRL::ComPtr;

#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include "DirectX11Helper.h"
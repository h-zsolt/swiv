//
// pch.h
// Header for standard system include files.
//

#pragma once

#include <winsdkver.h>
#define _WIN32_WINNT 0x0602
#include <sdkddkver.h>

// Use the C++ standard templated min/max
#define NOMINMAX

// DirectX apps don't need GDI
//#define NODRAWTEXT
//#define NOGDI
//#define NOBITMAP

// Include <mcx.h> if you need this
#define NOMCX

// Include <winsvc.h> if you need this
#define NOSERVICE

// WinHelp is deprecated
#define NOHELP

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <wrl/client.h>

#include <d3d11_1.h>
#include <dxgi1_2.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <SimpleMath.h>
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <CommonStates.h>

#include <algorithm>
#include <exception>
#include <memory>
#include <stdexcept>
#include <iostream>
#include <list>
#include <cstdint>
#include <queue>
#include <bitset>
#include <array>
#include <assert.h>
#include <unordered_map>
#include <map>
#include <set>
#include <string>
#include <functional>
#include <fstream>
#include <sstream>

namespace DX
{
    inline void ThrowIfFailed(HRESULT hr)
    {
        if (FAILED(hr))
        {
            // Set a breakpoint on this line to catch DirectX API errors
            throw std::exception();
        }
    }
}

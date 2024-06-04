#pragma comment(lib, "DirectXTex.lib")

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <pch.h>
#include <Scarle2020.h>
#include <Initialize.h>
#include "Editor.h"

int WINAPI wWinMain(_In_ HINSTANCE _hInstance, _In_opt_ HINSTANCE _hPrevInstance, _In_ LPWSTR _lpCmdLine, _In_ int _nCmdShow)
{
	// Create the editor instance and initialize Scarle2020 engine
	return Scarle2020::Initialize(_hInstance, _hPrevInstance, _lpCmdLine, _nCmdShow, std::move(std::make_unique<Editor>()), "Editor", false);
}

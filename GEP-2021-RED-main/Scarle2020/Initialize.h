#pragma once

#include "pch.h"

class ScarleGame;

namespace Scarle2020
{
	int Initialize(_In_ HINSTANCE _hInstance, _In_opt_ HINSTANCE _hPrevInstance, _In_ LPWSTR _lpCmdLine, _In_ int _nCmdShow, std::unique_ptr<ScarleGame> _game, const std::string& title, bool fullscreen);
}

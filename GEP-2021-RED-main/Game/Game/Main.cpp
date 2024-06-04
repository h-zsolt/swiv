#include <Scarle2020.h>
#include <Initialize.h>
#include "Game.h"

int WINAPI wWinMain(_In_ HINSTANCE _hInstance, _In_opt_ HINSTANCE _hPrevInstance, _In_ LPWSTR _lpCmdLine, _In_ int _nCmdShow)
{
	// Create the game instance and initialize Scarle2020 engine
#ifdef _DEBUG
	return Scarle2020::Initialize(_hInstance, _hPrevInstance, _lpCmdLine, _nCmdShow, std::move(std::make_unique<Game>()), "Game", false);
#else
	return Scarle2020::Initialize(_hInstance, _hPrevInstance, _lpCmdLine, _nCmdShow, std::move(std::make_unique<Game>()), "Game", true);
#endif // _DEBUG
}

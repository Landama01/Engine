#pragma once
#include "Module.h"
#include "Globals.h"
#include "ImGui/imgui.h"
#include "ImGui/backends/imgui_impl_opengl3.h"
#include "ImGui/backends/imgui_impl_sdl.h"
#include "glmath.h"
#include "time.h"
#include "globals.h"

class imguiMenu : public Module
{
public:

	imguiMenu(bool start_enabled = true);
	~imguiMenu();

	bool Start();
	update_status Update(float dt);

	void AboutInfo();

	bool CleanUp();

private:
	char title[25];
	char sdl_version[25];
	char CPU[25];
	char SystemRAM[25];
	char MouseX[25];
	char MouseY[25];

	bool ShowAbout;

	Uint32 flagsFullTrue = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP;
	Uint32 flagsFullFalse = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

	SDL_version compiled;

public:

	SDL_GLContext context;
	
	int init_time = time(NULL);
	int final_time;
	int frame_count = 0;

	int sliderWidth = SCREEN_WIDTH;
	int sliderHeight = SCREEN_HEIGHT;
	bool fullscreen = WIN_FULLSCREEN;

	ImVector<float> fps_log;
};
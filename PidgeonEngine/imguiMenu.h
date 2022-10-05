#pragma once
#include "Module.h"
#include "Globals.h"
#include "ImGui/imgui.h"
#include "ImGui/backends/imgui_impl_opengl3.h"
#include "ImGui/backends/imgui_impl_sdl.h"
#include "glmath.h"
#include "time.h"

class imguiMenu : public Module
{
public:

	imguiMenu(bool start_enabled = true);
	~imguiMenu();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

private:
	char title[25];

public:

	SDL_GLContext context;
	
	int init_time = time(NULL);
	int final_time;
	int frame_count = 0;

	ImVector<float> fps_log;
};
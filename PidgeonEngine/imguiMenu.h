#pragma once
#include "Module.h"
#include "Globals.h"
#include "ImGui/imgui.h"
#include "ImGui/backends/imgui_impl_opengl3.h"
#include "ImGui/backends/imgui_impl_sdl.h"
#include "glmath.h"

class imguiMenu : public Module
{
public:

	imguiMenu(bool start_enabled = true);
	~imguiMenu();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

private:


public:

	SDL_GLContext context;
};
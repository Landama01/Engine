#include "Globals.h"
#include "Application.h"
#include "imguiMenu.h"
#include "ModuleWindow.h"
#include "time.h"


imguiMenu::imguiMenu(bool start_enabled) : Module(start_enabled)
{
	ShowAbout = false;
}

imguiMenu::~imguiMenu()
{}

// -----------------------------------------------------------------
bool imguiMenu::Start()
{
	LOG("Init menu");
	bool ret = true;

	context = SDL_GL_CreateContext(App->window->window);
	if (context == NULL)
	{
		LOG("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	fps_log.push_back(5);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplSDL2_InitForOpenGL(App->window->window, context);
	ImGui_ImplOpenGL3_Init("#version 130");

	SDL_GetVersion(&compiled);
	float RAM = (SDL_GetSystemRAM() * 0.001048576) / 1;

	sprintf_s(sdl_version, 25, "SDL Version: %u.%u.%u", compiled.major, compiled.patch, compiled.minor);
	sprintf_s(CPU, 25, "CPUs: %d (Cache: %dkb)", SDL_GetCPUCount(), SDL_GetCPUCacheLineSize());
	sprintf_s(SystemRAM, 25, "System RAM: %.1fGb", RAM);	

	return ret;
}

// -----------------------------------------------------------------
update_status imguiMenu::Update(float dt)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();	

	sprintf_s(MouseX, 25, "Mouse X: %d", App->input->GetMouseX());
	sprintf_s(MouseY, 25, "Mouse Y: %d", App->input->GetMouseY());
	sprintf_s(title, 25, "Framerate: %.1f", fps_log[fps_log.size() - 1]);	
	

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("Engine GitHub"))
			{
				ShellExecuteA(NULL, "open", "https://github.com/Landama01/Engine", NULL, NULL, SW_SHOWDEFAULT);
			}
			if (ImGui::MenuItem("About"))
			{
				ShowAbout = !ShowAbout;
			}
				
			ImGui::EndMenu();
		}
	}	
	ImGui::EndMainMenuBar();

	if (ImGui::Begin("Test"))
	{	
		if (ImGui::CollapsingHeader("Application"))
		{
			ImGui::BulletText("Engine Name: PidgeonEngine");
			ImGui::Spacing();

			ImGui::Indent();
			ImGui::PlotHistogram("##framerate", &fps_log[0], fps_log.size(), 0, title, 0.0f, 200.0f, ImVec2(310, 100));
			ImGui::Separator();
			ImGui::Text(sdl_version);
			ImGui::Text(CPU);
			ImGui::Text(SystemRAM);
			ImGui::Unindent();
			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::BulletText("MousePosition");
			ImGui::Indent();
			ImGui::Indent();
			ImGui::Text(MouseX);
			ImGui::Text(MouseY);
			ImGui::Unindent();
			ImGui::Unindent();
		}
		if (ImGui::CollapsingHeader("Window"))
		{
			ImGui::SliderInt("Width", &sliderWidth, 100, 2000);

			ImGui::SliderInt("Height", &sliderHeight, 100, 2000);

			ImGui::Checkbox("Fullscreen", &fullscreen);

		}
		
	}
	ImGui::End();

	if (ShowAbout)
	{
		AboutInfo();
	}

	SDL_SetWindowSize(App->window->window, sliderWidth, sliderHeight);
	if (fullscreen)
	{
		SDL_SetWindowFullscreen(App->window->window, flagsFullTrue);
	}
	else
	{
		SDL_SetWindowFullscreen(App->window->window, flagsFullFalse); 
	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------

void imguiMenu::AboutInfo()
{
	ImGui::Begin("About", &ShowAbout);

	ImGui::Text("Pidgeon Engine");
	ImGui::Text("An Engine developed for a CITM Videogame Degree subject");
	ImGui::Text("By Andreu Landa");
	ImGui::Separator();
	ImGui::Spacing();
	ImGui::Text("3rd Party Libraries used:");
	ImGui::BulletText("SDL 2.0.24");
	ImGui::BulletText("Glew 2.1.0");
	ImGui::BulletText("OpenGL 4.6");
	ImGui::BulletText("ImGui 1.88");
	ImGui::Spacing();

	ImGui::Separator();
	ImGui::Spacing();
	ImGui::Text("License:");
	ImGui::Spacing();

	ImGui::Spacing();
	ImGui::Text("MIT License");
	ImGui::Spacing();

	ImGui::Spacing();
	ImGui::Text("Copyright (c) 2022 Landama01");
	ImGui::Spacing();

	ImGui::Spacing();	
	ImGui::Text("Permission is hereby granted, free of charge, to any person obtaining a copy");
	ImGui::Text("of this software and associated documentation files (the 'Software'), to deal");
	ImGui::Text("in the Software without restriction, including without limitation the rights");
	ImGui::Text("to use, copy, modify, merge, publish, distribute, sublicense, and/or sell");
	ImGui::Text("copies of the Software, and to permit persons to whom the Software is");
	ImGui::Text("furnished to do so, subject to the following conditions:");

	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Text("The above copyright notice and this permission notice shall be included in all");
	ImGui::Text("copies or substantial portions of the Software.");

	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Text("THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR");
	ImGui::Text("IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,");
	ImGui::Text("FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE");
	ImGui::Text("AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER");
	ImGui::Text("LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,");
	ImGui::Text("OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE");
	ImGui::Text("SOFTWARE.");

	ImGui::End();
}

// -----------------------------------------------------------------
bool imguiMenu::CleanUp()
{
	LOG("Cleaning menu");

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	return true;
}
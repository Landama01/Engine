#include "Globals.h"
#include "Application.h"
#include "imguiMenu.h"
#include "ModuleWindow.h"
#include "time.h"


imguiMenu::imguiMenu(bool start_enabled) : Module(start_enabled)
{
	
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

	return ret;
}

// -----------------------------------------------------------------
update_status imguiMenu::Update(float dt)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	sprintf_s(title, 25, "Framerate %.1f", fps_log[fps_log.size() - 1]);	

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("About"))
			{
				ShellExecuteA(NULL, "open", "https://github.com/Landama01/Engine", NULL, NULL, SW_SHOWDEFAULT);
			}
			ImGui::EndMenu();
		}
	}	
	ImGui::EndMainMenuBar();
	
	if (ImGui::Begin("Window"))
	{			
		ImGui::PlotHistogram("##framerate", &fps_log[0], fps_log.size(), 0, title, 0.0f, 200.0f, ImVec2(310, 100));		

		ImGui::End();
	}
	

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	return UPDATE_CONTINUE;
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
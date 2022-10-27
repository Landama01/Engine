#include "Application.h"

Application::Application() : debug(false), renderPrimitives(true), dt(0.16f)
{
    window = new ModuleWindow();
	input = new ModuleInput();
	scene = new ModuleScene();
	renderer3D = new ModuleRenderer3D();
	camera = new ModuleCamera3D();
	imgui = new imguiMenu();
	loadMesh = new LoadMesh();

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(window);
	AddModule(camera);
	AddModule(input);
	AddModule(scene);
	AddModule(imgui);
	AddModule(loadMesh);
	
	// Scenes

	// Renderer last!
	AddModule(renderer3D);
}

Application::~Application()
{
	for (int i = list_modules.size(); i <= 0; i--)
	{
		if (list_modules[i] != nullptr)
		{
			delete[] list_modules[i];
			list_modules[i] = nullptr;
		}
	}

	list_modules.clear();
}

bool Application::Init()
{
	bool ret = true;

    App = this;

	// Call Init() in all modules
	for (size_t i = 0; i < list_modules.size() && ret == true; i++)
	{
		ret = list_modules[i]->Init();
	}

	// After all Init calls we call Start() in all modules
	LOG("Application Start --------------");
	for (size_t i = 0; i < list_modules.size(); i++)
	{
		ret = list_modules[i]->Start();
	}

	ms_timer.Start();
	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	dt = (float)ms_timer.Read() / 1000.0f;
	ms_timer.Start();
}

// ---------------------------------------------
void Application::FinishUpdate()
{
}

// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;
	PrepareUpdate();
	
	for (size_t i = 0; i < list_modules.size() && ret == UPDATE_CONTINUE; i++)
	{
		ret = list_modules[i]->PreUpdate(dt);
	}

	for (size_t i = 0; i < list_modules.size() && ret == UPDATE_CONTINUE; i++)
	{
		ret = list_modules[i]->Update(dt);
	}

	for (size_t i = 0; i < list_modules.size() && ret == UPDATE_CONTINUE; i++)
	{
		ret = list_modules[i]->PostUpdate(dt);
	}

	FinishUpdate();
	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;
	for (size_t i = 0; i < list_modules.size() && ret == true; i++)
	{
		ret = list_modules[i]->CleanUp();
	}
	return ret;
}

void Application::AddModule(Module* mod)
{
	list_modules.push_back(mod);
}

Application* App = nullptr;

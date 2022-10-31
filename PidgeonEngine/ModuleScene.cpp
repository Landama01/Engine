#include "Globals.h"
#include "Application.h"


ModuleScene::ModuleScene(bool start_enabled) : Module(start_enabled)
{
}

// Destructor
ModuleScene::~ModuleScene()
{}

// Called before render is available
bool ModuleScene::Init()
{
	LOG("Creating 3D Renderer context");
	bool ret = true;
	
	App->camera->Move(vec3(1, 0 , 0));
	App->camera->LookAt(vec3(0, 0 , 0));

	return ret;
}

bool ModuleScene::Start()
{
	App->loadMesh->LoadFile("Assets/BakerHouse.fbx");

	return true;
}

// PreUpdate: clear buffer
update_status ModuleScene::PreUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

update_status ModuleScene::Update(float dt)
{

	Plane plane(0, 1, 0, 0);
	plane.Render();

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleScene::PostUpdate(float dt)
{
	App->renderer3D->DrawAll();

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleScene::CleanUp()
{
	LOG("Cleaning scene");
	

	return true;
}

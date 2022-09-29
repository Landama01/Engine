#include "Globals.h"
#include "Application.h"
#include "imguiMenu.h"

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

	return ret;
}

// -----------------------------------------------------------------
update_status imguiMenu::Update(float dt)
{

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
bool imguiMenu::CleanUp()
{
	LOG("Cleaning menu");

	return true;
}
#pragma once

#include "Module.h"
#include "Globals.h"
#include "Light.h"

#include "Glew/include/glew.h"

class LoadTexture : public Module
{
public:
	LoadTexture(bool start_enabled = true);
	
	bool Start();
	bool CleanUp();	

	void LoadTex(const char* file_path);

public:


private:

	uint textureID = 0;
	uint imageID = 0;
};
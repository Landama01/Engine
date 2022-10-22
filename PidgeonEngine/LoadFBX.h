#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "Light.h"


struct StoreMesh
{
	uint id_index = 0; // index in VRAM
	uint num_index = 0;
	uint* index = nullptr;
	uint id_vertex = 0; // unique vertex in VRAM
	uint num_vertex = 0;
	float* vertex = nullptr;
};

class LoadFBX : public Module
{
public:
	LoadFBX(bool start_enabled = true);
	bool Init();
	update_status Update(float dt);
	void LoadFile(const char* file_path);
	bool CleanUp();

	void GenerateBuffer(StoreMesh* OurMesh);

public:


private:
};
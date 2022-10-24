#pragma once

#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "Light.h"

#include <vector>

#define Vertex_Count 3

struct StoreMesh
{
	StoreMesh() {};
	~StoreMesh();

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

	void LoadFile(const char* file_path);
	void GenerateBuffer(StoreMesh* OurMesh);

	bool Init();
	update_status PostUpdate(float dt);	
	bool CleanUp();	

public:

	uint VAO = 0;
	uint VBO = 0;

	//std::vector<StoreMesh*> meshes;

private:
	
};
#pragma once

#include "Module.h"
#include "Globals.h"
#include "Light.h"

#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"

#include <vector>

#define Vertex_Count 3

struct StoreMesh
{
	~StoreMesh();

	uint id_index = 0; // index in VRAM
	uint num_index = 0;
	uint* index = nullptr;
	uint id_vertex = 0; // unique vertex in VRAM
	uint num_vertex = 0;
	float* vertex = nullptr;

	void Draw();
};

class LoadMesh : public Module
{
public:
	LoadMesh(bool start_enabled = true);

	void LoadFile(const char* file_path);
	void GenerateBuffer(StoreMesh* OurMesh);
	

	bool Init();
	update_status PostUpdate(float dt);	
	bool CleanUp();	

public:

	std::vector<StoreMesh*> meshCount;

private:

	uint textureID = 0;
};
#pragma once
#include "Module.h"
#include "Globals.h"
#include "Light.h"

#define MAX_LIGHTS 8

class ModuleRenderer3D : public Module
{
public:
    ModuleRenderer3D(bool start_enabled = true);
	~ModuleRenderer3D();

	bool Init();
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void OnResize(int width, int height);

	void DrawDirectCube();
	void DrawVACube();
	void DrawIndexCube();

	void DrawSphere();

public:

	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	mat3x3 NormalMatrix;
	mat4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;

	int fps;
	int fps_cont = 0;

	uint geometriesCount;
	bool cubeExist = false;
	bool sphereExist = false;
	
	bool initResize;
private:

	double rotate_x;

	//sphere
	float radius = 1;
	uint rings = 12;
	uint sectors = 24;
};
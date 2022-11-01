#pragma once
#include "Module.h"
#include "glmath.h"

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(bool start_enabled = true);
	~ModuleCamera3D();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void OrbitalCamera();
	void Look(const vec3& Position, const vec3& Reference, bool RotateAroundReference = false);
	void LookAt(const vec3& Spot);
	void Move(const vec3& Movement);
	float* GetViewMatrix();

public:

	vec3 X, Y, Z, Position, Reference;

private:

	void CalculateViewMatrix();


private:

	mat4x4 ViewMatrix, ViewMatrixInverse;
	//Frustum cameraFrustum;

};
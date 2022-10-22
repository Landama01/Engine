#include "Globals.h"
#include "Application.h"
#include "ModuleRenderer3D.h"

#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"

#include "time.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")

LoadFBX::LoadFBX(bool start_enabled) : Module(start_enabled)
{
}

// Called before render is available
bool LoadFBX::Init()
{
	LOG("Loading FBX files");
	bool ret = true;
	
	//Stream log messages to Debug window
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	return ret;
}


update_status LoadFBX::Update(float dt)
{

	

	return UPDATE_CONTINUE;
}

void LoadFBX::LoadFile(const char* file_path)
{
	const aiScene* scene = aiImportFile(file_path, aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene != nullptr && scene->HasMeshes())
	{
		// Use scene->mNumMeshes to iterate on scene->mMeshes array
		for (size_t i = 0; i < scene->mNumMeshes; i++)
		{
			StoreMesh* OurMesh = new StoreMesh();
			// copy vertices
			OurMesh->num_vertex = scene->mMeshes[i]->mNumVertices;
			OurMesh->vertex = new float[OurMesh->num_vertex * 3];
			memcpy(OurMesh->vertex, scene->mMeshes[i]->mVertices, sizeof(float) * OurMesh->num_vertex * 3);
			LOG("New mesh with %d vertices", OurMesh->num_vertex);

			// copy faces
			if (scene->mMeshes[i]->HasFaces())
			{
				OurMesh->num_index = scene->mMeshes[i]->mNumFaces * 3;
				OurMesh->index = new uint[OurMesh->num_index]; // assume each face is a triangle
				for (uint x = 0; x < scene->mMeshes[i]->mNumFaces; ++x)
				{
					if (scene->mMeshes[i]->mFaces[x].mNumIndices != 3)
					{
						LOG("WARNING, geometry face with != 3 indices!");
					}						
					else
						memcpy(&OurMesh->index[x * 3], scene->mMeshes[i]->mFaces[x].mIndices, 3 * sizeof(uint));
				}
			}
			GenerateBuffer(OurMesh);

		}
		aiReleaseImport(scene);
	}
	else
		LOG("Error loading scene % s", file_path);
}

// Called before quitting
bool LoadFBX::CleanUp()
{
	// detach log stream
	aiDetachAllLogStreams();	

	return true;
}

void LoadFBX::GenerateBuffer(StoreMesh* OurMesh)
{
	GLuint BufferID;
	glGenBuffers(1, &BufferID);
	glBindBuffer(GL_ARRAY_BUFFER, BufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * OurMesh->num_vertex, &OurMesh->vertex, GL_STATIC_DRAW);

}
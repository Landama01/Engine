#include "Globals.h"
#include "Application.h"
#include "ModuleRenderer3D.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")

#define CHECKERS_HEIGHT 256
#define CHECKERS_WIDTH 256

StoreMesh::~StoreMesh()
{
	glDeleteBuffers(1, &id_vertex);
	glDeleteBuffers(1, &id_index);

	if (index != nullptr)
		delete[] index;
	if (vertex != nullptr)
		delete[] vertex;

	index = nullptr;
	vertex = nullptr;
}

void StoreMesh::Draw()
{
	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, id_vertex);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_index);	
	
	glDrawElements(GL_TRIANGLES, num_index, GL_UNSIGNED_INT, NULL);

	if (App->imgui->wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glDisableClientState(GL_VERTEX_ARRAY);

}

LoadMesh::LoadMesh(bool start_enabled) : Module(start_enabled)
{
}

void LoadMesh::LoadFile(const char* file_path)
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
			OurMesh->vertex = new float[OurMesh->num_vertex * Vertex_Count];
			memcpy(OurMesh->vertex, scene->mMeshes[i]->mVertices, sizeof(float) * OurMesh->num_vertex * Vertex_Count);
			LOG("New mesh with %d vertices", OurMesh->num_vertex);

			// copy faces
			if (scene->mMeshes[i]->HasFaces())
			{
				OurMesh->num_index = scene->mMeshes[i]->mNumFaces * 3;
				OurMesh->index = new uint[OurMesh->num_index]; // assume each face is a triangle
				for (uint x = 0; x < scene->mMeshes[i]->mNumFaces; x++)
				{
					if (scene->mMeshes[i]->mFaces[x].mNumIndices != 3)
					{
						LOG("WARNING, geometry face with != 3 indices!");
					}
					else
					{
						memcpy(&OurMesh->index[x * 3], scene->mMeshes[i]->mFaces[x].mIndices, 3 * sizeof(uint));
					}
				}
				GenerateBuffer(OurMesh);
				meshCount.push_back(OurMesh);
			}

		}
		aiReleaseImport(scene);
	}
	else
	{
	LOG("Error loading scene % s", file_path);
	}
		
}

void LoadMesh::GenerateBuffer(StoreMesh* OurMesh)
{
	//vertex buffer	
	glGenBuffers(1, (GLuint*)&(OurMesh->id_vertex));
	glBindBuffer(GL_ARRAY_BUFFER, OurMesh->id_vertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * Vertex_Count * OurMesh->num_vertex, &OurMesh->vertex[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//index buffer	
	glGenBuffers(1, (GLuint*)&(OurMesh->id_index));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, OurMesh->id_index);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * OurMesh->num_index, &OurMesh->index[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// Called before render is available
bool LoadMesh::Init()
{
	LOG("Loading meshes and textures");
	bool ret = true;
	
	//Stream log messages to Debug window
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	GLubyte checkerImage[CHECKERS_HEIGHT][CHECKERS_WIDTH][4];

	for (int i = 0; i < CHECKERS_HEIGHT; i++) {
		for (int j = 0; j < CHECKERS_WIDTH; j++) {
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checkerImage[i][j][0] = (GLubyte)c;
			checkerImage[i][j][1] = (GLubyte)c;
			checkerImage[i][j][2] = (GLubyte)c;
			checkerImage[i][j][3] = (GLubyte)255;
		}
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_WIDTH, CHECKERS_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkerImage);

	return ret;
}

update_status LoadMesh::PostUpdate(float dt)
{	
	/*for (int i = 0; i < meshCount.size(); i++) {
		meshCount[i]->Draw();
	}*/

	return UPDATE_CONTINUE;
}

// Called before quitting
bool LoadMesh::CleanUp()
{
	// detach log stream
	aiDetachAllLogStreams();

	return true;
}
#include "Globals.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "LoadTexture.h"

#include "DevIL\include\ilu.h"
#include "DevIL\include\ilut.h"

#pragma comment (lib, "DevIL/libx86/DevIL.lib")
#pragma comment (lib, "DevIL/libx86/ILU.lib")
#pragma comment (lib, "DevIL/libx86/ILUT.lib")

#define CHECKERS_HEIGHT 64
#define CHECKERS_WIDTH 64

LoadTexture::LoadTexture(bool start_enabled) : Module(start_enabled)
{
	ilInit();
	iluInit();
	ilutInit();
}

// Called before render is available
bool LoadTexture::Start()
{
	LOG("Loading meshes and textures");
	bool ret = true;
	
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
	glBindTexture(GL_TEXTURE_2D, 0);

	return ret;
}

void LoadTexture::LoadTex(const char* file_path)
{
	ilGenImages(1, &imageID);
	ilBindImage(imageID);
}

// Called before quitting
bool LoadTexture::CleanUp()
{

	return true;
}
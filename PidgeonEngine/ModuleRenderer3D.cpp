#include "Globals.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "imguiMenu.h"

#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#include "time.h"

#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */
#pragma comment (lib, "Glew/libx86/glew32.lib")

ModuleRenderer3D::ModuleRenderer3D(bool start_enabled) : Module(start_enabled)
{
	center = (5.0f, 5.0f, 5.0f);
}

// Destructor
ModuleRenderer3D::~ModuleRenderer3D()
{}

// Called before render is available
bool ModuleRenderer3D::Init()
{
	LOG("Creating 3D Renderer context");
	bool ret = true;
	
	App->loadFbx->LoadFile("../Assets/FBX/BakerHouse.fbx");

	//Create context
	context = SDL_GL_CreateContext(App->window->window);
	if(context == NULL)
	{
		LOG("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	
	//init GLEW lib
	GLenum err = glewInit();

	//print GLEW info
	LOG("Using Glew %s", glewGetString(GLEW_VERSION));

	LOG("Vendor: %s", glGetString(GL_VENDOR));
	LOG("Renderer: %s", glGetString(GL_RENDERER));
	LOG("OpenGL version supported %s", glGetString(GL_VERSION));
	LOG("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));


	if(ret == true)
	{
		//Use Vsync
		if(VSYNC && SDL_GL_SetSwapInterval(1) < 0)
			LOG("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());

		//Initialize Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//Check for error
		GLenum error = glGetError();
		if(error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		//Initialize Modelview Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}
		
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);
		
		//Initialize clear color
		glClearColor(0.f, 0.f, 0.f, 1.f);

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}
		
		GLfloat LightModelAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);
		
		lights[0].ref = GL_LIGHT0;
		lights[0].ambient.Set(0.25f, 0.25f, 0.25f, 1.0f);
		lights[0].diffuse.Set(0.75f, 0.75f, 0.75f, 1.0f);
		lights[0].SetPos(0.0f, 0.0f, 2.5f);
		lights[0].Init();
		
		GLfloat MaterialAmbient[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		GLfloat MaterialDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);
		
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		lights[0].Active(true);
		glEnable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_TEXTURE_2D);
	}

	// Projection matrix for
	OnResize(SCREEN_WIDTH, SCREEN_HEIGHT);
	

	return ret;
}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(App->camera->GetViewMatrix());

	// light 0 on cam pos
	lights[0].SetPos(App->camera->Position.x, App->camera->Position.y, App->camera->Position.z);

	for(uint i = 0; i < MAX_LIGHTS; ++i)
		lights[i].Render();

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate(float dt)
{
	//DrawCube(center);

	glColor3f(0.0f, 0.0f, 1.0f);

	glEnableClientState(GL_VERTEX_ARRAY);
	//glVertexPointer(3, GL_FLOAT, 0, vertices);
	
	//unsigned int indices[] = { 0, 1, 2};

	//glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, indices);

	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDisableClientState(GL_VERTEX_ARRAY);

	SDL_GL_SwapWindow(App->window->window);

	App->imgui->frame_count++;
	App->imgui->final_time = time(NULL);
	if (App->imgui->final_time - App->imgui->init_time > 0)
	{
		fps = (App->imgui->frame_count / (App->imgui->final_time - App->imgui->init_time));
		App->imgui->fps_log.push_back(fps);

		App->imgui->frame_count = 0;
		App->imgui->init_time = App->imgui->final_time;	
	}

	
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	LOG("Destroying 3D Renderer");
	
	SDL_GL_DeleteContext(context);

	return true;
}


void ModuleRenderer3D::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	ProjectionMatrix = perspective(60.0f, (float)width / (float)height, 0.125f, 512.0f);
	glLoadMatrixf(&ProjectionMatrix);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void ModuleRenderer3D::DrawCube(vec3 center)
{
	glLineWidth(2.0f);
	glColor3f(1, 0, 0);
	glRotatef(0.1f, 1.0f, 1.0f, 0.0f);

	glBegin(GL_TRIANGLES);

	//front face
	glVertex3f(center.x + 5, center.y - 5, center.z + 5); //v0
	glVertex3f(center.x - 5, center.y - 5, center.z + 5); //v1
	glVertex3f(center.x - 5, center.y + 5, center.z + 5); //v2

	glVertex3f(center.x - 5, center.y + 5, center.z + 5);
	glVertex3f(center.x + 5, center.y + 5, center.z + 5); //v3
	glVertex3f(center.x + 5, center.y - 5, center.z + 5);

	//right face		  		  				 			
	glVertex3f(center.x + 5, center.y - 5, center.z + 5);
	glVertex3f(center.x + 5, center.y + 5, center.z + 5);
	glVertex3f(center.x + 5, center.y + 5, center.z - 5); //v4

	glVertex3f(center.x + 5, center.y + 5, center.z - 5);
	glVertex3f(center.x + 5, center.y - 5, center.z - 5); //v5
	glVertex3f(center.x + 5, center.y - 5, center.z + 5);

	//top face			  		  				 			
	glVertex3f(center.x + 5, center.y - 5, center.z + 5);
	glVertex3f(center.x + 5, center.y - 5, center.z - 5);
	glVertex3f(center.x - 5, center.y - 5, center.z - 5); //v6

	glVertex3f(center.x - 5, center.y - 5, center.z - 5);
	glVertex3f(center.x - 5, center.y - 5, center.z + 5);
	glVertex3f(center.x + 5, center.y - 5, center.z + 5);

	//left face			   				 			   
	glVertex3f(center.x - 5, center.y + 5, center.z + 5);
	glVertex3f(center.x - 5, center.y - 5, center.z + 5);
	glVertex3f(center.x - 5, center.y - 5, center.z - 5);

	glVertex3f(center.x - 5, center.y - 5, center.z - 5);
	glVertex3f(center.x - 5, center.y - 5, center.z - 5);
	glVertex3f(center.x - 5, center.y + 5, center.z - 5); //v7

	//bot face			  	  				 		  	
	glVertex3f(center.x - 5, center.y + 5, center.z - 5);
	glVertex3f(center.x - 5, center.y + 5, center.z + 5);
	glVertex3f(center.x + 5, center.y + 5, center.z + 5);

	glVertex3f(center.x + 5, center.y + 5, center.z + 5);
	glVertex3f(center.x + 5, center.y + 5, center.z - 5);
	glVertex3f(center.x - 5, center.y + 5, center.z - 5);

	//back face			  	  				 		  	
	glVertex3f(center.x - 5, center.y + 5, center.z - 5);
	glVertex3f(center.x + 5, center.y + 5, center.z - 5);
	glVertex3f(center.x + 5, center.y - 5, center.z - 5);

	glVertex3f(center.x + 5, center.y - 5, center.z - 5);
	glVertex3f(center.x - 5, center.y - 5, center.z - 5);
	glVertex3f(center.x - 5, center.y + 5, center.z - 5);

	glEnd();
}
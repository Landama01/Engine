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
}

// Destructor
ModuleRenderer3D::~ModuleRenderer3D()
{}

// Called before render is available
bool ModuleRenderer3D::Init()
{
	LOG("Creating 3D Renderer context");
	bool ret = true;

	initResize = true;

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
	if(cubeExist)
		DrawIndexCube();

	if (sphereExist)
		DrawSphere();

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

//Direct mode
void ModuleRenderer3D::DrawDirectCube()
{
	GLfloat v0[3] = { 1.0f, 1.0f, 0.0f };
	GLfloat v1[3] = { 0.0f, 1.0f, 0.0f };
	GLfloat v2[3] = { 0.0f, 0.0f, 0.0f };
	GLfloat v3[3] = { 1.0f, 0.0f, 0.0f };
	GLfloat v4[3] = { 1.0f, 0.0f,-1.0f };
	GLfloat v5[3] = { 1.0f, 1.0f,-1.0f };
	GLfloat v6[3] = { 0.0f, 1.0f,-1.0f };
	GLfloat v7[3] = { 0.0f, 0.0f,-1.0f };

	glBegin(GL_TRIANGLES);

	//front face
	glColor3f(1, 0, 0);
	glVertex3fv(v0); //v0
	glVertex3fv(v1); //v1
	glVertex3fv(v2); //v2
			  
	glVertex3fv(v2);
	glVertex3fv(v3); //v3
	glVertex3fv(v0);
			  
	//right face	
	glColor3f(0, 1, 0);
	glVertex3fv(v0);
	glVertex3fv(v3);
	glVertex3fv(v4); //v4
			  
	glVertex3fv(v4);
	glVertex3fv(v5); //v5
	glVertex3fv(v0);
			  
	//top face  
	glColor3f(0, 0, 1);
	glVertex3fv(v0);
	glVertex3fv(v5);
	glVertex3fv(v6); //v6
			  
	glVertex3fv(v6);
	glVertex3fv(v1);
	glVertex3fv(v0);
			
	//back face			
	glColor3f(0, 1, 1);
	glVertex3fv(v7);
	glVertex3fv(v6);
	glVertex3fv(v5);

	glVertex3fv(v5);
	glVertex3fv(v4);
	glVertex3fv(v7);

	//left face		
	glColor3f(1, 0, 1);
	glVertex3fv(v7);
	glVertex3fv(v2);
	glVertex3fv(v1);
			  
	glVertex3fv(v1);
	glVertex3fv(v6);
	glVertex3fv(v7); //v7
			  
	//bot face	 
	glColor3f(1, 1, 0);
	glVertex3fv(v7);
	glVertex3fv(v4);
	glVertex3fv(v3);
			  
	glVertex3fv(v3);
	glVertex3fv(v2);
	glVertex3fv(v7);
			  
	glEnd();
}

//Vertex Arrays and glDrawArrays()
void ModuleRenderer3D::DrawVACube()
{
	GLfloat vertices[] =
	{
		1,  1,  0,
		0,  1,  0,
		0,  0,  0,
			    
		0,  0,  0,
		1,  0,  0,
		1,  1,  0,
			    
		1,  1,  0,
		1,  0,  0,
		1,  0, -1,

		1,  0, -1,
		1,  1, -1,
		1,  1,  0,

		1,  1,  0,
		1,  1, -1,
		0,  0, -1,

		0,  1, -1,
		0,  1,  0,
		1,  1,  0,

		0,  0, -1,
		0,  1, -1,
		1,  1, -1,

		1,  1, -1,
		1,  0, -1,
		0,  0, -1,

		0,  0, -1,
		0,  0,  0,
		0,  1,  0,

		0,  1,  0,
		0,  1, -1,
		0,  0, -1,

		0,  0, -1,
		1,  0, -1,
		1,  0,  0,

		1,  0,  0,
		0,  0,  0,
		0,  0, -1,
			
	};

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vertices);

	if (App->imgui->wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// draw a cube
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// deactivate vertex arrays after drawing
	glDisableClientState(GL_VERTEX_ARRAY);
}

//Indices and glDrawElements()
void ModuleRenderer3D::DrawIndexCube()
{
	GLfloat vertices[] =
	{
		1.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f,-1.0f,
		1.0f, 1.0f,-1.0f,
		0.0f, 1.0f,-1.0f,
		0.0f, 0.0f,-1.0f,
	};

	GLubyte indices[] = 
	{ 
		0,1,2, 2,3,0,   
		0,3,4, 4,5,0,
		0,5,6, 6,1,0,
		1,6,7, 7,2,1,
		7,4,3, 3,2,7,
		4,7,6, 6,5,4
	};

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vertices);

	if(App->imgui->wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// draw a cube
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, indices);

	// deactivate vertex arrays after drawing
	glDisableClientState(GL_VERTEX_ARRAY);
}

void ModuleRenderer3D::DrawSphere()
{
	float const R = 1. / (float)(rings - 1);
	float const S = 1. / (float)(sectors - 1);
	int r, s;

	std::vector<GLfloat> vertices;
	std::vector<GLfloat> normals;
	std::vector<GLfloat> texcoords;
	std::vector<GLushort> indices;

	vertices.resize(rings * sectors * 3);
	normals.resize(rings * sectors * 3);
	texcoords.resize(rings * sectors * 2);
	std::vector<GLfloat>::iterator v = vertices.begin();
	std::vector<GLfloat>::iterator n = normals.begin();
	std::vector<GLfloat>::iterator t = texcoords.begin();
	for (r = 0; r < rings; r++) for (s = 0; s < sectors; s++) {
		float const y = sin(-M_PI_2 + M_PI * r * R);
		float const x = cos(2 * M_PI * s * S) * sin(M_PI * r * R);
		float const z = sin(2 * M_PI * s * S) * sin(M_PI * r * R);

		*t++ = s * S;
		*t++ = r * R;

		*v++ = x * radius;
		*v++ = y * radius;
		*v++ = z * radius;

		*n++ = x;
		*n++ = y;
		*n++ = z;
	}

	indices.resize(rings * sectors * 4);
	std::vector<GLushort>::iterator i = indices.begin();
	for (r = 0; r < rings; r++) for (s = 0; s < sectors; s++) {
		*i++ = r * sectors + s;
		*i++ = r * sectors + (s + 1);
		*i++ = (r + 1) * sectors + (s + 1);
		*i++ = (r + 1) * sectors + s;
	}

	GLfloat x = 0, y = 0, z = 0;

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, &vertices[0]);
	glNormalPointer(GL_FLOAT, 0, &normals[0]);
	if (App->imgui->wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glTexCoordPointer(2, GL_FLOAT, 0, &texcoords[0]);
	glDrawElements(GL_QUADS, indices.size(), GL_UNSIGNED_SHORT, &indices[0]);
	glPopMatrix();
}
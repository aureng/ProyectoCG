/*
Semestre 2023-1
Práctica 5: Optimización y Carga de Modelos
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;


Model Llanta_M;
Model Camino_M;
Model Blackhawk_M;
Model Dado_M;
Model kitt_sin_cofre;
Model cofre_kitt;
Model audi_sin_cofre;
Model cofre_audi;
Model audi_llanta;

Skybox skybox;

//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;


// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";





void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	
	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);


}


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}



int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 3.0f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();

	Llanta_M = Model();
	Llanta_M.LoadModel("Models/llanta_M.obj");
	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/uh60.obj");
	Camino_M = Model();
	Camino_M.LoadModel("Models/railroad track.obj");
	kitt_sin_cofre = Model();
	kitt_sin_cofre.LoadModel("Models/cochesincofre.obj");
	cofre_kitt = Model();
	cofre_kitt.LoadModel("Models/cofre_kitt.obj");
	audi_sin_cofre = Model();
	audi_sin_cofre.LoadModel("Models/audi_sin_cofre.obj");
	audi_llanta = Model();
	audi_llanta.LoadModel("Models/audi_llanta.obj");
	cofre_audi = Model();
	cofre_audi.LoadModel("Models/cofre_audi.obj");


	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::mat4 modelcarro(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		meshList[2]->RenderMesh();

		//######### PARTE PARA KITT ################
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(40.0f, 1.5f, -1.5f));
		model = glm::translate(model, glm::vec3(mainWindow.getmuevekitt(), 0.0f, 0.0f));
		modelcarro = modelaux = model;
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		kitt_sin_cofre.RenderModel();

		model = modelaux;
		//pivote de cofre
		model = glm::translate(model, glm::vec3(5.0f, 2.0f,0.0f));
		modelaux = model;
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = modelaux;
		model = glm::rotate(model, glm::radians(mainWindow.getmuevecofrekitt()), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(5.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.0282f, 0.104f, 0.940f); //Azul
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		cofre_kitt.RenderModel();
		
		//Llanta delantera izquierda
		model = modelcarro;
		model = glm::translate(model, glm::vec3(7.0f, -1.0f, -6.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.980f, 0.441f, 0.00f); //Naranja
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Llanta_M.RenderModel();

		//Llanta delantera derecha
		model = modelcarro;
		model = glm::translate(model, glm::vec3(7.0f, -1.0f, 6.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getmuevex()), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.796f, 0.950f, 0.0285f); //Limon
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Llanta_M.RenderModel();


		//Llanta trasera izquierda
		model = modelcarro;
		model = glm::translate(model, glm::vec3(-10.0f, -1.0f, -6.0f));
		model = glm::rotate(model, 90*toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getmuevex()), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.00f, 0.960f, 0.224f); //Verde
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Llanta_M.RenderModel();

		//Llanta trasera derecha
		model = modelcarro;
		model = glm::translate(model, glm::vec3(-10.0f, -1.0f, 6.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getmuevex()), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.00f, 0.960f, 0.848f); //Celeste
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Llanta_M.RenderModel();


		//##################### PARTE PARA AUDI ###########################
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-30.0f, 3.0f, 0.0f));
		model = glm::translate(model, glm::vec3(mainWindow.getmueveaudi(), 0.5f, -1.5f));
		modelaux = modelcarro = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.412f, 0.576f, 0.840f); //Azul
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); 
		audi_sin_cofre.RenderModel();

		//cofre de audi
		model = modelaux;
		model = glm::translate(model, glm::vec3(8.0f, 2.0f, 0.5f));
		model = glm::rotate(model,glm::radians(mainWindow.getmuevecofreaudi()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		model = glm::translate(model, glm::vec3(6.0f, -0.2f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.620f, 0.650f, 0.730f); //Gris
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		cofre_audi.RenderModel();

		//Llanta delantera izquierda
		model = modelcarro;
		model = glm::translate(model, glm::vec3(12.5f, -2.5f, -7.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.634f, 0.0658f, 0.940f); //Morado
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		audi_llanta.RenderModel();

		//Llanta delantera derecha
		model = modelaux = modelcarro;
		model = glm::translate(model, glm::vec3(12.5f, -2.5f, 7.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.920f, 0.0644f, 0.849f); //Rosa
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		audi_llanta.RenderModel();

		//Llanta trasera izquierda
		model = modelcarro;
		model = glm::translate(model, glm::vec3(-11.5f, -2.5f, -6.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.980f, 0.00f, 0.261f); //Rojo
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		audi_llanta.RenderModel();

		//Llanta trasera derecha
		model = modelcarro;
		model = glm::translate(model, glm::vec3(-11.5f, -2.5f, 7.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.130f, 0.0624f, 0.0838f); //Negro
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		audi_llanta.RenderModel();

		/*model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 3.0f, -1.0));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Blackhawk_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.53f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Camino_M.RenderModel();*/


		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}

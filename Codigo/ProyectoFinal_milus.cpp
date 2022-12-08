/*
Semestre 2023-1
Práctica 7: Iluminación 1 
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

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

int prender = 0;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;

Model Base;

Skybox skybox_dia;
Skybox skybox_tarde;
Skybox skybox_noche;


//materiales
Material Material_brillante;
Material Material_opaco;


//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

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

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 5.0f, 0.5f);

	pisoTexture = Texture("Textures/piso.jpg");
	pisoTexture.LoadTextureA();

	Base = Model();
	Base.LoadModel("Models/1.obj");

	std::vector<std::string> skyboxFacesDia;
	skyboxFacesDia.push_back("Textures/Skybox/skybox_dia_rt.tga");
	skyboxFacesDia.push_back("Textures/Skybox/skybox_dia_lf.tga");
	skyboxFacesDia.push_back("Textures/Skybox/skybox_dia_dn.tga");
	skyboxFacesDia.push_back("Textures/Skybox/skybox_dia_up.tga");
	skyboxFacesDia.push_back("Textures/Skybox/skybox_dia_bk.tga");
	skyboxFacesDia.push_back("Textures/Skybox/skybox_dia_ft.tga");
	skybox_dia = Skybox(skyboxFacesDia);

	std::vector<std::string> skyboxFacesTarde;
	skyboxFacesTarde.push_back("Textures/Skybox/skybox_tarde_rt.tga");
	skyboxFacesTarde.push_back("Textures/Skybox/skybox_tarde_lf.tga");
	skyboxFacesTarde.push_back("Textures/Skybox/skybox_tarde_dn.tga");
	skyboxFacesTarde.push_back("Textures/Skybox/skybox_tarde_up.tga");
	skyboxFacesTarde.push_back("Textures/Skybox/skybox_tarde_bk.tga");
	skyboxFacesTarde.push_back("Textures/Skybox/skybox_tarde_ft.tga");
	skybox_tarde = Skybox(skyboxFacesTarde);

	std::vector<std::string> skyboxFacesNoche;
	skyboxFacesNoche.push_back("Textures/Skybox/skybox_noche_rt.tga");
	skyboxFacesNoche.push_back("Textures/Skybox/skybox_noche_lf.tga");
	skyboxFacesNoche.push_back("Textures/Skybox/skybox_noche_dn.tga");
	skyboxFacesNoche.push_back("Textures/Skybox/skybox_noche_up.tga");
	skyboxFacesNoche.push_back("Textures/Skybox/skybox_noche_bk.tga");
	skyboxFacesNoche.push_back("Textures/Skybox/skybox_noche_ft.tga");
	skybox_noche = Skybox(skyboxFacesNoche);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	//luz direccional, sólo 1 y siempre debe de existir  (MODIFICACION LUZ)
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.5f, 0.3f,   //MODIFICACION DE LUZ DIA Y NOCHE TAMBIEN POSICION SOL DIA Y NOCHE
		0.0f, 0.0f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;


	//luz stop
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
		0.5f, 0.5f,
		76.733f, 4.916f,-75.802f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

	//luz intermitente
	pointLights[1] = PointLight(1.0f, 0.4f, 0.0f,
		0.5f, 0.5f,
		76.733f, 4.916f, -88.423f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

	unsigned int spotLightCount = 0;

	//luz de farola
	spotLights[0] = SpotLight(1.0f, 1.0f, 0.8f,
		1.0f, 2.0f,
		17.624f, 31.955f, -44.614f,
		-0.3f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		16.0f);
	spotLightCount++;

	//luz de farola
	spotLights[1] = SpotLight(1.0f, 1.0f, 0.8f,
		1.0f, 2.0f,
		-15.624f, 31.955f, -44.614f,
		0.3f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		16.0f);
	spotLightCount++;

	//luz de farola
	spotLights[2] = SpotLight(1.0f, 1.0f, 0.8f,
		1.0f, 2.0f,
		17.624f, 31.955f, 88.765f,
		-0.3f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		16.0f);
	spotLightCount++;

	//luz de farola
	spotLights[3] = SpotLight(1.0f, 1.0f, 0.8f,
		1.0f, 2.0f,
		-15.624f, 31.955f, 88.765f,
		0.3f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		16.0f);
	spotLightCount++;

	//luz de farola
	spotLights[4] = SpotLight(1.0f, 1.0f, 0.8f,
		1.0f, 2.0f,
		-198.163f, 31.469f, -55.39f,
		0.0f, -1.0f, -0.3f,
		1.0f, 0.0f, 0.0f,
		16.0f);
	spotLightCount++;



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
		skybox_tarde.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();

		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);



		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);

		



		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		pisoTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.03f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Base.RenderModel();

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}

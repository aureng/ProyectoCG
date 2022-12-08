/*
SEMESTRE 2023-1
PROYECTO FINAL CGeIHC LAB Grupo 10

INTEGRANTES:
Garcia Sanchez Emilio
Gonzalez Peña Jared
Rojas Eng Aurelio
*/

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
#include "Model.h"
#include "Skybox.h"

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

//########## VARIABLES DE ANIMACION ##############
//float movCoche;
//float movOffset;	//Velocidad de movimiento
//float rotLlanta;
//float rotLlantaOffset; //Velocidad de movimiento
//bool avanza;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

//########## TEXTURAS ##############
Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;

//########## MODELOS ##############
Model Puesto_tortas;
Model Puesto_aguas;
Model puesto_tortas_supremo;
//MegamanX
Model MegamanX_cabeza;
Model MegamanX_pecho;
Model MegamanX_torso;
Model MegamanX_hombroD;
Model MegamanX_hombroI;
Model MegamanX_brazoD;
Model MegamanX_brazoI;
Model MegamanX_manoD;
Model MegamanX_manoI;
Model MegamanX_buster;
Model MegamanX_musloD;
Model MegamanX_musloI;
Model MegamanX_piernaD;
Model MegamanX_piernaI;
Model MegamanX_pieD;
Model MegamanX_pieI;
//Lambert
Model Lambert_CabezayTorso;
Model Lambert_BrazoIzq;
Model Lambert_Brazoder;
Model Lambert_PiernaIzq;
Model Lambert_Piernader;
Model Escenario;

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

//########## LUCES ##############
// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];
SpotLight lucesAvanzando[MAX_SPOT_LIGHTS];
SpotLight lucesRetrocediendo[MAX_SPOT_LIGHTS];


// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";

void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = -glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}



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

	unsigned int vegetacionIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4,5,6,
	   4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,


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


	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);


}


void CrearDado()
{
	unsigned int cubo_indices[] = {
		0,0,0,
		1,0,1,
		2,0,2,

		2,0,2,
		3,0,3,
		0,0,0,

		4,0,4,
		5,0,5,
		6,0,6,

		6,0,6,
		7,0,7,
		4,0,4,

		7,0,7,
		6,0,6,
		8,0,8,

		8,0,8,
		9,0,9,
		7,0,7,

		9,0,9,
		8,0,8,
		10,0,10,

		10,0,10,
		11,0,11,
		9,0,9,
//f 8 / 1 / 8 7 / 1 / 7 9 / 1 / 9
//f 9 / 1 / 9 10 / 1 / 10 8 / 1 / 8
//f 10 / 1 / 10 9 / 1 / 9 11 / 1 / 11
//f 11 / 1 / 11 12 / 1 / 12 10 / 1 / 10
//f 12 / 1 / 12 11 / 1 / 11 13 / 1 / 13
//f 13 / 1 / 13 14 / 1 / 14 12 / 1 / 12
//f 14 / 1 / 14 13 / 1 / 13 15 / 1 / 15
//f 15 / 1 / 15 16 / 1 / 16 14 / 1 / 14
//f 16 / 1 / 16 15 / 1 / 15 17 / 1 / 17
//f 17 / 1 / 17 18 / 1 / 18 16 / 1 / 16
//f 18 / 1 / 18 17 / 1 / 17 19 / 1 / 19
//f 19 / 1 / 19 20 / 1 / 20 18 / 1 / 18
//f 20 / 1 / 20 19 / 1 / 19 21 / 1 / 21
//f 21 / 1 / 21 22 / 1 / 22 20 / 1 / 20
//f 22 / 1 / 22 21 / 1 / 21 23 / 1 / 23
//f 23 / 1 / 23 24 / 1 / 24 22 / 1 / 22
//s 2
//f 23 / 1 / 23 25 / 1 / 25 24 / 1 / 24
//s 4
//f 26 / 1 / 26 27 / 1 / 27 2 / 1 / 2
//f 2 / 1 / 2 1 / 1 / 1 26 / 1 / 26
//s 16
//f 9 / 1 / 9 7 / 1 / 7 6 / 1 / 6
//f 6 / 1 / 6 23 / 1 / 23 21 / 1 / 21
//f 21 / 1 / 21 19 / 1 / 19 17 / 1 / 17
//f 6 / 1 / 6 21 / 1 / 21 17 / 1 / 17
//f 9 / 1 / 9 6 / 1 / 6 17 / 1 / 17
//f 17 / 1 / 17 15 / 1 / 15 13 / 1 / 13
//f 9 / 1 / 9 17 / 1 / 17 13 / 1 / 13
//f 9 / 1 / 9 13 / 1 / 13 11 / 1 / 11
//f 18 / 1 / 18 20 / 1 / 20 22 / 1 / 22
//f 22 / 1 / 22 24 / 1 / 24 5 / 1 / 5
//f 5 / 1 / 5 8 / 1 / 8 10 / 1 / 10
//f 22 / 1 / 22 5 / 1 / 5 10 / 1 / 10
//f 18 / 1 / 18 22 / 1 / 22 10 / 1 / 10
//f 10 / 1 / 10 12 / 1 / 12 14 / 1 / 14
//f 18 / 1 / 18 10 / 1 / 10 14 / 1 / 14
//f 18 / 1 / 18 14 / 1 / 14 16 / 1 / 16
//s 2
//f 25 / 1 / 25 23 / 1 / 23 28 / 1 / 28
//f 25 / 1 / 25 28 / 1 / 28 29 / 1 / 29
//f 25 / 1 / 25 29 / 1 / 29 24 / 1 / 24
//f 29 / 1 / 29 28 / 1 / 28 30 / 1 / 28
//f 30 / 1 / 28 31 / 1 / 29 29 / 1 / 29
//s 16
//f 4 / 1 / 4 32 / 1 / 30 29 / 1 / 29
//f 29 / 1 / 29 31 / 1 / 29 4 / 1 / 4
//f 28 / 1 / 28 33 / 1 / 31 3 / 1 / 3
//f 3 / 1 / 3 30 / 1 / 28 28 / 1 / 28
//s 1
//f 4 / 1 / 4 3 / 1 / 3 33 / 1 / 31
//f 33 / 1 / 31 32 / 1 / 30 4 / 1 / 4
//s 2
//f 31 / 1 / 29 30 / 1 / 28 27 / 1 / 27
//f 27 / 1 / 27 26 / 1 / 26 31 / 1 / 29
//s 16
//f 4 / 1 / 4 31 / 1 / 29 26 / 1 / 26
//f 26 / 1 / 26 1 / 1 / 1 4 / 1 / 4
//f 30 / 1 / 28 3 / 1 / 3 2 / 1 / 2
//f 2 / 1 / 2 27 / 1 / 27 30 / 1 / 28
//s off
//f 34 / 1 / 32 6 / 1 / 6 5 / 1 / 5
//f 5 / 1 / 5 35 / 1 / 33 34 / 1 / 32
//f 35 / 1 / 33 5 / 1 / 5 24 / 1 / 24
//f 24 / 1 / 24 36 / 1 / 34 35 / 1 / 33
//f 36 / 1 / 34 24 / 1 / 24 29 / 1 / 29
//f 29 / 1 / 29 37 / 1 / 34 36 / 1 / 34
//f 37 / 1 / 34 29 / 1 / 29 32 / 1 / 30
//f 32 / 1 / 30 38 / 1 / 35 37 / 1 / 34
//f 38 / 1 / 35 32 / 1 / 30 33 / 1 / 31
//f 33 / 1 / 31 39 / 1 / 36 38 / 1 / 35
//f 39 / 1 / 36 33 / 1 / 31 28 / 1 / 28
//f 28 / 1 / 28 40 / 1 / 37 39 / 1 / 36
//f 40 / 1 / 37 28 / 1 / 28 23 / 1 / 23
//f 23 / 1 / 23 41 / 1 / 37 40 / 1 / 37
//f 41 / 1 / 37 23 / 1 / 23 6 / 1 / 6
//f 6 / 1 / 6 34 / 1 / 32 41 / 1 / 37
//		
//		// right
//		4, 5, 6,
//		6, 7, 4,
//		// back
//		8, 9, 10,
//		10, 11, 8,
//
//		// left
//		12, 13, 14,
//		14, 15, 12,
//		// bottom
//		16, 17, 18,
//		18, 19, 16,
//		// top
//		20, 21, 22,
//		22, 23, 20,
//		// front
//		0, 1, 2,
//		2, 3, 0
	};

	GLfloat cubo_vertices[] = {
		7.6481f,	0.2015f,	10.7072f,		0.0f,  0.0f,		0.0f,	0.0f,	0.0f,
		7.6481f,	0.2364f,	-10.7074f,		0.0f,  0.0f,		0.0f,	0.0f,	0.0f,
		7.6572f,	1.6546f,	-10.7074f,		0.0f,  0.0f,		0.0f,	0.0f,	0.0f,
		7.6572f,	1.6197f,	10.7072f,		0.0f,  0.0f,		0.0f,	0.0f,	0.0f,
		7.7544f,	16.6859f,	10.7072f,		0.0f,  0.0f,		0.0f,	0.0f,	0.0f,
		7.7544f,	16.7207f,	-10.7074f,		0.0f,  0.0f,		0.0f,	0.0f,	0.0f,
		5.1540f,	18.5832f,	-10.7074f,		0.0f,  0.0f,		0.0f,	0.0f,	0.0f,
		5.1540f,	18.5832f,	10.7072f,		0.0f,  0.0f,		0.0f,	0.0f,	0.0f,
		3.7507f,	19.4040f,	-10.7074f,		0.0f,  0.0f,		0.0f,	0.0f,	0.0f,
		3.7507f,	19.4040f,	10.7072f,		0.0f,  0.0f,		0.0f,	0.0f,	0.0f,
		2.2523f,	19.9437f,	-10.7074f,		0.0f,  0.0f,		0.0f,	0.0f,	0.0f,
		2.2523f,	19.9437f,	10.7072f,		0.0f,  0.0f,		0.0f,	0.0f,	0.0f,
		0.6984f,	20.2029f,	-10.7074f,		0.0f,  0.0f,		0.0f,	0.0f,	0.0f,
		0.6984f,	20.2028f,	10.7072f,		0.0f,  0.0f,		0.0f,	0.0f,	0.0f,
		0.8716f,	20.1819f,	-10.7074f,		0.0f,  0.0f,		0.0f,	0.0f,	0.0f,
		-0.8716f,	20.1819f,	10.7072f,		0.0f,  0.0f,		0.0f,	0.0f,	0.0f,
		-2.4181f,	19.8814f,	-10.7074f,		0.0f,  0.0f,		0.0f,	0.0f,	0.0f,
		-2.4181f,	19.8814f,	10.7072f,		0.0f,  0.0f,		0.0f,	0.0f,	0.0f,
		-3.9016f,	19.3020f,	-10.7074f,		0.0f,  0.0f,		0.0f,	0.0f,	0.0f,
		-3.9016f,	19.3020f,	10.7072f,		0.0f,  0.0f,		0.0f,	0.0f,	0.0f,
		-5.2824f,	18.4440f,	-10.7074f,		0.0f,  0.0f,		0.0f,	0.0f,	0.0f,
		-5.2824f,	18.4440f,	10.7072f,		0.0f,  0.0f,		0.0f,	0.0f,	0.0f,
		-7.7544f,	16.4015f,	-10.7074f,		0.0f,  0.0f,		0.0f,	0.0f,	0.0f,
		-7.7544f,	16.3667f,	10.7072f,		0.0f,  0.0f,		0.0f,	0.0f,	0.0f,
		-7.7542f,	16.3762f,	-10.6803f,		0.0f,  0.0f,		0.0f,	0.0f,	0.0f,
		-7.6040f,	-0.1024f,	10.7072f,		0.0f,  0.0f,		0.0f,	0.0f,	0.0f,
		-7.6040f,	-0.0675f,	-10.7074f,		0.0f,  0.0f,		0.0f,	0.0f,	0.0f,
		-7.6749f,	7.6919f,	-10.7074f,		0.0f,  0.0f,		0.0f,	0.0f,	0.0f,
		-7.6749f,	7.6570f,	10.7072f,		0.0f,  0.0f,		0.0f,	0.0f,	0.0f,
		-7.6170f,	1.3493f,	-10.7074f,		0.0f,  0.0f,		0.0f,	0.0f,	0.0f,
		-7.6170f,	1.3145f,	10.7072f,		0.0f,  0.0f,		0.0f,	0.0f,	0.0f,
		7.6982f,	7.9681f,	10.7072f,		0.0f,  0.0f,		0.0f,	0.0f,	0.0f,
		7.6982f,	8.0030f,	-10.7074f,		0.0f,  0.0f,		0.0f,	0.0f,	0.0f,
		7.7544f,	16.7194f,	-10.7074f,		0.0f,  0.0f,		0.0f,	0.0f,	0.0f,
		7.7544f,	16.6846f,	10.7072f,		0.0f,  0.0f,		0.0f,	0.0f,	0.0f,
		-7.7532f,	16.3654f,	10.7072f,		0.0f,  0.0f,		0.0f,	0.0f,	0.0f,
		-7.6736f,	7.6583f,	10.7072f,		0.0f,  0.0f,		0.0f,	0.0f,	0.0f,
		7.6982f,	7.9694f,	10.7072f,		0.0f,  0.0f,		0.0f,	0.0f,	0.0f,
		7.6982f,	8.0042f,	-10.7074f,		0.0f,  0.0f,		0.0f,	0.0f,	0.0f,
		-7.6736f,	7.6932f,	-10.7074f,		0.0f,  0.0f,		0.0f,	0.0f,	0.0f,
		-7.7532f,	16.4003f,	-10.7074f,		0.0f,  0.0f,		0.0f,	0.0f,	0.0f
		//
		//// front
		////x		y		z		S		T			NX		NY		NZ
		//-0.5f, -0.5f,  0.5f,	0.0f,  0.0f,		0.0f,	0.0f,	-1.0f,	//0
		//0.5f, -0.5f,  0.5f,		1.0f,	0.0f,		0.0f,	0.0f,	-1.0f,	//1
		//0.5f,  0.5f,  0.5f,		1.0f,	1.0f,		0.0f,	0.0f,	-1.0f,	//2
		//-0.5f,  0.5f,  0.5f,	0.0f,	1.0f,		0.0f,	0.0f,	-1.0f,	//3
		//// right
		////x		y		z		S		T
		//0.5f, -0.5f,  0.5f,	    0.0f,  0.0f,		-1.0f,	0.0f,	0.0f,
		//0.5f, -0.5f,  -0.5f,	1.0f,	0.0f,		-1.0f,	0.0f,	0.0f,
		//0.5f,  0.5f,  -0.5f,	1.0f,	1.0f,		-1.0f,	0.0f,	0.0f,
		//0.5f,  0.5f,  0.5f,	    0.0f,	1.0f,		-1.0f,	0.0f,	0.0f,
		//// back
		//-0.5f, -0.5f, -0.5f,	0.0f,  0.0f,		0.0f,	0.0f,	1.0f,
		//0.5f, -0.5f, -0.5f,		1.0f,	0.0f,		0.0f,	0.0f,	1.0f,
		//0.5f,  0.5f, -0.5f,		1.0f,	1.0f,		0.0f,	0.0f,	1.0f,
		//-0.5f,  0.5f, -0.5f,	0.0f,	1.0f,		0.0f,	0.0f,	1.0f,

		//// left
		////x		y		z		S		T
		//-0.5f, -0.5f,  -0.5f,	0.0f,  0.0f,		1.0f,	0.0f,	0.0f,
		//-0.5f, -0.5f,  0.5f,	1.0f,	0.0f,		1.0f,	0.0f,	0.0f,
		//-0.5f,  0.5f,  0.5f,	1.0f,	1.0f,		1.0f,	0.0f,	0.0f,
		//-0.5f,  0.5f,  -0.5f,	0.0f,	1.0f,		1.0f,	0.0f,	0.0f,

		//// bottom
		////x		y		z		S		T
		//-0.5f, -0.5f,  0.5f,	0.0f,  0.0f,		0.0f,	1.0f,	0.0f,
		//0.5f,  -0.5f,  0.5f,	1.0f,	0.0f,		0.0f,	1.0f,	0.0f,
		// 0.5f,  -0.5f,  -0.5f,	1.0f,	1.0f,		0.0f,	1.0f,	0.0f,
		//-0.5f, -0.5f,  -0.5f,	0.0f,	1.0f,		0.0f,	1.0f,	0.0f,

		////UP
		// //x		y		z		S		T
		// -0.5f, 0.5f,  0.5f,	0.0f,  0.0f,		0.0f,	-1.0f,	0.0f,
		// 0.5f,  0.5f,  0.5f,	1.0f,	0.0f,		0.0f,	-1.0f,	0.0f,
		//  0.5f, 0.5f,  -0.5f,	1.0f,	1.0f,		0.0f,	-1.0f,	0.0f,
		// -0.5f, 0.5f,  -0.5f,	0.0f,	1.0f,		0.0f,	-1.0f,	0.0f,

	};

	Mesh* dado = new Mesh();
	dado->CreateMesh(cubo_vertices, cubo_indices, 320, 72);
	meshList.push_back(dado);
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

	CreateObjects(); //3
	CrearDado(); //1
	CreateShaders();
	camera = Camera(glm::vec3(50.0f, 10.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 1.0f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	AgaveTexture = Texture("Textures/Agave.tga");
	AgaveTexture.LoadTextureA();

	Puesto_aguas = Model();
	Puesto_aguas.LoadModel("Models/modelo_aguas.obj");
	Puesto_tortas = Model();
	Puesto_tortas.LoadModel("Models/modelo_tortas.obj");
	puesto_tortas_supremo = Model();
	puesto_tortas_supremo.LoadModel("Models/puesto_tortas_supremo.obj");


	//######## MEGAMAN #############
	MegamanX_cabeza = Model();
	MegamanX_cabeza.LoadModel("Models/MegamanX/megamanX_cabeza.obj");
	MegamanX_pecho = Model();
	MegamanX_pecho.LoadModel("Models/MegamanX/megamanX_pecho.obj");
	MegamanX_torso = Model();
	MegamanX_torso.LoadModel("Models/MegamanX/megamanX_torso.obj");
	MegamanX_hombroD = Model(),
	MegamanX_hombroD.LoadModel("Models/MegamanX/megamanX_hombroD.obj");
	MegamanX_hombroI = Model();
	MegamanX_hombroI.LoadModel("Models/MegamanX/megamanX_hombroI.obj");
	MegamanX_brazoD = Model();
	MegamanX_brazoD.LoadModel("Models/MegamanX/megamanX_brazoD.obj");
	MegamanX_brazoI = Model();
	MegamanX_brazoI.LoadModel("Models/MegamanX/megamanX_brazoI.obj");
	MegamanX_manoD = Model();
	MegamanX_manoD.LoadModel("Models/MegamanX/megamanX_manoD.obj");
	MegamanX_manoI = Model();
	MegamanX_manoI.LoadModel("Models/MegamanX/megamanX_manoI.obj");
	MegamanX_buster = Model();
	MegamanX_buster.LoadModel("Models/MegamanX/megamanX_buster.obj");
	MegamanX_musloD = Model();
	MegamanX_musloD.LoadModel("Models/MegamanX/megamanX_musloD.obj");
	MegamanX_musloI = Model();
	MegamanX_musloI.LoadModel("Models/MegamanX/megamanX_musloI.obj");
	MegamanX_piernaD = Model();
	MegamanX_piernaD.LoadModel("Models/MegamanX/megamanX_piernaD.obj");
	MegamanX_piernaI = Model();
	MegamanX_piernaI.LoadModel("Models/MegamanX/megamanX_piernaI.obj");
	MegamanX_pieD = Model();
	MegamanX_pieD.LoadModel("Models/MegamanX/megamanX_pieD.obj");
	MegamanX_pieI = Model();
	MegamanX_pieI.LoadModel("Models/MegamanX/megamanX_pieI.obj");

	//Lambert
	Lambert_CabezayTorso = Model();
	Lambert_CabezayTorso.LoadModel("Models/cuerpo sagrado del cordero corazon de jesus.obj");
	Lambert_BrazoIzq = Model();
	Lambert_BrazoIzq.LoadModel("Models/manita izq cordero.obj");
	Lambert_Brazoder = Model();
	Lambert_Brazoder.LoadModel("Models/manita der corderol.obj");
	Lambert_PiernaIzq = Model();
	Lambert_PiernaIzq.LoadModel("Models/pierna izq cordero.obj");
	Lambert_Piernader = Model();
	Lambert_CabezayTorso.LoadModel("Models/pierna der cordero.obj");

	Escenario = Model();
	Escenario.LoadModel("Models/escenario.obj");


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

	unsigned int pointLightCount = 0;
	unsigned int spotLightCount = 0;


	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.5f, 0.3f, //se le mueve el ambientIntensity para que sea de dia o de noche
		0.0f, 0.0f, -1.0f); //Se mueve la dirección para hacer un efecto de movimiento de la luz del sol
	
	

	//Declaración de primer luz puntual
	pointLights[0] = PointLight(0.897f, 0.900f, 0.801f,
		0.0f, 1.0f,
		-10.0f, 1.0f, 20.0f,
		0.5f, 0.1f, 0.1f); //menos es mayor alcance
	pointLightCount++;
	
	
	// #################### SPOTLIGHTS ######################
	//linterna de camara
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;

	//luz faro delante
	spotLights[1] = SpotLight(0.502f, 0.909f, 0.930f,
		1.0f, 1.0f,
		-25.0f, 2.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		10.0f);
	spotLightCount++;

	//luz faro atras
	spotLights[2] = SpotLight(0.860f, 0.112f, 0.112f,
		1.0f, 1.0f,
		25.0f, 2.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		10.0f);
	spotLightCount++;

	//luz de helicóptero
	spotLights[3] = SpotLight(0.865f, 0.880f, 0.431f,
		1.0f, 2.0f,
		-25.0f, 30.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		25.0f);
	spotLightCount++;

	//PARA LAS LUCES APAGADAS : Linterna de camara
	lucesAvanzando[0] = spotLights[0];
	
	//Luz trasera
	lucesAvanzando[1] = SpotLight(0.502f, 0.909f, 0.930f,
		1.0f, 1.0f,
		-25.0f, 2.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		10.0f);

	//Luz trasera
	lucesAvanzando[2] = spotLights[2];

	lucesRetrocediendo[0] = spotLights[0];
	lucesRetrocediendo[1] = spotLights[1];
	lucesRetrocediendo[2] = SpotLight(0.860f, 0.112f, 0.112f,
		1.0f, 1.0f,
		25.0f, 2.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		10.0f);


	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	
	//VARIABLES PARA ANIMACION
	//movCoche = 0.0f;
	//movOffset = 0.5f;	//Velocidad de movimiento
	//rotLlanta = 0.0f;
	//rotLlantaOffset = 10.0f; //Velocidad de movimiento
	//avanza = true;

	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		/*if (avanza)
		{
			if (movCoche < 300.0f)
			{
				movCoche += movOffset * deltaTime;
				rotLlanta += rotLlantaOffset*deltaTime;
			}
			else
			{
				avanza = false;
			}
		}
		else
		{
			if (movCoche > -300.0f)
			{
				movCoche -= movOffset * deltaTime;
				rotLlanta -= rotLlantaOffset * deltaTime;
			}
			else
			{
				avanza = true;
			}
		}*/
		//Recibir eventos del usuario
		glfwPollEvents();
		if (camera.getCameraPosition().y < -5.0f) {
			if (camera.getCameraDirection().y > 0.0f) {
				camera.keyControl(mainWindow.getsKeys(), deltaTime);
			}
		}
		else {
			camera.keyControl(mainWindow.getsKeys(), deltaTime);
		}
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		if (mainWindow.getBandera()) {
			skybox_tarde.DrawSkybox(camera.calculateViewMatrix(), projection);
		}
		else {
			skybox_dia.DrawSkybox(camera.calculateViewMatrix(), projection);
		}
		//skybox_dia.DrawSkybox(camera.calculateViewMatrix(), projection);
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



		// luz ligada a la cámara de tipo flash
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);


		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::mat4 modelX(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Escenario.RenderModel();

		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		puesto_tortas_supremo.RenderModel();

		//################## JERARQUIA PARA MEGAMAN #######################
		modelX = glm::mat4(1.0);
		modelX = glm::translate(modelX, glm::vec3(-5.0f, 4.5f, -5.0f));
		modelX = glm::translate(modelX, glm::vec3(mainWindow.getMov(), 0.0f, 0.0f));
		model = modelaux = modelX;				//model tiene el punto central de Megaman
		modelX = glm::translate(modelX, glm::vec3(0.0f, 2.5f, 0.0f));
		modelaux = modelX;						//modelaux servira para los movimientos que no se quieren guardar entre secciones
		modelX = glm::rotate(modelX, mainWindow.getCabezaR() * toRadians, glm::vec3(0.0f,0.0f,1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelX));
		MegamanX_cabeza.RenderModel();

		modelX = modelaux;
		modelX = glm::translate(modelX, glm::vec3(0.0f, -0.56f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelX));
		MegamanX_pecho.RenderModel();

		modelX = glm::translate(modelX, glm::vec3(0.0f, -0.5f, 0.0f));
		modelaux = modelX;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelX));
		MegamanX_torso.RenderModel();

		modelX = glm::translate(modelX, glm::vec3(0.107f, -1.1f, 0.4f));
		modelX = glm::rotate(modelX, mainWindow.getMusloDR() * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelX));
		MegamanX_musloD.RenderModel();

		modelX = glm::translate(modelX, glm::vec3(-0.107f, -1.644f, 0.506f));
		modelX = glm::rotate(modelX, mainWindow.getPiernaDR() * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelX));
		MegamanX_piernaD.RenderModel();

		modelX = glm::translate(modelX, glm::vec3(0.0f, -3.0f, 0.672f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelX));
		MegamanX_pieD.RenderModel();

		modelX = modelaux;
		modelX = glm::translate(modelX, glm::vec3(-0.107f, -1.1f, -0.4f));
		modelX = glm::rotate(modelX, mainWindow.getMusloDR() * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelX));
		MegamanX_musloI.RenderModel();

		modelX = glm::translate(modelX, glm::vec3(0.107f, -1.644f, -0.506f));
		modelX = glm::rotate(modelX, mainWindow.getPiernaDR() * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelX));
		MegamanX_piernaI.RenderModel();

		modelX = glm::translate(modelX, glm::vec3(0.0f, -3.0f, -0.752f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelX));
		MegamanX_pieI.RenderModel();

		modelX = modelaux;
		modelX = glm::translate(modelX, glm::vec3(0.0f, 0.752f, 0.67f));
		modelX = glm::rotate(modelX, mainWindow.getMusloDR() * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelX));
		MegamanX_hombroD.RenderModel();

		modelX = glm::translate(modelX, glm::vec3(-0.1f, -0.605f, 0.605f));
		modelX = glm::rotate(modelX, mainWindow.getPiernaDR() * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelX));
		MegamanX_brazoD.RenderModel();

		modelX = glm::translate(modelX, glm::vec3(0.0f, -1.5f, 0.605f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelX));
		MegamanX_manoD.RenderModel();

		modelX = modelaux;
		modelX = glm::translate(modelX, glm::vec3(0.0f, 0.752f, -0.67f));
		modelX = glm::rotate(modelX, mainWindow.getMusloDR() * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelX));
		MegamanX_hombroI.RenderModel();

		modelX = glm::translate(modelX, glm::vec3(-0.1f, -0.905f, -0.505f));
		modelX = glm::rotate(modelX, mainWindow.getPiernaDR() * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelX));
		MegamanX_brazoI.RenderModel();

		modelX = glm::translate(modelX, glm::vec3(0.0f, -1.3f, -0.605f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelX));
		MegamanX_manoI.RenderModel();

		//if (mainWindow.getBandera()) {

		//	glm::vec3 lowerLight = camera.getCameraPosition();
		//	lowerLight.y -= 0.3f;
		//	lucesAvanzando[0].SetFlash(lowerLight, camera.getCameraDirection());

		//	glm::vec3 faroderecho = glm::vec3(-25.0f + mainWindow.getmueveaudi(), 2.0f, mainWindow.getmuevecofreaudi());
		//	lucesAvanzando[1].SetFlash(faroderecho, glm::vec3(1.0f, 0.0f, 0.0f));

		//	glm::vec3 faroizquierdo = glm::vec3(-25.0f + mainWindow.getmueveaudi(), 2.0f, mainWindow.getmuevecofreaudi());
		//	lucesAvanzando[2].SetFlash(faroizquierdo, glm::vec3(-1.0f, 0.0f, 0.0f));

		//	shaderList[0].SetSpotLights(lucesAvanzando, 3);

		//}
		
		
		//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//Agave ¿qué sucede si lo renderizan antes del coche y de la pista?
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.5f, -2.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		
		AgaveTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();
		
		glDisable(GL_BLEND);

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}

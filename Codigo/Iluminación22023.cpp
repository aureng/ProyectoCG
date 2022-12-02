/*
Semestre 2023-1
Práctica 8: Iluminación 2 
//para cargar imagen
Que queremos iluminar?
Quien emite la luz? fuentes de luz
Como se afecta el objeto?
Quien interactua?*/
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

//VARIABLES DE ANIMACION
float movCoche;
float movOffset;	//Velocidad de movimiento
float rotLlanta;
float rotLlantaOffset; //Velocidad de movimiento
bool avanza;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;
Texture DadoTexture;
Texture dadoColoresTexture;
Texture estebanDodTexture;


Model Kitt_M;
Model Llanta_M;
Model Camino_M;
Model Blackhawk_M;
Model Dado_M;
Model Dado_Colores;
Model kitt_sin_cofre;
Model cofre_kitt;
Model ToadCircuit;
Model CarroLowPoly;
Model CarroLowPoly_Llanta;
Model PaloDeLuz;
Model Tele;

Skybox skybox;

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
		// right
		4, 5, 6,
		6, 7, 4,
		// back
		8, 9, 10,
		10, 11, 8,

		// left
		12, 13, 14,
		14, 15, 12,
		// bottom
		16, 17, 18,
		18, 19, 16,
		// top
		20, 21, 22,
		22, 23, 20,
		// front
		0, 1, 2,
		2, 3, 0
	};
	//Ejercicio 1: reemplazar con sus dados de 6 caras texturizados, agregar normales
// average normals
	GLfloat cubo_vertices[] = {
		// front
		//x		y		z		S		T			NX		NY		NZ
		-0.5f, -0.5f,  0.5f,	0.0f,  0.0f,		0.0f,	0.0f,	-1.0f,	//0
		0.5f, -0.5f,  0.5f,		1.0f,	0.0f,		0.0f,	0.0f,	-1.0f,	//1
		0.5f,  0.5f,  0.5f,		1.0f,	1.0f,		0.0f,	0.0f,	-1.0f,	//2
		-0.5f,  0.5f,  0.5f,	0.0f,	1.0f,		0.0f,	0.0f,	-1.0f,	//3
		// right
		//x		y		z		S		T
		0.5f, -0.5f,  0.5f,	    0.0f,  0.0f,		-1.0f,	0.0f,	0.0f,
		0.5f, -0.5f,  -0.5f,	1.0f,	0.0f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  -0.5f,	1.0f,	1.0f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  0.5f,	    0.0f,	1.0f,		-1.0f,	0.0f,	0.0f,
		// back
		-0.5f, -0.5f, -0.5f,	0.0f,  0.0f,		0.0f,	0.0f,	1.0f,
		0.5f, -0.5f, -0.5f,		1.0f,	0.0f,		0.0f,	0.0f,	1.0f,
		0.5f,  0.5f, -0.5f,		1.0f,	1.0f,		0.0f,	0.0f,	1.0f,
		-0.5f,  0.5f, -0.5f,	0.0f,	1.0f,		0.0f,	0.0f,	1.0f,

		// left
		//x		y		z		S		T
		-0.5f, -0.5f,  -0.5f,	0.0f,  0.0f,		1.0f,	0.0f,	0.0f,
		-0.5f, -0.5f,  0.5f,	1.0f,	0.0f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  0.5f,	1.0f,	1.0f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  -0.5f,	0.0f,	1.0f,		1.0f,	0.0f,	0.0f,

		// bottom
		//x		y		z		S		T
		-0.5f, -0.5f,  0.5f,	0.0f,  0.0f,		0.0f,	1.0f,	0.0f,
		0.5f,  -0.5f,  0.5f,	1.0f,	0.0f,		0.0f,	1.0f,	0.0f,
		 0.5f,  -0.5f,  -0.5f,	1.0f,	1.0f,		0.0f,	1.0f,	0.0f,
		-0.5f, -0.5f,  -0.5f,	0.0f,	1.0f,		0.0f,	1.0f,	0.0f,

		//UP
		 //x		y		z		S		T
		 -0.5f, 0.5f,  0.5f,	0.0f,  0.0f,		0.0f,	-1.0f,	0.0f,
		 0.5f,  0.5f,  0.5f,	1.0f,	0.0f,		0.0f,	-1.0f,	0.0f,
		  0.5f, 0.5f,  -0.5f,	1.0f,	1.0f,		0.0f,	-1.0f,	0.0f,
		 -0.5f, 0.5f,  -0.5f,	0.0f,	1.0f,		0.0f,	-1.0f,	0.0f,

	};

	Mesh* dado = new Mesh();
	dado->CreateMesh(cubo_vertices, cubo_indices, 192, 36);
	meshList.push_back(dado);
}

void CrearDodecaedro()
{
	unsigned int dode_indices[] = {
		// superior
		0, 1, 2,
		0, 2, 3,
		0, 3, 4,
		// arriba frente izquierda
		8, 7, 6,
		8, 6, 5,
		8, 9, 5,
		// arriba izquierda
		13, 12, 11,
		13, 11, 10,
		13, 14, 10,
		// arriba atras
		18, 17, 16,
		18, 16, 15,
		18, 19, 15,
		// arriba derecha
		23, 22, 21,
		23, 21, 20,
		23, 24, 20,
		// arriba frente derecha
		28, 27, 26,
		28, 26, 25,
		28, 29, 25,
		// abajo frente
		33, 32, 31,
		33, 31, 30,
		33, 34, 30,
		// abajo izquierda
		38, 37, 36,
		38, 36, 35,
		38, 39, 35,
		// abajo atras izquierda
		43, 42, 41,
		43, 41, 40,
		43, 44, 40,
		// abajo atras derecha
		48, 47, 46,
		48, 46, 45,
		48, 49, 45,
		// abajo derecha
		53, 52, 51,
		53, 51, 50,
		53, 54, 50,
		// inferior
		58, 57, 56,
		58, 56, 55,
		58, 59, 55
	};

	GLfloat dode_vertices[] = {
		// superior.
		//x		y		z			S			T			NX		NY		NZ
		0.00f, 3.25f,  2.50f,		0.2539f,	0.7656f,		0.0f,	-1.0f,	0.0f,	//0
		-2.5f, 3.25f,  0.5f,		0.3535f,	0.6191f,		0.0f,	-1.0f,	0.0f,	//1
		-1.5f,  3.25f,  -2.5f,		0.3184f,	0.3848f,		0.0f,	-1.0f,	0.0f,	//2
		1.5f, 3.25f,  -2.5f,		0.1992f,	0.3848f,		0.0f,	-1.0f,	0.0f,	//3
		2.5f, 3.25f,  0.5f,			0.1563f,	0.6191f,		0.0f,	-1.0f,	0.0f,	//4
		// arriba frente izquierda.
		//x		y		z			S		T
		0.00f, 3.25f,  2.50f,		0.2617f,	0.7656f,		1.0f,	0.0f,	-1.0f,	//5 -0
		-2.5f, 3.25f,  0.5f,		0.3535f,	0.6191f,		1.0f,	0.0f,	-1.0f,	//6 -1
		-4.0f,  0.75f,  1.5f,		0.4531f,	0.7656f,		1.0f,	0.0f,	-1.0f,	//7
		-3.5f, -0.25f,  3.5f,		0.4180f,	1.0000f,		1.0f,	0.0f,	-1.0f,	//8
		0.00f,  0.75f,  4.0f,		0.2969f,	1.0000f,		1.0f,	0.0f,	-1.0f,	//9
		// arriba izquierda.
		-2.5f, 3.25f,  0.5f,		0.3613f,	0.6191f,		1.0f,	0.0f,	0.0f,	//10 -1
		-1.5f,  3.25f,  -2.5f,		0.3262f,	0.3848f,		1.0f,	0.0f,	0.0f,	//11 -2
		-2.5f,  0.75f, -3.5f,		0.4179f,	0.2441f,		1.0f,	0.0f,	0.0f,	//12
		-4.0f,	-0.25f, -1.5f,		0.5136f,	0.3848f,		1.0f,	0.0f,	0.0f,	//13
		-4.0f,  0.75f,  1.5f,		0.4746f,	0.6191f,		1.0f,	0.0f,	0.0f,	//14
		// arriba atras.
		//x		y		z			S		T
		-1.5f,  3.25f,  -2.5f,		0.3183f,	0.3848f,		0.0f,	0.0f,	1.0f,	//15 -2
		1.5f, 3.25f,  -2.5f,		0.1992f,	0.3848f,		0.0f,	0.0f,	1.0f,	//16 -3
		2.5f,  0.75f, -3.5f,		0.1641f,	0.1484f,		0.0f,	0.0f,	1.0f,	//17
		0.00f, -0.25f,  -4.0f,		0.2597f,	0.0000f,		0.0f,	0.0f,	1.0f,	//18
		-2.5f,  0.75f, -3.5f,		0.3554f,	0.1484f,		0.0f,	0.0f,	1.0f,	//19
		// arriba derecha. 
		//x		y		z			S		T
		1.5f, 3.25f,  -2.5f,		0.1953f,	0.3848f,		-1.0f,	0.0f,	0.0f,	//20 -3
		2.5f, 3.25f,  0.5f,			0.1562f,	0.6191f,		-1.0f,	0.0f,	0.0f,	//21 -4
		4.0f,  0.75f,  1.5f,		0.0391f,	0.6191f,		-1.0f,	0.0f,	0.0f,	//22
		4.0f,	-0.25f, -1.5f,		0.0000f,	0.3848f,		-1.0f,	0.0f,	0.0f,	//23
		2.5f,  0.75f, -3.5f,		0.0977f,	0.2421f,		-1.0f,	0.0f,	0.0f,	//24
		//arriba frente derecha.
		 //x		y		z		S		T
		 2.5f, 3.25f,  0.5f,		0.1582f,	0.6191f,		-1.0f,	0.0f,	-1.0f,	//25 -4
		 0.00f, 3.25f,  2.50f,		0.2558f,	0.7656f,		-1.0f,	0.0f,	-1.0f,	//26 -0
		 0.00f,  0.75f,  4.0f,		0.2187f,	1.0000f,		-1.0f,	0.0f,	-1.0f,	//27
		 3.5f, -0.25f,  3.5f,		0.1015f,	1.0000f,		-1.0f,	0.0f,	-1.0f,	//28
		 4.0f,  0.75f,  1.5f,		0.0625f,	0.7656f,		-1.0f,	0.0f,	-1.0f,	//29
		 // abajo frente.
		//x		y		z			S		T			NX		NY		NZ
		-1.5f, -3.25f,  2.5f,		0.7968f,	0.6152f,		0.0f,	0.0f,	-1.0f,	//30 -57
		1.5f, -3.25f,  2.5f,		0.6816f,	0.6152f,		0.0f,	0.0f,	-1.0f,	//31 -58
		3.5f, -0.25f,  3.5f,		0.6425f,	0.8515f,		0.0f,	0.0f,	-1.0f,	//32
		0.00f,  0.75f,  4.0f,		0.7382f,	1.0000f,		0.0f,	0.0f,	-1.0f,	//33
		-3.5f, -0.25f,  3.5f,		0.8359f,	0.8515f,		0.0f,	0.0f,	-1.0f,	//34
		// abajo izquierda.
		//x		y		z			S		T
		-2.50f,  -3.25f,  -0.5f,	0.8378f,	0.3847f,	1.0f,	0.0f,	0.0f,	//35 -56
		-1.5f, -3.25f,  2.5f,		0.8027f,	0.6152f,	1.0f,	0.0f,	0.0f,	//36 -57
		-3.5f, -0.25f,  3.5f,		0.8984f,	0.7617f,	1.0f,	0.0f,	0.0f,	//37
		-4.0f,  0.75f,  1.5f,		0.9941f,	0.6152f,	1.0f,	0.0f,	0.0f,	//38
		-4.0f,	-0.25f, -1.5f,		0.9570f,	0.3847f,	1.0f,	0.0f,	0.0f,	//39
		// abajo atras izquierda.
		0.00f, -3.25f,  -2.50f,		0.7421f,	0.2500f,	1.0f,	0.0f,	1.0f,	//40 -55
		-2.50f,  -3.25f,  -0.5f,	0.8359f,	0.3847f,	1.0f,	0.0f,	1.0f,	//41 -56
		-4.0f,	-0.25f, -1.5f,		0.9316f,	0.2500f,	1.0f,	0.0f,	1.0f,	//42
		-2.5f,  0.75f, -3.5f,		0.8984f,	0.0078f,	1.0f,	0.0f,	1.0f,	//43
		0.00f, -0.25f,  -4.0f,		0.7773f,	0.0078f,	1.0f,	0.0f,	1.0f,	//44
		// abajo atras derecha.
		//x		y		z			S		T
		2.50f, -3.25f,  -0.5f,		0.6386f,	0.3847f,	-1.0f,	0.0f,	1.0f,	//45 -59
		0.00f, -3.25f,  -2.50f,		0.7324f,	0.2500f,	-1.0f,	0.0f,	1.0f,	//46 -55
		0.00f, -0.25f,  -4.0f,		0.6972f,	0.0078f,	-1.0f,	0.0f,	1.0f,	//47
		2.5f,  0.75f, -3.5f,		0.5800f,	0.0078f,	-1.0f,	0.0f,	1.0f,	//48
		4.0f,	-0.25f, -1.5f,		0.5429f,	0.2500f,	-1.0f,	0.0f,	1.0f,	//49
		// abajo derecha
		//x		y		z			S		T
		1.5f, -3.25f,  2.5f,		0.6718f,	0.6152f,	-1.0f,	0.0f,	0.0f,	//50 -58
		2.50f, -3.25f,  -0.5f,		0.6367f,	0.3847f,	-1.0f,	0.0f,	0.0f,	//51 -59
		4.0f,	-0.25f, -1.5f,		0.5156f,	0.3847f,	-1.0f,	0.0f,	0.0f,	//52
		4.0f,  0.75f,  1.5f,		0.4843f,	0.6152f,	-1.0f,	0.0f,	0.0f,	//53
		3.5f, -0.25f,  3.5f,		0.5781f,	0.7597f,	-1.0f,	0.0f,	0.0f,	//54
		//inferior.
		 //x		y		z		S		T
		 0.00f, -3.25f,  -2.50f,	0.7402f,	0.2500f,	0.0f,	1.0f,	0.0f,	//55
		 -2.50f,  -3.25f,  -0.5f,	0.8320f,	0.3847f,	0.0f,	1.0f,	0.0f,	//56
		 -1.5f, -3.25f,  2.5f,		0.7968f,	0.6152f,	0.0f,	1.0f,	0.0f,	//57
		 1.5f, -3.25f,  2.5f,		0.6816f,	0.6152f,	0.0f,	1.0f,	0.0f,	//58
		 2.50f, -3.25f,  -0.5f,		0.6464f,	0.3847f,	0.0f,	1.0f,	0.0f	//59
	};

	Mesh* dodecaedro = new Mesh();
	dodecaedro->CreateMesh(dode_vertices, dode_indices, 480, 108);
	meshList.push_back(dodecaedro);

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
	CrearDado();
	CrearDodecaedro();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 1.0f, 0.5f);

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
	DadoTexture = Texture("Textures/dadoscolor.tga");
	DadoTexture.LoadTextureA();
	dadoColoresTexture = Texture("Textures/dadocolores.tga");
	dadoColoresTexture.LoadTextureA();
	estebanDodTexture = Texture("Textures/estebandod.tga");
	estebanDodTexture.LoadTextureA();

	Kitt_M = Model();
	Kitt_M.LoadModel("Models/kitt_optimizado.obj");
	Llanta_M = Model();
	Llanta_M.LoadModel("Models/k_rueda.3ds");
	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/uh60.obj");
	Camino_M = Model();
	Camino_M.LoadModel("Models/railroad track.obj");
	kitt_sin_cofre = Model();
	kitt_sin_cofre.LoadModel("Models/cochesincofre.obj");
	cofre_kitt = Model();
	cofre_kitt.LoadModel("Models/cofre_kitt.obj");
	Dado_Colores = Model();
	Dado_Colores.LoadModel("Models/dadocolores.obj");
	ToadCircuit = Model();
	ToadCircuit.LoadModel("Models/toadcircuit.obj");
	CarroLowPoly = Model();
	CarroLowPoly.LoadModel("Models/carrolpsinllantas.obj");
	CarroLowPoly_Llanta = Model();
	CarroLowPoly_Llanta.LoadModel("Models/llantacarrolp.obj");
	PaloDeLuz = Model();
	PaloDeLuz.LoadModel("Models/palodeluz.obj");
	Tele = Model();
	Tele.LoadModel("Models/tele.obj");

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

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
	movCoche = 0.0f;
	movOffset = 0.5f;	//Velocidad de movimiento
	rotLlanta = 0.0f;
	rotLlantaOffset = 10.0f; //Velocidad de movimiento
	avanza = true;

	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		if (avanza)
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
		}
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
		glm::mat4 modelcarro(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[2]->RenderMesh();

		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(15.0f, 38.5f, 240.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(movCoche, 0.5f, -1.5f));
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, mainWindow.getmuevecofreaudi()));
		modelaux = modelcarro = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CarroLowPoly.RenderModel();

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

		model = modelcarro;
		model = glm::translate(model, glm::vec3(7.3f, -2.6f, -4.3f));
		model = glm::rotate(model, -rotLlanta * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CarroLowPoly_Llanta.RenderModel();

		model = modelcarro;
		model = glm::translate(model, glm::vec3(7.3f, -2.6f, 6.0f));
		model = glm::rotate(model, 180*toRadians, glm::vec3(0.0f,1.0f,0.0f));
		model = glm::rotate(model, rotLlanta*toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CarroLowPoly_Llanta.RenderModel();

		model = modelcarro;
		model = glm::translate(model, glm::vec3(-6.6f, -2.6f, -4.3f));
		model = glm::rotate(model, -rotLlanta * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CarroLowPoly_Llanta.RenderModel();

		model = modelcarro;
		model = glm::translate(model, glm::vec3(-6.6f, -2.6f, 6.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotLlanta * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CarroLowPoly_Llanta.RenderModel();

		model = modelcarro;
		model = glm::translate(model, glm::vec3(0.0f, 45.0f, -1.0f));
		model = glm::translate(model, glm::vec3(0.0f, mainWindow.getmuevekitt(), 0.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glm::vec3 luzHelicoptero = glm::vec3(-25.0f + mainWindow.getmuevecoche(), 40.0f + mainWindow.getmuevekitt(), -1.0f);
		luzHelicoptero.y -= 0.3f;
		spotLights[3].SetFlash(luzHelicoptero, glm::vec3(0.0f, -1.0f, 0.0f));

		Blackhawk_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 20.0f, -100.0f));
		//model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		ToadCircuit.RenderModel();

		/*
		//DODECAEDRO
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-4.0f, 30.0f, -2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		estebanDodTexture.UseTexture();
		meshList[5]->RenderMesh();
		*/
		//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		/*
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-5.0f, 6.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		DadoTexture.UseTexture();
		meshList[4]->RenderMesh();
		*/


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

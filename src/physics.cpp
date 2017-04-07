#include <imgui\imgui.h>
#include <imgui\imgui_impl_glfw_gl3.h>
#include <GL\glew.h>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <cstdio>
#include "GL_framework.h"
#include <stdlib.h>
#include <time.h>
#include <iostream>

void Spring(int part1, int part2);
bool show_test_window = false;
int updateRange = 20;
bool euler = true;
int waterfallIncrementX = -3;
float timePerFrame = 0.033;
float lenght = 0.5;
float d;
int eixX = 14;
int eixY = 18;
int maxMesh = eixX*eixY;
float Ks = 0;
float Kd = 10;
float coefElasticity = 0.2;
float coefFriction = 0.1;
glm::vec3 temp;
glm::vec3 vTangencial;
bool collision = false;
float col;
glm::vec3 initial;
//void Spring(int part1, int part2);

glm::vec3 normal;
glm::vec3 gravity = glm::vec3(0, -9.8, 0);

void GUI() {
	{	//FrameRate
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		//TODO
	}

	// ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
	if (show_test_window) {
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
		ImGui::ShowTestWindow(&show_test_window);
	}
}

namespace LilSpheres {
	extern const int maxParticles;
	extern void setupParticles(int numTotalParticles, float radius = 0.05f);
	extern void cleanupParticles();
	extern void updateParticles(int startIdx, int count, float* array_data);
	extern void drawParticles(int startIdx, int count);
}
namespace ClothMesh {
	extern void setupClothMesh();
	extern void cleanupClothMesh();
	extern void updateClothMesh(float* array_data);
	extern void drawClothMesh();
}
namespace Sphere {
	extern glm::vec3 centro = { 0.f, 1.f, 0.f };
	extern void setupSphere(glm::vec3 pos = centro, float radius = 1.f);
	extern void cleanupSphere();
	extern void updateSphere(glm::vec3 pos, float radius = 1.f);
	extern void drawSphere();

}

struct OurShpere {
	glm::vec3 pos;
	float rad;
};

struct Particle {
	glm::vec3 pos;
	glm::vec3 lastPos;
	glm::vec3 vel;
	glm::vec3 lastVel;
	//float force;
	glm::vec3 force;
};

float *mesh = new float[3 * maxMesh];
Particle *pC = new Particle[LilSpheres::maxParticles];
OurShpere *sphere = new OurShpere();

void PhysicsInit() {
	float random = rand() % 10;
	sphere->pos = glm::vec3(1, 0, 0);
	sphere->rad = 1;
	//Grid
	for (int i = 0; i < LilSpheres::maxParticles; ++i) {
		//particlesContainer[i].pos = glm::vec3(arra[i * 3], arra[i * 3 + 1], arra[i * 3 + 2]) = glm::vec3(-5 + lenght * (i % 14) , 10 - lenght * (i / 14), 0);

		mesh[3 * i + 0] = pC[i].pos.x = -3 + lenght * (i % 14);
		mesh[3 * i + 1] = pC[i].pos.y = 9;
		mesh[3 * i + 2] = pC[i].pos.z = (14 * lenght) / 2 + lenght - lenght * (i / 14);

		pC[i].vel = glm::vec3(0.f, 0.f, 0.f);
		pC[i].force = glm::vec3(0.f, 0.f, 0.f);

	}

	//sphere->pos = glm::vec3(random, random, random);//glm::vec3(Sphere::centro.x*random, Sphere::centro.y*random, Sphere::centro.z*random);
	//sphere->rad = abs(rand());
	//if(sphere->pos <= )
	srand(time(NULL));
	ClothMesh::updateClothMesh(mesh);
	//Sphere::setupSphere(sphere->pos, sphere->rad);


	//float random = rand() % 100;
	//Sphere::setupSphere(Sphere::centro*random, abs(rand()));
	//Sphere::updateSphere(Sphere::centro*random, abs(rand()));

}


void PhysicsUpdate(float dt) {

	

	//euler
	for (int i = 0; i < maxMesh; i++)
	{
		//calcular en una variable on en quina fila i columna es troba la i.
		//segons a on esta, dirli si pot anar dreta o esquerra, dalt o baix.
		//sumatori forces

//------SPRINGS----------------------------------------------------------------------
		int fila = i / eixX;
		int column = i % eixX;

		int diagLeftDown = i + eixX - 1;
		int diagLeftUp = i - eixX - 1;
		int diagRightDown = i + eixX + 1;
		int diagRightUp = i - eixX + 1;
		int up = i - eixX;
		int down = i + eixX;
		int right = i + 1;
		int left = i - 1;
		int twoDown = i + 2 * eixX;
		int twoUp = i - 2 * eixX;
		int twoLeft = i - 2;
		int twoRight = i + 2;
		int twoDiagLeftUp = i - (2*eixX - 1);
		int twoDiagRightUp = i - (2 * eixX + 1);
		int twoDiagLeftDown = i + (2 * eixX - 1);
		int twoDiagRightDown = i + (2 * eixX + 1);

		pC[i].force = gravity;

		if (fila == 0) {
			if (column == 0) {
				//structural
				Spring(i, right);
				Spring(i, down);

				//shear
				Spring(i, diagRightDown);

				//bending
				Spring(i, twoRight);
				Spring(i, twoDown);

			}
			if (column == (eixX - 1)) {
				//structural
				Spring(i, left);
				Spring(i, down);

				//shear
				Spring(i, diagLeftDown);

				//bending
				Spring(i, twoDown);
			}
			else {
				//structural
				Spring(i, right);
				Spring(i, left);
				Spring(i, down);

				//shear
				Spring(i, diagLeftDown);
				Spring(i, diagRightDown);

				//bending
				if (column % 2 == 0) {
					Spring(i, twoRight);
					Spring(i, twoLeft);
					Spring(i, twoDown);
				}
				if (column % 2 != 0) {
					Spring(i, twoDown);
				}
			}

		}
		else if (fila == (eixY - 1)) {
			if (column == 0) {
				//structural
				Spring(i, right);
				Spring(i, up);

				//shear
				Spring(i, diagRightUp);
				
				//bending
				//Spring(i, twoRight);	
			}
			if (column == (eixX - 1)) {
				//structural
				Spring(i, left);
				Spring(i, up);

				//shear
				Spring(i, diagLeftUp);

				//bending

			}
			else {
				//structural
				Spring(i, right);
				Spring(i, left);
				//std::cout << up;
				Spring(i, up);
				Spring(i, right);
				Spring(i, left);

				//shear
				Spring(i, diagRightUp);
				Spring(i, diagLeftUp);
				
				//bending
				/*if (column % 2 == 0) {
					Spring(i, twoRight);
					Spring(i, twoLeft);
				}*/

				
			}
		}
		else {
			if (column == 0) {
				//structural
				Spring(i, right);
				Spring(i, down);
				Spring(i, up);

				//shear
				Spring(i, diagRightDown);
				Spring(i, diagRightUp);
				
				//bending
				/*if (fila % 2 == 0) {
					Spring(i, twoUp);
					Spring(i, twoDown);
					Spring(i, twoRight);
				}
				if (fila % 2 != 0) {
					Spring(i, twoRight);
				}*/	
			}
			if (column == (eixX - 1)) {
				//structural
				Spring(i, left);
				Spring(i, down);
				Spring(i, up);

				//shear
				Spring(i, diagLeftDown);
				Spring(i, diagLeftUp);

				//bending
				/*if (fila % 2 == 0) {
					Spring(i, twoUp);
					Spring(i, twoDown);
				}*/

			}
			else {

				//structural
				Spring(i, right);
				Spring(i, left);
				Spring(i, down);
				Spring(i, up);

				//shear
				Spring(i, diagLeftUp);
				Spring(i, diagLeftDown);
				Spring(i, diagRightUp);
				Spring(i, diagRightDown);

				//bending
				/*if ((column % 2 == 0) && (fila % 2 == 0)) {
					Spring(i, twoUp);
					Spring(i, twoDown);
					Spring(i, twoLeft);
					Spring(i, twoRight);
				}
				else if ((column % 2 != 0) && (fila % 2 == 0)) {
					Spring(i, twoUp);
					Spring(i, twoDown);
				}
				else if ((column % 2 == 0) && (fila % 2 != 0)) {
					Spring(i, twoLeft);
					Spring(i, twoRight);
				}*/
			}
			//else pC[i].force = gravity;
		}

		//----------FIXED POINTS----------------------------------------
		initial = pC[i].pos;
		if (i == 0 || i == (eixX - 1)) {
			pC[i].pos = initial;
			pC[i].vel = { 0, 0, 0 };
		}
		else {
			pC[i].pos = pC[i].pos + pC[i].vel *dt;
			pC[i].vel = pC[i].vel + pC[i].force* dt;
		}

		//------------COLISIONS--------------------------------------
				// Floor Colision
		normal = glm::vec3(0, 1, 0);
		temp = glm::dot(normal, pC[i].vel) * normal;
		d = 0;
		if ((glm::dot(normal, pC[i].pos) + d)*((glm::dot(normal, initial) + d)) < 0) {
			pC[i].pos = pC[i].pos - (1 + coefElasticity) * (glm::dot(normal, pC[i].pos) + d)*normal;
			pC[i].vel = pC[i].vel - (1 + coefElasticity) * (glm::dot(normal, pC[i].vel))* normal - coefFriction*(pC[i].vel - temp);
		}



		// Top Colision 
		temp = glm::dot(normal, pC[i].vel) * normal;
		normal = glm::vec3(0, -1, 0);
		d = 10;
		if ((glm::dot(normal, pC[i].pos) + d)*((glm::dot(normal, initial) + d)) < 0) {
			pC[i].pos = pC[i].pos - (1 + coefElasticity) * (glm::dot(normal, pC[i].pos) + d)*normal;
			pC[i].vel = pC[i].vel - (1 + coefElasticity) * (glm::dot(normal, pC[i].vel))* normal - coefFriction*(pC[i].vel - temp);
		}


		// Right Face Colision
		normal = glm::vec3(-1, 0, 0);
		temp = glm::dot(normal, pC[i].vel) * normal;
		d = 5;
		if ((glm::dot(normal, pC[i].pos) + d)*((glm::dot(normal, initial) + d)) < 0) {
			pC[i].pos = pC[i].pos - (1 + coefElasticity) * (glm::dot(normal, pC[i].pos) + d)*normal;
			pC[i].vel = pC[i].vel - (1 + coefElasticity) * (glm::dot(normal, pC[i].vel))* normal - coefFriction*(pC[i].vel - temp);
		}


		// Left Face Colision
		normal = glm::vec3(1, 0, 0);
		temp = glm::dot(normal, pC[i].vel) * normal;
		d = 5;
		if ((glm::dot(normal, pC[i].pos) + d)*((glm::dot(normal, initial) + d)) < 0) {
			pC[i].pos = pC[i].pos - (1 + coefElasticity) * (glm::dot(normal, pC[i].pos) + d)*normal;
			pC[i].vel = pC[i].vel - (1 + coefElasticity) * (glm::dot(normal, pC[i].vel))* normal - coefFriction*(pC[i].vel - temp);

		}


		// Front Face Colision
		normal = glm::vec3(0, 0, -1);
		temp = glm::dot(normal, pC[i].vel) * normal;
		d = 5;
		if ((glm::dot(normal, pC[i].pos) + d)*((glm::dot(normal, initial) + d)) < 0) {
			pC[i].pos = pC[i].pos - (1 + coefElasticity) * (glm::dot(normal, pC[i].pos) + d)*normal;
			pC[i].vel = pC[i].vel - (1 + coefElasticity) * (glm::dot(normal, pC[i].vel))* normal - coefFriction*(pC[i].vel - temp);

		}


		// Back Face Colision
		normal = glm::vec3(0, 0, 1);
		temp = glm::dot(normal, pC[i].vel) * normal;
		d = 5;
		//int deb = -(particlesContainer[i].pos.x*normal.x) - (particlesContainer[i].pos.y*normal.y) - (particlesContainer[i].pos.z*normal.z);
		//std::cout << deb;
		if ((glm::dot(normal, pC[i].pos) + d)*((glm::dot(normal, initial) + d)) < 0) {
			pC[i].pos = pC[i].pos - (1 + coefElasticity) * (glm::dot(normal, pC[i].pos) + d)*normal;
			pC[i].vel = pC[i].vel - (1 + coefElasticity) * (glm::dot(normal, pC[i].vel))* normal - coefFriction*(pC[i].vel - temp);

		}

		//Shpere Colision
		float A = ((pC[i].pos.x - initial.x) + (pC[i].pos.y - initial.y) + (pC[i].pos.z - initial.z));
		float B = 4 * ((pC[i].pos.x - initial.x) * (initial.x + sphere->pos.x) + (pC[i].pos.y - initial.y) * (initial.x + sphere->pos.y) + (pC[i].pos.z - initial.z) * (initial.x + sphere->pos.z));
		float C = 2 * ((initial.x * sphere->pos.x) + (initial.y * sphere->pos.y) + (initial.z * sphere->pos.z)) + (glm::pow(initial.x, 2) + glm::pow(initial.y, 2) + glm::pow(initial.z, 2) + glm::pow(sphere->pos.x, 2) + glm::pow(sphere->pos.y, 2) + glm::pow(sphere->pos.z, 2));
		//equacio 2n grau
		/*if ((-B + glm::sqrt(glm::pow(B, 2)) - 4 * A*C) / (2 * A) < 0 || (-B - glm::sqrt(glm::pow(B, 2)) - 4 * A*C) / (2 * A) < 0) {
			normal = glm::normalize(sphere->pos);
			//normal = sphere->pos / (glm::sqrt(glm::pow(sphere->pos.x, 2) + glm::pow(sphere->pos.y, 2) + glm::pow(sphere->pos.z, 2)));
			//normal = glm::vec3(particlesContainer[i].pos - sphere->pos);
			temp = glm::dot(normal, pC[i].vel) * normal;
			d = -(pC[i].pos.x*normal.x) - (pC[i].pos.y*normal.y) - (pC[i].pos.z*normal.z);

			pC[i].pos = pC[i].pos - (1 + coefElasticity) * (glm::dot(normal, pC[i].pos) + d)*normal;
			pC[i].vel = pC[i].vel - (1 + coefElasticity) * (glm::dot(normal, pC[i].vel))* normal - coefFriction*(pC[i].vel - temp);
		}*/
		if (glm::sqrt(glm::pow(pC[i].pos.x - sphere->pos.x, 2) + glm::pow(pC[i].pos.y - sphere->pos.y, 2) + glm::pow(pC[i].pos.z - sphere->pos.z, 2)) < sphere->rad)
		{
			normal = glm::normalize(pC[i].pos - sphere->pos);
			//normal = sphere->pos / (glm::sqrt(glm::pow(sphere->pos.x, 2) + glm::pow(sphere->pos.y, 2) + glm::pow(sphere->pos.z, 2)));
			//normal = glm::vec3(pC[i].pos - sphere->pos);
			//normal = glm::normalize(normal);
			temp = glm::dot(normal, pC[i].vel) * normal;
			d = -(pC[i].pos.x*normal.x) - (pC[i].pos.y*normal.y) - (pC[i].pos.z*normal.z);

			pC[i].pos = pC[i].pos - (1 + coefElasticity) * (glm::dot(normal, pC[i].pos) + d)*normal;
			pC[i].vel = pC[i].vel - (1 + coefElasticity) * (glm::dot(normal, pC[i].vel))* normal - coefFriction*(pC[i].vel - temp);
		}

		mesh[3 * i + 0] = pC[i].pos.x;
		mesh[3 * i + 1] = pC[i].pos.y;
		mesh[3 * i + 2] = pC[i].pos.z;
	}
	Sphere::updateSphere(sphere->pos, sphere->rad);
	ClothMesh::updateClothMesh(mesh);

}
void Spring(int part1, int part2) {
	glm::vec3 distAB = pC[part1].pos - pC[part2].pos;
	float modul = glm::sqrt(glm::pow(pC[part1].pos.x - pC[part2].pos.x, 2) + glm::pow(pC[part1].pos.y - pC[part2].pos.y, 2) + glm::pow(pC[part1].pos.z - pC[part2].pos.z, 2));
	glm::vec3 nAB = distAB / modul;

	glm::vec3 f1, f2;

	f1 = -((Ks * (modul - lenght) + Kd * (pC[part1].vel - pC[part2].vel)) * nAB)*nAB;
	f2 = -f1;
	//if(part1 == 0) pC[part1].force = f1;
	//else {
	pC[part1].force += f1;
	pC[part2].force += f2;
	//}
	/*pC[part1].force += -(Ke * (modul - lenght) + Kd * (pC[part1].vel - pC[part2].vel) * nAB)*nAB + gravity;
	pC[part2].force += -pC[part1].force;*/

	/*pC[part1].force -= (Ke * (modul - lenght) + Kd * (pC[part1].vel - pC[part2].vel) * nAB)*nAB + gravity;
	pC[part2].force -= pC[part1].force;*/

}

void PhysicsCleanup() {

	//TODO
	delete[] mesh;
	delete[] pC;
}

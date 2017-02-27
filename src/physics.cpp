#include <imgui\imgui.h>
#include <imgui\imgui_impl_glfw_gl3.h>
#include <glm\gtx\rotate_vector.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\constants.hpp>

bool show_test_window = false;
bool useEuler = true;

int partArrayLastPos;//posicion del array donde inicializar la siguiente particula
int partArrayFirstPos;//posicion de la primera particula a renderizar


namespace Sphere {
	extern void setupSphere(glm::vec3 pos = glm::vec3(0.f, 1.f, 0.f), float radius = 1.f);
	extern void cleanupSphere();
	extern void updateSphere(glm::vec3 pos, float radius = 1.f);
	extern void drawSphere();
}
namespace Capsule {
	extern void setupCapsule(glm::vec3 posA = glm::vec3(-3.f, 2.f, -2.f), glm::vec3 posB = glm::vec3(-4.f, 2.f, 2.f), float radius = 1.f);
	extern void cleanupCapsule();
	extern void updateCapsule(glm::vec3 posA, glm::vec3 posB, float radius = 1.f);
	extern void drawCapsule();
}
namespace LilSpheres {
	extern const int maxParticles;
	extern float lifeTime;
	extern void setupParticles(int numTotalParticles, float radius = 0.05f, float lifeT=3.0f);
	extern void cleanupParticles();
	extern void updateParticles(int startIdx, int count, float* array_data);
	extern void drawParticles(int startIdx, int count);
}

struct particlesInfo{
	glm::vec3 position;
	glm::vec3 speed;
	glm::vec3 acceleration;
	float radius;
	float timeAlive;
};

particlesInfo* partArray;
float* toUpdate;

class emitter {
public:
	emitter() {}
	virtual void generateParticles(float& dt)=0;
	static void setIterators(int first, int last) {
		partArrayFirstPos = first;
		partArrayLastPos = last;
	}

protected:
	glm::vec3 position;
	float apertureAngle;
	float particlesRadius;
	float particlesSpeed;
	int intensity;//cantidad de particulas/s genereadas

};

class PointEmitter: emitter {
public:
	PointEmitter() {
		//colocamos el emisor en el centro
		position.x =0;
		position.y = 5;
		position.z = 0;
		//apuntamos hacia abajo (y hacemos el vector direccion unitario)
		direction.x = 0;
		direction.y = -1;
		direction.z = 0;
		//seteamos el primero de los vectores de rotacion (perpendicular al primero)
		rotateVec1.x = direction.y;
		rotateVec1.y = -direction.x;
		rotateVec1.z = 0;
		//seteamos el segundo vector de rotacion (perpendicular a los otros 2 por ser un cross product de ellos)
		rotateVec2=glm::cross(direction,rotateVec1);
		//seteamos las caracteristicas de las particulas que emitira
		particlesRadius = 0.05;
		particlesSpeed = 5;
		//seteamos la apertura del emisor
		apertureAngle = glm::radians<float>(45.0f);
		//seteamos la intensidad
		intensity = 100;
	}
	 void generateParticles(float& dt) override {
		int particlesToGenerate = intensity*dt;
		for (int i = 0; i < particlesToGenerate; i++) {
			//colocamos las nuevas particulas en el inicio
			partArray[partArrayLastPos].position.x = position.x;
			partArray[partArrayLastPos].position.y = position.y;
			partArray[partArrayLastPos].position.z = position.z;
			//seteamos sus velocidades usando un random dentro de los limites del angulo de apertura
			//primero lo giramos un angulo random entre 0 y el angulo maximo, despues giramos el vector sobre el segundo vector de rotacion
			//en un angulo random (entre 0 y 360).
			//una vez hecho esto, como el vector inicial era unitario este tambien lo serà, y asi lo podemos multiplicar por la constante particlesspeed que tenemos
			partArray[partArrayLastPos].speed = particlesSpeed*glm::rotate(glm::rotate(direction, ((float)rand() / RAND_MAX)*apertureAngle,rotateVec1),((float)rand()/RAND_MAX)*2*glm::pi<float>(),rotateVec2);
			
			//seteamos la aceleracion
			partArray[partArrayLastPos].acceleration.x = 0;
			partArray[partArrayLastPos].acceleration.y = -9.8;
			partArray[partArrayLastPos].acceleration.z = 0;

			//seteamos el tamaño de esta esfera 
			partArray[partArrayLastPos].radius = particlesRadius;

			//seteamos el tiempo de vida de la esfera a 0
			partArray[partArrayLastPos].timeAlive = 0;
			//aumentamos la posicion del iterador del array, si es demasiado grande volvemos al principio
			if (partArrayLastPos++>=LilSpheres::maxParticles) {
				partArrayLastPos = 0;
			}
			
		}
	}
	
	
private:
	glm::vec3 direction;
	glm::vec3 rotateVec1;
	glm::vec3 rotateVec2;
};

PointEmitter fountain;

void updateParticlesEuler(float& dt) {
	//usamos un for desde la primmera particula viva y ultima inicializada.
	for (int i =partArrayFirstPos; i <= partArrayLastPos; i++) {
		//update de la velocidad
		partArray[i].speed.x = partArray[i].speed.x+(dt*partArray[i].acceleration.x);
		partArray[i].speed.y = partArray[i].speed.y+(dt*partArray[i].acceleration.y);
		partArray[i].speed.z = partArray[i].speed.z+(dt*partArray[i].acceleration.z);
		
		//update de la posicion
		partArray[i].position.x = partArray[i].position.x + (dt*partArray[i].speed.x);
		partArray[i].position.y = partArray[i].position.y + (dt*partArray[i].speed.y);
		partArray[i].position.z = partArray[i].position.z + (dt*partArray[i].speed.z);

		//update del tiempo de vida
		partArray[i].timeAlive += dt;
	}
}
void killParticles(float& dt) {
	bool stillGoing=true;
	int i = partArrayFirstPos;
	while (stillGoing) {
		
		if (partArray[i].timeAlive>=LilSpheres::lifeTime) {
			partArrayFirstPos++;
		}
		else {
			stillGoing = false;
		}
		i++;
	}
}
void updateIntermediateArray() {
	for (int i = 0; i < LilSpheres::maxParticles;i++) {
		toUpdate[i * 3 + 0] = partArray[i].position.x;
		toUpdate[i * 3 + 1] = partArray[i].position.y;
		toUpdate[i * 3 + 2] = partArray[i].position.z;
	}
	
}

void GUI() {
	{	//FrameRate
		int partAlive = partArrayLastPos - partArrayFirstPos;
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text("El numero de particulas vivas es:%i",partAlive);
		//TODO
	}

	// ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
	if(show_test_window) {
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
		ImGui::ShowTestWindow(&show_test_window);
	}
}

void PhysicsInit() {
	//TODO
	emitter::setIterators(0,0);
	partArray = new particlesInfo[LilSpheres::maxParticles];
	toUpdate = new float[LilSpheres::maxParticles * 3];
}
void PhysicsUpdate(float dt) {
	fountain.generateParticles(dt);
	killParticles(dt);
	updateParticlesEuler(dt);
	updateIntermediateArray();

	if (partArrayFirstPos <= partArrayLastPos) {
		LilSpheres::updateParticles(partArrayFirstPos*3, partArrayLastPos - partArrayFirstPos, toUpdate);
	}
	else {
		LilSpheres::updateParticles(0, partArrayLastPos, toUpdate);
		LilSpheres::updateParticles(partArrayFirstPos * 3, LilSpheres::maxParticles - partArrayFirstPos, toUpdate);
	}
	
}
void PhysicsCleanup() {
	//TODO
	delete[] partArray;
}
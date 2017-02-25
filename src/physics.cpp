#include <imgui\imgui.h>
#include <imgui\imgui_impl_glfw_gl3.h>
#include <glm\gtx\rotate_vector.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>

bool show_test_window = false;
bool useEuler = true;

class emitter {
public:
	emitter() {
		//colocamos el emisor en el centro
		position.x =0;
		position.y = 5;
		position.z = 0;
		//apuntamos hacia abajo (y hacemos el vector direccion unitario)
		direction.x = 0;
		direction.y = 1;
		direction.z = 0;
		//seteamos las caracteristicas de las particulas que emitira
		particlesRadius = 0.05;
		particlesSpeed = 5;
		//seteamos la apertura del emisor
		apertureAngle = 10;
	}
	void generateParticles() {
		
	}
private:
	glm::vec3 position;
	glm::vec3 direction;
	int apertureAngle;
	float particlesRadius;
	float particlesSpeed;
	static int partArrayIterator;//posicion del array donde inicializar la siguiente particula
	static int partArrayFirstInit;//posicion de la primera particula a renderizar
	static int intensity;//cantidad de particulas/s genereadas

	 

};

void GUI() {
	{	//FrameRate
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

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
}
void PhysicsUpdate(float dt) {
	//TODO

}
void PhysicsCleanup() {
	//TODO
}
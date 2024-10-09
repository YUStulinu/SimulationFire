#ifndef RENDERER_H
#define RENDERER_H

#include <SDL.h>
#include <gl\glew.h>
#include <glm\gtc\matrix_transform.hpp>
#include <string>
#include <sstream>
#include "ParticleSystem.h"
#include "Helper.h"
#include <GLFW/glfw3.h>

using namespace std;

// Clasa de randare
class Renderer
{
public:
	SDL_Window* window;
	int  windowWidth;
	int  windowHeight;
	int deltaTime;
	int mods;
	float yaw;
	float pitch;

	ParticleSystem* particleSystem;
	SDL_GLContext glContext;
	GLuint shaderModelMat;
	GLuint shaderViewMat;
	GLuint shaderProjMat;
	GLuint shaderOrthoMat;
	GLuint colourRedVar;
	GLuint colourGreenVar;
	GLuint colourBlueVar;
	GLuint pFixed;
	glm::mat4 perspective;
	glm::mat4 ortho;
	glm::mat4 view;
	glm::mat4 projection;

	int numFrames;
	int lastTime;

	Renderer();
	~Renderer();
	void initiate();
	void setShaders();
	void renderParticles();
	void update();
	void OnInputUpdate(float deltaTime, int mods);
	void OnKeyPress(int key, int mods);
	void OnKeyRelease(int key, int mods);
	void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY);
	void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods);
	void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods);
	void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY);
	void OnWindowResize(int width, int height);

	bool isRunning;

	// Alte functii si variabile necesare
private:
	float cameraPosX;
	float cameraPosY;
	float cameraPosZ;

	bool keys[GLFW_KEY_LAST + 1] = { false }; // Starea tastelor
	bool mouseButtons[GLFW_MOUSE_BUTTON_LAST + 1] = { false }; // Starea butoanelor mouse-ului
	int mouseScrollOffsetX = 0; // Offset-ul de derulare pe axa X
	int mouseScrollOffsetY = 0; // Offset-ul de derulare pe axa Y
};

#endif
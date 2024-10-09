#include "Renderer.h"
#include "Camera.h"

// Crearea camerei
Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));

// Crearea ferestrei pentru dispunere si functiile pentru randare
Renderer::Renderer() {
	this->windowWidth = 1500;
	this->windowHeight = 800;
	yaw = -90.0f;
	pitch = 0.0f;
	window = SDL_CreateWindow("Simulator Foc", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, this->windowWidth, this->windowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	glContext = SDL_GL_CreateContext(window);
	SDL_GL_SetSwapInterval(1);
	this->numFrames = 0;
	this->lastTime = SDL_GetTicks();
	initiate();
}

Renderer::~Renderer() {
	SDL_GL_DeleteContext(glContext);
	SDL_DestroyWindow(window);
}

// Initiere randare
void Renderer::initiate() {
	glewExperimental = GL_TRUE;
	glewInit();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);
	glClearColor(0.1, 0.1, 0.1, 1);
	setShaders();
}

// Setarea shader-elor in randare
void Renderer::setShaders() {
	GLuint program = glCreateProgram();

	string vertexString = loadShader("Imports/Shaders/vertexShader.glsl");
	string fragString = loadShader("Imports/Shaders/fragmentShader.glsl");

	const char* vertexShader = vertexString.c_str();
	const char* fragmentShader = fragString.c_str();

	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderID, 1, &vertexShader, NULL);
	glCompileShader(vertexShaderID);
	glAttachShader(program, vertexShaderID);

	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderID, 1, &fragmentShader, NULL);
	glCompileShader(fragmentShaderID);
	glAttachShader(program, fragmentShaderID);

	GLuint VERTEX_BUFFER = 0;
	GLuint COLOUR_BUFFER = 1;
	GLuint TEXTURE_BUFFER = 2;
	glBindAttribLocation(program, VERTEX_BUFFER, "position");
	glBindAttribLocation(program, COLOUR_BUFFER, "colour");
	glBindAttribLocation(program, TEXTURE_BUFFER, "texCoord");
	glLinkProgram(program);
	glUseProgram(program);


	this->perspective = glm::perspective(45.0f, 4.0f / 3.0f, 1.0f, 5000.0f);
	ortho = glm::ortho(0.0f, (float)this->windowWidth, 0.0f, (float)this->windowHeight);
	glm::mat4 view = glm::mat4();
	glm::mat4 model = glm::mat4();

	this->colourGreenVar = glGetUniformLocation(program, "colourGreenVar");
	this->colourRedVar = glGetUniformLocation(program, "colourRedVar");
	this->colourBlueVar = glGetUniformLocation(program, "colourBlueVar");

	this->pFixed = glGetUniformLocation(program, "pFixed");
	this->shaderModelMat = glGetUniformLocation(program, "modelMatrix");
	glUniformMatrix4fv(this->shaderModelMat, 1, GL_FALSE, (float*)&model);
	this->shaderViewMat = glGetUniformLocation(program, "viewMatrix");
	glUniformMatrix4fv(this->shaderViewMat, 1, GL_FALSE, (float*)&view);
	this->shaderProjMat = glGetUniformLocation(program, "projMatrix");
	glUniformMatrix4fv(this->shaderProjMat, 1, GL_FALSE, (float*)&this->perspective);

	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&model);
	glUniformMatrix4fv(glGetUniformLocation(program, "viewMatrix"), 1, GL_FALSE, (float*)&view);
	glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_FALSE, (float*)&this->perspective);

	GLint sampler = glGetUniformLocation(program, "tex0");
	glUniform1i(sampler, 0);
}

// Actulaizarea randarii
void Renderer::update() {
	// Stergere buffere la valorile prestabilite
	glClear(GL_COLOR_BUFFER_BIT);

	this->renderParticles();

	// Actualizare fereastra cu redare OpenGL
	SDL_GL_SwapWindow(this->window);
	this->particleSystem->update();
}

// Optimizarea randarii particulelor
void Renderer::renderParticles() {
	glUniform1f(this->pFixed, false);
	glBindVertexArray(this->particleSystem->particleVAO_ID);
	glBindTexture(GL_TEXTURE_2D, this->particleSystem->parseTexture);
	glUniformMatrix4fv(this->shaderProjMat, 1, GL_FALSE, (float*)&this->perspective);

	for (const auto& particle : this->particleSystem->particles) {
		glUniformMatrix4fv(this->shaderModelMat, 1, GL_FALSE, (float*)&particle.modelMatrix);

		// Optimizarea culorilor
		if (particle.lifeSpan <= 0.3) {
			glUniform1f(this->colourRedVar, 1.0f);
			glUniform1f(this->colourBlueVar, 1.0f);
			glUniform1f(this->colourGreenVar, 1.0f);
		}
		else {
			glUniform1f(this->colourGreenVar, particle.lifeSpan / 0.255f);
			glUniform1f(this->colourRedVar, particle.lifeSpan / -1.35f);
			glUniform1f(this->colourBlueVar, 0);
		}

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 80); // Randare optimizata

	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

float cameraPosX = 0.0f;
float cameraPosY = 0.0f;
float cameraPosZ = 0.0f;

// Modificati variabilele globale pentru pozitia camerei
glm::vec3 cameraPosition(0.0f, 0.0f, 5.0f);
glm::vec3 cameraFront(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);

// Definirea vitezei camerei
static const float cameraSpeed = 0.1f;

// Actualizarea starii inputului pentru miscarea camerei in spatiul 3D
void Renderer::OnInputUpdate(float deltaTime, int mods) {
	// Controlul camerei pe axele X si Z
	if (keys[GLFW_KEY_W]) {
		cameraPosition += cameraSpeed * cameraFront * deltaTime;
	}
	if (keys[GLFW_KEY_S]) {
		cameraPosition -= cameraSpeed * cameraFront * deltaTime;
	}
	if (keys[GLFW_KEY_A]) {
		cameraPosition -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed * deltaTime;
	}
	if (keys[GLFW_KEY_D]) {
		cameraPosition += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed * deltaTime;
	}
}

// Actualizarea starii tastelor
void Renderer::OnKeyPress(int key, int mods) {
	// Setarea starii tastei la apasat
	keys[key] = true;

	// Mutarea camerei in fata, in spate, la stanga si la dreapta folosind tastele sageti
	const float cameraSpeed = 0.1f;
	if (keys[GLFW_KEY_UP])
		cameraPosition += cameraSpeed * cameraFront;
	if (keys[GLFW_KEY_DOWN])
		cameraPosition -= cameraSpeed * cameraFront;
	if (keys[GLFW_KEY_LEFT])
		cameraPosition -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (keys[GLFW_KEY_RIGHT])
		cameraPosition += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void Renderer::OnKeyRelease(int key, int mods) {
	// Setarea starii tastei la eliberata
	keys[key] = false;
}

void Renderer::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) {
	// Rotirea camerei in jurul scenei folosind miscarea mouse-ului
	const float sensitivity = 0.10f;
	deltaX *= sensitivity;
	deltaY *= sensitivity;

	yaw += deltaX;
	pitch += deltaY;

	// Contrain pitch-ul pentru a evita problemele de rotatie excesiva
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);

};

void Renderer::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) {
	// Setarea starii butonului mouse-ului la apasat
	mouseButtons[button] = true;
}

void Renderer::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) {
	// Setarea starii butonului mouse-ului la eliberat
	mouseButtons[button] = false;
}

void Renderer::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) {
	// Actualizarea offsetului de derulare a mouse-ului
	mouseScrollOffsetX = offsetX;
	mouseScrollOffsetY = offsetY;
}

void Renderer::OnWindowResize(int width, int height) {
	// Actualizare perspectiva de vizualizare
	glViewport(0, 0, width, height);
	projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
}
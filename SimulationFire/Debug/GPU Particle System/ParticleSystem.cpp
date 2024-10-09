#include "ParticleSystem.h"
#include "Helper.h"
#include <SDL.h>
#include <algorithm>
#include <SOIL.h>


// Contructor default pentru Sistemul de Particule
ParticleSystem::ParticleSystem() : isInitiated(true) {
	setParticles();

	gravity.x = 0;
	gravity.y = 8;
	gravity.z = 0;

	friction = 0.0f;

	lastTime = SDL_GetTicks();
}

void ParticleSystem::setParticles() {
	mapTexture(parseTexture, "Imports/Textures/fire.jpg");

	glm::vec2 texture[80][4];
	glm::vec3 position[80][4];
	glm::vec4 colour[80][4];
	float high = 2, low = -2;
	for (int i = 0; i < 4; i++) {
		float t1 = randomFloatBetween(0.f, 0.225f);
		float t2 = randomFloatBetween(0.f, 0.225f);
		float t3 = randomFloatBetween(0.f, 0.225f);
		texture[i][0] = glm::vec2(0.f, t1);
		texture[i][1] = glm::vec2(0.f, 0.f);
		texture[i][2] = glm::vec2(t2, t2);
		texture[i][3] = glm::vec2(t3, 0.f);
		for (int j = 0; j < 4; j++) {
			float r1 = randomFloatBetween(low, high);
			float r2 = randomFloatBetween(low, high);
			float r3 = randomFloatBetween(0, 1);
			float r4 = randomFloatBetween(-40.f, -50.f);
			position[i][j] = glm::vec3(r1, r2, r4);
			colour[i][j] = glm::vec4(1.0, 1.0, 0.0, isInBetween(r4));
		}
	}


	// Generare nume de obiecte per matrice 
	glGenVertexArrays(1, &particleVAO_ID);

	// Legare obiect de o matrice
	glBindVertexArray(particleVAO_ID);

	// Creare si legare buffer pentru datele de pozitie, culoare si textura
	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Calcularea dimensiunii totale a datelor
	int totalSize = 4 * 80 * (sizeof(glm::vec3) + sizeof(glm::vec4) + sizeof(glm::vec2));
	glBufferData(GL_ARRAY_BUFFER, totalSize, nullptr, GL_STATIC_DRAW);

	// Setarea pointerelor de atribute pentru datele de pozitie, culoare si textura
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)(80 * sizeof(glm::vec3)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)((80 * sizeof(glm::vec3)) + (80 * sizeof(glm::vec4))));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	// Umplerea buffer-ului cu datele corespunzatoare
	glBufferSubData(GL_ARRAY_BUFFER, 0, 80 * sizeof(glm::vec3), position);
	glBufferSubData(GL_ARRAY_BUFFER, 80 * sizeof(glm::vec3), 80 * sizeof(glm::vec4), colour);
	glBufferSubData(GL_ARRAY_BUFFER, (80 * sizeof(glm::vec3)) + (80 * sizeof(glm::vec4)), 80 * sizeof(glm::vec2), texture);

	glBindVertexArray(0);
}


// Update la Sistemul de Particule
void ParticleSystem::update(){
	int thisTime = SDL_GetTicks();
	deltaTime = (float)(thisTime - lastTime) / 1000;
	lastTime = thisTime;


	// Verificare daca o particula trebuie stearsa
	for (std::vector<Particle>::iterator i = this->particles.begin(); i != this->particles.end();) {
		i->velocity = computeVelocity(i->velocity, i->acceleration + gravity, deltaTime);
		i->position = computeEuler(i->position, i->velocity, i->acceleration + gravity, deltaTime);
		i->lifeSpan -= deltaTime;
		if (i->lifeSpan <= 0.0f)
			i = this->particles.erase(i);
		else ++i;
	}


	// Apelare functie de update din Particle, care modifica pozitia particulelor
	for (std::vector<Particle>::iterator i = particles.begin(); i != particles.end(); ++i)
		i->update();

	// Sortare particule, in functie de z-index
	for (int i = 0; i < deltaTime * 3000; ++i) this->particles.push_back(Particle());
	std::sort(this->particles.begin(), this->particles.end(), Particle::sortParticles);
}
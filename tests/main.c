
#include <cglm/util.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../src/glib.h"
#include "main.h"

int main()  {
	struct Window window;
	struct Scene scene;
	struct Camera camera;
	struct Mouse mouse;

	initWindow(&window, 1280, 720, "Rendering_library", 120);
	initScene(&scene, 4);
	initCamera(&camera, (vec3){0.0, 0.0, 10.0}, -90.0, 0.0, 90.0, 0.1, 1000.0);
	initMouse(&mouse, 0.2);

	generateSphere(&scene.models[0], 10, 10);
	generateSphere(&scene.models[1], 25, 25);
	generateSphere(&scene.models[2], 50, 50);
	generateSphere(&scene.models[3], 100, 100);
	initModel(&scene.models[0], 1);
	initModel(&scene.models[1], 1);
	initModel(&scene.models[2], 1);
	initModel(&scene.models[3], 1);

	for (uint i = 0; i < 4; i++) 
		scene.models[i].shader = createShaderProgram("./shaders/vertex.glsl", NULL, "./shaders/fragment.glsl");

	initInstance(&scene.models[0].instances[0], (vec3){-7.5, 0.0, 0.0}, (vec3){0.0, 0.0, 0.0}, (vec3){1.0, 1.0, 1.0}, 0.0);
	initInstance(&scene.models[1].instances[0], (vec3){-2.5, 0.0, 0.0}, (vec3){0.0, 0.0, 0.0}, (vec3){1.0, 1.0, 1.0}, 0.0);
	initInstance(&scene.models[2].instances[0], (vec3){2.5, 0.0, 0.0}, (vec3){0.0, 0.0, 0.0}, (vec3){1.0, 1.0, 1.0}, 0.0);
	initInstance(&scene.models[3].instances[0], (vec3){7.5, 0.0, 0.0}, (vec3){0.0, 0.0, 0.0}, (vec3){1.0, 1.0, 1.0}, 0.0);

	window.userPtr.camera = &camera;
	window.userPtr.mouse = &mouse;

	scene.camera = &camera;

	float lastTime = glfwGetTime();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DEPTH_TEST);
	glfwSwapInterval(0);
	while(!glfwWindowShouldClose(window.frame)) {	
		glClearColor(0.01, 0.0, 0.03, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		delayFrame(window.lastFrame, window.targetFrameLength);
		updateDeltaTime(&window.currentFrame, &window.lastFrame, &window.deltaTime);

		//printf("Fps: %f\n", 1.0 / window.deltaTime);

		processKeyInput(&window, &camera);

		renderScene(&scene, &window);

		glfwSwapBuffers(window.frame);
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}

// Model

void generateCube(struct Model *model, float scale) {
	model->vertexCount = 8;
	model->indiceCount = 36;

	model->verticies = generateCubeVerticies(scale);
	model->indices = generateCubeIndices();
}

void generateSphere(struct Model *model, int verticalSlices, int horizontalSlices) {
	model->vertexCount = verticalSlices * (horizontalSlices - 1) + 2;
	model->indiceCount = verticalSlices * (horizontalSlices - 1) * 6 + 3;

	model->verticies = generateSphereVerticies(verticalSlices, horizontalSlices);
	model->indices = generateSphereIndices(verticalSlices, horizontalSlices);
}

void initScene(struct Scene *scene, int modelCount) {
	scene->modelCount = modelCount;
	scene->models = malloc(sizeof(struct Model) * scene->modelCount);
}

void initModel(struct Model *model, int instanceCount) {
	generateModelArrays(&model->VAO, &model->VBO, &model->EBO, model->vertexCount, model->indiceCount, model->verticies, model->indices);

	model->instanceCount = instanceCount;
	model->instances = malloc(sizeof(struct Instance) * model->instanceCount);
}

void initInstance(struct Instance *instance, vec3 position, vec3 rotation, vec3 scale, float rotationScale) {
	if (position)
		glm_vec3_copy(position, instance->position);
	else
		glm_vec3_zero(instance->position);

	if (rotation)
		glm_vec3_copy(rotation, instance->rotation);
	else
		glm_vec3_zero(instance->rotation);

	if (scale)
		glm_vec3_copy(scale, instance->scale);
	else
		glm_vec3_zero(instance->scale);

	instance->rotationScale = rotationScale;
}



// Render

void renderScene(struct Scene *scene, struct Window *window) {
	struct Camera *camera = scene->camera;

	setProjectionMat(scene->projection, glm_rad(camera->fov), window->width/window->height, camera->nearPlane, camera->farPlane);
	setViewMat(scene->view, camera->position, camera->front, camera->target, camera->up);

	for (uint i = 0; i < scene->modelCount; i++)
		renderModel(&scene->models[i], scene->view, scene->projection);
}

void renderModel(struct Model *model, mat4 viewMat, mat4 projectionMat) {
	glUseProgram(model->shader);

	shaderSetMat4(model->shader, "view", GL_FALSE, viewMat);
	shaderSetMat4(model->shader, "projection", GL_FALSE, projectionMat);

	glBindVertexArray(model->VAO);

	for (uint i = 0; i < model->instanceCount; i++)
		renderInstance(&model->instances[i], model->shader, model->indiceCount);

	glBindVertexArray(0);
}

void renderInstance(struct Instance *instance, uint shaderProgram, uint indiceCount) {
	setModelMat(instance->model, instance->position, instance->rotation, instance->scale, instance->rotationScale);
		
	shaderSetMat4(shaderProgram, "model", GL_FALSE, instance->model);

	glDrawElements(GL_TRIANGLES, indiceCount, GL_UNSIGNED_INT, 0);
}

// Controls

void processKeyInput(struct Window *window, struct Camera *camera) {
	GLFWwindow *frame = window->frame;

	if(isKeyPressed(frame, GLFW_KEY_ESCAPE))
		glfwSetWindowShouldClose(frame, 1);

	if(isKeyPressed(frame, GLFW_KEY_TAB) && window->tabDelay < glfwGetTime())
		toggleCursorMode(window);

	float moveSpeed = 5.0 * window->deltaTime;
	float turnSpeed = 75.0 * window->deltaTime;
	float zoomSpeed = 50.0 * window->deltaTime;

	if(isKeyPressed(frame, GLFW_KEY_LEFT_SHIFT))
		moveSpeed *= 5.0;

	if(isKeyPressed(frame, GLFW_KEY_W))
		cameraMoveZ(camera->up, camera->front, camera->right, camera->position, moveSpeed);
	if(isKeyPressed(frame, GLFW_KEY_S))
		cameraMoveZ(camera->up, camera->front, camera->right, camera->position, -moveSpeed);
	if(isKeyPressed(frame, GLFW_KEY_A))
		cameraMoveX(camera->up, camera->front, camera->right, camera->position, moveSpeed);
	if(isKeyPressed(frame, GLFW_KEY_D))
		cameraMoveX(camera->up, camera->front, camera->right, camera->position, -moveSpeed);
	if(isKeyPressed(frame, GLFW_KEY_E))
		cameraMoveY(camera->up, camera->front, camera->right, camera->position, moveSpeed);
	if(isKeyPressed(frame, GLFW_KEY_Q))
		cameraMoveY(camera->up, camera->front, camera->right, camera->position, -moveSpeed);

	if(isKeyPressed(frame, GLFW_KEY_RIGHT))
		cameraTurnYaw(camera->front, &camera->yaw, &camera->pitch, &camera->roll, turnSpeed);
	if(isKeyPressed(frame, GLFW_KEY_LEFT))
		cameraTurnYaw(camera->front, &camera->yaw, &camera->pitch, &camera->roll, -turnSpeed);
	if(isKeyPressed(frame, GLFW_KEY_UP))
		cameraTurnPitch(camera->front, &camera->yaw, &camera->pitch, &camera->roll, turnSpeed);
	if(isKeyPressed(frame, GLFW_KEY_DOWN))
		cameraTurnPitch(camera->front, &camera->yaw, &camera->pitch, &camera->roll, -turnSpeed);
	if(isKeyPressed(frame, GLFW_KEY_C))
		cameraZoom(camera->fov,  zoomSpeed);
	if(isKeyPressed(frame, GLFW_KEY_Z))
		cameraZoom(camera->fov, -zoomSpeed);
}
void toggleCursorMode(struct Window *window) {
	window->tabDelay = glfwGetTime() + 0.1;
	glfwSetInputMode(window->frame, GLFW_CURSOR,
		glfwGetInputMode(window->frame, GLFW_CURSOR) == GLFW_CURSOR_DISABLED?
		GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED
	);

	if (glfwGetInputMode(window->frame, GLFW_CURSOR) == GLFW_CURSOR_NORMAL)
		return;

	struct WindowUserPointer *windowPtr = glfwGetWindowUserPointer(window->frame);
	windowPtr->mouse->firstMouse = true;
}

void initMouse(struct Mouse *mouse, const float sensitivity) {
	mouse->firstMouse = true;
	mouse->sensitivity = sensitivity;
}

// Camera

void initCamera(struct Camera *camera, vec3 position, float yaw, float pitch, float fov, float nearPlane, float farPlane) {
	glm_vec3_copy(position, camera->position);
	glm_vec3_copy((vec3){0.0, 1.0, 0.0}, camera->up);

	camera->yaw = yaw;
	camera->pitch = pitch;
	camera->roll = 0.0;

	camera->fov = fov;

	camera->nearPlane = 0.1;
	camera->farPlane = 1000.0;

	glm_vec3_copy((vec3){
		cos(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch)),
		sin(glm_rad(camera->pitch)),
		sin(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch)),
	}, camera->front);

	glm_normalize(camera->front);
}

// Other

void mouse_callback(GLFWwindow* frame, double xPos, double yPos) {
	if (glfwGetInputMode(frame, GLFW_CURSOR) == GLFW_CURSOR_NORMAL)
		return;

	struct WindowUserPointer *windowPtr = glfwGetWindowUserPointer(frame);
	struct Mouse *mouse = windowPtr->mouse;
	struct Camera *camera = windowPtr->camera;

	if (mouse->firstMouse) {
		mouse->lastX = xPos;
		mouse->lastY = yPos;

		mouse->firstMouse = false;
	}

	float xOffset = xPos - mouse->lastX;
	float yOffset = mouse->lastY - yPos;
	xOffset *= mouse->sensitivity;
	yOffset *= mouse->sensitivity;

	mouse->lastX = xPos;
	mouse->lastY = yPos;

	cameraTurnYaw(camera->front, &camera->yaw, &camera->pitch, &camera->roll, xOffset);
	cameraTurnPitch(camera->front, &camera->yaw, &camera->pitch, &camera->roll, yOffset);
}

void scroll_callback(GLFWwindow *frame, double xOffset, double yOffset) {
	struct WindowUserPointer *windowPtr = glfwGetWindowUserPointer(frame);
	struct Camera *camera = windowPtr->camera;

	cameraZoom(camera->fov, 2 * yOffset);
}



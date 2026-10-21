
#include <cglm/cglm.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "stdlib.h"

struct Instance {
	vec3 position;
	vec3 rotation;
	vec3 scale;

	float rotationScale;

	mat4 model;
};

struct Model {
	struct Instance *instances;
	int instanceCount;
	
	int shader;

	uint VAO;
	uint VBO;
	uint EBO;

	struct Vertex *verticies;
	uint *indices;

	uint vertexCount;
	uint indiceCount;
};

struct Scene {
	struct Model *models;
	int modelCount;

	struct Camera *camera;

	mat4 view;
	mat4 projection;
};

struct Mouse {
	double lastX;
	double lastY;

	float sensitivity;
	bool firstMouse;
};

struct Camera {
	vec3 position;
	vec3 front;
	vec3 right;
	vec3 up;
	vec3 target;

	float yaw;
	float pitch;
	float roll;

	float fov;

	float nearPlane;
	float farPlane;
};

void generateCube(struct Model *model, float scale);
void generateSphere(struct Model *model, int verticalSlices, int horizontalSlices);

void initScene(struct Scene *scene, int modelCount);
void initModel(struct Model *model, int instanceCount);
void initInstance(struct Instance *instance, vec3 position, vec3 rotation, vec3 scale, float rotationScale);

void renderScene(struct Scene *scene, struct Window *window);
void renderModel(struct Model *model, mat4 viewMat, mat4 projectionMat);
void renderInstance(struct Instance *instance, uint shaderProgram, uint indiceCount);

void processKeyInput(struct Window *window, struct Camera *camera);
void toggleCursorMode(struct Window *window);
void initMouse(struct Mouse *mouse, const float sensitivity);

void initCamera(struct Camera *camera, vec3 position, float yaw, float pitch, float fov, float nearPlane, float farPlane);

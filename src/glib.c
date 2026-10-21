
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>

#include "glib.h"

// WINDOW_C

void initWindow(struct Window *window, const int width, const int height, const char *name, const float fps) {
	window->width = width;
	window->height = height;
	window->name = name;
	window->targetFps = fps;
	window->targetFrameLength = 1.0 / fps;

	window->frame = createWindow(window->width, window->height, window->name, (void*)&window->userPtr);
}

GLFWwindow *createWindow(const int width, const int height, const char *name, void *userPointer) {
	glfwInit();
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow *frame = glfwCreateWindow(width, height, name, NULL, NULL);
	if (frame == NULL) {
		printf("Failed to create window\n");
		return NULL;
	}

	glfwSetWindowUserPointer(frame, userPointer);

	glfwMakeContextCurrent(frame);
	glfwSetFramebufferSizeCallback(frame, framebuffer_size_callback);  
	glfwSetCursorPosCallback(frame, mouse_callback);
	glfwSetScrollCallback(frame, scroll_callback);

	glfwSetInputMode(frame, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		printf("Failed to load GLAD\n");
		return NULL;
	}

	return frame;
}

void framebuffer_size_callback(GLFWwindow *frame, const int width, const int height) {
	glViewport(0, 0, width, height);
} 

// SHADER_C

uint createShaderProgram(const char *vertexShaderPath, const char *geometryShaderPath, const char *fragmentShaderPath) {
	uint shaderProgram = glCreateProgram();

	if (vertexShaderPath) loadShader(shaderProgram, vertexShaderPath, GL_VERTEX_SHADER);
	if (geometryShaderPath) loadShader(shaderProgram, geometryShaderPath, GL_GEOMETRY_SHADER);
	if (fragmentShaderPath) loadShader(shaderProgram, fragmentShaderPath, GL_FRAGMENT_SHADER);

	return shaderProgram;
}

void loadShader(const uint shaderProgram, const char *shaderPath, const int shaderType) {
	uint shader;
	int success;

	const char *shaderContent = getShaderContent(shaderPath);
	if (!shaderContent) return;

	shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &shaderContent, NULL);
	glCompileShader(shader);

	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char infoLog[1024];
		glGetShaderInfoLog(shader, 1024, NULL, infoLog);
		printf("Shader compilation error\nError log\n%s\nEnd\n", infoLog);

		goto freeShaderMemory;
	}

	glAttachShader(shaderProgram, shader);
	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    	if (!success) {
		char infoLog[1024];
        	glGetProgramInfoLog(shaderProgram, 1024, NULL, infoLog);
        	printf("Shader linking error\nError log\n%s\nEnd\n", infoLog);

		goto freeShaderMemory;
    	}

freeShaderMemory:

	freeShader(shaderContent, shader);
}

char *getShaderContent(const char *shaderFileName) {
	FILE *fPtr = fopen(shaderFileName, "r");
	if (!fPtr)
		return NULL;

	char *shaderContent = readFileToArray(fPtr);

	fclose(fPtr);

	return shaderContent;
}

char *readFileToArray(FILE *fPtr) {
	int size = 1024;
	char *array = malloc(sizeof(char) * size);
	if (!array)
		return NULL;

	char buffer = 0;
	int i = 0;
	for (; (buffer = fgetc(fPtr)) != EOF; i++) {
		if (i >= size) {
			size *= 2;
			array = (char*)resizeArray(array, sizeof(char) * size);
		}
		
		array[i] = buffer;
	}

	array[i] = '\0';

	return array;
}

void* resizeArray(void *array, uint size) {
	void *temp = realloc(array, size);
	return temp;
}

void freeShader(const char *shaderSource, uint shader) {
	free((char*)shaderSource);
	glDeleteShader(shader);
}

// RENDER_C

void setProjectionMat(mat4 projection, float fov, float aspectRatio, float nearPlane, float farPlane) {
	glm_mat4_identity(projection);

	glm_perspective(fov, aspectRatio, nearPlane, farPlane, projection);
}

void setViewMat(mat4 view, vec3 position, vec3 front, vec3 target, vec3 up) {
	glm_mat4_identity(view);

	glm_vec3_add(position, front, target);
	glm_lookat(position, target, up, view);
}

void setModelMat(mat4 model, vec3 position, vec3 rotation, vec3 scale, float rotationScale) {
	glm_mat4_identity(model);

	glm_translate(model, position);
	glm_rotate(model, rotationScale, rotation);
	glm_scale(model, scale);
}

// MODEL_C


void generateModelArrays(uint *VAO, uint *VBO, uint *EBO, uint vCount, uint iCount, struct Vertex *verticies, uint *indices) {
	glGenVertexArrays(1, VAO);
	glGenBuffers(1, VBO);
	glGenBuffers(1, EBO);

	glBindVertexArray(*VAO);

	glBindBuffer(GL_ARRAY_BUFFER, *VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(struct Vertex) * vCount, &verticies[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * iCount, indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (void*)offsetof(struct Vertex, normal));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (void*)offsetof(struct Vertex, texture));
	glEnableVertexAttribArray(2);
}




struct Vertex *generateCubeVerticies(float scale) {
	struct Vertex *verticies;
	verticies = malloc((sizeof(struct Vertex) * 8));

	float edge = scale / 2;

	glm_vec3_copy((vec3){-edge, -edge, edge}, verticies[0].position);
	glm_vec3_copy((vec3){edge, -edge, edge}, verticies[1].position);
	glm_vec3_copy((vec3){-edge, edge, edge}, verticies[2].position);
	glm_vec3_copy((vec3){edge, edge, edge}, verticies[3].position);
	glm_vec3_copy((vec3){-edge, -edge, -edge}, verticies[4].position);
	glm_vec3_copy((vec3){edge, -edge, -edge}, verticies[5].position);
	glm_vec3_copy((vec3){-edge, edge, -edge}, verticies[6].position);
	glm_vec3_copy((vec3){edge, edge, -edge}, verticies[7].position);

	return verticies;
}

uint *generateCubeIndices() {
	uint *indices;
	indices = malloc(sizeof(uint) * 36);
	
	uint temp[] = {
		0, 1, 2,
		1, 2, 3,
		4, 5, 6,
		5, 6, 7,

		0, 1, 4,
		1, 4, 5,
		2, 3, 6,
		3, 6, 7,

		1, 3, 5,
	   	3, 5, 7,
		0, 2, 4,
		2, 4, 6
	};

	for (uint i = 0; i < 36; i++)
		indices[i] = temp[i];

	return indices;
}


struct Vertex *generateSphereVerticies(int verticalSlices, int horizontalSlices) {
	uint vertexCount = verticalSlices * (horizontalSlices - 1) + 2;
	struct Vertex *verticies = malloc(sizeof(struct Vertex) * vertexCount);

	glm_vec3_copy((vec3){0.0, 1.0, 0.0}, verticies[0].position);

	uint m = 1;
	for (uint h = 0; h < horizontalSlices - 1; h++) {
		float phi = (h + 1) * (M_PI / horizontalSlices);

		for (uint v = 0; v < verticalSlices; v++) {
			float theta = v * (2.0 * M_PI / verticalSlices);

			glm_vec3_copy((vec3){sin(phi) * cos(theta), cos(phi), sin(phi) * sin(theta)}, verticies[m].position);
			m++;
		}
	}

	glm_vec3_copy((vec3){0.0, -1.0, 0.0}, verticies[m].position);

	return verticies;
}

uint *generateSphereIndices(int verticalSlices, int horizontalSlices) {
	uint vertexCount = verticalSlices * (horizontalSlices - 1) + 2;
	uint indiceCount = verticalSlices * (horizontalSlices - 1) * 6 + 3;
	uint *indices = malloc(sizeof(uint) * indiceCount);

	uint x = 1;
	uint y = x + horizontalSlices;
	uint m = 0;

	// Add the last, first layer triangle
	// TODO; FIX THIS AWFUL CODE
	indices[m++] = 1;
	indices[m++] = verticalSlices;
	indices[m++] = y;

	for (uint i = 1; i < (horizontalSlices - 1); i++) {
		for (uint j = 0; j < verticalSlices; j++) {
			y = x + horizontalSlices;
			indices[m++] = x;
			indices[m++] = x + 1;
			indices[m++] = y;

			indices[m++] = x + 1;
			indices[m++] = y;
			indices[m++] = y + 1;
			x++;
		}
	}

	// Add top triangles
	x = 1;
	for (uint i = 0; i < verticalSlices - 1; i++) {
		indices[m++] = 0;
		indices[m++] = x;
		indices[m++] = x + 1;
		x++;
	}

	indices[m++] = 0;
	indices[m++] = 1;
	indices[m++] = x;

	// Add bottom triangles
	x = vertexCount - 2;
	for (uint i = 0; i < verticalSlices - 1; i++) {
		indices[m++] = vertexCount - 1;
		indices[m++] = x;
		indices[m++] = x - 1;
		x--;
	}

	indices[m++] = vertexCount - 1;
	indices[m++] = vertexCount - 2;
	indices[m++] = x;

	return indices;
}

void floatToVec3(float a, vec3 dest) {
	glm_vec3_copy((vec3){a, a, a}, dest);
}

// CONTROLS_C



// CAMERA_C

void cameraMoveX(vec3 up, vec3 front, vec3 right, vec3 position, float moveSpeed) {
	vec3 move;
	glm_vec3_cross(up, front, right);
	glm_normalize(right);
	glm_vec3_scale(right, moveSpeed, move);
	glm_vec3_add(position, move, position);
}

void cameraMoveY(vec3 up, vec3 front, vec3 right, vec3 position, float moveSpeed) {
	vec3 move;
	glm_vec3_scale(up, moveSpeed, move);
	glm_vec3_add(position, move, position);
}

void cameraMoveZ(vec3 up, vec3 front, vec3 right, vec3 position, float moveSpeed) {
	vec3 move;
	glm_vec3_scale(front, moveSpeed, move);
	glm_vec3_add(position, move, position);
}

void cameraTurnYaw(vec3 front, float *yaw, float *pitch, float *roll, float turnSpeed) {
	*yaw += turnSpeed;

	glm_vec3_copy((vec3){
		cos(glm_rad(*yaw)) * cos(glm_rad(*pitch)),
		sin(glm_rad(*pitch)),
		sin(glm_rad(*yaw)) * cos(glm_rad(*pitch)),
	}, front);

	glm_normalize(front);
}

void cameraTurnPitch(vec3 front, float *yaw, float *pitch, float *roll, float turnSpeed) {
	if (*pitch + turnSpeed >= 90.0 || *pitch + turnSpeed <= -90.0)
		return;

	*pitch += turnSpeed;

	glm_vec3_copy((vec3){
		cos(glm_rad(*yaw)) * cos(glm_rad(*pitch)),
		sin(glm_rad(*pitch)),
		sin(glm_rad(*yaw)) * cos(glm_rad(*pitch)),
	}, front);

	glm_normalize(front);
}

void cameraTurnRoll(vec3 front, float yaw, float pitch, float roll, float turnSpeed) {

}

void cameraZoom(float fov, float zoomSpeed) {
	if (fov - zoomSpeed >= 180 || fov - zoomSpeed <= 0)
		return;

	fov -= zoomSpeed;
}



#ifndef MAIN_H
#define MAIN_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

#include <stdlib.h>
#include <stdio.h>

#ifndef WINDOW_H
#define WINDOW_H

GLFWwindow *createWindow(const int height, const int width, const char *name, void *userPointer);

void framebuffer_size_callback(GLFWwindow *frame, int width, int height);
void mouse_callback(GLFWwindow *frame, double xPos, double yPos);
void scroll_callback(GLFWwindow *frame, double xOffset, double yOffset);

static void delayFrame(double lastFrame, float targetFrameLength) {
	while (glfwGetTime() < lastFrame + targetFrameLength) {}
}

static void updateDeltaTime(double *currentFrame, double *lastFrame, double *deltaTime) {
	*currentFrame = glfwGetTime();
	*deltaTime = *currentFrame - *lastFrame;
	*lastFrame = *currentFrame;
}

#endif // WINDOW_H

#ifndef SHADER_H 
#define SHADER_H

uint createShaderProgram(const char *vertexShaderPath, const char *geometryShaderPath, const char *fragmentShaderPath);
void loadShader(const uint shaderProgram, const char *shaderPath, const int shaderType);
char *getShaderContent(const char *shaderFileName);
char *readFileToArray(FILE *fPtr);
void *resizeArray(void *array, uint size);
void freeShader(const char *shaderSource, uint shader);

static inline void shaderSetMat4(uint shaderProgram, const char *name, int gl_bool, mat4 data) {	
	int location = glGetUniformLocation(shaderProgram, name);
	glUniformMatrix4fv(location, 1, gl_bool, (float*)data);
}
static inline void shaderSetVec3(uint shaderProgram, const char *name, vec3 data) {
	int location = glGetUniformLocation(shaderProgram, name);
	glUniform3fv(location, 1, (float*)data);
}
static inline void shaderSetVec2(uint shaderProgram, const char *name, vec2 data) {
	int location = glGetUniformLocation(shaderProgram, name);
	glUniform2fv(location, 1, (float*)data);
}
static inline void shaderSetFloat(uint shaderProgram, const char *name, float data) {
	int location = glGetUniformLocation(shaderProgram, name);
	glUniform1f(location, data);
}
static inline void shaderSetInt(uint shaderProgram, const char *name, int data) {
	int location = glGetUniformLocation(shaderProgram, name);
	glUniform1i(location, data);
}
static inline void shaderSetUInt(uint shaderProgram, const char *name, uint data) {
	int location = glGetUniformLocation(shaderProgram, name);
	glUniform1ui(location, data);
}

#endif // SHADER_H

#ifndef RENDER_H
#define RENDER_H

void setProjectionMat(mat4 projection, float fov, float aspectRatio, float nearPlane, float farPlane);
void setViewMat(mat4 view, vec3 position, vec3 front, vec3 target, vec3 up);
void setModelMat(mat4 model, vec3 position, vec3 rotation, vec3 scale, float rotationScale);


#endif // RENDER_H

#ifndef MODEL_H
#define MODEL_H

struct Vertex {
	vec3 position;
	vec3 normal;
	vec2 texture;
};

void generateModelArrays(uint *VAO, uint *VBO, uint *EBO, uint vCount, uint iCount, struct Vertex *verticies, uint *indices);

struct Vertex *generateCubeVerticies(float scale);
uint *generateCubeIndices();

struct Vertex *generateSphereVerticies(int verticalSlices, int horizontalSlices);
uint *generateSphereIndices(int verticalSlices, int horizontalSlices);

void floatToVec3(float a, vec3 dest);

#endif // MODEL_H

#ifndef CONTROLS_H
#define CONTROLS_H

static inline bool isKeyPressed(GLFWwindow *frame, int key) {
	return glfwGetKey(frame, key) == GLFW_PRESS;
}

#endif // CONTROLS_H

#ifndef CAMERA_H
#define CAMERA_H

void cameraMoveX(vec3 up, vec3 front, vec3 right, vec3 position, float moveSpeed);
void cameraMoveY(vec3 up, vec3 front, vec3 right, vec3 position, float moveSpeed);
void cameraMoveZ(vec3 up, vec3 front, vec3 right, vec3 position, float moveSpeed);
void cameraTurnYaw(vec3 front, float *yaw, float *pitch, float *roll, float turnSpeed);
void cameraTurnPitch(vec3 front, float *yaw, float *pitch, float *roll, float turnSpeed);
void cameraTurnRoll(vec3 front, float *yaw, float *pitch, float *roll, float turnSpeed);
void updateCameraFront(vec3 front, float yaw, float pitch, float roll);
void cameraZoom(float *fov, float zoomSpeed);

#endif // CAMERA_H

#endif // MAIN_H

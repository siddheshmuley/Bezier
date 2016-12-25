// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <array>
#include <sstream>
#include <math.h>
// Include GLEW
#include <GL/glew.h>
// Include GLFW
#include <glfw3.h>
// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
using namespace glm;
// Include AntTweakBar
#include <AntTweakBar.h>

#include <common/shader.hpp>
#include <common/controls.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>

typedef struct Vertex {
	float XYZW[4];
	float RGBA[4];
	void SetCoords(float *coords) {
		XYZW[0] = coords[0];
		XYZW[1] = coords[1];
		XYZW[2] = coords[2];
		XYZW[3] = coords[3];
	}
	void SetColor(float *color) {
		RGBA[0] = color[0];
		RGBA[1] = color[1];
		RGBA[2] = color[2];
		RGBA[3] = color[3];
	}
};

// ATTN: USE POINT STRUCTS FOR EASIER COMPUTATIONS

// function prototypes
int initWindow(void);
void initOpenGL(void);
void createVAOs(Vertex[], unsigned short[], size_t, size_t, int);
void pickVertex(void);
void moveVertex(void);
void drawScene(void);
void cleanup(void);
void bezier(void);
static void mouseCallback(GLFWwindow*, int, int, int);
static void keyCallback(GLFWwindow*, int, int, int, int);

// GLOBAL VARIABLES
GLFWwindow* window;
const GLuint window_width = 1024, window_height = 768;

glm::mat4 gProjectionMatrix;
glm::mat4 gViewMatrix, gViewMatrix2;

GLuint gPickedIndex;
std::string gMessage;

GLuint programID;
GLuint pickingProgramID;

int bezierFlag = 0, shiftFlag = 0, pointPosition = 0, normalsFlag = 0;
bool dualScreenEnabled = false;

// ATTN: INCREASE THIS NUMBER AS YOU CREATE NEW OBJECTS
const GLuint NumObjects = 7;	// number of different "objects" to be drawn
GLuint VertexArrayId[NumObjects] = { 0, 1, 2, 3, 4, 5, 6 };
GLuint VertexBufferId[NumObjects] = { 0, 1, 2, 3, 4, 5, 6 };
GLuint IndexBufferId[NumObjects] = { 0, 1, 2, 3, 4, 5, 6 };
size_t NumVert[NumObjects] = { 0, 1, 2, 3, 4, 5, 6 };

GLuint MatrixID;
GLuint ViewMatrixID;
GLuint ModelMatrixID;
GLuint PickingMatrixID;
GLuint pickingColorArrayID;
GLuint pickingColorID;
GLuint LightID;

// Define objects
Vertex Vertices[] =
{
	{ { 1.0f, 1.0f, 0.0f, 1.0f },{ 0.0f, 1.0f, 0.0f, 1.0f } }, // 0
	{ { 0.0f, 1.4f, 0.0f, 1.0f },{ 0.0f, 1.0f, 0.0f, 1.0f } }, // 1
	{ { -1.0f, 1.0f, 0.0f, 1.0f },{ 0.0f, 1.0f, 0.0f, 1.0f } }, // 2
	{ { -1.4f, 0.0f, 0.0f, 1.0f },{ 0.0f, 1.0f, 0.0f, 1.0f } }, // 3
	{ { -1.0f, -1.0f, 0.0f, 1.0f },{ 0.0f, 1.0f, 0.0f, 1.0f } }, // 4
	{ { 0.0f, -1.4f, 0.0f, 1.0f },{ 0.0f, 1.0f, 0.0f, 1.0f } },// 5
	{ { 1.0f, -1.0f, 0.0f, 1.0f },{ 0.0f, 1.0f, 0.0f, 1.0f } }, // 6
	{ { 1.4f, 0.0f, 0.0f, 1.0f },{ 0.0f, 1.0f, 0.0f, 1.0f } },// 7
};
Vertex OriginalVertices[] =
{
	{ { 1.0f, 1.0f, 0.0f, 1.0f },{ 0.0f, 1.0f, 0.0f, 1.0f } }, // 0
	{ { 0.0f, 1.4f, 0.0f, 1.0f },{ 0.0f, 1.0f, 0.0f, 1.0f } }, // 1
	{ { -1.0f, 1.0f, 0.0f, 1.0f },{ 0.0f, 1.0f, 0.0f, 1.0f } }, // 2
	{ { -1.4f, 0.0f, 0.0f, 1.0f },{ 0.0f, 1.0f, 0.0f, 1.0f } }, // 3
	{ { -1.0f, -1.0f, 0.0f, 1.0f },{ 0.0f, 1.0f, 0.0f, 1.0f } }, // 4
	{ { 0.0f, -1.4f, 0.0f, 1.0f },{ 0.0f, 1.0f, 0.0f, 1.0f } },// 5
	{ { 1.0f, -1.0f, 0.0f, 1.0f },{ 0.0f, 1.0f, 0.0f, 1.0f } }, // 6
	{ { 1.4f, 0.0f, 0.0f, 1.0f },{ 0.0f, 1.0f, 0.0f, 1.0f } },// 7
};
Vertex LineVertices[] =
{
	{ { 1.0f, 1.0f, 0.0f, 1.0f },{ 1.0f, 1.0f, 1.0f, 1.0f } }, // 0
	{ { 0.0f, 1.4f, 0.0f, 1.0f },{ 1.0f, 1.0f, 1.0f, 1.0f } }, // 1
	{ { -1.0f, 1.0f, 0.0f, 1.0f },{ 1.0f, 1.0f, 1.0f, 1.0f } }, // 2
	{ { -1.4f, 0.0f, 0.0f, 1.0f },{ 1.0f, 1.0f, 1.0f, 1.0f } }, // 3
	{ { -1.0f, -1.0f, 0.0f, 1.0f },{ 1.0f, 1.0f, 1.0f, 1.0f } }, // 4
	{ { 0.0f, -1.4f, 0.0f, 1.0f },{ 1.0f, 1.0f, 1.0f, 1.0f } },// 5
	{ { 1.0f, -1.0f, 0.0f, 1.0f },{ 1.0f, 1.0f, 1.0f, 1.0f } }, // 6
	{ { 1.4f, 0.0f, 0.0f, 1.0f },{ 1.0f, 1.0f, 1.0f, 1.0f } },// 7
};

Vertex BezierVertices[32], BezierCurveVertices[240], Tangent[2], Normal[2], Binormal[2], BezierTangent[240];
unsigned short BezierIndices[32], BezierCurveIndices[240];
unsigned short TangentIndex[] = { 0, 1 }, NormalIndex[] = { 0, 1 }, BinormalIndex[] = { 0, 1 };
unsigned short Indices[] = {
	0, 1, 2, 3, 4, 5, 6, 7
};
unsigned short LineIndices[] = {
	0, 1, 2, 3, 4, 5, 6, 7
};
const size_t IndexCount = sizeof(Indices) / sizeof(unsigned short);
// ATTN: DON'T FORGET TO INCREASE THE ARRAY SIZE IN THE PICKING VERTEX SHADER WHEN YOU ADD MORE PICKING COLORS
float pickingColor[IndexCount] = { 0 / 255.0f, 1 / 255.0f, 2 / 255.0f, 3 / 255.0f, 4 / 255.0f, 5 / 255.0f, 6 / 255.0f, 7 / 255.0f };

// ATTN: ADD YOU PER-OBJECT GLOBAL ARRAY DEFINITIONS HERE

void bezier() {
	Vertex v_1, v, v1, v2;
	int length = sizeof(Vertices) / sizeof(Vertices[0]);
	for (int i = 0; i < 8; i++) {
		v_1 = Vertices[abs(length + i - 1) % length];
		v = Vertices[i];
		v1 = Vertices[abs(length + i + 1) % length];
		v2 = Vertices[abs(length + i + 2) % length];

		BezierVertices[abs((4 * i) + 1) % 32].XYZW[0] = ((4.0f * v_1.XYZW[0]) + (7.0f * v.XYZW[0]) + v1.XYZW[0]) / 12.0f;
		BezierVertices[abs((4 * i) + 2) % 32].XYZW[0] = ((4.0f * v_1.XYZW[0]) + (16.0f * v.XYZW[0]) + (4.0f * v1.XYZW[0])) / 24.0f;
		BezierVertices[abs((4 * i) + 3) % 32].XYZW[0] = (v_1.XYZW[0] + (7.0f * v.XYZW[0]) + (4.0f * v1.XYZW[0])) / 12.0f;
		BezierVertices[abs((4 * i) + 4) % 32].XYZW[0] = (v_1.XYZW[0] + (11.0f * v.XYZW[0]) + (11.0f * v1.XYZW[0]) + v2.XYZW[0]) / 24.0f;

		BezierVertices[abs((4 * i) + 1) % 32].XYZW[1] = ((4.0f * v_1.XYZW[1]) + (7.0f * v.XYZW[1]) + v1.XYZW[1]) / 12.0f;
		BezierVertices[abs((4 * i) + 2) % 32].XYZW[1] = ((4.0f * v_1.XYZW[1]) + (16.0f * v.XYZW[1]) + (4.0f * v1.XYZW[1])) / 24.0f;
		BezierVertices[abs((4 * i) + 3) % 32].XYZW[1] = (v_1.XYZW[1] + (7.0f * v.XYZW[1]) + (4.0f * v1.XYZW[1])) / 12.0f;
		BezierVertices[abs((4 * i) + 4) % 32].XYZW[1] = (v_1.XYZW[1] + (11.0f * v.XYZW[1]) + (11.0f * v1.XYZW[1]) + v2.XYZW[1]) / 24.0f;

		BezierVertices[abs((4 * i) + 1) % 32].XYZW[2] = ((4.0f * v_1.XYZW[2]) + (7.0f * v.XYZW[2]) + v1.XYZW[2]) / 12.0f;
		BezierVertices[abs((4 * i) + 2) % 32].XYZW[2] = ((4.0f * v_1.XYZW[2]) + (16.0f * v.XYZW[2]) + (4.0f * v1.XYZW[2])) / 24.0f;
		BezierVertices[abs((4 * i) + 3) % 32].XYZW[2] = (v_1.XYZW[2] + (7.0f * v.XYZW[2]) + (4.0f * v1.XYZW[2])) / 12.0f;
		BezierVertices[abs((4 * i) + 4) % 32].XYZW[2] = (v_1.XYZW[2] + (11.0f * v.XYZW[2]) + (11.0f * v1.XYZW[2]) + v2.XYZW[2]) / 24.0f;
	}

	int curvePoint = 0;
	Vertex Q[5], colorVertex = { { 1.0f, 1.0f, 0.0f, 1.0f },{ 1.0f, 1.0f, 0.0f, 1.0f } };
	for (int i = 0; i < 32; i = i + 4) {
		for (int u = 0; u < 30; u++) {
			float t = u / 30.0f;
			for (int j = 0; j < 5; j++) {
				Q[j].XYZW[0] = BezierVertices[i + j].XYZW[0];
				Q[j].XYZW[1] = BezierVertices[i + j].XYZW[1];
				Q[j].XYZW[2] = BezierVertices[i + j].XYZW[2];
			}
			BezierTangent[curvePoint].XYZW[0] = (-4 * pow((1 - t), 3) * Q[0].XYZW[0]) + 4 * pow((1 - t), 2) * (-(3 * t) + (1 - t)) * Q[1].XYZW[0] + 12 * t * (1 - t) * ((1 - t) - (t)) * Q[2].XYZW[0] + 4 * pow(t, 2) * (3 * (1 - t) - (t)) * Q[3].XYZW[0] + 4 * pow(t, 3) * Q[4].XYZW[0];
			BezierTangent[curvePoint].XYZW[1] = (-4 * pow((1 - t), 3) * Q[0].XYZW[1]) + 4 * pow((1 - t), 2) * (-(3 * t) + (1 - t)) * Q[1].XYZW[1] + 12 * t * (1 - t) * ((1 - t) - (t)) * Q[2].XYZW[1] + 4 * pow(t, 2) * (3 * (1 - t) - (t)) * Q[3].XYZW[1] + 4 * pow(t, 3) * Q[4].XYZW[1];
			BezierTangent[curvePoint].XYZW[2] = (-4 * pow((1 - t), 3) * Q[0].XYZW[2]) + 4 * pow((1 - t), 2) * (-(3 * t) + (1 - t)) * Q[1].XYZW[2] + 12 * t * (1 - t) * ((1 - t) - (t)) * Q[2].XYZW[2] + 4 * pow(t, 2) * (3 * (1 - t) - (t)) * Q[3].XYZW[2] + 4 * pow(t, 3) * Q[4].XYZW[2];
			BezierTangent[curvePoint].XYZW[3] = 1.0f;
			for (int j = 1; j <= 4;j++)
				for (int k = 0; k <= 4 - j; k++)
				{
					Q[k].XYZW[0] = (1.0f - t)*(Q[k].XYZW[0]) + t*(Q[k + 1].XYZW[0]);
					Q[k].XYZW[1] = (1.0f - t)*(Q[k].XYZW[1]) + t*(Q[k + 1].XYZW[1]);
					Q[k].XYZW[2] = (1.0f - t)*(Q[k].XYZW[2]) + t*(Q[k + 1].XYZW[2]);
				}
			BezierCurveVertices[curvePoint].XYZW[0] = Q[0].XYZW[0];
			BezierCurveVertices[curvePoint].XYZW[1] = Q[0].XYZW[1];
			BezierCurveVertices[curvePoint].XYZW[2] = Q[0].XYZW[2];
			BezierCurveVertices[curvePoint].XYZW[3] = colorVertex.XYZW[3];
			BezierCurveVertices[curvePoint].SetColor(colorVertex.RGBA);
			curvePoint++;
		}
	}
}

void drawScene(void)
{
	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	// Re-clear the screen for real rendering
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (dualScreenEnabled) {
		glViewport(0, window_height / 2, window_width, window_height / 2);
	}
	else {
		glViewport(0, 0, window_width, window_height);
	}
	glUseProgram(programID);
	{
		glm::mat4 ModelMatrix = glm::mat4(1.0); // TranslationMatrix * RotationMatrix;
		glm::mat4 MVP = gProjectionMatrix * gViewMatrix * ModelMatrix;

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &gViewMatrix[0][0]);
		glm::vec3 lightPos = glm::vec3(4, 4, 4);
		glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

		glEnable(GL_PROGRAM_POINT_SIZE);

		glBindVertexArray(VertexArrayId[0]);	// draw Vertices
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[0]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices), Vertices);				// update buffer data
																						//glDrawElements(GL_LINE_LOOP, NumVert[0], GL_UNSIGNED_SHORT, (void*)0);
		glDrawElements(GL_POINTS, NumVert[0], GL_UNSIGNED_SHORT, (void*)0);
		// ATTN: OTHER BINDING AND DRAWING COMMANDS GO HERE, one set per object:
		//glBindVertexArray(VertexArrayId[<x>]); etc etc
		glBindVertexArray(0);

		if (bezierFlag == 1) {
			glBindVertexArray(VertexArrayId[2]);	// draw Vertices
			glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[2]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(BezierVertices), BezierVertices);
			glDrawElements(GL_POINTS, NumVert[2], GL_UNSIGNED_SHORT, (void*)0);
			glBindVertexArray(0);

			glBindVertexArray(VertexArrayId[3]);	// draw Vertices
			glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[3]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(BezierCurveVertices), BezierCurveVertices);				// update buffer data
			glDrawElements(GL_LINE_STRIP, NumVert[3], GL_UNSIGNED_SHORT, (void*)0);
			glBindVertexArray(0);
		}
		if (normalsFlag == 1) {
			glBindVertexArray(VertexArrayId[4]);	// draw Vertices
			glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[4]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Tangent), Tangent);				// update buffer data
			glDrawElements(GL_LINE_STRIP, NumVert[4], GL_UNSIGNED_SHORT, (void*)0);
			glBindVertexArray(0);

			glBindVertexArray(VertexArrayId[5]);	// draw Vertices
			glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[5]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Normal), Normal);				// update buffer data
			glDrawElements(GL_LINE_STRIP, NumVert[5], GL_UNSIGNED_SHORT, (void*)0);
			glBindVertexArray(0);

			glBindVertexArray(VertexArrayId[6]);	// draw Vertices
			glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[6]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Binormal), Binormal);				// update buffer data
			glDrawElements(GL_LINE_STRIP, NumVert[6], GL_UNSIGNED_SHORT, (void*)0);
			glBindVertexArray(0);
		}

		glBindVertexArray(VertexArrayId[1]);	// draw Vertices
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[1]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(LineVertices), LineVertices);				// update buffer data
		glDrawElements(GL_LINE_STRIP, NumVert[1], GL_UNSIGNED_SHORT, (void*)0);
		glBindVertexArray(0);
	}
	glUseProgram(0);
	if (dualScreenEnabled) {
		glViewport(0, 0, window_width, window_height / 2);
		glUseProgram(programID);
		{
			glm::mat4 ModelMatrix = glm::mat4(1.0); // TranslationMatrix * RotationMatrix;
			glm::mat4 MVP = gProjectionMatrix * gViewMatrix2 * ModelMatrix;

			// Send our transformation to the currently bound shader, 
			// in the "MVP" uniform
			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
			glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &gViewMatrix2[0][0]);
			glm::vec3 lightPos = glm::vec3(4, 4, 4);
			glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

			glEnable(GL_PROGRAM_POINT_SIZE);

			glBindVertexArray(VertexArrayId[0]);	// draw Vertices
			glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[0]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices), Vertices);				// update buffer data
																							//glDrawElements(GL_LINE_LOOP, NumVert[0], GL_UNSIGNED_SHORT, (void*)0);
			glDrawElements(GL_POINTS, NumVert[0], GL_UNSIGNED_SHORT, (void*)0);
			// ATTN: OTHER BINDING AND DRAWING COMMANDS GO HERE, one set per object:
			//glBindVertexArray(VertexArrayId[<x>]); etc etc
			glBindVertexArray(0);

			if (bezierFlag == 1) {
				glBindVertexArray(VertexArrayId[2]);	// draw Vertices
				glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[2]);
				glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(BezierVertices), BezierVertices);
				glDrawElements(GL_POINTS, NumVert[2], GL_UNSIGNED_SHORT, (void*)0);
				glBindVertexArray(0);

				glBindVertexArray(VertexArrayId[3]);	// draw Vertices
				glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[3]);
				glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(BezierCurveVertices), BezierCurveVertices);				// update buffer data
				glDrawElements(GL_LINE_STRIP, NumVert[3], GL_UNSIGNED_SHORT, (void*)0);
				glBindVertexArray(0);
			}

			if (normalsFlag == 1) {
				glBindVertexArray(VertexArrayId[4]);	// draw Vertices
				glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[4]);
				glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Tangent), Tangent);				// update buffer data
				glDrawElements(GL_LINE_STRIP, NumVert[4], GL_UNSIGNED_SHORT, (void*)0);
				glBindVertexArray(0);

				glBindVertexArray(VertexArrayId[5]);	// draw Vertices
				glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[5]);
				glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Normal), Normal);				// update buffer data
				glDrawElements(GL_LINE_STRIP, NumVert[5], GL_UNSIGNED_SHORT, (void*)0);
				glBindVertexArray(0);

				glBindVertexArray(VertexArrayId[6]);	// draw Vertices
				glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[6]);
				glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Binormal), Binormal);				// update buffer data
				glDrawElements(GL_LINE_STRIP, NumVert[6], GL_UNSIGNED_SHORT, (void*)0);
				glBindVertexArray(0);
			}

			glBindVertexArray(VertexArrayId[1]);	// draw Vertices
			glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[1]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(LineVertices), LineVertices);				// update buffer data
			glDrawElements(GL_LINE_STRIP, NumVert[1], GL_UNSIGNED_SHORT, (void*)0);
			glBindVertexArray(0);
		}
		glUseProgram(0);
		bezier();
	}
	// Draw GUI
	TwDraw();

	// Swap buffers
	glfwSwapBuffers(window);
	glfwPollEvents();
}

void pickVertex(void)
{
	// Clear the screen in white
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, window_width, window_height);
	glUseProgram(pickingProgramID);
	{
		glm::mat4 ModelMatrix = glm::mat4(1.0); // TranslationMatrix * RotationMatrix;
		glm::mat4 MVP = gProjectionMatrix * gViewMatrix * ModelMatrix;

		// Send our transformation to the currently bound shader, in the "MVP" uniform
		glUniformMatrix4fv(PickingMatrixID, 1, GL_FALSE, &MVP[0][0]);
		glUniform1fv(pickingColorArrayID, NumVert[0], pickingColor);	// here we pass in the picking marker array

																		// Draw the ponts
		glEnable(GL_PROGRAM_POINT_SIZE);
		glBindVertexArray(VertexArrayId[0]);
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[0]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices), Vertices);	// update buffer data
		glDrawElements(GL_POINTS, NumVert[0], GL_UNSIGNED_SHORT, (void*)0);
		glBindVertexArray(0);
	}
	glUseProgram(0);

	// Wait until all the pending drawing commands are really done.
	// Ultra-mega-over slow ! 
	// There are usually a long time between glDrawElements() and
	// all the fragments completely rasterized.
	glFlush();
	glFinish();

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Read the pixel at the center of the screen.
	// You can also use glfwGetMousePos().
	// Ultra-mega-over slow too, even for 1 pixel, 
	// because the framebuffer is on the GPU.
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	unsigned char data[4];
	glReadPixels(xpos, window_height - ypos, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data); // OpenGL renders with (0,0) on bottom, mouse reports with (0,0) on top

																					 // Convert the color back to an integer ID
	gPickedIndex = int(data[0]);

	// Uncomment these lines to see the picking shader in effect
	//glfwSwapBuffers(window);
	//continue; // skips the normal rendering
}

// fill this function in!
void moveVertex(void)
{
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	unsigned char data[4];
	glReadPixels(xpos, 768 - ypos, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data); // OpenGL renders with (0,0) on bottom, mouse reports with (0,0) on top
	glm::mat4 ModelMatrix = glm::mat4(1.0);
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glm::vec4 vp = glm::vec4(viewport[0], viewport[1], viewport[2], viewport[3]);
	if (shiftFlag == 1) {
		glm::vec3 vertex = glm::unProject(glm::vec3(xpos, 768 - ypos, 0.0), ModelMatrix, gProjectionMatrix, vp);
		Vertices[gPickedIndex].XYZW[2] = -vertex[0];
		Vertices[gPickedIndex].XYZW[1] = vertex[1];
		LineVertices[gPickedIndex].XYZW[2] = -vertex[0];
		LineVertices[gPickedIndex].XYZW[1] = vertex[1];
		bezier();
	}
	else {

		// retrieve your cursor position
		// get your world coordinates
		// move points

		if (gPickedIndex == 255) { // Full white, must be the background !
			gMessage = "background";
		}
		else {
			std::ostringstream oss;
			oss << "point " << gPickedIndex;
			gMessage = oss.str();
		}
		if ((glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)) == GLFW_PRESS) {
			Vertices[gPickedIndex].RGBA[0] = 0.5f;
			Vertices[gPickedIndex].RGBA[1] = 0.5f;
			Vertices[gPickedIndex].RGBA[2] = 0.5f;
			Vertices[gPickedIndex].RGBA[3] = 1.0f;
			GLint newViewport[4];
			glGetIntegerv(GL_VIEWPORT, newViewport);

			glm::vec3 vertex = glm::unProject(glm::vec3(xpos, 768 - ypos, 0.0), ModelMatrix, gProjectionMatrix, vp);
			Vertices[gPickedIndex].XYZW[0] = -vertex[0];
			Vertices[gPickedIndex].XYZW[1] = vertex[1];
			LineVertices[gPickedIndex].XYZW[0] = -vertex[0];
			LineVertices[gPickedIndex].XYZW[1] = vertex[1];
			bezier();
		}
		else {
			Vertices[gPickedIndex].RGBA[0] = OriginalVertices[gPickedIndex].RGBA[0];
			Vertices[gPickedIndex].RGBA[1] = OriginalVertices[gPickedIndex].RGBA[1];
			Vertices[gPickedIndex].RGBA[2] = OriginalVertices[gPickedIndex].RGBA[2];
			Vertices[gPickedIndex].RGBA[3] = OriginalVertices[gPickedIndex].RGBA[3];
		}
	}
}

int initWindow(void)
{
	// Initialise GLFW
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(window_width, window_height, "Siddhesh Muley (25901911)", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	// Initialize the GUI
	TwInit(TW_OPENGL_CORE, NULL);
	TwWindowSize(window_width, window_height);
	TwBar * GUI = TwNewBar("Picking");
	TwSetParam(GUI, NULL, "refresh", TW_PARAM_CSTRING, 1, "0.1");
	TwAddVarRW(GUI, "Last picked object", TW_TYPE_STDSTRING, &gMessage, NULL);

	// Set up inputs
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_FALSE);
	glfwSetCursorPos(window, window_width / 2, window_height / 2);
	glfwSetMouseButtonCallback(window, mouseCallback);
	glfwSetKeyCallback(window, keyCallback);
	return 0;
}

void initOpenGL(void)
{
	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);
	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	//glm::mat4 ProjectionMatrix = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	// Or, for an ortho camera :
	float aspectX;
	if (dualScreenEnabled) {
		aspectX = 8.0f;
	}
	else {
		aspectX = 4.0f;
	}
	gProjectionMatrix = glm::ortho(-aspectX, aspectX, -3.0f, 3.0f, 0.0f, 100.0f); // In world coordinates

																				  // Camera matrix
	gViewMatrix = glm::lookAt(
		glm::vec3(0, 0, -5), // Camera is at (4,3,3), in World Space
		glm::vec3(0, 0, 0), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);
	gViewMatrix2 = glm::lookAt(
		glm::vec3(-5, 0, 0), // Camera is at (4,3,3), in World Space
		glm::vec3(0, 0, 0), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);

	// Create and compile our GLSL program from the shaders
	programID = LoadShaders("StandardShading.vertexshader", "StandardShading.fragmentshader");
	pickingProgramID = LoadShaders("Picking.vertexshader", "Picking.fragmentshader");

	// Get a handle for our "MVP" uniform
	MatrixID = glGetUniformLocation(programID, "MVP");
	ViewMatrixID = glGetUniformLocation(programID, "V");
	ModelMatrixID = glGetUniformLocation(programID, "M");
	PickingMatrixID = glGetUniformLocation(pickingProgramID, "MVP");
	// Get a handle for our "pickingColorID" uniform
	pickingColorArrayID = glGetUniformLocation(pickingProgramID, "PickingColorArray");
	pickingColorID = glGetUniformLocation(pickingProgramID, "PickingColor");
	// Get a handle for our "LightPosition" uniform
	LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

	createVAOs(Vertices, Indices, sizeof(Vertices), sizeof(Indices), 0);
	createVAOs(LineVertices, LineIndices, sizeof(LineVertices), sizeof(LineIndices), 1);
	createVAOs(BezierVertices, BezierIndices, sizeof(BezierVertices), sizeof(BezierIndices), 2);
	createVAOs(BezierCurveVertices, BezierCurveIndices, sizeof(BezierCurveVertices), sizeof(BezierCurveIndices), 3);
	createVAOs(Tangent, TangentIndex, sizeof(Tangent), sizeof(TangentIndex), 4);
	createVAOs(Normal, NormalIndex, sizeof(Normal), sizeof(NormalIndex), 5);
	createVAOs(Binormal, BinormalIndex, sizeof(Binormal), sizeof(BinormalIndex), 6);
	bezier();
}

void createVAOs(Vertex Vertices[], unsigned short Indices[], size_t BufferSize, size_t IdxBufferSize, int ObjectId) {

	NumVert[ObjectId] = IdxBufferSize / (sizeof GLubyte);

	GLenum ErrorCheckValue = glGetError();
	size_t VertexSize = sizeof(Vertices[0]);
	size_t RgbOffset = sizeof(Vertices[0].XYZW);

	// Create Vertex Array Object
	glGenVertexArrays(1, &VertexArrayId[ObjectId]);
	glBindVertexArray(VertexArrayId[ObjectId]);

	// Create Buffer for vertex data
	glGenBuffers(1, &VertexBufferId[ObjectId]);
	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[ObjectId]);
	glBufferData(GL_ARRAY_BUFFER, BufferSize, Vertices, GL_STATIC_DRAW);

	// Create Buffer for indices
	glGenBuffers(1, &IndexBufferId[ObjectId]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferId[ObjectId]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, IdxBufferSize, Indices, GL_STATIC_DRAW);

	// Assign vertex attributes
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, VertexSize, 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, VertexSize, (GLvoid*)RgbOffset);

	glEnableVertexAttribArray(0);	// position
	glEnableVertexAttribArray(1);	// color

									// Disable our Vertex Buffer Object 
	glBindVertexArray(0);

	ErrorCheckValue = glGetError();
	if (ErrorCheckValue != GL_NO_ERROR)
	{
		fprintf(
			stderr,
			"ERROR: Could not create a VBO: %s \n",
			gluErrorString(ErrorCheckValue)
		);
	}
}

void cleanup(void)
{
	// Cleanup VBO and shader
	for (int i = 0; i < NumObjects; i++) {
		glDeleteBuffers(1, &VertexBufferId[i]);
		glDeleteBuffers(1, &IndexBufferId[i]);
		glDeleteVertexArrays(1, &VertexArrayId[i]);
	}
	glDeleteProgram(programID);
	glDeleteProgram(pickingProgramID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();
}

static void mouseCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && !dualScreenEnabled) {
		pickVertex();
	}
}

static void keyCallback(GLFWwindow* window, int button, int scancode, int action, int mods) {
	if (button == GLFW_KEY_2 && action == GLFW_PRESS) {
		if (bezierFlag == 0) {
			bezierFlag = 1;
		}
		else {
			bezierFlag = 0;
		}
		//bezier();
	}

	if (button == GLFW_KEY_5 && action == GLFW_PRESS) {
		if (normalsFlag == 0) {
			normalsFlag = 1;
		}
		else {
			normalsFlag = 0;
		}
		//bezier();
	}

	if (button == GLFW_KEY_4 && action == GLFW_PRESS) {
		if (!dualScreenEnabled) {
			dualScreenEnabled = true;
			initOpenGL();
		}
		else {
			dualScreenEnabled = false;
			initOpenGL();
		}
	}
	if (button == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS) {
		if (shiftFlag == 0) {
			shiftFlag = 1;
		}
		else {
			shiftFlag = 0;
		}
	}
}

int main(void)
{
	// initialize window
	int errorCode = initWindow();
	if (errorCode != 0)
		return errorCode;
	// initialize OpenGL pipeline
	Vertex templateVertex = { { 1.0f, 1.0f, 0.0f, 1.0f },{ 1.0f, 0.0f, 0.0f, 1.0f } };
	for (int j = 0; j < (sizeof(BezierVertices) / sizeof(BezierVertices[0])); j++)
	{
		BezierVertices[j].SetColor(templateVertex.RGBA);
		BezierVertices[j].XYZW[2] = 0.0f;
		BezierVertices[j].XYZW[3] = 1.0f;
	}

	for (int i = 0; i < 240; i++) {
		if (i < 32) {
			BezierIndices[i] = i;
			BezierCurveIndices[i] = i;
		}
		else {
			BezierCurveIndices[i] = i;
		}
	}
	initOpenGL();
	// For speed computation
	double lastTime = glfwGetTime();
	int nbFrames = 0;
	do {
		// Measure speed
		double currentTime = glfwGetTime();
		nbFrames++;
		if (currentTime - lastTime >= 1.0) { // If last prinf() was more than 1sec ago
											 // printf and reset
											 //printf("%f ms/frame\n", 1000.0 / double(nbFrames));
			nbFrames = 0;
			lastTime += 1.0;
		}
		glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, 1);
		// DRAGGING: move current (picked) vertex with cursor
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT))
			moveVertex();

		// DRAWING SCENE
		glfwSetKeyCallback(window, keyCallback);
		// re-evaluate curves in case vertices have been moved
		Tangent[0] = BezierCurveVertices[pointPosition];
		/*Tangent[0].XYZW[1] = BezierCurveVertices[pointPosition].XYZW[1];
		Tangent[0].XYZW[2] = BezierCurveVertices[pointPosition].XYZW[2];
		Tangent[0].XYZW[3] = 1.0f;*/
		Tangent[0].RGBA[0] = 1.0f;
		Tangent[0].RGBA[1] = 0.0f;
		Tangent[0].RGBA[2] = 0.0f;
		Tangent[0].RGBA[3] = 1.0f;
		Tangent[1].XYZW[0] = BezierTangent[pointPosition].XYZW[0] + BezierCurveVertices[pointPosition].XYZW[0];
		Tangent[1].XYZW[1] = BezierTangent[pointPosition].XYZW[1] + BezierCurveVertices[pointPosition].XYZW[1];
		Tangent[1].XYZW[2] = BezierTangent[pointPosition].XYZW[2] + BezierCurveVertices[pointPosition].XYZW[2];
		Tangent[1].XYZW[3] = 1.0f;
		Tangent[1].RGBA[0] = 1.0f;
		Tangent[1].RGBA[1] = 0.0f;
		Tangent[1].RGBA[2] = 0.0f;
		Tangent[1].RGBA[3] = 1.0f;

		Normal[0] = BezierCurveVertices[pointPosition];
		Normal[0].RGBA[0] = 0.0f;
		Normal[0].RGBA[1] = 1.0f;
		Normal[0].RGBA[2] = 0.0f;
		Normal[0].RGBA[3] = 1.0f;
		vec3 nrml1 = glm::vec3(Normal[0].XYZW[0], Normal[0].XYZW[1], Normal[0].XYZW[2]);
		vec3 tngnt1 = glm::vec3(Tangent[1].XYZW[0], Tangent[1].XYZW[1], Tangent[1].XYZW[2]);
		vec3 cross1 = glm::cross(nrml1, tngnt1);
		Normal[1].XYZW[0] = cross1[0] + BezierCurveVertices[pointPosition].XYZW[0];
		Normal[1].XYZW[1] = cross1[1] + BezierCurveVertices[pointPosition].XYZW[1];
		Normal[1].XYZW[2] = cross1[2] + BezierCurveVertices[pointPosition].XYZW[2];
		Normal[1].XYZW[3] = 1.0f;
		Normal[1].RGBA[0] = 0.0f;
		Normal[1].RGBA[1] = 1.0f;
		Normal[1].RGBA[2] = 0.0f;
		Normal[1].RGBA[3] = 1.0f;

		Binormal[0] = BezierCurveVertices[pointPosition];
		Binormal[0].RGBA[0] = 0.0f;
		Binormal[0].RGBA[1] = 0.0f;
		Binormal[0].RGBA[2] = 1.0f;
		Binormal[0].RGBA[3] = 1.0f;
		vec3 nrml2 = glm::vec3(Normal[1].XYZW[0] - Normal[0].XYZW[0], Normal[1].XYZW[1] - Normal[0].XYZW[1], Normal[1].XYZW[2] - Normal[0].XYZW[2]);
		vec3 tngnt2 = glm::vec3(Tangent[1].XYZW[0] - Tangent[0].XYZW[0], Tangent[1].XYZW[1] - Tangent[0].XYZW[1], Tangent[1].XYZW[2] - Tangent[0].XYZW[2]);
		vec3 cross2 = glm::cross(nrml2, tngnt2);
		Binormal[1].XYZW[0] = cross2[0] + BezierCurveVertices[pointPosition].XYZW[0];
		Binormal[1].XYZW[1] = cross2[1] + BezierCurveVertices[pointPosition].XYZW[1];
		Binormal[1].XYZW[2] = cross2[2] + BezierCurveVertices[pointPosition].XYZW[2];
		Binormal[1].XYZW[3] = 1.0f;
		Binormal[1].RGBA[0] = 0.0f;
		Binormal[1].RGBA[1] = 0.0f;
		Binormal[1].RGBA[2] = 1.0f;
		Binormal[1].RGBA[3] = 1.0f;

		pointPosition = (pointPosition + 1) % 240;
		drawScene();


	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	cleanup();

	return 0;
}
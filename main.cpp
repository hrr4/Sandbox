#include <stdio.h>
#include <stdlib.h>
#include <GL\glew.h>
#include <GL\glfw.h>
#include <vector>

const int MAX_ROW = 15;
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

struct Vertex {
	float x, y, z; // Vertex
	float nx, ny, nz; // Normal
	float s0, t0; // Texcoord
};

Vertex myVertices[4];
GLushort myIndices[9];

bool topLeft = true;
std::vector<GLubyte> indexVec;
std::vector<Vertex> vertexVec;

GLuint VertexShaderId,
	 FragmentShaderId,
	 ProgramId,
	 VaoId,
	 IndexBufferId,
	 VboId;

const GLcharARB* VertexShader = {
	"#version 400\n"\

	"layout(location=0) in vec4 in_Position;\n"\
	"layout(location=1) in vec4 in_Color;\n"\
	"out vec4 ex_Color;\n"\

	"void main(void) {\n"\
	"	gl_Position = in_Position;\n"\
	"	ex_Color = in_Color;\n"\
	"}\n"
};

const GLcharARB* FragmentShader = {
	"#version 400\n"\

	"in vec4 ex_Color;\n"\
	"out vec4 out_Color;\n"\

	"void main(void) {\n"\
	"	out_Color = ex_Color;\n"\
	"}\n"\
};

void CreateVBO() {
	GLenum ErrorCheckValue = glGetError();
	const size_t BufferSize = vertexVec.size()*sizeof(Vertex);
	const size_t VertexSize = sizeof(Vertex);
	const size_t IndexSize = sizeof(indexVec);
	/*const size_t BufferSize2 = sizeof(myVertices);
	const size_t VertexSize2 = sizeof(myVertices[0]);
	const size_t IndexSize2 = sizeof(myIndices);*/

	glGenBuffers(1, &VboId);

	glGenVertexArrays(1, &VaoId);
	glBindVertexArray(VaoId);

	glBindBuffer(GL_ARRAY_BUFFER, VboId);
	//glBufferData(GL_ARRAY_BUFFER, BufferSize2, myVertices, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, BufferSize, &vertexVec[0], GL_STATIC_DRAW);

	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VertexSize2, 0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VertexSize, 0);

	glEnableVertexAttribArray(0);

	// Pushing Indices 
	glGenBuffers(1, &IndexBufferId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferId);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, IndexSize2, myIndices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, IndexSize, &indexVec[0], GL_STATIC_DRAW);

	ErrorCheckValue = glGetError();
	if (ErrorCheckValue != GL_NO_ERROR) {
		//fprintf(stderr, "ERROR: Could not create a VBO: %s \n", gluErrorString(ErrorCheckValue));

		exit(-1);
	}
}

void buildVertices() {
	float k = 0.5f;
	int j = 0;
	Vertex tempVertex;

	for (int i = 0; i < MAX_ROW; ++i) {
		switch(j) {
		case 0:
			tempVertex.x = -k; tempVertex.y = k; tempVertex.z = 0;
			j++;
			break;
		case 1:
			tempVertex.x = -k; tempVertex.y = -k; tempVertex.z = 0;
			j++;
			break;
		case 2:
		tempVertex.x = k; tempVertex.y = k; tempVertex.z = 0;
			j = 0;
			k+=0.5f;
			break;
		}
		
		vertexVec.push_back(tempVertex);

	}
}

void buildIndices() {
	int j = 0;
	//for (int j = 0; j < 3; ++j) {
			for (int i = 0; i < MAX_ROW; i+=2) {
				indexVec.push_back(i); indexVec.push_back(i+1); indexVec.push_back(i+2);
				indexVec.push_back(i+2); indexVec.push_back(i+1); indexVec.push_back(i+3);
		/*myIndices[0] = j; myIndices[1] = j+1; myIndices[2] = j+2; 
		myIndices[3] = j+2; myIndices[4] = j+1; myIndices[5] = j+3; 
		myIndices[6] = j+2; myIndices[7] = j+3; myIndices[8] = j+4; 
			}*/
	}
}

void DestroyVBO() {
	GLenum ErrorCheckValue = glGetError();

	//glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDeleteBuffers(1, &VboId);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &IndexBufferId);

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VaoId);

	ErrorCheckValue = glGetError();
	if (ErrorCheckValue != GL_NO_ERROR) {
		//fprintf(stderr, "ERROR: Could not destroy the VBO: %s \n", gluErrorString(ErrorCheckValue));

		exit(-1);
	}
}

void CreateShaders() {
	GLenum ErrorCheckValue = glGetError();

	VertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(VertexShaderId, 1, &VertexShader, NULL);
	glCompileShader(VertexShaderId);

	FragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(FragmentShaderId, 1, &FragmentShader, NULL);
	glCompileShader(FragmentShaderId);

	ProgramId = glCreateProgram();
		glAttachShader(ProgramId, VertexShaderId);
		glAttachShader(ProgramId, FragmentShaderId);
	glLinkProgram(ProgramId);
	glUseProgram(ProgramId);

	ErrorCheckValue = glGetError();
	if (ErrorCheckValue != GL_NO_ERROR) {
		//fprintf(stderr, "ERROR: Could not create the shaders: %s \n", gluErrorString(ErrorCheckValue));

		exit(-1);
	}
}

void DestroyShaders() {
	GLenum ErrorCheckValue = glGetError();

	glUseProgram(0);

	glDetachShader(ProgramId, VertexShaderId);
	glDetachShader(ProgramId, FragmentShaderId);

	glDeleteShader(FragmentShaderId);
	glDeleteShader(VertexShaderId);

	glDeleteProgram(ProgramId);

	ErrorCheckValue = glGetError();
	if (ErrorCheckValue != GL_NO_ERROR) {
		//fprintf(stderr, "ERROR: Could not destroy shaders: %s \n", gluErrorString(ErrorCheckValue));

		exit(-1);
	}
}

void ResizeFunction(int Width, int Height) {
	glViewport(0, 0, Width, Height);
}

void RenderFunction() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glDrawElements(GL_TRIANGLE_STRIP, 3, GL_UNSIGNED_BYTE, (GLvoid*)0);

	glfwSwapBuffers();
}

void Cleanup() {
	DestroyShaders();
	DestroyVBO();
	glfwCloseWindow();
}

void Initialize() {
	GLenum GlewInitResult;

	glfwInit();

	glfwOpenWindow(640, 480, 24, 24, 24, 24, 24, 24, GLFW_WINDOW);

	glfwSetWindowSizeCallback(ResizeFunction);
	//glutDisplayFunc(RenderFunction);

	glewExperimental = GL_TRUE;
	GlewInitResult = glewInit();
	if (GLEW_OK != GlewInitResult) {
		//fprintf(stderr, "ERROR: %s\n", glewGetErrorString(GlewInitResult));
		exit(EXIT_FAILURE);
	}

	buildIndices();
	buildVertices();
	CreateShaders();
	CreateVBO();

	glClearColor(0, 0, 0, 0);
}

int main(int argc, char* argv[]) {
	int Running = GL_TRUE;

	Initialize();

	while (Running) 
		RenderFunction();

	Cleanup();

	glfwTerminate();
}
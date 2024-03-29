#include <stdio.h>
#include <stdlib.h>
#include <GL\glew.h>
#include <GL\glfw.h>
#include <vector>

const int MAX_PER_ROW = 15;
const int MAX_ROWS = 5;
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

struct Vertex {
	float x, y, z, w; // Vertex
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

	glGenBuffers(1, &VboId);

	glGenVertexArrays(1, &VaoId);
	glBindVertexArray(VaoId);

	glBindBuffer(GL_ARRAY_BUFFER, VboId);
	glBufferData(GL_ARRAY_BUFFER, BufferSize, &vertexVec[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, VertexSize, 0);

	glEnableVertexAttribArray(0);

	// Pushing Indices 
	glGenBuffers(1, &IndexBufferId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, IndexSize, &indexVec[0], GL_STATIC_DRAW);

	ErrorCheckValue = glGetError();
	if (ErrorCheckValue != GL_NO_ERROR) {
		//fprintf(stderr, "ERROR: Could not create a VBO: %s \n", gluErrorString(ErrorCheckValue));

		exit(-1);
	}
}

void buildVertices() {
	float x = -0.5f;
	float y = 0.3f;
	bool top = true;
	Vertex tempVertex;

	for (int j = 0; j < MAX_ROWS; ++j) {
		for (int i = 0; i < MAX_PER_ROW; ++i) {
			if (top == true) {
				tempVertex.x = x; tempVertex.y = y; tempVertex.z = 0; tempVertex.w = 1.0f;
				top = false;
			} else if (top == false) {
				tempVertex.x = x; tempVertex.y = -y; tempVertex.z = 0; tempVertex.w = 1.0f;
				x+=0.5;
				top = true;
			}

			vertexVec.push_back(tempVertex);
		}
		y-=0.6f;
	}
}

void buildIndices() {
	// I think i need to build indices per row as well...so gotta fix this before multiple rows will work properly
	/* Don't know if ill need to do my voodoo magic index code yet.... it might just merge the vertices and 
		share the shit 
		i - has to be able to hold it's count outside of the inner loop.
	*/
	//int index_counter = 1;
	int i = 0;
	for (int k = 0; k < MAX_ROWS; ++k) {
		for (; i < MAX_PER_ROW; i+=2) {
			indexVec.push_back(i); indexVec.push_back(i+1); indexVec.push_back(i+2);
			indexVec.push_back(i+2); indexVec.push_back(i+1); indexVec.push_back(i+3);
		}
		//k+=2;
	}
}

void DestroyVBO() {
	GLenum ErrorCheckValue = glGetError();

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

	glDrawElements(GL_TRIANGLE_STRIP, MAX_PER_ROW*MAX_ROWS, GL_UNSIGNED_BYTE, (GLvoid*)0);

	glfwSwapBuffers();
}

void Cleanup() {
	DestroyShaders();
	DestroyVBO();
	glfwCloseWindow();
}

void Initialize() {
	float zoomFactor = 1.0f;
	GLenum GlewInitResult;

	glfwInit();

	glfwOpenWindow(640, 480, 24, 24, 24, 24, 24, 24, GLFW_WINDOW);

	glfwSetWindowSizeCallback(ResizeFunction);

	glewExperimental = GL_TRUE;
	GlewInitResult = glewInit();
	if (GLEW_OK != GlewInitResult) {
		//fprintf(stderr, "ERROR: %s\n", glewGetErrorString(GlewInitResult));
		exit(EXIT_FAILURE);
	}

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();
	gluPerspective(100.0*zoomFactor, (float)SCREEN_WIDTH/(float)SCREEN_HEIGHT, -1, 1);
	glMatrixMode(GL_MODELVIEW);

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
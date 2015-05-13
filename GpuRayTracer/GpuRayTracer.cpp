#include "stdafx.h"

const string shaderName = "raytracer";

// Handles
GLuint programHandle;
GLuint vaoHandle;

// Two triangles
const float positionData[] = {
	-1.0f, -1.0f, 0.0f,
	1.0f, -1.0f, 0.0f,
	-1.0f, 1.0f, 0.0f,

	1.0f, 1.0f, 0.0f,
	1.0f, -1.0f, 0.0f,
	-1.0f, 1.0f, 0.0f,
};

const float texCoordData[] = {
	0.0f, 0.0f,
	1.0f, 0.0f,
	0.0f, 1.0f,

	1.0f, 1.0f,
	1.0f, 0.0f,
	0.0f, 1.0f
};

// Camera
const float cameraPosition[] = {150.0f, 150.0f, 150.0f};
const float cameraLookAt[] = {0.0f, 0.0f, 0.0f};
const float cameraUp[] = {0.0f, 1.0f, 0.0f};
const float cameraViewPlaneDistance = 1000.0f;

float angle = 0.0f;

// Screen
int screenWidth = 640;
int screenHeight = 480;

GLuint CreateShader(GLenum shaderType, const string &shaderSource)
{
	GLuint shader = glCreateShader(shaderType);

	const char *sourseData = shaderSource.c_str();
	glShaderSource(shader, 1, &sourseData, 0);

	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *infoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(shader, infoLogLength, 0, infoLog);

		char str[0xff];
		sprintf(str, "Compile failure in %s shader:\n%s\n", shaderType == GL_VERTEX_SHADER ? "vertex" : "fragment", infoLog);
		OutputDebugStringA(str);
		delete[] infoLog;
	}

	return shader;
}

GLuint CreateProgram(const vector<GLuint> &shaders)
{
	GLuint program = glCreateProgram();

	vector<GLuint>::const_iterator it;
	for(it = shaders.begin(); it != shaders.end(); ++it)
		glAttachShader(program, *it);

	glBindAttribLocation(program, 0, "vertexPosition");
	glBindAttribLocation(program, 1, "vertexTexCoord");

	glLinkProgram(program);

	// Verify the link status
	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *infoLog = new GLchar[infoLogLength + 1];
		glGetProgramInfoLog(program, infoLogLength, 0, infoLog);

		char str[0xff];
		sprintf(str, "Link failure: %s\n", infoLog);
		OutputDebugStringA(str);
		delete[] infoLog;
	}

	return program;
}

string LoadShaderAsString(const string &fileName)
{
	string line, result;
	ifstream file;
	
	file.open(fileName);
	
	if (file.is_open())
	{
		while(!file.eof())
		{
			getline(file ,line); 
			result += line + '\n';
		}
	}

	file.close();

	return result;
}

void InitializeProgram()
{
	vector<GLuint> shaders;

	shaders.push_back(CreateShader(GL_VERTEX_SHADER, LoadShaderAsString("shaders/" + shaderName + ".vert")));
	shaders.push_back(CreateShader(GL_FRAGMENT_SHADER, LoadShaderAsString("shaders/" + shaderName + ".frag")));

	programHandle = CreateProgram(shaders);

	for_each(shaders.begin(), shaders.end(), glDeleteShader);
}

void Initialize()
{
	InitializeProgram();

	glUseProgram(programHandle);

	// Create buffer objects
	GLuint vboHandles[2];
	glGenBuffers(2, vboHandles);
	GLuint positionBufferHandle = vboHandles[0];
	GLuint texCoordBufferHandle = vboHandles[1];

	// Populate position buffer
	glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(float), positionData, GL_STATIC_DRAW);

	// Populate tex coord buffer
	glBindBuffer(GL_ARRAY_BUFFER, texCoordBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), texCoordData, GL_STATIC_DRAW);

	// Create and set-up vertex array object
	glGenVertexArrays(1, &vaoHandle);
	glBindVertexArray(vaoHandle);

	// Enable vertex attribute arrays
	glEnableVertexAttribArray(0);		// positions
	glEnableVertexAttribArray(1);		// tex coords

	// Map index 0 to position buffer
	glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *) 0);

	// Map index 1 to tex coord buffer buffer
	glBindBuffer(GL_ARRAY_BUFFER, texCoordBufferHandle);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLubyte *) 0);
}

void Display()
{
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	mat4 rotationMatrix = glm::rotate(mat4(1.0f), angle, vec3(0.0f, 1.0f, 0.0f));

	// Pass uniform variables into shader
	GLuint location = glGetUniformLocation(programHandle, "cameraPosition");
	glUniform3fv(location, 1, cameraPosition);

	location = glGetUniformLocation(programHandle, "cameraLookAt");
	glUniform3fv(location, 1, cameraLookAt);

	location = glGetUniformLocation(programHandle, "cameraUp");
	glUniform3fv(location, 1, cameraUp);

	location = glGetUniformLocation(programHandle, "cameraViewPlaneDistance");
	glUniform1f(location, cameraViewPlaneDistance);

	location = glGetUniformLocation(programHandle, "screenWidth");
	glUniform1i(location, screenWidth);

	location = glGetUniformLocation(programHandle, "screenHeight");
	glUniform1i(location, screenHeight);

	location =glGetUniformLocation(programHandle,"rotationMatrix");
	glUniformMatrix4fv(location, 1, GL_FALSE,	&rotationMatrix[0][0]);
	angle += 0.5f;

	glBindVertexArray(vaoHandle);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glutSwapBuffers();

	glutPostRedisplay();
}

void Reshape(int w, int h)
{
	screenWidth = w;
	screenHeight = h;

	glViewport(0, 0, screenWidth, screenHeight);
}

void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		glutLeaveMainLoop();
		break;
	}
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	freopen("stdout.txt", "w", stdout);

	int argc = 0;
	glutInit(&argc, 0);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);
	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitWindowSize(screenWidth, screenHeight); 
	glutInitWindowPosition(300, 200);

	glutCreateWindow("GPU Raytracer");

	LoadFunctions();
	
	Initialize();

	// Bind handlers and start
	glutDisplayFunc(Display); 
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	
	glutMainLoop();

	return 0;
}

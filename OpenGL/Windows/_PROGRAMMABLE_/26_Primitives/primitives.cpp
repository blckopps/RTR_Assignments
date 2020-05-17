//	Headers
#include<windows.h>
#include<stdio.h>
#include "vmath.h"	
#include<gl/glew.h>
#include<gl/gl.h>


#pragma comment (lib, "glew32.lib")
#pragma comment (lib, "opengl32.lib")

//	Macros
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

//namespaces
using namespace vmath;

enum
{
	AMC_ATTRIBUTE_POSITION = 0,
	AMC_ATTRIBUTE_COLOR,
	AMC_ATTRIBUTE_NORMAL,
	AMC_ATTRIBUTE_TEXCOORD0
};

//	Global function declaration
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//	Global variables
FILE *gpfile;
HWND ghwnd = NULL;
bool gbActiveWindow = false;
bool gbFullScreen = false;
DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
HDC ghdc = NULL;
HGLRC ghrc = NULL;

GLuint gShaderProgramObject;
GLuint vao;
GLuint vbo;
GLuint vbo_elements;

GLuint mvpUniform;
GLuint primitiveColorUniform;

mat4 perspectiveProjectionMatrix;

//WinMain()
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	//function declarations
	int initialize(void);
	void display(void);

	//variable declarations
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("Primitives");
	bool bDone = false;
	int iRet = 0;

	//code
	if (fopen_s(&gpfile, "log.txt", "w") != 0)
	{
		MessageBox(NULL, TEXT("Log file cannot be created"), TEXT("Error"), MB_OK);
		exit(0);
	}

	//initialization of wndclass structure
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.cbWndExtra = 0;
	wndclass.cbClsExtra = 0;
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(hInstance, IDI_APPLICATION);

	//register above class 
	RegisterClassEx(&wndclass);

	//create window
	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		szAppName,
		TEXT("Primitives-SHUBHAM"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		100,
		100,
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);

	ghwnd = hwnd;
	iRet = initialize();
	if (iRet == -1)
	{
		fprintf(gpfile, "ChoosePixelFormat() Failed\n");
		DestroyWindow(hwnd);
	}
	else if (iRet == -2)
	{
		fprintf(gpfile, "SetPixelFormat() Failed\n");
		DestroyWindow(hwnd);
	}
	else if (iRet == -3)
	{
		fprintf(gpfile, "wglCreateContext() Failed\n");
		DestroyWindow(hwnd);
	}
	else if (iRet == -4)
	{
		fprintf(gpfile, "wglMakeCurrent() Failed\n");
		DestroyWindow(hwnd);
	}
	else
		fprintf(gpfile, "Initialize() Succeded\n");

	//show window
	ShowWindow(hwnd, iCmdShow);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	//gameloop
	while (bDone == false)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				bDone = true;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if (gbActiveWindow == true)
			{
				//update should be called here
			}
			display();
		}
	}
	return((int)msg.wParam);
}

//callback function WndProc()
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	//function declaration
	void resize(int, int);
	void ToggleFullScreen(void);
	void uninitialize(void);

	//variables
	static GLfloat width, height;

	//code
	switch (iMsg)
	{
	case WM_SETFOCUS:
		gbActiveWindow = true;
		break;

	case WM_KILLFOCUS:
		gbActiveWindow = false;
		break;

	case WM_ERASEBKGND:
		return(0);

	case WM_SIZE:
		width = LOWORD(lParam);
		height = HIWORD(lParam);
		resize(LOWORD(lParam), HIWORD(lParam));
		break;

	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;

	case WM_KEYDOWN:
		switch (LOWORD(wParam))
		{
		case VK_ESCAPE:
			DestroyWindow(hwnd);
			break;

		case 0x46:
			ToggleFullScreen();
			break;
		}
		break;

	case WM_DESTROY:
		uninitialize();
		PostQuitMessage(0);
		break;
	}
	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}

//ToggleFullScreen() function
void ToggleFullScreen(void)
{
	MONITORINFO mi;
	if (gbFullScreen == false)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			mi = { sizeof(MONITORINFO) };
			if (GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd,
					HWND_TOP,
					mi.rcMonitor.left,
					mi.rcMonitor.top,
					mi.rcMonitor.right - mi.rcMonitor.left,
					mi.rcMonitor.bottom - mi.rcMonitor.top,
					SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
		gbFullScreen = true;
	}
	else
	{
		fprintf(gpfile, "Full Screen to normal\n");
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPED);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd,
			HWND_TOP,
			0,
			0,
			0,
			0,
			SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER);
		ShowCursor(TRUE);
		gbFullScreen = false;
	}
}

//initialize() function
int initialize(void)
{
	//function declarations
	void resize(int, int);
	void uninitialize(void);

	//vaiable declaraions
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;
	GLenum result;
	GLint iShaderCompileStatus = 0;
	GLint iInfoLogLength = 0;
	GLchar *szInfoLog = NULL;
	GLint iProgramLinkStatus = 0;
	GLuint gVertexShaderObject;
	GLuint gFragmentShaderObject;

	//code
	//initialize pfd structure
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;

	ghdc = GetDC(ghwnd);
	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if (iPixelFormatIndex == 0)
	{
		return(-1);
	}
	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == false)
	{
		return(-2);
	}
	ghrc = wglCreateContext(ghdc);
	if (ghrc == NULL)
	{
		return(-3);
	}
	if (wglMakeCurrent(ghdc, ghrc) == false)
	{
		return(-4);
	}
	result = glewInit();
	if (result != GLEW_OK)
	{
		fprintf(gpfile, "glewInit() failed\n");
		uninitialize();
		DestroyWindow(ghwnd);
	}

	//define vertex shader object
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	//write vertex shader code
	const GLchar *VertexShaderSourceCode =
		"#version 430 core" \
		"\n	" \
		"in vec4 vPosition;" \
		"\n	" \
		"uniform mat4 u_mvp_matrix;" \
		"\n	" \
		"void main(void)" \
		"{" \
		"	gl_Position = u_mvp_matrix * vPosition;	" \
		"}";

	//specify above source code to the vertex shader object
	glShaderSource(gVertexShaderObject, 1, (const GLchar **)&VertexShaderSourceCode, NULL);

	//compile the vertex shader
	glCompileShader(gVertexShaderObject);
	glGetShaderiv(gVertexShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpfile, "VERTEX SHADER COMPILATION LOG : %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(ghwnd);
				exit(0);
			}
		}
	}

	//define fragment shader object
	gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	//write fragment shader code
	const GLchar *FragmentShaderSourceCode =
		"#version 430 core" \
		"\n	" \
		"out vec4 FragColor;" \
		"\n	" \
		"uniform vec3 u_primitive_color;\n	" \
		"\n	" \
		"void main(void)" \
		"{" \
		"FragColor = vec4(u_primitive_color, 1.0);" \
		"}";

	//specify above source code to fragmnet shader object
	glShaderSource(gFragmentShaderObject, 1, (const GLchar **)&FragmentShaderSourceCode, NULL);

	//compile fragment shader
	iShaderCompileStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;
	glCompileShader(gFragmentShaderObject);
	glGetShaderiv(gFragmentShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpfile, "FRAGMENT SHADER COMPILATION LOG : %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(ghwnd);
				exit(0);
			}
		}
	}

	//create shader program object
	gShaderProgramObject = glCreateProgram();

	//attach vertex shader to the shader program 
	glAttachShader(gShaderProgramObject, gVertexShaderObject);
	//attach fragment shader 
	glAttachShader(gShaderProgramObject, gFragmentShaderObject);

	//prelinking binding to vertex attribute
	glBindAttribLocation(gShaderProgramObject, AMC_ATTRIBUTE_POSITION, "vPosition");

	//link shader program
	glLinkProgram(gShaderProgramObject);
	//error checking
	iProgramLinkStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;
	glGetProgramiv(gShaderProgramObject, GL_LINK_STATUS, &iProgramLinkStatus);
	if (iProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLint written;
				glGetProgramInfoLog(gShaderProgramObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpfile, "SHADER PROGRAM LINK LOG : %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(ghwnd);
				exit(0);
			}
		}
	}

	//postlinking retieving uniform location
	mvpUniform = glGetUniformLocation(gShaderProgramObject, "u_mvp_matrix");
	primitiveColorUniform = glGetUniformLocation(gShaderProgramObject, "u_primitive_color");

	const GLfloat pointPosition[] =
	{
		2.0f, 1.0f, 0.0f,	1.0f, 1.0f, 0.0f,		0.0f, 1.0f, 0.0f,	-1.0f, 1.0f, 0.0f,
		2.0f, 0.0f, 0.0f,	1.0f, 0.0f, 0.0f,		0.0f, 0.0f, 0.0f,	-1.0f, 0.0f, 0.0f,
		2.0f, -1.0f, 0.0f,	1.0f, -1.0, 0.0f,		0.0f, -1.0f, 0.0f,	-1.0f, -1.0f, 0.0f,
		2.0f, -2.0f, 0.0f,	1.0f, -2.0f, 0.0f,		0.0f, -2.0f, 0.0f,	-1.0f, -2.0f, 0.0f
	};

	unsigned short pointIndices_1[] = 
	{
		0, 1, 2, 3,
		4, 5, 6, 7,
		8, 9, 10, 11,
		12, 13, 14, 15
	};

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	// vbo for position
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pointPosition), pointPosition, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// vbofor elements
	glGenBuffers(1, &vbo_elements);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_elements);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(pointIndices_1), NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);	//r,g,b,a	//clear the screen by opengl color

	glClearDepth(1.0f);						//giving existance to depth buffer
	glEnable(GL_DEPTH_TEST);				// enableing depth test
	glDepthFunc(GL_LEQUAL);

	perspectiveProjectionMatrix = mat4::identity();

	//wramup call to resize
	resize(WIN_WIDTH, WIN_HEIGHT);

	return(0);
}


//display function
void display(void)
{
	//declaration of matrices
	mat4 modelViewMatrix;
	mat4 modelViewProjectionMatrix;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(gShaderProgramObject);
	//initialize above matrices to identity
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();

	/*	part  1 start */
	unsigned short pointIndices_1[] = {
		0, 1, 2, 3,
		4, 5, 6, 7,
		8, 9, 10, 11,
		12, 13, 14, 15
	};

	//do necessary matrix multiplication
	modelViewMatrix = translate(-7.0f, 3.0f, -15.0f);
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	//send necessary matrices to shader in respective uniforms
	glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	glUniform3fv(primitiveColorUniform, 1, vec3(1.0f, 1.0f, 1.0f));

	glPointSize(5.0f);
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_elements);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(pointIndices_1), pointIndices_1, GL_DYNAMIC_DRAW);
	glDrawElements(GL_POINTS, 16, GL_UNSIGNED_SHORT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//unbind vao
	glBindVertexArray(0);
	/*	part  1 end */

	/*	part  2 start */
	unsigned short pointIndices_2[] = 
	{
		0, 3, 7, 4, 11, 8,
		3, 15, 2, 14, 1, 13,
		7, 2, 11, 1,
		15, 0, 14, 4,
		13, 8
	};

	// reinitialize matrices to identity
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();

	//do necessary matrix multiplication
	modelViewMatrix = translate(0.0f, 3.0f, -15.0f);
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	//send necessary matrices to shader in respective uniforms
	glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	glUniform3fv(primitiveColorUniform, 1, vec3(1.0f, 1.0f, 1.0f));

	glPointSize(5.0f);
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_elements);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(pointIndices_2), pointIndices_2, GL_DYNAMIC_DRAW);
	glDrawElements(GL_LINES, 22, GL_UNSIGNED_SHORT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//unbind vao
	glBindVertexArray(0);

	unsigned short pointIndices_3[] = {
		0, 3, 7, 4, 11, 8, 15, 12,
		3, 15, 2, 14, 1, 13, 0, 12
	};

	// reinitialize matrices to identity
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();

	//do necessary matrix multiplication
	modelViewMatrix = translate(7.0f, 3.0f, -15.0f);
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	//send necessary matrices to shader in respective uniforms
	glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	glUniform3fv(primitiveColorUniform, 1, vec3(1.0f, 1.0f, 1.0f));

	glPointSize(5.0f);
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_elements);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(pointIndices_3), pointIndices_3, GL_DYNAMIC_DRAW);
	glDrawElements(GL_LINES, 16, GL_UNSIGNED_SHORT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//unbind vao
	glBindVertexArray(0);

	/*	part 3 end */

	/*	part 4 start */
	unsigned short pointIndices_4[] = {
		0, 3, 7, 4, 11, 8, 15, 12,
		3, 15, 2, 14, 1, 13, 0, 12,
		7, 2, 11, 1, 15, 0, 14, 4, 13, 8
	};

	// reinitialize matrices to identity
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();

	//do necessary matrix multiplication
	modelViewMatrix = translate(-7.0f, -3.0f, -15.0f);
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	//send necessary matrices to shader in respective uniforms
	glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	glUniform3fv(primitiveColorUniform, 1, vec3(1.0f, 1.0f, 1.0f));

	glPointSize(5.0f);
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_elements);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(pointIndices_4), pointIndices_4, GL_DYNAMIC_DRAW);
	glDrawElements(GL_LINES, 26, GL_UNSIGNED_SHORT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	//unbind vao
	glBindVertexArray(0);

	/*	part 4 end */

	/*	part 5 start */
	unsigned short pointIndices_5[] = {
		3, 0, 3, 4,
		3, 8, 3, 12,
		3, 13, 3, 14,
		3, 15, 0, 12,
		15, 12
	};

	// reinitialize matrices to identity
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();

	//do necessary matrix multiplication
	modelViewMatrix = translate(0.0f, -3.0f, -15.0f);
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	//send necessary matrices to shader in respective uniforms
	glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	glUniform3fv(primitiveColorUniform, 1, vec3(1.0f, 1.0f, 1.0f));

	glPointSize(5.0f);
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_elements);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(pointIndices_5), pointIndices_5, GL_DYNAMIC_DRAW);
	glDrawElements(GL_LINES, 18, GL_UNSIGNED_SHORT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	//unbind vao
	glBindVertexArray(0);
	/*	part 5 end */

	/*	part 6 start */
	unsigned short pointIndices_6[] = {
		2, 3, 7, 7, 6, 2,
		6, 7, 11, 11, 10, 6,
		10, 11, 15, 15, 14, 10,				// red strip
		1, 2, 6, 6, 5, 1,
		5, 6, 10, 10, 9, 5,
		9, 10, 14, 14, 13, 9,				// green strip
		0, 1, 5, 5, 4, 0,
		4, 5, 9, 9, 8, 4,
		8, 9, 13, 13, 12, 8,				// blue strip
		0, 3, 4, 7, 8, 11, 12, 15,			// horizontal lines
		0, 12, 1, 13, 2, 14, 3, 15			// vertical lines
	};

	// reinitialize matrices to identity
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();

	//do necessary matrix multiplication
	modelViewMatrix = translate(7.0f, -3.0f, -15.0f);
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	//send necessary matrices to shader in respective uniforms
	glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	glUniform3fv(primitiveColorUniform, 1, vec3(1.0f, 0.0f, 0.0f));

	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_elements);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(pointIndices_6), pointIndices_6, GL_DYNAMIC_DRAW);
	glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_SHORT, 0);		// red strip
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(vao);

	glUniform3fv(primitiveColorUniform, 1, vec3(0.0f, 1.0f, 0.0f));

	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_elements);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(pointIndices_6), pointIndices_6, GL_DYNAMIC_DRAW);
	glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_SHORT, (const void*)(9 * sizeof(unsigned int)));	// green strip
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glUniform3fv(primitiveColorUniform, 1, vec3(0.0f, 0.0f, 1.0f));

	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_elements);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(pointIndices_6), pointIndices_6, GL_DYNAMIC_DRAW);
	glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_SHORT, (const void*)(18 * sizeof(unsigned int)));	// green strip
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glUniform3fv(primitiveColorUniform, 1, vec3(1.0f, 1.0f, 1.0f));

	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_elements);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(pointIndices_6), pointIndices_6, GL_DYNAMIC_DRAW);
	glDrawElements(GL_LINES, 16, GL_UNSIGNED_SHORT, (const void*)(27 * sizeof(unsigned int)));	// green strip
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	/*	part 6 end */

	//unuse program
	glUseProgram(0);
	SwapBuffers(ghdc);
}

//reize() function
void resize(int win_width, int win_height)
{
	if (win_height == 0)
		win_height = 1;
	glViewport(0, 0, (GLsizei)win_width, (GLsizei)win_height);
	perspectiveProjectionMatrix = perspective(45.0f, (GLfloat)win_width / (GLfloat)win_height, 0.1f, 100.0f);
}

//uninitialize() function
void uninitialize(void)
{
	if (vbo)
	{
		glDeleteBuffers(1, &vbo);
		vbo = 0;
	}
	if (vao)
	{
		glDeleteVertexArrays(1, &vao);
		vao = 0;
	}
	if (gShaderProgramObject)
	{
		GLsizei shaderCount;
		GLsizei shaderNumber;

		glUseProgram(gShaderProgramObject);
		glGetProgramiv(gShaderProgramObject, GL_ATTACHED_SHADERS, &shaderCount);
		GLuint *pShaders = (GLuint *)malloc(sizeof(GLuint) * shaderCount);

		if (pShaders)
		{
			glGetAttachedShaders(gShaderProgramObject, shaderCount, &shaderCount, pShaders);
			for (shaderNumber = 0; shaderNumber < shaderCount; shaderNumber++)
			{
				//detach all the shader one by one
				glDetachShader(gShaderProgramObject, pShaders[shaderNumber]);
				//delete detached shader
				glDeleteShader(pShaders[shaderNumber]);
				pShaders[shaderNumber] = 0;
			}
			free(pShaders);
		}
		glDeleteProgram(gShaderProgramObject);
		gShaderProgramObject = 0;
		glUseProgram(0);
	}

	if (gbFullScreen == true)
	{
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPED);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd,
			HWND_TOP,
			0,
			0,
			0,
			0,
			SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER);
		ShowCursor(TRUE);
	}

	//break the current context
	if (wglGetCurrentContext() == ghrc)
	{
		wglMakeCurrent(NULL, NULL);
	}

	if (ghrc)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
	}

	if (ghdc)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	if (gpfile)
	{
		fprintf(gpfile, "log file closed successfully\n");
		fclose(gpfile);
		gpfile = NULL;
	}
}

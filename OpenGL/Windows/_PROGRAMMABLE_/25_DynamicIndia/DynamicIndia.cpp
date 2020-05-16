#include<Windows.h>
#include<GL/glew.h>
#include<gl/GL.h>
#include<stdio.h>
#include<math.h>

#include "vmath.h"
#define _USE_MATH_DEFINES 1

#define num 1000;

#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"opengl32.lib")


#define WIN_WIDTH 800
#define WIN_HEIGHT 600

using namespace vmath;

enum 
{
	AMC_ATTRIBUTE_POSITION = 0,
	AMC_ATTRIBUTE_COLOR,
	AMC_ATTRIBUTE_NORMAL,
	AMC_ATTRIBUTE_TEXCOORD0
};

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

bool bFullScreen = false;
HWND ghHwnd = NULL;
DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
HDC ghdc = NULL;
HGLRC ghrc = NULL;
bool gbActiveWindow = false;
FILE *gpFile = NULL;

static GLfloat angle_tri = 0.0f;
static GLfloat angle_rect = 0.0f;

float  planePosX = -1.95f;
float planeI = -1.46f;
float planeA = -1.65;
float planeF = -1.40f;

GLint gVertexShaderObject_SB;
GLint gFragmentShaderObject_SB;
GLint gShaderProgramObject_SB;

GLuint vao_letters_SB;
GLuint vao_rectangle_SB;
GLuint vbo_position_letters_SB;
GLuint vbo_position_rectangle_SB;
GLuint vbo_color_letters_SB;

GLuint mvpUniform;
mat4 perspectiveProjectionMatrix;

//declaration of matrices
mat4 modelViewMatrix;
mat4 modelViewProjectionMatrix;
mat4 translationMatrix;
mat4 scaleMatrix;


/////////////////

const GLfloat IVertices[] = {
	 -1.75f, 0.75f,0.0f,
	-1.75f, 0.65f,0.0f,
	-1.25f, 0.65f,0.0f ,
	-1.25f, 0.75f ,0.0f,
	-1.35f, 0.65f ,0.0f,
	-1.65f, 0.65f ,0.0f,
	-1.65f, -0.65f ,0.0f,
	-1.35f, -0.65f ,0.0f,
	-1.75f, -0.65f ,0.0f,
	-1.75f, -0.75f ,0.0f,
	-1.25f, -0.75f ,0.0f,
	-1.25f, -0.65f ,0.0f
};
const GLfloat NVertices[] = {
	-1.10f, 0.75f ,0.0f,
	-1.25f, 0.75f ,0.0f,
	-1.25f, -0.75f ,0.0f,
	-1.10f, -0.75f ,0.0f,
	-1.10f, 0.75f ,0.0f,
	-1.25f, 0.75f ,0.0f,
	-0.95f, -0.75f ,0.0f,
	-0.8f, -0.75f ,0.0f,
	-0.80f, 0.75f ,0.0f,
	-0.95f, 0.75f ,0.0f,
	-0.95f, -0.75f ,0.0f,
	-0.80f, -0.75f ,0.0f
};
const GLfloat DVertices[] = {
-0.0f, 0.75f ,0.0,
-0.60f, 0.75f ,0.0,
-0.60f, -0.75f ,0.0,
0.0f, -0.75f ,0.0,
-0.15f, 0.60f ,0.0,
-0.35f, 0.60f ,0.0,
-0.35f, -0.60f,0.0,
-0.15f, -0.60f,0.0,
-0.5f, 0.60f,0.0,
-0.60f, 0.60f,0.0,
-0.60f, -0.60f,0.0,
-0.5f, -0.60f,0.0

};

const GLfloat AVertices[] = {
1.25f, 0.75f,0.0,
	1.1f, 0.75f,0.0,
	0.9f, -0.75f,0.0,
	1.55f, -0.75f,0.0,
	1.18f, 0.5f,0.0,
	1.05f, -0.75f,0.0,
	1.40f, -0.75f,0.0

};
const GLfloat FlagVertices[] = {
	0.057f, 0.0f,0.0,
	0.203f, 0.0f,0.0,
	0.205f, -0.05f,0.0,
	0.045f, -0.05f,0.0,
	0.057f, 0.0f ,0.0,
	0.2029f, 0.0f,0.0,
	0.195f, 0.05f,0.0,
	 0.065f, 0.05f,0.0f
};


const GLfloat IColor[] = {
1.0f, 0.6f, 0.2f,
1.0f, 0.6f, 0.2f,
1.0f, 0.6f, 0.2f,
1.0f, 0.6f, 0.2f ,
1.0f, 0.6f, 0.2f ,
1.0f, 0.6f, 0.2f,
0.071f, 0.533f, 0.027f ,
0.071f, 0.533f, 0.027f ,
0.071f, 0.533f, 0.027f ,
0.071f, 0.533f, 0.027f ,
0.071f, 0.533f, 0.027f ,
0.071f, 0.533f, 0.027f
};
const GLfloat NColor[] = {
1.0f, 0.6f, 0.2f ,
1.0f, 0.6f, 0.2f ,
0.071f, 0.533f, 0.027f ,
0.071f, 0.533f, 0.027f ,
1.0f, 0.6f, 0.2f ,
1.0f, 0.6f, 0.2f ,
0.071f, 0.533f, 0.027f ,
0.071f, 0.533f, 0.027f ,
1.0f, 0.6f, 0.2f ,
1.0f, 0.6f, 0.2f ,
0.071f, 0.533f, 0.027f ,
0.071f, 0.533f, 0.027f
};

const GLfloat AColor[] = {
1.0f, 0.6f, 0.2f,
1.0f, 0.6f, 0.2f,
0.071f, 0.533f, 0.027f,
0.071f, 0.533f, 0.027f,
0.0f, 0.0f, 0.0f,
0.0f, 0.0f, 0.0f,
0.0f, 0.0f, 0.0f
};

const GLfloat FlagColor[] = {
1.0f, 1.0f, 1.0f,
1.0f, 1.0f, 1.0f,
0.071f, 0.533f, 0.027f,
0.071f, 0.533f, 0.027f,
1.0f, 1.0f, 1.0f,
1.0f, 1.0f, 1.0f,
1.0f, 0.6f, 0.2f,
1.0f, 0.6f, 0.2f
};
///////////////////////////////////////////



///////////////////////////////////

////////////////////////////
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	//function Declarations
	int initialize(void);
	void Display();
	bool bDone = false;
	int iRet = 0;
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("DYNAMIC INDIA -SHUBHAM");
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	RegisterClassEx(&wndclass);

	if (fopen_s(&gpFile, "log.txt", "w") != 0)
	{
		MessageBox(NULL, TEXT("Log file canont be created"), TEXT("Error"), MB_OK);
		exit(0);
	}
	else
	{
		fprintf(gpFile, "Log file is created\n");
		//fprintf();
	}

	hwnd = CreateWindowEx(WS_EX_APPWINDOW, szAppName, TEXT("Dynamic India PP"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		100,
		100,
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);
	ghHwnd = hwnd;
	iRet = initialize();

	if (iRet == -1)
	{
		fprintf(gpFile, "Choose pixel format Failed\n");
		DestroyWindow(hwnd);
	}
	else if (iRet == -2)
	{
		fprintf(gpFile, "Set pixel format Failed\n");
		DestroyWindow(hwnd);
	}
	else if (iRet == -3)
	{
		fprintf(gpFile, "wglCreateContext Failed\n");
		DestroyWindow(hwnd);
	}
	else if (iRet == -4)
	{
		fprintf(gpFile, "wglMakeCurrent Failed\n");
		DestroyWindow(hwnd);
	}
	else
	{
		fprintf(gpFile, "Initialization Succeeded\n");

	}
	ShowWindow(hwnd, iCmdShow);
	//UpdateWindow(hwnd);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);


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
			if (gbActiveWindow)
			{
				//UPDATE
			}
			Display();
		}

	}
	return ((int)msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	void ToggleFullScreen();
	void Resize(int, int);
	void uninitialize();

	switch (iMsg)
	{
	case WM_CREATE:
		PostMessage(hwnd, WM_KEYDOWN, 0x46, NULL);
		break;

	case WM_SETFOCUS:
		gbActiveWindow = true;
		break;
	case WM_KILLFOCUS:
		gbActiveWindow = false;
		break;
	case WM_SIZE:
		Resize(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_ERASEBKGND:
		return 0;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;

		
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			if (bFullScreen == true)
				ToggleFullScreen();
			DestroyWindow(hwnd);
			break;
		case 0x46:
			ToggleFullScreen();

		case'p':
			fprintf(gpFile,"pos for plane x : %f\n", planePosX);
			break;
		}
		break;
	case WM_DESTROY:
		uninitialize();
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}

void ToggleFullScreen()
{
	MONITORINFO mi;
	if (bFullScreen == false)
	{
		dwStyle = GetWindowLong(ghHwnd, GWL_STYLE);
		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			mi = { sizeof(MONITORINFO) };
			if (GetWindowPlacement(ghHwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghHwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghHwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghHwnd,
					HWND_TOP,
					mi.rcMonitor.left,
					mi.rcMonitor.top,
					mi.rcMonitor.right - mi.rcMonitor.left,
					mi.rcMonitor.bottom - mi.rcMonitor.top,
					SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
		bFullScreen = true;
	}
	else
	{
		SetWindowLong(ghHwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghHwnd, &wpPrev);
		SetWindowPos(ghHwnd,
			HWND_TOP,
			0,
			0,
			0,
			0,
			SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER);
		ShowCursor(TRUE);
		bFullScreen = false;
	}
}
int initialize(void)
{
	void Resize(int, int);
	void uninitialize(void);

	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;
	GLenum result;

	
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cBlueBits = 8;
	pfd.cGreenBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 32;

	ghdc = GetDC(ghHwnd);
	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if (iPixelFormatIndex == 0)
	{
		return -1;
	}
	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE)
	{
		return -2;
	}
	ghrc = wglCreateContext(ghdc);
	if (ghrc == NULL)
	{
		return -3;
	}
	if (wglMakeCurrent(ghdc, ghrc) == FALSE)
	{
		return -4;
	}
	result = glewInit();
	fwprintf(gpFile, TEXT("Value of ivertex shader compile Status is : %d"), result);
	if (result != GLEW_OK)
	{
		fprintf(gpFile, "GLEW Init Failed\n");
		uninitialize();
		DestroyWindow(ghHwnd);
	}
	//define vertex shader object
	gVertexShaderObject_SB = glCreateShader(GL_VERTEX_SHADER);

	//write vertex shader code
	const GLchar *vertexShaderSourceCode =
		"#version 330 core" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec4 vColor;" \
		"uniform mat4 u_mvp_matrix;" \
		"out vec4 out_color;" \
		"void main(void)" \
		"{" \
		"gl_Position=u_mvp_matrix * vPosition;" \
		"out_color=vColor;" \
		"}";

	//specify above source code to vertexShaderObject
	glShaderSource(gVertexShaderObject_SB, 1,
		(const GLchar**)&vertexShaderSourceCode,
		NULL);
	glCompileShader(gVertexShaderObject_SB);
	GLint iShaderCompileStatus = 0;
	GLint iInfoLogLength = 0;
	GLchar *szInfoLog = NULL;

	glGetShaderiv(gVertexShaderObject_SB,
		GL_COMPILE_STATUS,
		&iShaderCompileStatus);
	fwprintf(gpFile, TEXT("Value of ivertex shader compile Status is : %d"), iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject_SB,
			GL_INFO_LOG_LENGTH,
			&iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei writtened;
				glGetShaderInfoLog(gVertexShaderObject_SB,
					iInfoLogLength,
					&writtened,
					szInfoLog);
				fwprintf(gpFile, TEXT("Error in compiling vertex Shader : %hs"), szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(ghHwnd);
			}
		}
	}

	//define Fragment shader object
	gFragmentShaderObject_SB = glCreateShader(GL_FRAGMENT_SHADER);

	//write fragment shader code
	const GLchar *fragmentShaderSourceCode =
		"#version 330 core" \
		"\n" \
		"in vec4 out_color;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"FragColor=out_color;" \
		"}";

	//specify above source code to FragmentShaderObject
	glShaderSource(gFragmentShaderObject_SB, 1,
		(const GLchar**)&fragmentShaderSourceCode,
		NULL);
	glCompileShader(gFragmentShaderObject_SB);

	iShaderCompileStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;

	glGetShaderiv(gFragmentShaderObject_SB,
		GL_COMPILE_STATUS,
		&iShaderCompileStatus);
	fwprintf(gpFile, TEXT("Value of ifragment shader compile Status is : %d"), iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject_SB,
			GL_INFO_LOG_LENGTH,
			&iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei writtened;
				glGetShaderInfoLog(gFragmentShaderObject_SB,
					iInfoLogLength,
					&writtened,
					szInfoLog);
				fwprintf(gpFile, TEXT("Error in compiling Fragment Shader : %hs"), szInfoLog);
				free(szInfoLog);
				uninitialize();
				//DestroyWindow(ghHwnd);
			}
		}
	}

	//create Shader program object
	gShaderProgramObject_SB = glCreateProgram();

	//attach vertex/fragment shaders
	glAttachShader(gShaderProgramObject_SB,
		gVertexShaderObject_SB);
	glAttachShader(gShaderProgramObject_SB,
		gFragmentShaderObject_SB);

	//prelinking binding to vertex attributes
	glBindAttribLocation(gShaderProgramObject_SB, AMC_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(gShaderProgramObject_SB, AMC_ATTRIBUTE_COLOR, "vColor");
	//Link the shader program
	fwprintf(gpFile, TEXT("attach comp\n"));
	glLinkProgram(gShaderProgramObject_SB);
	//ErrorCheck for linking

	GLint iProgramLinkStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;

	glGetProgramiv(gShaderProgramObject_SB,
		GL_LINK_STATUS,
		&iProgramLinkStatus);
	fwprintf(gpFile, TEXT("Value of iProgramLinkStatus is : %d"), iProgramLinkStatus);
	if (iProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObject_SB,
			GL_INFO_LOG_LENGTH,
			&iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei writtened;
				glGetProgramInfoLog(gShaderProgramObject_SB,
					iInfoLogLength,
					&writtened,
					szInfoLog);
				fwprintf(gpFile, TEXT("Error in Linking Shader : %hs"), szInfoLog);
				free(szInfoLog);
				uninitialize();
				//DestroyWindow(ghHwnd);
			}
		}
	}

	//post linking retrieving uniform location
	mvpUniform = glGetUniformLocation(gShaderProgramObject_SB, "u_mvp_matrix");

	const GLfloat triangleVerticesColor[] = {

	1.0f, 0.6f, 0.2f ,		
	1.0f, 0.6f, 0.2f ,		
	1.0f, 0.6f, 0.2f ,		
	1.0f, 0.6f, 0.2f ,		
	1.0f, 0.6f, 0.2f ,		
	1.0f, 0.6f, 0.2f ,		
	0.071f, 0.533f, 0.027f ,
	0.071f, 0.533f, 0.027f ,
	0.071f, 0.533f, 0.027f ,
	0.071f, 0.533f, 0.027f ,
	0.071f, 0.533f, 0.027f ,
	0.071f, 0.533f, 0.027f 

	};

	//create vao
	glGenVertexArrays(1, &vao_letters_SB);
	glBindVertexArray(vao_letters_SB);
	glGenBuffers(1, &vbo_position_letters_SB);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_letters_SB);
	glBufferData(GL_ARRAY_BUFFER, 500 * 3 * 4 * sizeof(GLfloat),
		NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//for color
	glGenBuffers(1, &vbo_color_letters_SB);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_color_letters_SB);
	glBufferData(GL_ARRAY_BUFFER, 500 * 3 * 4 * sizeof(GLfloat),
		NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	/////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);
	perspectiveProjectionMatrix = mat4::identity();
	Resize(WIN_WIDTH, WIN_HEIGHT);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	return 0;
}
void Resize(int width, int height)
{
	if (height == 0)
		height = 1;
	glViewport(0, 0, GLsizei(width), GLsizei(height));
	perspectiveProjectionMatrix = perspective(45.0f,
		(GLfloat)width / (GLfloat)height,
		0.1f,
		100.0f);
}
void Display(void)
{
	void DrawPlane();

	static GLfloat Cr = 0.0f, Cg = 0.0f, Cb = 0.0f;
	static GLfloat Gr = 0.0f, Gg = 0.0f, Gb = 0.0f;

	static GLfloat Counter_one = 0.0f;
	static GLfloat Counter_two = 0.0f;
	static GLfloat Counter_three = 0.0f;
	static GLfloat Counter_four = 0.0f;
	static GLfloat counterP1 = 0.0f;
	static GLfloat xTranslate = 0.0f;
	static GLfloat timer = 0.0f;

	static bool flagI1 = false;
	static bool flagA = false;
	static bool flagN = false;
	static bool flagI2 = false;
	static bool flagD = false;
	
	const GLfloat DColor[] = 
	{
			Cr, Cg, Cb ,
			Cr, Cg, Cb ,
			Gr, Gg, Gb ,
			Gr, Gg, Gb ,
			0.0f, 0.0f, 0.0f ,
			0.0f, 0.0f, 0.0f ,
			0.0f, 0.0f, 0.0f ,
			0.0f, 0.0f, 0.0f ,
			0.0f, 0.0f, 0.0f ,
			0.0f, 0.0f, 0.0f ,
			0.0f, 0.0f, 0.0f ,
			0.0f, 0.0f, 0.0f
	};
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(gShaderProgramObject_SB);

	//initialize above to identity
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();
	translationMatrix = mat4::identity();
	scaleMatrix = mat4::identity();
	//do necessary transformation
	translationMatrix = translate(-2.0f + Counter_one, 0.0f, -4.0f);
	scaleMatrix = scale(0.8f, 0.9f, 0.0f);
	modelViewMatrix = translationMatrix;
	//do necessary matrix multiplication
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	//send necessary matrices to shaders in respective uniforms
	glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	//similarly bind with textures if any
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_letters_SB);
	glBufferData(GL_ARRAY_BUFFER, sizeof(IVertices),
		IVertices, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_color_letters_SB);
	glBufferData(GL_ARRAY_BUFFER, sizeof(IColor),
		IColor, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//bind with vao
	glBindVertexArray(vao_letters_SB);
	//draw necessary scene
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	//unbind vao
	glBindVertexArray(0);
	modelViewMatrix = mat4::identity();
	translationMatrix = mat4::identity();
	translationMatrix = translate(0.2f, 6.5f - Counter_three, -4.0f);
	modelViewMatrix = translationMatrix * modelViewMatrix;
	//do necessary matrix multiplication
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	//send necessary matrices to shaders in respective uniforms
	glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_letters_SB);
	glBufferData(GL_ARRAY_BUFFER, sizeof(NVertices),
		NVertices, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_color_letters_SB);
	glBufferData(GL_ARRAY_BUFFER, sizeof(NColor),
		NColor, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//bind with vao
	glBindVertexArray(vao_letters_SB);
	//draw necessary scene
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	//unbind vao
	glBindVertexArray(0);

	modelViewMatrix = mat4::identity();
	translationMatrix = mat4::identity();
	translationMatrix = translate(0.2f, 0.0f, -4.0f);
	modelViewMatrix = translationMatrix * modelViewMatrix;
	//do necessary matrix multiplication
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	//send necessary matrices to shaders in respective uniforms
	glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_letters_SB);
	glBufferData(GL_ARRAY_BUFFER, sizeof(DVertices),
		DVertices, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_color_letters_SB);
	glBufferData(GL_ARRAY_BUFFER, sizeof(DColor),
		DColor, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//bind with vao
	glBindVertexArray(vao_letters_SB);
	//draw necessary scene
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	//unbind vao
	glBindVertexArray(0);

	modelViewMatrix = mat4::identity();
	translationMatrix = mat4::identity();
	translationMatrix = translate(2.15f, -2.5f + Counter_four, -4.0f);
	modelViewMatrix = translationMatrix * modelViewMatrix;
	//do necessary matrix multiplication
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	//send necessary matrices to shaders in respective uniforms
	glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_letters_SB);
	glBufferData(GL_ARRAY_BUFFER, sizeof(IVertices),
		IVertices, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_color_letters_SB);
	glBufferData(GL_ARRAY_BUFFER, sizeof(IColor),
		IColor, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//bind with vao
	glBindVertexArray(vao_letters_SB);
	//draw necessary scene
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	//unbind vao
	glBindVertexArray(0);

	modelViewMatrix = mat4::identity();
	translationMatrix = mat4::identity();
	translationMatrix = translate(2.05f - Counter_two, 0.0f, -4.0f);
	modelViewMatrix = translationMatrix * modelViewMatrix;
	//do necessary matrix multiplication
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	//send necessary matrices to shaders in respective uniforms
	glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_letters_SB);
	glBufferData(GL_ARRAY_BUFFER, sizeof(AVertices),
		AVertices, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_color_letters_SB);
	glBufferData(GL_ARRAY_BUFFER, sizeof(AColor),
		AColor, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//bind with vao
	glBindVertexArray(vao_letters_SB);
	//draw necessary scene
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDrawArrays(GL_TRIANGLES, 4, 3);
	
	glBindVertexArray(0);

	if (flagI1 == true && flagA == true && flagN == true && flagI2 == true && Cr >= 1.0f && Cg >= 0.6f && Cb >= 0.2f)
	{
		translationMatrix = mat4::identity();
		if (xTranslate <= 1.25f)
		{
			translationMatrix = translate(-4.5f + 2.4f*counterP1 + 1.0f, (float)sin((counterP1*2.0f - 2.0f) / 2.0f) / 0.5f - 0.5f, -3.0f);
			modelViewMatrix = translationMatrix * modelViewMatrix;
			//do necessary matrix multiplication
			modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

			//send necessary matrices to shaders in respective uniforms
			glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
			//glTranslatef(-4.5f + 2.4f*counterP1 + 1.0f, sin((counterP1*2.0f - 2.0f) / 2.0f) / 0.5f - 0.5f, -4.0f);
		}
		else
		{
			translationMatrix = translate(-4.5f + 2.4f*xTranslate + 1.0f, 0.0f, -3.0f);
			modelViewMatrix = translationMatrix * modelViewMatrix;
			//do necessary matrix multiplication
			modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

			//send necessary matrices to shaders in respective uniforms
			glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
			//glTranslatef(-4.5f + 2.4f*xTranslate + 1.0f, 0.0f, -4.0f);
		}
		if (xTranslate > 2.09f)
		{
			translationMatrix = translate(-4.5f + 2.4f*counterP1 + 1.5f, (float)sin((counterP1*2.0f - 2.0f) / 2.0f) / 0.5f - 0.5f, -3.0f);
			modelViewMatrix = translationMatrix * modelViewMatrix;
			//do necessary matrix multiplication
			modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

			//send necessary matrices to shaders in respective uniforms
			glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
			//glTranslatef(-4.5f + 2.4f*counterP1 + 1.5f, sin((counterP1*2.0f - 2.0f) / 2.0f) / 0.5f - 0.5f, 0.0f);
		}

		if (planePosX < 2.5f)
		{
			DrawPlane();

			planePosX += 0.005f;
		}
		
		/*planeI += 0.0019f;
		planeA += 0.002f;
		planeF += 0.002f;*/

		//Draw Flag
		if (planePosX > 1.2f )
		{
			modelViewMatrix = mat4::identity();
			translationMatrix = mat4::identity();
			translationMatrix = translate(0.93f, 0.0f, -3.0f);
			modelViewMatrix = translationMatrix * scaleMatrix*modelViewMatrix;
			//do necessary matrix multiplication
			modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

			//send necessary matrices to shaders in respective uniforms
			glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
			glBindBuffer(GL_ARRAY_BUFFER, vbo_position_letters_SB);
			glBufferData(GL_ARRAY_BUFFER, sizeof(FlagVertices),
				FlagVertices, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindBuffer(GL_ARRAY_BUFFER, vbo_color_letters_SB);
			glBufferData(GL_ARRAY_BUFFER, sizeof(FlagColor),
				FlagColor, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			//bind with vao
			glBindVertexArray(vao_letters_SB);
			//draw necessary scene
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
			glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
			//unbind vao
			glBindVertexArray(0);

			GLfloat circle[1000];
			GLfloat circleColor[1000];
			for (int i = 0; i < 998; i = i + 3)
			{
				GLfloat angle = 2 * M_PI*i / num;
				circle[i] = 0.01f*cos(angle);
				circle[i + 1] = 0.01f*sin(angle);
				circle[i + 2] = 0.0f;
				circleColor[i] = 0.0f;
				circleColor[i + 1] = 0.0f;
				circleColor[i + 2] = 1.0f;
			}
			modelViewMatrix = mat4::identity();
			translationMatrix = mat4::identity();
			translationMatrix = translate(1.02f, 0.0f, -3.0f);
			modelViewMatrix = translationMatrix * modelViewMatrix;
			//do necessary matrix multiplication
			modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

			//send necessary matrices to shaders in respective uniforms
			glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
			glBindBuffer(GL_ARRAY_BUFFER, vbo_position_letters_SB);
			glBufferData(GL_ARRAY_BUFFER, sizeof(circle),
				circle, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindBuffer(GL_ARRAY_BUFFER, vbo_color_letters_SB);
			glBufferData(GL_ARRAY_BUFFER, sizeof(circleColor),
				circleColor, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			//bind with vao
			glBindVertexArray(vao_letters_SB);
			//draw necessary scene
			//glPointSize(2.0f);
			for (int i = 0; i < 998; i += 2)
			{
				glDrawArrays(GL_POINTS, i, i + 2);
			}
			//unbind vao
			glBindVertexArray(0);
		}

	}
	//unuse program
	glUseProgram(0);
	SwapBuffers(ghdc);


	/////////Update///////////////////////////////

	if (Counter_one <= 2.0f && flagI1 == false)
		Counter_one = Counter_one + 0.020f;
	else
	{
		flagI1 = true;
	}
	if (Counter_two <= 1.85f && flagI1 == true)
		Counter_two = Counter_two + 0.020f;
	else
	{
		flagA = true;
	}
	if (Counter_three <= 6.5f &&  flagA == true)
		Counter_three = Counter_three + 0.020f;
	else
	{
		flagN = true;
	}
	if (Counter_four <= 2.5f &&  flagN == true)
		Counter_four = Counter_four + 0.020f;
	else
	{
		flagI2 = true;
	}
	if (Counter_four >= 2.5f)
	{
		if (Cr <= 1.0f)
			Cr = Cr + 0.020f;
		if (Cg <= 0.6f)
			Cg = Cg + 0.020f;
		if (Cb <= 0.2)
			Cb = Cb + 0.020f;
		if (Gr <= 0.071f)
			Gr = Gr + 0.020f;
		if (Gg <= 0.533f)
			Gg = Gg + 0.020f;
		if (Gb <= 0.027)
			Gb = Gb + 0.020f;
	}
	else
	{
		flagD = true;
	}
	if (counterP1 <= 2.9f &&   Cr > 0.99f && Gg > 0.472f)
	{
		if (xTranslate <= 1.25f)
		{
			counterP1 = counterP1 + 0.0007f;
			xTranslate += 0.0007f;
		}
		else if (xTranslate >= 1.25f && xTranslate <= 2.09f)
		{
			xTranslate += 0.0007f;
		}
		else
		{
			
			timer += 0.0007f;
		}
		if (timer >= 1.92f)
		{
			counterP1 = counterP1 + 0.001f;

		}
	}

	
}

void DrawPlane()
{
	////////////
	/////////////////////Plane Arrays
//I
	const GLfloat IVertices[] =
	{
		 -1.75f, 0.75f,0.0f,
		-1.75f, 0.65f,0.0f,
		-1.25f, 0.65f,0.0f ,
		-1.25f, 0.75f ,0.0f,
		-1.35f, 0.65f ,0.0f,
		-1.65f, 0.65f ,0.0f,
		-1.65f, -0.65f ,0.0f,
		-1.35f, -0.65f ,0.0f,
		-1.75f, -0.65f ,0.0f,
		-1.75f, -0.75f ,0.0f,
		-1.25f, -0.75f ,0.0f,
		-1.25f, -0.65f ,0.0f
	};

	//A
	const GLfloat AColor[] =
	{
		1.0f, 0.6f, 0.2f,
		1.0f, 0.6f, 0.2f,
		0.071f, 0.533f, 0.027f,
		0.071f, 0.533f, 0.027f,
		70.0f / 255.0f, 130.0f / 255.0f, 180.0f / 255.0f,
		70.0f / 255.0f, 130.0f / 255.0f, 180.0f / 255.0f,
		70.0f / 255.0f, 130.0f / 255.0f, 180.0f / 255.0f
	};

	//
	const GLfloat planeVertices[] =
	{
			0.0f , 0.1f , 0.0f,
		   -0.05f, 0.05f, 0.0f,
		   -0.05f,-0.05f, 0.0f,
			0.0f ,-0.1f , 0.0f,
		   -0.05f,-0.2f , 0.0f,
			0.0f ,-0.25f, 0.0f,
			0.2f ,-0.1f , 0.0f,
			0.4f ,-0.05f, 0.0f,
			0.45f, 0.0f , 0.0f,
			0.4f , 0.05f, 0.0f,
			0.2f , 0.1f , 0.0f,
			0.0f , 0.25f, 0.0f,
		   -0.05f, 0.2f , 0.0f
	};

	const GLfloat IColor[] =
	{
		1.0f, 0.6f, 0.2f,
		1.0f, 0.6f, 0.2f,
		1.0f, 0.6f, 0.2f,
		1.0f, 0.6f, 0.2f ,
		1.0f, 0.6f, 0.2f ,
		1.0f, 0.6f, 0.2f,
		0.071f, 0.533f, 0.027f ,
		0.071f, 0.533f, 0.027f ,
		0.071f, 0.533f, 0.027f ,
		0.071f, 0.533f, 0.027f ,
		0.071f, 0.533f, 0.027f ,
		0.071f, 0.533f, 0.027f
	};
	const GLfloat planeColor[] =
	{
		70.0f / 255.0f, 130.0f / 255.0f, 180.0f / 255.0f,
		70.0f / 255.0f, 130.0f / 255.0f, 180.0f / 255.0f,
		70.0f / 255.0f, 130.0f / 255.0f, 180.0f / 255.0f,
		70.0f / 255.0f, 130.0f / 255.0f, 180.0f / 255.0f,
		70.0f / 255.0f, 130.0f / 255.0f, 180.0f / 255.0f,
		70.0f / 255.0f, 130.0f / 255.0f, 180.0f / 255.0f,
		70.0f / 255.0f, 130.0f / 255.0f, 180.0f / 255.0f,
		70.0f / 255.0f, 130.0f / 255.0f, 180.0f / 255.0f,
		70.0f / 255.0f, 130.0f / 255.0f, 180.0f / 255.0f,
		70.0f / 255.0f, 130.0f / 255.0f, 180.0f / 255.0f,
		70.0f / 255.0f, 130.0f / 255.0f, 180.0f / 255.0f,
		70.0f / 255.0f, 130.0f / 255.0f, 180.0f / 255.0f,
		70.0f / 255.0f, 130.0f / 255.0f, 180.0f / 255.0f
	};
	const GLfloat flagVertices[] =
	{
		-0.2f, -0.05f ,0.0f,
		-0.05f, -0.05f,0.0f,
		-0.2f, 0.0f,0.0f,
		-0.05f, 0.0f,0.0f,
		-0.2f, 0.05f,0.0f,
		-0.05f, 0.05f,0.0f
	};

	const GLfloat AVertices[] =
	{
		1.25f, 0.75f,0.0, //A
			1.1f, 0.75f,0.0,  //A
			0.9f, -0.75f,0.0, //A
			1.55f, -0.75f,0.0,//A
			1.18f, 0.5f,0.0,//triangle A
			1.05f, -0.75f,0.0,
			1.40f, -0.75f,0.0

	};
	const GLfloat flagColor[] =
	{
		0.071f, 0.533f, 0.027f,
		0.071f, 0.533f, 0.027f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 0.6f, 0.2f,
		1.0f, 0.6f, 0.2f
	};
	const GLfloat FColor[] =
	{
			1.0f, 0.6f, 0.2f,
			1.0f, 0.6f, 0.2f,
		0.071f, 0.533f, 0.027f,
		0.071f, 0.533f, 0.027f,
		70.0f / 255.0f, 130.0f / 255.0f, 180.0f / 255.0f,
		70.0f / 255.0f, 130.0f / 255.0f, 180.0f / 255.0f,
		70.0f / 255.0f, 130.0f / 255.0f, 180.0f / 255.0f,
		70.0f / 255.0f, 130.0f / 255.0f, 180.0f / 255.0f,
		70.0f / 255.0f, 130.0f / 255.0f, 180.0f / 255.0f,
		70.0f / 255.0f, 130.0f / 255.0f, 180.0f / 255.0f,
		70.0f / 255.0f, 130.0f / 255.0f, 180.0f / 255.0f,
		70.0f / 255.0f, 130.0f / 255.0f, 180.0f / 255.0f,
		70.0f / 255.0f, 130.0f / 255.0f, 180.0f / 255.0f,
		70.0f / 255.0f, 130.0f / 255.0f, 180.0f / 255.0f,
		70.0f / 255.0f, 130.0f / 255.0f, 180.0f / 255.0f,
		70.0f / 255.0f, 130.0f / 255.0f, 180.0f / 255.0f
	};
	const GLfloat FVertices[] =
	{
		-0.0f, 0.75f,0.0f,
		-0.60f, 0.75f,0.0f,
		-0.60f, -0.75f,0.0f,
		0.0f, -0.75f,0.0f,
		0.0f, 0.60f,0.0f,
		-0.45f, 0.60f,0.0f,
		-0.45f, 0.1f,0.0f,
		0.0f, 0.1f,0.0f,
		0.0f, -0.05f,0.0f,
		-0.45f, -0.05f,0.0f,
		-0.45f, -0.75f,0.0f,
		0.0f, -0.75f,0.0f,
		0.0f, 0.15f,0.0f,
		-0.2f, 0.15f,0.0f,
		-0.2f, -0.75f,0.0f,
		0.0f, -0.75f,0.0f
	};

	/////////////
	mat4 modelViewMatrix;
	mat4 translationMatrix;
	mat4 scaleMatrix;
	mat4 modelViewProjectionMatrix;
	modelViewMatrix = mat4::identity();
	translationMatrix = mat4::identity();
	scaleMatrix = mat4::identity();

	/////////////////////////////Plane movements////////////////////

	translationMatrix = translate(planePosX, 0.0f, -3.0f);
	scaleMatrix = scale(0.1f, 0.05f, 0.0f);
	modelViewMatrix = translationMatrix * modelViewMatrix;
	//do necessary matrix multiplication
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	//Plane Draw
	//send necessary matrices to shaders in respective uniforms
	glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_letters_SB);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices),
		planeVertices, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glBindBuffer(GL_ARRAY_BUFFER, vbo_color_letters_SB);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeColor),
		planeColor, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//bind with vao
	glBindVertexArray(vao_letters_SB);
	//draw necessary scene
	glDrawArrays(GL_TRIANGLE_FAN, 0, 13);
	//unbind vao
	glBindVertexArray(0);


	//Plane draw end


	//send necessary matrices to shaders in respective uniforms
	glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_letters_SB);
	glBufferData(GL_ARRAY_BUFFER, sizeof(flagVertices),
		flagVertices, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_color_letters_SB);
	glBufferData(GL_ARRAY_BUFFER, sizeof(flagColor),
		flagColor, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//bind with vao
	glBindVertexArray(vao_letters_SB);
	//draw necessary scene
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);
	//unbind vao
	glBindVertexArray(0);



	translationMatrix = mat4::identity();
	modelViewMatrix = mat4::identity();

	/////////////////////////I on Plane//////////////////////////////////////////////


	//translationMatrix = translate(planePosX+0.49f, 0.0f, -2.5f);
	//modelViewMatrix = modelViewMatrix * translationMatrix*scaleMatrix;
	////do necessary matrix multiplication
	//modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	////send necessary matrices to shaders in respective uniforms
	//glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	//glBindBuffer(GL_ARRAY_BUFFER, vbo_position_letters_SB);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(IVertices),
	//	IVertices, GL_DYNAMIC_DRAW);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	//glBindBuffer(GL_ARRAY_BUFFER, vbo_color_letters_SB);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(IColor),
	//	IColor, GL_DYNAMIC_DRAW);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	////bind with vao
	//glBindVertexArray(vao_letters_SB);
	////draw necessary scene
	//glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	//glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	//glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	////unbind vao
	//glBindVertexArray(0);


	//translationMatrix = mat4::identity();
	//modelViewMatrix = mat4::identity();

	///////////////F/////////////////////////////////////////////

	////translationMatrix = translate(-1.46f + factorForA, 0.0f, -2.5f);
	//translationMatrix = translate(planeF, 0.0f, -2.5f);
	//modelViewMatrix = modelViewMatrix * translationMatrix*scaleMatrix;
	////do necessary matrix multiplication
	//modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	////send necessary matrices to shaders in respective uniforms
	//glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	//glBindBuffer(GL_ARRAY_BUFFER, vbo_position_letters_SB);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(FVertices),
	//	FVertices, GL_DYNAMIC_DRAW);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	//glBindBuffer(GL_ARRAY_BUFFER, vbo_color_letters_SB);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(FColor),
	//	FColor, GL_DYNAMIC_DRAW);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	////bind with vao
	//glBindVertexArray(vao_letters_SB);
	////draw necessary scene
	//glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	//glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	//glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	//glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	////unbind vao
	//glBindVertexArray(0);

	//translationMatrix = mat4::identity();
	//modelViewMatrix = mat4::identity();

	///////////////////////A/////////////////////////////////////

	//translationMatrix = translate(planeA, 0.0f, -2.5f);
	//modelViewMatrix = modelViewMatrix * translationMatrix*scaleMatrix;
	////do necessary matrix multiplication
	//modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	////send necessary matrices to shaders in respective uniforms
	//glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	//glBindBuffer(GL_ARRAY_BUFFER, vbo_position_letters_SB);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(AVertices),
	//	AVertices, GL_DYNAMIC_DRAW);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindBuffer(GL_ARRAY_BUFFER, vbo_color_letters_SB);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(AColor),
	//	AColor, GL_DYNAMIC_DRAW);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	////bind with vao
	//glBindVertexArray(vao_letters_SB);
	////draw necessary scene
	//glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	//glDrawArrays(GL_TRIANGLES, 4, 3);
	////unbind vao
	//glBindVertexArray(0);
}

void uninitialize()
{
	if (bFullScreen == true)
	{
		SetWindowLong(ghHwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghHwnd, &wpPrev);
		SetWindowPos(ghHwnd,
			HWND_TOP,
			0,
			0,
			0,
			0,
			SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER);
		ShowCursor(TRUE);
	}
	if (vbo_position_letters_SB)
	{
		glDeleteBuffers(1, &vbo_position_letters_SB);
		vbo_position_letters_SB = 0;
	}
	if (vao_letters_SB)
	{
		glDeleteBuffers(1, &vao_letters_SB);
		vao_letters_SB = 0;
	}
	if (vbo_position_rectangle_SB)
	{
		glDeleteBuffers(1, &vbo_position_rectangle_SB);
		vbo_position_rectangle_SB = 0;
	}
	if (vao_rectangle_SB)
	{
		glDeleteBuffers(1, &vao_rectangle_SB);
		vao_rectangle_SB = 0;
	}
	glUseProgram(gShaderProgramObject_SB);
	glDetachShader(gShaderProgramObject_SB, GL_FRAGMENT_SHADER);
	glDetachShader(gShaderProgramObject_SB, GL_VERTEX_SHADER);
	glDeleteShader(gFragmentShaderObject_SB);
	gFragmentShaderObject_SB = 0;
	glDeleteShader(gVertexShaderObject_SB);
	gVertexShaderObject_SB = 0;
	glDeleteProgram(gShaderProgramObject_SB);
	gShaderProgramObject_SB = 0;
	glUseProgram(0);
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
		ReleaseDC(ghHwnd, ghdc);
		ghdc = NULL;
	}
	if (gpFile)
	{
		fprintf(gpFile, "Log File is Closed\n");
		fclose(gpFile);
		gpFile = NULL;
	}

}















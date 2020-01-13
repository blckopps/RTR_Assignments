#include<Windows.h>
#include<stdio.h>
#include<gl/glew.h>
#include<gl/GL.h>
#include"vmath.h"

#include"Sphere.h"

using namespace vmath;

#pragma comment(lib,"openGL32.lib")
#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"Sphere.lib")


#define WIN_WIDTH 800
#define WIN_HEIGHT 600

int width_vp = WIN_WIDTH;
int height_vp = WIN_HEIGHT;

#define NUMOFSPHERES 24

FILE *gpfile = NULL;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

enum
{
	AMC_ATTRIBUTES_POSITION = 0,
	AMC_ATTRIBUTES_COLOR,
	AMC_ATTRIBUTES_NORMAL,
	AMC_ATTRIBUTES_TEXCOORD0
};

//GLOBAL VARIABLES
bool bFullScreen = false;
DWORD dwstyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
HWND ghwnd = NULL;
bool gbActiveWindow = false;
HDC ghdc = NULL;
HGLRC ghrc = NULL;

GLfloat angle_X = 0.0f;
int iRotation = 0;
//Shader program objects
GLuint gShaderProgramObject_PV;
GLuint gShaderProgramObject_PF;
//vao vbo
GLuint vao_sphere;
GLuint vbo_position_vbo;
GLuint vbo_normals_vbo;
GLuint vbo_element_vbo;

//Transformation matrix uniforms
GLuint modelMatrixUniform_PV;
GLuint viewMatrixUniform_PV;
GLuint projectionUniform_PV;

GLuint modelMatrixUniform_PF;
GLuint viewMatrixUniform_PF;
GLuint projectionUniform_PF;

mat4 perspectiveProjectionMatrix;

/////////////*******shader uniforms for per vertex*******///////////////////

GLuint isLKeyIsPressedUniforms_PV;
GLuint shininessUniform_PV;
GLuint lightPositionUniform_PV;
// 
GLuint laUniform_PV;
GLuint ldUniform_PV;
GLuint lsUniform_PV;

//Material Per Vertex 
GLuint kaUniform_PV;
GLuint kdUniform_PV;
GLuint ksUniform_PV;

////////////******shader uniforms for per fragment****/////////////
GLuint isLKeyIsPressedUniforms_PF;
GLuint shininessUniform_PF;
GLuint lightPositionUniform_PF;
//
GLuint laUniform_PF;
GLuint ldUniform_PF;
GLuint lsUniform_PF;
//Material Per Fragment
GLuint kaUniform_PF;
GLuint kdUniform_PF;
GLuint ksUniform_PF;
//
bool isLighting = false;
bool isPerVertex = true;
bool isPerFragment = false;

//sphere var
float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_textures[764];
unsigned short sphere_elements[2280];
GLsizei gNumVertices, gNumElements;

//light array 
float lightAmbient[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
float lightDifuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
float lightSpecular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

//material array
struct MaterialStruct
{
	float materialAmbient[4];

	float materialDifuse[4];


	float materialSpecular[4];

	float materialShininess;
}* myMaterialStruct[24];

float xPosition = 0.0f;
float yPosition = 0.0f;
float zPosition = 0.0f;

float lightPosition[4] = { xPosition, yPosition, zPosition };

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	//FUnction declaration
	int initialize(void);
	void display(void);
	void update();

	//variable decl
	int iret = 0;
	bool bdone = false;

	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("MYWINDOW ");

	if (fopen_s(&gpfile, "log.txt", "w") != 0)
	{
		MessageBox(NULL, TEXT("Cant create log"), TEXT("ERROR!!!"), MB_OK);

	}
	else
	{
		fwprintf(gpfile, TEXT("log file created\n"));
	}

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

	//create window

	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		szAppName,
		TEXT("24SPHERES-SHUBHAM"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPCHILDREN | WS_VISIBLE,
		100,
		100,
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);
	ghwnd = hwnd;
	iret = initialize();
	//handling return values and create log
	if (iret == -1)
	{
		fwprintf(gpfile, TEXT("CHoice pixel format failed!!\n"));
		DestroyWindow(hwnd);
	}
	else if (iret == -2)
	{
		fwprintf(gpfile, TEXT("SetPixelFormat failed!! \n"));
		DestroyWindow(hwnd);
	}
	else if (iret == -3)
	{
		fwprintf(gpfile, TEXT("create context failed\n"));
		DestroyWindow(hwnd);
	}
	else if (iret == -4)
	{
		fwprintf(gpfile, TEXT("wgl make current failed!!\n"));
		DestroyWindow(hwnd);

	}
	else
	{
		fwprintf(gpfile, TEXT("Initialization Successful"));


	}

	ShowWindow(hwnd, iCmdShow);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);
	//call in game loop UpdateWindow(hwnd);

	while (bdone == false)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				bdone = true;
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
				//here call update
				update();
			}
			display();

		}

	}
	return((int)msg.wParam);


}
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	//FUnction Declarations
	void resize(int, int);


	void uninitialize(void);

	void toogle_screen(void);

	switch (iMsg)
	{

	case WM_CHAR:
		switch (wParam)
		{
		case 'l':
		case 'L':
			if (isLighting == false)
			{
				isLighting = true;

			}
			else
			{
				isLighting = false;
			}
			break;
		case 'f':
		case 'F':
			if (isPerFragment == false)
			{
				isPerFragment = true;

				isPerVertex = false;

			}
			break;

		case 'v':
		case 'V':
			if (isPerVertex == false)
			{
				isPerVertex = true;
				isPerFragment = false;

			}
			break;
		case 'X':
		case 'x':
			iRotation = 0;
			break;

		case 'Y':
		case 'y':
			iRotation = 1;
			break;

		case 'Z':
		case 'z':
			iRotation = 2;
			break;

		case 'q':
		case 'Q':
			if (bFullScreen == true)				//We should exit from fullscreen and then destroy the window.
			{
				SetWindowLong(ghwnd,
					GWL_STYLE,
					dwstyle |
					WS_OVERLAPPEDWINDOW);

				SetWindowPlacement(ghwnd, &wpPrev);

				SetWindowPos(ghwnd,
					HWND_TOP,
					0, 0, 0, 0,
					SWP_NOZORDER |
					SWP_FRAMECHANGED |
					SWP_NOMOVE |
					SWP_NOSIZE |
					SWP_NOOWNERZORDER);

				ShowCursor(TRUE);
			}
			DestroyWindow(hwnd);

			break;

		default:
			break;
		}
		break;


	case WM_KEYDOWN:

		switch (wParam)
		{
		case VK_ESCAPE:

			toogle_screen();

			break;

		}

		break;
	case WM_SETFOCUS:
		gbActiveWindow = true;
		break;

	case WM_KILLFOCUS:
		gbActiveWindow = false;
		break;

	case WM_SIZE:
		resize(LOWORD(lParam), HIWORD(lParam));
		break;

	case WM_CLOSE:

		DestroyWindow(hwnd);
		break;

	case WM_ERASEBKGND:
		return 0;

	case WM_DESTROY:

		uninitialize();
		PostQuitMessage(0);
		break;
	}
	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}
//User Defined Functions

void toogle_screen(void)
{
	//MONITORINFO mi;


	if (bFullScreen == false)
	{
		dwstyle = GetWindowLong(ghwnd, GWL_STYLE);

		if (dwstyle & WS_OVERLAPPEDWINDOW)
		{
			MONITORINFO	mi = { sizeof(MONITORINFO) };

			if (GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE, dwstyle&~WS_OVERLAPPEDWINDOW);

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
		//bFullScreen = true;
	}
	else
	{
		SetWindowLong(ghwnd,
			GWL_STYLE,
			dwstyle |
			WS_OVERLAPPEDWINDOW);

		SetWindowPlacement(ghwnd, &wpPrev);

		SetWindowPos(ghwnd,
			HWND_TOP,
			0, 0, 0, 0,
			SWP_NOZORDER |
			SWP_FRAMECHANGED |
			SWP_NOMOVE |
			SWP_NOSIZE |
			SWP_NOOWNERZORDER);

		ShowCursor(TRUE);
		bFullScreen = false;
	}
}

int initialize(void)
{
	void resize(int, int);
	void uninitialize(void);
	void toogle_screen();
	void defStruct();

	//fill all material arrays
	defStruct();
	
	//shader objects for per vertex and per fragment
	GLuint gVertexShaderObject_PV;
	GLuint gVertexShaderObject_PF;

	GLuint gFragmentShaderObject_PV;
	GLuint gFragmentShaderObject_PF;

	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;
	GLenum result;
	//code
	ZeroMemory(&pfd, sizeof(PPIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 32;
	ghdc = GetDC(ghwnd);
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
	///programmable Pipeline changes..
	result = glewInit();
	if (result != GLEW_OK)
	{
		fwprintf(gpfile, TEXT("glewInit failed"));
		uninitialize();
		DestroyWindow(ghwnd);
	}
	//************VERTEX SHADER***************************
	//Define vertex shader Object
	gVertexShaderObject_PV = glCreateShader(GL_VERTEX_SHADER);
	gVertexShaderObject_PF = glCreateShader(GL_VERTEX_SHADER);

	//write vertex shader code
	// 
	//vertex shader for per vertex
	const GLchar* vertexShaderSourceCode_PV =
		"#version 430 core" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec3 vNormal;" \

		"uniform mat4 u_model_matrix;" \
		"uniform mat4 u_view_matrix;" \
		"uniform mat4 u_projection_matrix;" \
		"uniform int islkeypressed_PV;" \
	
		"uniform vec3 u_la_PV;" \
		"uniform vec3 u_ld_PV;" \
		"uniform vec3 u_ls_PV;" \
		
		"uniform vec3 u_ka_PV;" \
		"uniform vec3 u_kd_PV;" \
		"uniform vec3 u_ks_PV;" \

		"uniform float u_shininess_PV;" \

		"uniform vec4 u_light_position_PV;" \

		"out vec3 phong_ads_light_PV;" \
		"void main(void)" \
		"{" \
			"if(islkeypressed_PV == 1)" \
			"{" \
				"vec4 eye_coordinates = u_view_matrix *  u_model_matrix  * vPosition;" \
				"vec3 tnorm = normalize(mat3(u_view_matrix * u_model_matrix) * vNormal);" \

				"vec3 light_direction = normalize(vec3(u_light_position_PV - eye_coordinates));" \

				"float tn_dot_ldirection = max(dot(light_direction, tnorm), 0);" \

				"vec3 reflection_vector = reflect(-light_direction, tnorm);" \

				"vec3 viewer_vector = normalize(vec3(-eye_coordinates));" \

				"vec3 ambient = u_la_PV * u_ka_PV;" \

				"vec3 difuse = u_ld_PV * u_kd_PV * tn_dot_ldirection;" \

				"vec3 specular = u_ls_PV * u_ks_PV * pow(max(dot(reflection_vector, viewer_vector),0),u_shininess_PV);" \

				"phong_ads_light_PV = ambient + difuse + specular;" \
			"}" \
			"else" \
			"{" \
				"phong_ads_light_PV = vec3(1.0, 1.0, 1.0);" \
			"}" \
			"gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" \
		" } ";
	//vertex shader for per fragment..
	const GLchar* vertexShaderSourceCode_PF =
		"#version 430 core" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec3 vNormal;" \

		"uniform mat4 u_model_matrix;" \
		"uniform mat4 u_view_matrix;" \
		"uniform mat4 u_projection_matrix;" \
		"uniform int islkeypressed_PF;" \

		"uniform vec4 u_light_position_PF;" \

		"out vec3 tnorm_PF;" \

		"out vec3 light_direction_PF;" \

		"out vec3 viewer_vector_PF;" \
		"void main(void)" \
		"{" \
			"if(islkeypressed_PF == 1)" \
			"{" \
				"vec4 eye_coordinates = u_view_matrix *  u_model_matrix  * vPosition;" \

				"tnorm_PF = mat3(u_view_matrix * u_model_matrix) * vNormal;" \

				"light_direction_PF = vec3(u_light_position_PF - eye_coordinates);" \

				"viewer_vector_PF = vec3(-eye_coordinates.xyz);" \
			"}" \
		"gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" \
		" } ";
	//specify vertex source code to vetex shader object

	glShaderSource(gVertexShaderObject_PV,
					1,
					(GLchar **)&vertexShaderSourceCode_PV,
					NULL);

	glShaderSource(gVertexShaderObject_PF,
					1,
					(GLchar **)&vertexShaderSourceCode_PF,
					NULL);

	//compile vertex shader code per vertex
	glCompileShader(gVertexShaderObject_PV);

	//Error checking
	GLint iShaderCompilationStatus = 0;
	GLint iInfoLength = 0;
	GLchar *szInfoLog = NULL;

	glGetShaderiv(gVertexShaderObject_PV,
		GL_COMPILE_STATUS,
		&iShaderCompilationStatus);

	if (iShaderCompilationStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject_PV,
			GL_INFO_LOG_LENGTH,
			&iInfoLength);
		if (iInfoLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLength);

			if (szInfoLog != NULL)
			{
				GLsizei writtened;

				glGetShaderInfoLog(gVertexShaderObject_PV,
					iInfoLength,
					&writtened,
					szInfoLog);
				fprintf(gpfile, "ERROR:VERTEX SHADER PER VERTEX :%s", szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(ghwnd);
				exit(0);
			}
		}
	}
	//compile vertex shader code per fragment
	glCompileShader(gVertexShaderObject_PF);

	//Error checking
	iShaderCompilationStatus = 0;
	iInfoLength = 0;
	szInfoLog = NULL;

	glGetShaderiv(gVertexShaderObject_PF,
		GL_COMPILE_STATUS,
		&iShaderCompilationStatus);

	if (iShaderCompilationStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject_PF,
			GL_INFO_LOG_LENGTH,
			&iInfoLength);
		if (iInfoLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLength);

			if (szInfoLog != NULL)
			{
				GLsizei writtened;

				glGetShaderInfoLog(gVertexShaderObject_PF,
					iInfoLength,
					&writtened,
					szInfoLog);
				fprintf(gpfile, "ERROR:VERTEX SHADER PER FRAGMENT :%s", szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(ghwnd);
				exit(0);
			}
		}
	}
	//********************FRAGMENT SHADER*****************
	gFragmentShaderObject_PV = glCreateShader(GL_FRAGMENT_SHADER);
	gFragmentShaderObject_PF = glCreateShader(GL_FRAGMENT_SHADER);

	//write vertex shader code 
	// 
	const GLchar * fragmentShaderSourceCode_PV =
		"#version 430 core" \
		"\n" \
		"out vec4 fragColor;" \
		"in vec3 phong_ads_light_PV;" \
		"void main(void)" \
		"{" \
			"fragColor = vec4(phong_ads_light_PV, 1.0);" \
		"}";
	//fragment shader for per fragment.
	const GLchar * fragmentShaderSourceCode_PF =
		"#version 430 core" \
		"\n" \
		"out vec4 fragColor;" \
		"uniform int islkeypressed_PF;" \

		"uniform vec3 u_la_PF;" \

		"uniform vec3 u_ld_PF;" \

		"uniform vec3 u_ls_PF;" \

		"uniform vec3 u_ka_PF;" \
		"uniform vec3 u_kd_PF;" \
		"uniform vec3 u_ks_PF;" \

		"in vec3 tnorm_PF;" \

		"in vec3 light_direction_PF;" \

		"in vec3 viewer_vector_PF;" \
		"uniform float u_shininess_PF;" \
		"void main(void)" \
		"{" \
			"if(islkeypressed_PF == 1)" \
			"{" \
				"vec3 tnorm_normalized = normalize(tnorm_PF);" \

				"vec3 light_direction_normalized = normalize(light_direction_PF);" \

				"vec3 viewer_vector_normalized = normalize(viewer_vector_PF);" \

				"float tn_dot_ldirection = max(dot(light_direction_normalized, tnorm_normalized), 0);" \

				"vec3 reflection_vector = reflect(-light_direction_normalized, tnorm_normalized);" \

				"vec3 ambient = u_la_PF * u_ka_PF;" \

				"vec3 difuse = u_ld_PF * u_kd_PF * tn_dot_ldirection;" \

				"vec3 specular = u_ls_PF * u_ks_PF * pow(max(dot(reflection_vector, viewer_vector_normalized),0),u_shininess_PF);" \
		
				"vec3 phong_light_pf = ambient + difuse + specular;" \

				"fragColor = vec4(phong_light_pf, 1.0);" \

			"}" \
			"else" \
			"{" \
				"fragColor = vec4(1.0, 1.0 , 1.0 , 1.0);" \
			"}" \
		"}";
	//specify vertex source code to vetex shader object

	glShaderSource(gFragmentShaderObject_PV,
		1,
		(GLchar **)&fragmentShaderSourceCode_PV,
		NULL);

	glShaderSource(gFragmentShaderObject_PF,
		1,
		(GLchar **)&fragmentShaderSourceCode_PF,
		NULL);

	//compile fragment shader code
	glCompileShader(gFragmentShaderObject_PV);

	//Error checking
	iShaderCompilationStatus = 0;
	iInfoLength = 0;
	szInfoLog = NULL;

	glGetShaderiv(gFragmentShaderObject_PV,
		GL_COMPILE_STATUS,
		&iShaderCompilationStatus);

	if (iShaderCompilationStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject_PV,
			GL_INFO_LOG_LENGTH,
			&iInfoLength);
		if (iInfoLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLength);

			if (szInfoLog != NULL)
			{
				GLsizei writtened;

				glGetShaderInfoLog(gFragmentShaderObject_PV,
					iInfoLength,
					&writtened,
					szInfoLog);
				fprintf(gpfile, "ERROR:FRAGMENT SHADER PER VERTEX:%s", szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(ghwnd);
				exit(0);
			}
		}
	}

	glCompileShader(gFragmentShaderObject_PF);
	//error checking for fragment shader per fragment
	iShaderCompilationStatus = 0;
	iInfoLength = 0;
	szInfoLog = NULL;

	glGetShaderiv(gFragmentShaderObject_PF,
		GL_COMPILE_STATUS,
		&iShaderCompilationStatus);

	if (iShaderCompilationStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject_PF,
			GL_INFO_LOG_LENGTH,
			&iInfoLength);
		if (iInfoLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLength);

			if (szInfoLog != NULL)
			{
				GLsizei writtened;

				glGetShaderInfoLog(gFragmentShaderObject_PF,
					iInfoLength,
					&writtened,
					szInfoLog);
				fprintf(gpfile, "ERROR:FRAGMENT SHADER PER FRAGMENT:%s", szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(ghwnd);
				exit(0);
			}
		}
	}

	/////////Create shader program object///////////////////

	gShaderProgramObject_PV = glCreateProgram();
	gShaderProgramObject_PF = glCreateProgram();

	//Attach vertex,fragment shader program
	glAttachShader(gShaderProgramObject_PV, gVertexShaderObject_PV);
	glAttachShader(gShaderProgramObject_PV, gFragmentShaderObject_PV);
	glAttachShader(gShaderProgramObject_PF, gVertexShaderObject_PF);
	glAttachShader(gShaderProgramObject_PF, gFragmentShaderObject_PF);
	//*** PRELINKING BINDING TO VERTEX ATTRIBUTES***
	glBindAttribLocation(gShaderProgramObject_PV,
						AMC_ATTRIBUTES_POSITION,
						"vPosition");

	glBindAttribLocation(gShaderProgramObject_PV,
						AMC_ATTRIBUTES_NORMAL,
						"vNormal");

	glBindAttribLocation(gShaderProgramObject_PF,
							AMC_ATTRIBUTES_POSITION,
							"vPosition");

	glBindAttribLocation(gShaderProgramObject_PF,
						AMC_ATTRIBUTES_NORMAL,
						"vNormal");

	//link above shader program
	glLinkProgram(gShaderProgramObject_PV);

	glLinkProgram(gShaderProgramObject_PF);

	// ***ERROR CHECKING LINKING********
	//Error checking for program per vertex
	GLint iShaderLinkStatus = 0;
	iInfoLength = 0;
	szInfoLog = NULL;

	glGetProgramiv(gShaderProgramObject_PV,
		GL_LINK_STATUS,
		&iShaderLinkStatus);

	if (iShaderCompilationStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObject_PV,
			GL_INFO_LOG_LENGTH,
			&iInfoLength);

		if (iInfoLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLength);

			if (szInfoLog != NULL)
			{
				GLsizei wr;

				glGetShaderInfoLog(gShaderProgramObject_PV,
					iInfoLength,
					&wr,
					szInfoLog);

				fwprintf(gpfile, TEXT("ERROR:SHADER LINK PER VERTEX:%s"), szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(ghwnd);
				exit(0);
			}

		}
	}

	//Error checking for program per fragment
	iShaderLinkStatus = 0;
	iInfoLength = 0;
	szInfoLog = NULL;

	glGetProgramiv(gShaderProgramObject_PF,
		GL_LINK_STATUS,
		&iShaderLinkStatus);
	if (iShaderCompilationStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObject_PF,
			GL_INFO_LOG_LENGTH,
			&iInfoLength);
		if (iInfoLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLength);

			if (szInfoLog != NULL)
			{
				GLsizei wr;

				glGetShaderInfoLog(gShaderProgramObject_PF,
					iInfoLength,
					&wr,
					szInfoLog);

				fwprintf(gpfile, TEXT("ERROR:SHADER LINK PER FRAGMENT:%s"), szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(ghwnd);
				exit(0);
			}

		}
	}

	///***POST LINKING GETTING UNIFORMS**

	//getting light uniforms locations
	isLKeyIsPressedUniforms_PV = glGetUniformLocation(gShaderProgramObject_PV, "islkeypressed_PV");
	isLKeyIsPressedUniforms_PF = glGetUniformLocation(gShaderProgramObject_PF, "islkeypressed_PF");

	//PER VERTEX

	laUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_la_PV");

	ldUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_ld_PV");

	lsUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_ls_PV");

	kaUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_ka_PV");
	kdUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_kd_PV");
	ksUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_ks_PV");

	shininessUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_shininess_PV");

	lightPositionUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_light_position_PV");

	//PER FRAG
	laUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_la_PF");
	ldUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_ld_PF");

	lsUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_ls_PF");


	kaUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_ka_PF");
	kdUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_kd_PF");
	ksUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_ks_PF");


	shininessUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_shininess_PF");


	lightPositionUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_light_position_PF");

	//getting transformation matrices uniform locations
	//PV
	modelMatrixUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_model_matrix");

	viewMatrixUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_view_matrix");

	projectionUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_projection_matrix");

	//PF
	modelMatrixUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_model_matrix");

	viewMatrixUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_view_matrix");

	projectionUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_projection_matrix");

	fwprintf(gpfile, TEXT("Post link success!!\n"));

	//sphere 
	getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
	gNumVertices = getNumberOfSphereVertices();
	gNumElements = getNumberOfSphereElements();


	//VAO AND VBO
	glGenVertexArrays(1, &vao_sphere);

	glBindVertexArray(vao_sphere);

	//position
	glGenBuffers(1, &vbo_position_vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_vbo);

	glBufferData(GL_ARRAY_BUFFER,
		sizeof(sphere_vertices),
		sphere_vertices,
		GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTES_POSITION,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		NULL);

	glEnableVertexAttribArray(AMC_ATTRIBUTES_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//normal 

	glGenBuffers(1, &vbo_normals_vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_normals_vbo);

	glBufferData(GL_ARRAY_BUFFER,
		sizeof(sphere_normals),
		sphere_normals,
		GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTES_NORMAL,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		NULL);

	glEnableVertexAttribArray(AMC_ATTRIBUTES_NORMAL);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// element vbo
	glGenBuffers(1, &vbo_element_vbo);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element_vbo);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//unbind vao
	glBindVertexArray(0);

	//START:

	//END:
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
	glClearDepth(1.0f);

	perspectiveProjectionMatrix = mat4::identity();
	
	resize(WIN_WIDTH, WIN_HEIGHT);
	toogle_screen();
	return 0;
}

void resize(int width, int height)
{
	width_vp = width;
	height_vp = height;

	//fprintf(gpfile,"width*height is %d  %d\n", width_vp, height_vp);	//1536 * 864
	if (height == 0)
	{
		height = 1;
	}
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	perspectiveProjectionMatrix =
		perspective(45.0f , (((GLfloat)width) / ((GLfloat)height)), 0.1f, 100.0f);

}

void display(void)
{
	//fwprintf(gpfile,TEXT("In Display\n"));
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//declare matrices
	mat4 viewMatrix;
	mat4 modelMatrix;

	//make identity matrices
	viewMatrix = mat4::identity();
	modelMatrix = mat4::identity();
	float Y = 0.0f;
	float X = 0.0f;
	float Z = -2.0f;

	//Do transformations...
	modelMatrix = translate(X, Y, Z);

	//send neccessary matrix to shader in respective uniforms

	if (isPerVertex == true)
	{
		glUseProgram(gShaderProgramObject_PV);
		//fprintf(gpfile,"PER VERTEX\n");
		if (isLighting == true)
		{
			glUniform1i(isLKeyIsPressedUniforms_PV, 1);

			glUniform3fv(laUniform_PV, 1, lightAmbient);

			glUniform3fv(ldUniform_PV, 1, lightDifuse);

			glUniform3fv(lsUniform_PV, 1, lightSpecular);


			glUniform4fv(lightPositionUniform_PV, 1, lightPosition);
		}
		else
		{
			glUniform1i(isLKeyIsPressedUniforms_PV, 0);
		}

		glUniformMatrix4fv(modelMatrixUniform_PV,
							1,
							GL_FALSE,
							modelMatrix);

		glUniformMatrix4fv(viewMatrixUniform_PV,
								1,
								GL_FALSE,
								viewMatrix);

		glUniformMatrix4fv(projectionUniform_PV,
							1,
							GL_FALSE,
							perspectiveProjectionMatrix);

		glUseProgram(gShaderProgramObject_PV);

	}
	else
	{
		glUseProgram(gShaderProgramObject_PF);
		//fprintf(gpfile, "PER FRAGMENT\n");
		if (isLighting == true)
		{
			glUniform1i(isLKeyIsPressedUniforms_PF, 1);

			glUniform3fv(laUniform_PF, 1, lightAmbient);

			glUniform3fv(ldUniform_PF, 1, lightDifuse);

			glUniform3fv(lsUniform_PF, 1, lightSpecular);

			glUniform4fv(lightPositionUniform_PF, 1, lightPosition);
		}
		else
		{
			glUniform1i(isLKeyIsPressedUniforms_PF, 0);
		}
		
		glUniformMatrix4fv(modelMatrixUniform_PF,
			1,
			GL_FALSE,
			modelMatrix
		);

		glUniformMatrix4fv(viewMatrixUniform_PF,
			1,
			GL_FALSE,
			viewMatrix);

		glUniformMatrix4fv(projectionUniform_PF,
			1,
			GL_FALSE,
			perspectiveProjectionMatrix);


	}

	//similarly bind with texture any
	glBindVertexArray(vao_sphere);

	GLuint xPos = 0;
	GLuint yPos = height_vp - height_vp / 6;

	for (int i = 1; i <= NUMOFSPHERES; i++)

	{
		modelMatrix = mat4::identity();

		modelMatrix = translate(X, Y, Z);

		if (isPerVertex)
		{
			glUniform3fv(kaUniform_PV, 1, myMaterialStruct[i - 1]->materialAmbient);
			glUniform3fv(kdUniform_PV, 1, myMaterialStruct[i - 1]->materialDifuse);
			glUniform3fv(ksUniform_PV, 1, myMaterialStruct[i - 1]->materialSpecular);
			glUniform1f(shininessUniform_PV, myMaterialStruct[i - 1]->materialShininess);

			glUniformMatrix4fv(modelMatrixUniform_PV,
								1,
								GL_FALSE,
								modelMatrix);
		}
		else
		{
			glUniform3fv(kaUniform_PF, 1, myMaterialStruct[i - 1]->materialAmbient);
			glUniform3fv(kdUniform_PF, 1, myMaterialStruct[i - 1]->materialDifuse);
			glUniform3fv(ksUniform_PF, 1, myMaterialStruct[i - 1]->materialSpecular);
			glUniform1f(shininessUniform_PF, myMaterialStruct[i - 1]->materialShininess);

			glUniformMatrix4fv(modelMatrixUniform_PF,
								1,
								GL_FALSE,
								modelMatrix);
		}
		
		//fprintf(gpfile,"\nx : y %f %f ",X,Y);

		//view Port 
		glViewport(xPos, yPos, (GLsizei)width_vp / 4, (GLsizei)height_vp / 6);
		
		

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element_vbo);
		glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

		if (i % 6 == 0)
		{
			xPos += width_vp / 4;
			yPos = height_vp - height_vp / 6;
		}
		else
		{
			yPos -= height_vp / 6;
		}

	}

	glBindVertexArray(0);

	glUseProgram(0);
	SwapBuffers(ghdc);
	//fwprintf(gpfile,TEXT("End OF display\n"));
}

void update()
{
	if (angle_X >= 360.0f)
	{
		angle_X = 0.0f;
	}
	else
	{
		angle_X += 0.005f;
	}
	if (iRotation == 0)
	{
		lightPosition[0] = 0.0f;
		lightPosition[1] = 100.0f*sin(angle_X);
		lightPosition[2] = 100.0f *cos(angle_X);
		lightPosition[3] = 1.0f; 
	}
	else if (iRotation == 1)
	{
		lightPosition[0] = 100.0f*sin(angle_X);
		lightPosition[1] = 0.0f;
		lightPosition[2] = 100.0f *cos(angle_X);
		lightPosition[3] = 1.0f;
	}
	else
	{
		lightPosition[0] = 100.0f*sin(angle_X);
		lightPosition[1] = 100.0f *cos(angle_X);
		lightPosition[2] = 0.0f;
		lightPosition[3] = 1.0f;
	}

}
void uninitialize(void)
{
	int uninitialize2(GLuint);

	if (vbo_element_vbo)
	{
		glDeleteBuffers(1, &vbo_element_vbo);
		vbo_element_vbo = 0;
	}

	if (vbo_normals_vbo)
	{
		glDeleteVertexArrays(1, &vbo_normals_vbo);
		vbo_normals_vbo = 0;
	}

	if (vbo_position_vbo)
	{
		glDeleteVertexArrays(1, &vbo_position_vbo);
		vbo_position_vbo = 0;
	}
	//Free material array
	for (int i = 0; i<NUMOFSPHERES; i++)
	{
		free(myMaterialStruct[i]);
		myMaterialStruct[i] = NULL;
	}
	//	////shader uninitialize code
	int ret = 0;

	ret = uninitialize2(gShaderProgramObject_PV);
	ret = uninitialize2(gShaderProgramObject_PF);

	if (ret != 0)
	{
		fprintf(gpfile, "\nFaild uninitialize2\n");
	}
	else
	{
		fprintf(gpfile, " \nuninitialize2 successfull\n");
	}


	if (bFullScreen == true)
	{
		SetWindowLong(ghwnd,
			GWL_STYLE,
			dwstyle |
			WS_OVERLAPPEDWINDOW);

		SetWindowPlacement(ghwnd, &wpPrev);

		SetWindowPos(ghwnd,
			HWND_TOP,
			0, 0, 0, 0,
			SWP_NOZORDER |
			SWP_FRAMECHANGED |
			SWP_NOMOVE |
			SWP_NOSIZE |
			SWP_NOOWNERZORDER);

		ShowCursor(TRUE);
	}
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
}

int uninitialize2(GLuint shaderObject)
{
	GLsizei shaderCount;
	GLsizei shaderNumber;

	if (shaderObject)
	{
		glUseProgram(shaderObject);

		glGetProgramiv(shaderObject,
			GL_ATTACHED_SHADERS,
			&shaderCount);


		GLuint *pShaders = (GLuint*)malloc(sizeof(GLuint)*shaderCount);

		if (pShaders)
		{
			glGetAttachedShaders(shaderObject,
				shaderCount,
				&shaderCount,
				pShaders);

			for (shaderNumber = 0; shaderNumber < shaderCount; shaderNumber++)
			{
				glDetachShader(shaderObject,
					pShaders[shaderNumber]);

				glDeleteShader(pShaders[shaderNumber]);

				pShaders[shaderNumber] = 0;
			}
			free(pShaders);
		}
		else
		{
			fprintf(gpfile, "\nUninitialize2 : Failed to alloc memory\n");
			return 1;
		}

		glDeleteProgram(shaderObject);

		shaderObject = 0;

		glUseProgram(0);
	}
	return 0;
}

void defStruct(void)
{
	for (int i = 0; i < NUMOFSPHERES; i++)
	{
		//myMaterialStruct[i] = (struct MaterialStruct*)malloc(4.0f * 20.0f);
		myMaterialStruct[i] = (struct MaterialStruct *)malloc( sizeof(MaterialStruct));
		fprintf(gpfile, "sizeof myMaterialStruct of %d is %d \n", i, sizeof(myMaterialStruct[i]));
		
	}
	//fclose(gpfile);
	//0th
	

	myMaterialStruct[0]->materialAmbient[0] = 0.0215f;
	myMaterialStruct[0]->materialAmbient[1] = 0.1745f;
	myMaterialStruct[0]->materialAmbient[2] = 0.0215f;
	myMaterialStruct[0]->materialAmbient[3] = 1.0f;
					   
	myMaterialStruct[0]->materialDifuse[0] = 0.07568f;
	myMaterialStruct[0]->materialDifuse[1] = 0.61424f;
	myMaterialStruct[0]->materialDifuse[2] = 0.07568f;
	myMaterialStruct[0]->materialDifuse[3] = 1.0f;
					   
	myMaterialStruct[0]->materialSpecular[0] = 0.633f;
	myMaterialStruct[0]->materialSpecular[1] = 0.727811f;
	myMaterialStruct[0]->materialSpecular[2] = 0.633f;
	myMaterialStruct[0]->materialSpecular[3] = 1.0f;
					   
	myMaterialStruct[0]->materialShininess = 0.6f*128.0f;
					   
	//1th		2nd on first col	   ->
	myMaterialStruct[1]->materialAmbient[0] = 0.135f;
	myMaterialStruct[1]->materialAmbient[1] = 0.2225f;
	myMaterialStruct[1]->materialAmbient[2] = 0.1575f;
	myMaterialStruct[1]->materialAmbient[3] = 1.0f;
					   
	myMaterialStruct[1]->materialDifuse[0] = 0.54f;
	myMaterialStruct[1]->materialDifuse[1] = 0.89f;
	myMaterialStruct[1]->materialDifuse[2] = 0.63f;
	myMaterialStruct[1]->materialDifuse[3] = 1.0f;
					   
	myMaterialStruct[1]->materialSpecular[0] = 0.316228f;
	myMaterialStruct[1]->materialSpecular[1] = 0.316228f;
	myMaterialStruct[1]->materialSpecular[2] = 0.316228f;
	myMaterialStruct[1]->materialSpecular[3] = 1.0f;
					   
	myMaterialStruct[1]->materialShininess = 0.1f *128.0f;
					   
	//2th			   ->
	myMaterialStruct[2]->materialAmbient[0] = 0.05375f;
	myMaterialStruct[2]->materialAmbient[1] = 0.05f;
	myMaterialStruct[2]->materialAmbient[2] = 0.06625f;
	myMaterialStruct[2]->materialAmbient[3] = 1.0f;
					   
	myMaterialStruct[2]->materialDifuse[0] = 0.18275f;
	myMaterialStruct[2]->materialDifuse[1] = 0.17f;
	myMaterialStruct[2]->materialDifuse[2] = 0.22525f;
	myMaterialStruct[2]->materialDifuse[3] = 1.0f;
					   
	myMaterialStruct[2]->materialSpecular[0] = 0.332741f;
	myMaterialStruct[2]->materialSpecular[1] = 0.328634f;
	myMaterialStruct[2]->materialSpecular[2] = 0.346435f;
	myMaterialStruct[2]->materialSpecular[3] = 1.0f;
					   
	myMaterialStruct[2]->materialShininess = 0.3f*128.0f;
					   
	//3th		4th on 1st colmn	   ->
	myMaterialStruct[3]->materialAmbient[0] = 0.25f;
	myMaterialStruct[3]->materialAmbient[1] = 0.20725f;
	myMaterialStruct[3]->materialAmbient[2] = 0.20725f;
	myMaterialStruct[3]->materialAmbient[3] = 1.0f;
					   
	myMaterialStruct[3]->materialDifuse[0] = 1.0f;
	myMaterialStruct[3]->materialDifuse[1] = 0.829f;
	myMaterialStruct[3]->materialDifuse[2] = 0.829f;
	myMaterialStruct[3]->materialDifuse[3] = 1.0f;
					   
	myMaterialStruct[3]->materialSpecular[0] = 0.296648f;
	myMaterialStruct[3]->materialSpecular[1] = 0.296648f;
	myMaterialStruct[3]->materialSpecular[2] = 0.296648f;
	myMaterialStruct[3]->materialSpecular[3] = 1.0f;
					 
	myMaterialStruct[3]->materialShininess = 0.88f*128.0f;
					   
	//4th			   5th on 1st colmn->
	myMaterialStruct[4]->materialAmbient[0] = 0.1745f;
	myMaterialStruct[4]->materialAmbient[1] = 0.01175f;
	myMaterialStruct[4]->materialAmbient[2] = 0.01175f;
	myMaterialStruct[4]->materialAmbient[3] = 1.0f;
					   
	myMaterialStruct[4]->materialDifuse[0] = 0.61424f;
	myMaterialStruct[4]->materialDifuse[1] = 0.04136f;
	myMaterialStruct[4]->materialDifuse[2] = 0.04136f;
	myMaterialStruct[4]->materialDifuse[3] = 1.0f;
					   
	myMaterialStruct[4]->materialSpecular[0] = 0.727811f;
	myMaterialStruct[4]->materialSpecular[1] = 0.626959f;
	myMaterialStruct[4]->materialSpecular[2] = 0.626959f;
	myMaterialStruct[4]->materialSpecular[3] = 1.0f;
					   
	myMaterialStruct[4]->materialShininess = 0.6f * 128.0f;
	//5th	6th on 1st col		   ->
	myMaterialStruct[5]->materialAmbient[0] = 0.1f;
	myMaterialStruct[5]->materialAmbient[1] = 0.18725f;
	myMaterialStruct[5]->materialAmbient[2] = 0.1745f;
	myMaterialStruct[5]->materialAmbient[3] = 1.0f;
					   
	myMaterialStruct[5]->materialDifuse[0] = 0.396f;
	myMaterialStruct[5]->materialDifuse[1] = 0.74151f;
	myMaterialStruct[5]->materialDifuse[2] = 0.69102f;
	myMaterialStruct[5]->materialDifuse[3] = 1.0f;
					  
	myMaterialStruct[5]->materialSpecular[0] = 0.297254f;
	myMaterialStruct[5]->materialSpecular[1] = 0.30829f;
	myMaterialStruct[5]->materialSpecular[2] = 0.30829f;
	myMaterialStruct[5]->materialSpecular[3] = 1.0f;
					   
	myMaterialStruct[5]->materialShininess = 0.1f*128.0f;
	////6th			1st on 2nd colmn   ->
	myMaterialStruct[6]->materialAmbient[0] = 0.329412f;
	myMaterialStruct[6]->materialAmbient[1] = 0.223529f;
	myMaterialStruct[6]->materialAmbient[2] = 0.027451f;
	myMaterialStruct[6]->materialAmbient[3] = 1.0f;

	myMaterialStruct[6]->materialDifuse[0] = 0.780392f;
	myMaterialStruct[6]->materialDifuse[1] = 0.568627f;
	myMaterialStruct[6]->materialDifuse[2] = 0.113725f;
	myMaterialStruct[6]->materialDifuse[3] = 1.0f;

	myMaterialStruct[6]->materialSpecular[0] = 0.992157f;
	myMaterialStruct[6]->materialSpecular[1] = 0.941176f;
	myMaterialStruct[6]->materialSpecular[2] = 0.807843f;
	myMaterialStruct[6]->materialSpecular[3] = 1.0f;

	myMaterialStruct[6]->materialShininess = 0.21794872f*128.0f;
					   
	//7th 2nd on 2nd colm  ->
	myMaterialStruct[7]->materialAmbient[0] = 0.2125f;
	myMaterialStruct[7]->materialAmbient[1] = 0.1275f;
	myMaterialStruct[7]->materialAmbient[2] = 0.054f;
	myMaterialStruct[7]->materialAmbient[3] = 1.0f;

	myMaterialStruct[7]->materialDifuse[0] = 0.714f;
	myMaterialStruct[7]->materialDifuse[1] = 0.4284f;
	myMaterialStruct[7]->materialDifuse[2] = 0.18144f;
	myMaterialStruct[7]->materialDifuse[3] = 1.0f;

	myMaterialStruct[7]->materialSpecular[0] = 0.393548f;
	myMaterialStruct[7]->materialSpecular[1] = 0.271906f;
	myMaterialStruct[7]->materialSpecular[2] = 0.166721f;
	myMaterialStruct[7]->materialSpecular[3] = 1.0f;

	myMaterialStruct[7]->materialShininess = 0.2f*128.0f;
	//8th   3rd on 2nd colm->
	myMaterialStruct[8]->materialAmbient[0] = 0.25f;
	myMaterialStruct[8]->materialAmbient[1] = 0.25f;
	myMaterialStruct[8]->materialAmbient[2] = 0.25f;
	myMaterialStruct[8]->materialAmbient[3] = 1.0f;

	myMaterialStruct[8]->materialDifuse[0] = 0.4f;
	myMaterialStruct[8]->materialDifuse[1] = 0.4f;
	myMaterialStruct[8]->materialDifuse[2] = 0.4f;
	myMaterialStruct[8]->materialDifuse[3] = 1.0f;

	myMaterialStruct[8]->materialSpecular[0] = 0.774597f;
	myMaterialStruct[8]->materialSpecular[1] = 0.774597f;
	myMaterialStruct[8]->materialSpecular[2] = 0.774597f;
	myMaterialStruct[8]->materialSpecular[3] = 1.0f;

	myMaterialStruct[8]->materialShininess = 0.6f*128.0f;
	//9th 2nd col 4th sphre
	myMaterialStruct[9]->materialAmbient[0] = 0.19125f;
	myMaterialStruct[9]->materialAmbient[1] = 0.0735f;
	myMaterialStruct[9]->materialAmbient[2] = 0.0225f;
	myMaterialStruct[9]->materialAmbient[3] = 1.0f;

	myMaterialStruct[9]->materialDifuse[0] = 0.7038f;
	myMaterialStruct[9]->materialDifuse[1] = 0.27048f;
	myMaterialStruct[9]->materialDifuse[2] = 0.0828f;
	myMaterialStruct[9]->materialDifuse[3] = 1.0f;

	myMaterialStruct[9]->materialSpecular[0] = 0.256777f;
	myMaterialStruct[9]->materialSpecular[1] = 0.137622f;
	myMaterialStruct[9]->materialSpecular[2] = 0.086014f;
	myMaterialStruct[9]->materialSpecular[3] = 1.0f;

	myMaterialStruct[9]->materialShininess = 0.1f*128.0f;
	//10th 2nd col 5th sphere
	myMaterialStruct[10]->materialAmbient[0] = 0.24725f;
	myMaterialStruct[10]->materialAmbient[1] = 0.1995f;
	myMaterialStruct[10]->materialAmbient[2] = 0.0745f;
	myMaterialStruct[10]->materialAmbient[3] = 1.0f;

	myMaterialStruct[10]->materialDifuse[0] = 0.75164f;
	myMaterialStruct[10]->materialDifuse[1] = 0.60648f;
	myMaterialStruct[10]->materialDifuse[2] = 0.22648f;
	myMaterialStruct[10]->materialDifuse[3] = 1.0f;

	myMaterialStruct[10]->materialSpecular[0] = 0.628281f;
	myMaterialStruct[10]->materialSpecular[1] = 0.555802f;
	myMaterialStruct[10]->materialSpecular[2] = 0.366065f;
	myMaterialStruct[10]->materialSpecular[3] = 1.0f;

	myMaterialStruct[10]->materialShininess = 0.4f*128.0f;
	//11th 2nd col 6th sphere
	myMaterialStruct[11]->materialAmbient[0] = 0.19225f;
	myMaterialStruct[11]->materialAmbient[1] = 0.19225f;
	myMaterialStruct[11]->materialAmbient[2] = 0.19225f;
	myMaterialStruct[11]->materialAmbient[3] = 1.0f;

	myMaterialStruct[11]->materialDifuse[0] = 0.50754f;
	myMaterialStruct[11]->materialDifuse[1] = 0.50754f;
	myMaterialStruct[11]->materialDifuse[2] = 0.50754f;
	myMaterialStruct[11]->materialDifuse[3] = 1.0f;

	myMaterialStruct[11]->materialSpecular[0] = 0.508273f;
	myMaterialStruct[11]->materialSpecular[1] = 0.508273f;
	myMaterialStruct[11]->materialSpecular[2] = 0.508273f;
	myMaterialStruct[11]->materialSpecular[3] = 1.0f;

	myMaterialStruct[11]->materialShininess = 0.4f*128.0f;

	//12th 3rd colm 1
	myMaterialStruct[12]->materialAmbient[0] = 0.0f;
	myMaterialStruct[12]->materialAmbient[1] = 0.0f;
	myMaterialStruct[12]->materialAmbient[2] = 0.0f;
	myMaterialStruct[12]->materialAmbient[3] = 1.0f;

	myMaterialStruct[12]->materialDifuse[0] = 0.01f;
	myMaterialStruct[12]->materialDifuse[1] = 0.01f;
	myMaterialStruct[12]->materialDifuse[2] = 0.01f;
	myMaterialStruct[12]->materialDifuse[3] = 1.0f;

	myMaterialStruct[12]->materialSpecular[0] = 0.50f;
	myMaterialStruct[12]->materialSpecular[1] = 0.50f;
	myMaterialStruct[12]->materialSpecular[2] = 0.50f;
	myMaterialStruct[12]->materialSpecular[3] = 1.0f;

	myMaterialStruct[12]->materialShininess = 0.25f*128.0f;
	//13th 3rd colm 2
	myMaterialStruct[13]->materialAmbient[0] = 0.0f;
	myMaterialStruct[13]->materialAmbient[1] = 0.1f;
	myMaterialStruct[13]->materialAmbient[2] = 0.06f;
	myMaterialStruct[13]->materialAmbient[3] = 1.0f;

	myMaterialStruct[13]->materialDifuse[0] = 0.0f;
	myMaterialStruct[13]->materialDifuse[1] = 0.50980392f;
	myMaterialStruct[13]->materialDifuse[2] = 0.50980392f;
	myMaterialStruct[13]->materialDifuse[3] = 1.0f;

	myMaterialStruct[13]->materialSpecular[0] = 0.50196078f;
	myMaterialStruct[13]->materialSpecular[1] = 0.50196078f;
	myMaterialStruct[13]->materialSpecular[2] = 0.50196078f;
	myMaterialStruct[13]->materialSpecular[3] = 1.0f;

	myMaterialStruct[13]->materialShininess = 0.25f*128.0f;
	//14th 3rd colm 3
	myMaterialStruct[14]->materialAmbient[0] = 0.0f;
	myMaterialStruct[14]->materialAmbient[1] = 0.0f;
	myMaterialStruct[14]->materialAmbient[2] = 0.0f;
	myMaterialStruct[14]->materialAmbient[3] = 1.0f;

	myMaterialStruct[14]->materialDifuse[0] = 0.1f;
	myMaterialStruct[14]->materialDifuse[1] = 0.35f;
	myMaterialStruct[14]->materialDifuse[2] = 0.1f;
	myMaterialStruct[14]->materialDifuse[3] = 1.0f;

	myMaterialStruct[14]->materialSpecular[0] = 0.45f;
	myMaterialStruct[14]->materialSpecular[1] = 0.55f;
	myMaterialStruct[14]->materialSpecular[2] = 0.45f;
	myMaterialStruct[14]->materialSpecular[3] = 1.0f;

	myMaterialStruct[14]->materialShininess = 0.25f*128.0f;
	//15th 3rd colm 4
	myMaterialStruct[15]->materialAmbient[0] = 0.0f;
	myMaterialStruct[15]->materialAmbient[1] = 0.0f;
	myMaterialStruct[15]->materialAmbient[2] = 0.0f;
	myMaterialStruct[15]->materialAmbient[3] = 1.0f;

	myMaterialStruct[15]->materialDifuse[0] = 0.5f;
	myMaterialStruct[15]->materialDifuse[1] = 0.0f;
	myMaterialStruct[15]->materialDifuse[2] = 0.0f;
	myMaterialStruct[15]->materialDifuse[3] = 1.0f;

	myMaterialStruct[15]->materialSpecular[0] = 0.7f;
	myMaterialStruct[15]->materialSpecular[1] = 0.6f;
	myMaterialStruct[15]->materialSpecular[2] = 0.6f;
	myMaterialStruct[15]->materialSpecular[3] = 1.0f;

	myMaterialStruct[15]->materialShininess = 0.25f*128.0f;
	//16th 3rd colm 5
	myMaterialStruct[16]->materialAmbient[0] = 0.0f;
	myMaterialStruct[16]->materialAmbient[1] = 0.0f;
	myMaterialStruct[16]->materialAmbient[2] = 0.0f;
	myMaterialStruct[16]->materialAmbient[3] = 1.0f;

	myMaterialStruct[16]->materialDifuse[0] = 0.55f;
	myMaterialStruct[16]->materialDifuse[1] = 0.55f;
	myMaterialStruct[16]->materialDifuse[2] = 0.55f;
	myMaterialStruct[16]->materialDifuse[3] = 1.0f;

	myMaterialStruct[16]->materialSpecular[0] = 0.70f;
	myMaterialStruct[16]->materialSpecular[1] = 0.70f;
	myMaterialStruct[16]->materialSpecular[2] = 0.70f;
	myMaterialStruct[16]->materialSpecular[3] = 1.0f;

	myMaterialStruct[16]->materialShininess = 0.25f*128.0f;
					 
	//17th 3rd colm 6
	myMaterialStruct[17]->materialAmbient[0] = 0.0f;
	myMaterialStruct[17]->materialAmbient[1] = 0.0f;
	myMaterialStruct[17]->materialAmbient[2] = 0.0f;
	myMaterialStruct[17]->materialAmbient[3] = 1.0f;

	myMaterialStruct[17]->materialDifuse[0] = 0.5f;
	myMaterialStruct[17]->materialDifuse[1] = 0.5f;
	myMaterialStruct[17]->materialDifuse[2] = 0.0f;
	myMaterialStruct[17]->materialDifuse[3] = 1.0f;

	myMaterialStruct[17]->materialSpecular[0] = 0.60f;
	myMaterialStruct[17]->materialSpecular[1] = 0.60f;
	myMaterialStruct[17]->materialSpecular[2] = 0.50f;
	myMaterialStruct[17]->materialSpecular[3] = 1.0f;

	myMaterialStruct[17]->materialShininess = 0.25f*128.0f;
	//18th 4th colm 1
	myMaterialStruct[18]->materialAmbient[0] = 0.02f;
	myMaterialStruct[18]->materialAmbient[1] = 0.02f;
	myMaterialStruct[18]->materialAmbient[2] = 0.02f;
	myMaterialStruct[18]->materialAmbient[3] = 1.0f;

	myMaterialStruct[18]->materialDifuse[0] = 0.01f;
	myMaterialStruct[18]->materialDifuse[1] = 0.01f;
	myMaterialStruct[18]->materialDifuse[2] = 0.01f;
	myMaterialStruct[18]->materialDifuse[3] = 1.0f;

	myMaterialStruct[18]->materialSpecular[0] = 0.4f;
	myMaterialStruct[18]->materialSpecular[1] = 0.4f;
	myMaterialStruct[18]->materialSpecular[2] = 0.4f;
	myMaterialStruct[18]->materialSpecular[3] = 1.0f;

	myMaterialStruct[18]->materialShininess = 0.078125f*128.0f;
	//19th 4th colm 2
	myMaterialStruct[19]->materialAmbient[0] = 0.0f;
	myMaterialStruct[19]->materialAmbient[1] = 0.05f;
	myMaterialStruct[19]->materialAmbient[2] = 0.05f;
	myMaterialStruct[19]->materialAmbient[3] = 1.0f;

	myMaterialStruct[19]->materialDifuse[0] = 0.4f;
	myMaterialStruct[19]->materialDifuse[1] = 0.5f;
	myMaterialStruct[19]->materialDifuse[2] = 0.5f;
	myMaterialStruct[19]->materialDifuse[3] = 1.0f;

	myMaterialStruct[19]->materialSpecular[0] = 0.04f;
	myMaterialStruct[19]->materialSpecular[1] = 0.7f;
	myMaterialStruct[19]->materialSpecular[2] = 0.7f;
	myMaterialStruct[19]->materialSpecular[3] = 1.0f;

	myMaterialStruct[19]->materialShininess = 0.078125f*128.0f;
	//20th 4th col 3
	myMaterialStruct[20]->materialAmbient[0] = 0.0f;
	myMaterialStruct[20]->materialAmbient[1] = 0.05f;
	myMaterialStruct[20]->materialAmbient[2] = 0.0f;
	myMaterialStruct[20]->materialAmbient[3] = 1.0f;

	myMaterialStruct[20]->materialDifuse[0] = 0.4f;
	myMaterialStruct[20]->materialDifuse[1] = 0.5f;
	myMaterialStruct[20]->materialDifuse[2] = 0.4f;
	myMaterialStruct[20]->materialDifuse[3] = 1.0f;

	myMaterialStruct[20]->materialSpecular[0] = 0.04f;
	myMaterialStruct[20]->materialSpecular[1] = 0.7f;
	myMaterialStruct[20]->materialSpecular[2] = 0.04f;
	myMaterialStruct[20]->materialSpecular[3] = 1.0f;

	myMaterialStruct[20]->materialShininess = 0.078125f*128.0f;
	//21th 4th col 4
	myMaterialStruct[21]->materialAmbient[0] = 0.05f;
	myMaterialStruct[21]->materialAmbient[1] = 0.0f;
	myMaterialStruct[21]->materialAmbient[2] = 0.0f;
	myMaterialStruct[21]->materialAmbient[3] = 1.0f;

	myMaterialStruct[21]->materialDifuse[0] = 0.5f;
	myMaterialStruct[21]->materialDifuse[1] = 0.4f;
	myMaterialStruct[21]->materialDifuse[2] = 0.4f;
	myMaterialStruct[21]->materialDifuse[3] = 1.0f;

	myMaterialStruct[21]->materialSpecular[0] = 0.7f;
	myMaterialStruct[21]->materialSpecular[1] = 0.04f;
	myMaterialStruct[21]->materialSpecular[2] = 0.04f;
	myMaterialStruct[21]->materialSpecular[3] = 1.0f;

	myMaterialStruct[21]->materialShininess = 0.078125f*128.0f;
	//22th 4th col 5th
	myMaterialStruct[22]->materialAmbient[0] = 0.5f;
	myMaterialStruct[22]->materialAmbient[1] = 0.5f;
	myMaterialStruct[22]->materialAmbient[2] = 0.5f;
	myMaterialStruct[22]->materialAmbient[3] = 1.0f;

	myMaterialStruct[22]->materialDifuse[0] = 0.5f;
	myMaterialStruct[22]->materialDifuse[1] = 0.5f;
	myMaterialStruct[22]->materialDifuse[2] = 0.5f;
	myMaterialStruct[22]->materialDifuse[3] = 1.0f;

	myMaterialStruct[22]->materialSpecular[0] = 0.7f;
	myMaterialStruct[22]->materialSpecular[1] = 0.7f;
	myMaterialStruct[22]->materialSpecular[2] = 0.7f;
	myMaterialStruct[22]->materialSpecular[3] = 1.0f;

	myMaterialStruct[22]->materialShininess = 0.078125f*128.0f;
	//23th 4th col 6th
	myMaterialStruct[23]->materialAmbient[0] = 0.05f;
	myMaterialStruct[23]->materialAmbient[1] = 0.05f;
	myMaterialStruct[23]->materialAmbient[2] = 0.0f;
	myMaterialStruct[23]->materialAmbient[3] = 1.0f;

	myMaterialStruct[23]->materialDifuse[0] = 0.5f;
	myMaterialStruct[23]->materialDifuse[1] = 0.5f;
	myMaterialStruct[23]->materialDifuse[2] = 0.4f;
	myMaterialStruct[23]->materialDifuse[3] = 1.0f;

	myMaterialStruct[23]->materialSpecular[0] = 0.7f;
	myMaterialStruct[23]->materialSpecular[1] = 0.7f;
	myMaterialStruct[23]->materialSpecular[2] = 0.7f;
	myMaterialStruct[23]->materialSpecular[3] = 1.0f;

	myMaterialStruct[23]->materialShininess = 0.078125f*128.0f;
	

}
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
//RED COLOR
GLuint laUniform_PV_RED;
GLuint ldUniform_PV_RED;
GLuint lsUniform_PV_RED;

GLuint lightPositionUniform_PV_RED;
//GREEN COLOR
GLuint laUniform_PV_GREEN;
GLuint ldUniform_PV_GREEN;
GLuint lsUniform_PV_GREEN;

GLuint lightPositionUniform_PV_GREEN;
//BLUE COLOR
GLuint laUniform_PV_BLUE;
GLuint ldUniform_PV_BLUE;
GLuint lsUniform_PV_BLUE;

GLuint lightPositionUniform_PV_BLUE;;
//Material Per Vertex 
GLuint kaUniform_PV;
GLuint kdUniform_PV;
GLuint ksUniform_PV;



////////////******shader uniforms for per fragment****/////////////
GLuint isLKeyIsPressedUniforms_PF;
GLuint shininessUniform_PF;
//RED COLOR
GLuint laUniform_PF_RED;
GLuint ldUniform_PF_RED;
GLuint lsUniform_PF_RED;

GLuint lightPositionUniform_PF_RED;
//GREEN COLOR
GLuint laUniform_PF_GREEN;
GLuint ldUniform_PF_GREEN;
GLuint lsUniform_PF_GREEN;

GLuint lightPositionUniform_PF_GREEN;
//BLUE COLOR
GLuint laUniform_PF_BLUE;
GLuint ldUniform_PF_BLUE;
GLuint lsUniform_PF_BLUE;

GLuint lightPositionUniform_PF_BLUE;;
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

//light array Red
float lightAmbient_Red[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
float lightDifuse_Red[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
float lightSpecular_Red[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
//light array Green
//
float lightAmbient_Green[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
float lightDifuse_Green[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
float lightSpecular_Green[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
//light array Blue
float lightAmbient_Blue[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
float lightDifuse_Blue[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
float lightSpecular_Blue[4] = { 0.0f, 0.0f, 1.0f, 1.0f };

//material array
float materialAmbient[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
float materialDifuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
float materialSpecular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

float lightPosition_Red[4];
float lightPosition_Green[4];
float lightPosition_Blue[4];

float materialShininess = 50.0f;

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
		TEXT("SPHERE_PV_PF-SHUBHAM"),
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
		bFullScreen = true;
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
		//RED
		"uniform vec3 u_la_PV_RED;" \
		"uniform vec3 u_ld_PV_RED;" \
		"uniform vec3 u_ls_PV_RED;" \
		//GREEN
		"uniform vec3 u_la_PV_GREEN;" \
		"uniform vec3 u_ld_PV_GREEN;" \
		"uniform vec3 u_ls_PV_GREEN;" \
		//BLUE
		"uniform vec3 u_la_PV_BLUE;" \
		"uniform vec3 u_ld_PV_BLUE;" \
		"uniform vec3 u_ls_PV_BLUE;" \
		//Material
		"uniform vec3 u_ka_PV;" \
		"uniform vec3 u_kd_PV;" \
		"uniform vec3 u_ks_PV;" \

		"uniform float u_shininess_PV;" \

		"uniform vec4 u_light_position_PV_RED;" \
		"uniform vec4 u_light_position_PV_GREEN;" \
		"uniform vec4 u_light_position_PV_BLUE;" \

		"out vec3 phong_ads_light_PV;" \
		"void main(void)" \
		"{" \
			"if(islkeypressed_PV == 1)" \
			"{" \
				"vec4 eye_coordinates = u_view_matrix *  u_model_matrix  * vPosition;" \
				"vec3 tnorm = normalize(mat3(u_view_matrix * u_model_matrix) * vNormal);" \

				"vec3 light_direction_RED = normalize(vec3(u_light_position_PV_RED - eye_coordinates));" \
				"vec3 light_direction_BLUE = normalize(vec3(u_light_position_PV_BLUE - eye_coordinates));" \
				"vec3 light_direction_GREEN = normalize(vec3(u_light_position_PV_GREEN - eye_coordinates));" \

				"float tn_dot_ldirection_RED = max(dot(light_direction_RED, tnorm), 0);" \
				"float tn_dot_ldirection_GREEN = max(dot(light_direction_GREEN, tnorm), 0);" \
				"float tn_dot_ldirection_BLUE = max(dot(light_direction_BLUE, tnorm), 0);" \

				"vec3 reflection_vector_RED = reflect(-light_direction_RED, tnorm);" \
				"vec3 reflection_vector_GREEN = reflect(-light_direction_GREEN, tnorm);" \
				"vec3 reflection_vector_BLUE = reflect(-light_direction_BLUE, tnorm);" \

				"vec3 viewer_vector = normalize(vec3(-eye_coordinates));" \

				"vec3 ambient_RED = u_la_PV_RED * u_ka_PV;" \
				"vec3 ambient_GREEN = u_la_PV_GREEN * u_ka_PV;" \
				"vec3 ambient_BLUE = u_la_PV_BLUE * u_ka_PV;" \

				"vec3 difuse_RED = u_ld_PV_RED * u_kd_PV * tn_dot_ldirection_RED;" \
				"vec3 difuse_GREEN = u_ld_PV_GREEN * u_kd_PV * tn_dot_ldirection_GREEN;" \
				"vec3 difuse_BLUE = u_ld_PV_BLUE * u_kd_PV * tn_dot_ldirection_BLUE;" \

				"vec3 specular_RED = u_ls_PV_RED * u_ks_PV * pow(max(dot(reflection_vector_RED, viewer_vector),0),u_shininess_PV);" \
				"vec3 specular_GREEN = u_ls_PV_GREEN * u_ks_PV * pow(max(dot(reflection_vector_GREEN, viewer_vector),0),u_shininess_PV);" \
				"vec3 specular_BLUE = u_ls_PV_BLUE * u_ks_PV * pow(max(dot(reflection_vector_BLUE, viewer_vector),0),u_shininess_PV);" \
				
				"vec3 redLight = ambient_RED + difuse_RED + specular_RED;" \
				"vec3 blueLight = ambient_BLUE + difuse_BLUE + specular_BLUE;" \
				"vec3 greenLight = ambient_GREEN + difuse_GREEN + specular_GREEN;" \

				"phong_ads_light_PV = redLight + blueLight + greenLight;" \
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

		"uniform vec4 u_light_position_PF_RED;" \
		"uniform vec4 u_light_position_PF_GREEN;" \
		"uniform vec4 u_light_position_PF_BLUE;" \

		"out vec3 tnorm_PF;" \
		
		"out vec3 light_direction_PF_RED;" \
		"out vec3 light_direction_PF_GREEN;" \
		"out vec3 light_direction_PF_BLUE;" \

		"out vec3 viewer_vector_PF;" \
		"void main(void)" \
		"{" \
			"if(islkeypressed_PF == 1)" \
			"{" \
				"vec4 eye_coordinates = u_view_matrix *  u_model_matrix  * vPosition;" \

				"tnorm_PF = mat3(u_view_matrix * u_model_matrix) * vNormal;" \

				"light_direction_PF_RED = vec3(u_light_position_PF_RED - eye_coordinates);" \
				"light_direction_PF_GREEN = vec3(u_light_position_PF_GREEN - eye_coordinates);" \
				"light_direction_PF_BLUE = vec3(u_light_position_PF_BLUE - eye_coordinates);" \

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

		"uniform vec3 u_la_PF_RED;" \
		"uniform vec3 u_la_PF_GREEN;" \
		"uniform vec3 u_la_PF_BLUE;" \

		"uniform vec3 u_ld_PF_RED;" \
		"uniform vec3 u_ld_PF_GREEN;" \
		"uniform vec3 u_ld_PF_BLUE;" \

		"uniform vec3 u_ls_PF_RED;" \
		"uniform vec3 u_ls_PF_GREEN;" \
		"uniform vec3 u_ls_PF_BLUE;" \

		"uniform vec3 u_ka_PF;" \
		"uniform vec3 u_kd_PF;" \
		"uniform vec3 u_ks_PF;" \

		"in vec3 tnorm_PF;" \

		"in vec3 light_direction_PF_RED;" \
		"in vec3 light_direction_PF_GREEN;" \
		"in vec3 light_direction_PF_BLUE;" \

		"in vec3 viewer_vector_PF;" \
		"uniform float u_shininess_PF;" \
		"void main(void)" \
		"{" \
			"if(islkeypressed_PF == 1)" \
			"{" \
				"vec3 tnorm_normalized = normalize(tnorm_PF);" \

				"vec3 light_direction_normalized_RED = normalize(light_direction_PF_RED);" \
				"vec3 light_direction_normalized_GREEN = normalize(light_direction_PF_GREEN);" \
				"vec3 light_direction_normalized_BLUE = normalize(light_direction_PF_BLUE);" \

				"vec3 viewer_vector_normalized = normalize(viewer_vector_PF);" \

				"float tn_dot_ldirection_RED = max(dot(light_direction_normalized_RED, tnorm_normalized), 0);" \
				"float tn_dot_ldirection_GREEN = max(dot(light_direction_normalized_GREEN, tnorm_normalized), 0);" \
				"float tn_dot_ldirection_BLUE = max(dot(light_direction_normalized_BLUE, tnorm_normalized), 0);" \

				"vec3 reflection_vector_RED = reflect(-light_direction_normalized_RED, tnorm_normalized);" \
				"vec3 reflection_vector_GREEN = reflect(-light_direction_normalized_GREEN, tnorm_normalized);" \
				"vec3 reflection_vector_BLUE = reflect(-light_direction_normalized_BLUE, tnorm_normalized);" \

				"vec3 ambient_RED = u_la_PF_RED * u_ka_PF;" \
				"vec3 ambient_GREEN = u_la_PF_GREEN * u_ka_PF;" \
				"vec3 ambient_BLUE = u_la_PF_BLUE * u_ka_PF;" \

				"vec3 difuse_RED = u_ld_PF_RED * u_kd_PF * tn_dot_ldirection_RED;" \
				"vec3 difuse_GREEN = u_ld_PF_GREEN * u_kd_PF * tn_dot_ldirection_GREEN;" \
				"vec3 difuse_BLUE = u_ld_PF_BLUE * u_kd_PF * tn_dot_ldirection_BLUE;" \

				"vec3 specular_RED = u_ls_PF_RED * u_ks_PF * pow(max(dot(reflection_vector_RED, viewer_vector_normalized),0),u_shininess_PF);" \
				"vec3 specular_GREEN = u_ls_PF_GREEN * u_ks_PF * pow(max(dot(reflection_vector_GREEN, viewer_vector_normalized),0),u_shininess_PF);" \
				"vec3 specular_BLUE = u_ls_PF_BLUE * u_ks_PF * pow(max(dot(reflection_vector_BLUE, viewer_vector_normalized),0),u_shininess_PF);" \

				"vec3 redLight = ambient_RED + difuse_RED + specular_RED;" \
				"vec3 greenLight = ambient_GREEN + difuse_GREEN + specular_GREEN;" \
				"vec3 blueLight = ambient_BLUE + difuse_BLUE + specular_BLUE;" \


				"vec3 phong_light_pf = redLight + greenLight + blueLight;" \
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
	
	laUniform_PV_RED	= glGetUniformLocation(gShaderProgramObject_PV, "u_la_PV_RED");
	laUniform_PV_GREEN  = glGetUniformLocation(gShaderProgramObject_PV, "u_la_PV_GREEN");
	laUniform_PV_BLUE   = glGetUniformLocation(gShaderProgramObject_PV, "u_la_PV_BLUE");
	
	ldUniform_PV_RED	= glGetUniformLocation(gShaderProgramObject_PV, "u_ld_PV_RED");
	ldUniform_PV_GREEN  = glGetUniformLocation(gShaderProgramObject_PV, "u_ld_PV_GREEN");
	ldUniform_PV_BLUE	= glGetUniformLocation(gShaderProgramObject_PV, "u_ld_PV_BLUE");

	lsUniform_PV_RED	 = glGetUniformLocation(gShaderProgramObject_PV, "u_ls_PV_RED");
	lsUniform_PV_GREEN   = glGetUniformLocation(gShaderProgramObject_PV, "u_ls_PV_GREEN");
	lsUniform_PV_BLUE	 = glGetUniformLocation(gShaderProgramObject_PV, "u_ls_PV_BLUE");

	kaUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_ka_PV");
	kdUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_kd_PV");
	ksUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_ks_PV");

	shininessUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_shininess_PV");

	lightPositionUniform_PV_RED = glGetUniformLocation(gShaderProgramObject_PV, "u_light_position_PV_RED");
	lightPositionUniform_PV_GREEN = glGetUniformLocation(gShaderProgramObject_PV, "u_light_position_PV_GREEN");
	lightPositionUniform_PV_BLUE = glGetUniformLocation(gShaderProgramObject_PV, "u_light_position_PV_BLUE");

	//PER FRAG
	laUniform_PF_RED = glGetUniformLocation(gShaderProgramObject_PF, "u_la_PF_RED");
	laUniform_PF_GREEN = glGetUniformLocation(gShaderProgramObject_PF, "u_la_PF_GREEN");
	laUniform_PF_BLUE = glGetUniformLocation(gShaderProgramObject_PF, "u_la_PF_BLUE");

	ldUniform_PF_RED = glGetUniformLocation(gShaderProgramObject_PF, "u_ld_PF_RED");
	ldUniform_PF_GREEN = glGetUniformLocation(gShaderProgramObject_PF, "u_ld_PF_GREEN");
	ldUniform_PF_BLUE = glGetUniformLocation(gShaderProgramObject_PF, "u_ld_PF_BLUE");

	lsUniform_PF_RED = glGetUniformLocation(gShaderProgramObject_PF, "u_ls_PF_RED");
	lsUniform_PF_GREEN = glGetUniformLocation(gShaderProgramObject_PF, "u_ls_PF_GREEN");
	lsUniform_PF_BLUE = glGetUniformLocation(gShaderProgramObject_PF, "u_ls_PF_BLUE");

	
	kaUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_ka_PF");
	kdUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_kd_PF");
	ksUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_ks_PF");

	
	shininessUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_shininess_PF");

	
	lightPositionUniform_PF_RED = glGetUniformLocation(gShaderProgramObject_PF, "u_light_position_PF_RED");
	lightPositionUniform_PF_GREEN = glGetUniformLocation(gShaderProgramObject_PF, "u_light_position_PF_GREEN");
	lightPositionUniform_PF_BLUE = glGetUniformLocation(gShaderProgramObject_PF, "u_light_position_PF_BLUE");

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

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);

	perspectiveProjectionMatrix = mat4::identity();

	resize(WIN_WIDTH, WIN_HEIGHT);

	return 0;
}

void resize(int width, int height)
{
	if (height == 0)
	{
		height = 1;
	}
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	perspectiveProjectionMatrix =
		perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

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

	//Do transformations...
	modelMatrix = translate(0.0f, 0.0f, -3.0f);

	//send neccessary matrix to shader in respective uniforms

	if (isPerVertex == true)
	{
		glUseProgram(gShaderProgramObject_PV);
		//fprintf(gpfile,"PER VERTEX\n");
		if (isLighting == true)
		{
			glUniform1i(isLKeyIsPressedUniforms_PV, 1);

			glUniform1f(shininessUniform_PV, materialShininess);

			glUniform3fv(laUniform_PV_RED, 1, lightAmbient_Red);
			glUniform3fv(laUniform_PV_BLUE, 1, lightAmbient_Blue);
			glUniform3fv(laUniform_PV_GREEN, 1, lightAmbient_Green);

			glUniform3fv(ldUniform_PV_RED, 1, lightDifuse_Red);
			glUniform3fv(ldUniform_PV_GREEN, 1, lightDifuse_Green);
			glUniform3fv(ldUniform_PV_BLUE, 1, lightDifuse_Blue);

			glUniform3fv(lsUniform_PV_RED, 1, lightSpecular_Red);
			glUniform3fv(lsUniform_PV_GREEN, 1, lightSpecular_Green);
			glUniform3fv(lsUniform_PV_BLUE, 1, lightSpecular_Blue);


			glUniform3fv(kaUniform_PV, 1, materialAmbient);
			glUniform3fv(kdUniform_PV, 1, materialDifuse);
			glUniform3fv(ksUniform_PV, 1, materialSpecular);

			glUniform4fv(lightPositionUniform_PV_RED, 1, lightPosition_Red);
			glUniform4fv(lightPositionUniform_PV_GREEN, 1, lightPosition_Green);
			glUniform4fv(lightPositionUniform_PV_BLUE, 1, lightPosition_Blue);
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

			glUniform1f(shininessUniform_PF, materialShininess);

			glUniform3fv(laUniform_PF_RED, 1, lightAmbient_Red);
			glUniform3fv(laUniform_PF_GREEN, 1, lightAmbient_Green);
			glUniform3fv(laUniform_PF_BLUE, 1, lightAmbient_Blue);

			glUniform3fv(ldUniform_PF_RED, 1, lightDifuse_Red);
			glUniform3fv(ldUniform_PF_GREEN, 1, lightDifuse_Green);
			glUniform3fv(ldUniform_PF_BLUE, 1, lightDifuse_Blue);

			glUniform3fv(lsUniform_PF_RED, 1, lightSpecular_Red);
			glUniform3fv(lsUniform_PF_GREEN, 1, lightSpecular_Green);
			glUniform3fv(lsUniform_PF_BLUE, 1, lightSpecular_Blue);

			glUniform3fv(kaUniform_PF, 1, materialAmbient);
			glUniform3fv(kdUniform_PF, 1, materialDifuse);
			glUniform3fv(ksUniform_PF, 1, materialSpecular);

			glUniform4fv(lightPositionUniform_PF_RED, 1, lightPosition_Red);
			glUniform4fv(lightPositionUniform_PF_GREEN, 1, lightPosition_Green);
			glUniform4fv(lightPositionUniform_PF_BLUE, 1, lightPosition_Blue);
		}
		else
		{
			glUniform1i(isLKeyIsPressedUniforms_PF, 0);
		}
		//
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


	//similarly bind with texture in any
	glBindVertexArray(vao_sphere);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element_vbo);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

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
		angle_X += 0.04f;
	}

	lightPosition_Red[0] = 0.0f;
	lightPosition_Red[1] = 100.0f*sin(angle_X);
	lightPosition_Red[2] = 100.0f *cos(angle_X);
	lightPosition_Red[3] = 1.0f;

	lightPosition_Green[0] = 100.0f*cos(angle_X);
	lightPosition_Green[1] = 0.0f;
	lightPosition_Green[2] = 100.0f*sin(angle_X);
	lightPosition_Green[3] = 1.0f;

	lightPosition_Blue[0] = 100 * cos(angle_X);
	lightPosition_Blue[1] = 100 * sin(angle_X);
	lightPosition_Blue[2] = 0.0f;
	lightPosition_Blue[3] = 1.0f;

	

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
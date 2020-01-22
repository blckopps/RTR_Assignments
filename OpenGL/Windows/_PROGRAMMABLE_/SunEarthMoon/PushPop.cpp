#include<Windows.h>
#include<stdio.h>
#include<gl/glew.h>
#include<gl/GL.h>
#include"vmath.h"

#include"Sphere.h"
#include"List.h"

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


GLuint gShaderProgramObject;

//vao vbo
GLuint vao_sphere;
GLuint vbo_position_vbo;
GLuint vbo_normals_vbo;
GLuint vbo_element_vbo;

//Transformation matrix uniforms
GLuint modelMatrixUniform;
GLuint viewMatrixUniform;
GLuint projectionUniform;
mat4 perspectiveProjectionMatrix;

//shader uniforms
GLuint isLKeyIsPressedUniforms;
GLuint ldUniform;
GLuint kdUniform;
GLuint lightPositionUniform;

bool isLighting = false;
float gAngle = 0.0f;

//sphere var
float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_textures[764];
unsigned short sphere_elements[2280];
GLsizei gNumVertices, gNumElements;

//Stack varibales
list_t *gStack = NULL;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	//FUnction declaration
	int initialize(void);
	void display(void);
	//void update();

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
		TEXT("SPHERE BASE APP-SHUBHAM"),
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
				//update();
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


		default:
			break;
		}
		break;


	case WM_KEYDOWN:

		switch (wParam)
		{
		case 0x46:
			//	MessageBox(hwnd,TEXT("P BUTTON PRESSED!!"),TEXT("BUTTON P"),MB_OK);
			toogle_screen();

			break;
		case VK_ESCAPE:
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
		//MessageBox(hwnd,TEXT("This is WM_DESTROY!!"),TEXT("In Wm_DESTROY"),MB_OK);
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

	//Init for stack
	gStack = CreateList();
	if (gStack == NULL)
	{
		fprintf(gpfile,"Fail to create gStack!!\n");
	}
	else
	{
		fprintf(gpfile, " create gStack!!\n");
	}
	//shader objects
	GLuint gVertexShaderObject;
	GLuint gFragmentShaderObject;

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
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	//write vertex shader code
	// 
	const GLchar* vertexShaderSourceCode =
		"#version 430 core" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec3 vNormal;" \
		"uniform mat4 u_model_matrix;" \
		"uniform mat4 u_view_matrix;" \
		"uniform mat4 u_projection_matrix;" \
		"uniform int islkeypressed;" \
		"uniform vec3 u_ld;" \
		"uniform vec3 u_kd;" \
		"uniform vec4 u_light_position;" \
		"out vec3 difuse_color;" \
		"void main(void)" \
		"{" \
		"if(islkeypressed == 1)" \
		"{" \
		"vec4 eye_coordinates = u_view_matrix *  u_model_matrix  * vPosition;" \
		"mat3 normal_matrix = mat3(transpose(inverse(u_view_matrix * u_model_matrix )));" \
		"vec3 tnorm = normalize(normal_matrix * vNormal);" \
		"vec3 s = normalize(vec3(u_light_position - eye_coordinates));" \
		"difuse_color = u_ld * u_kd * dot(s, tnorm);" \
		"}" \
		"gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" \
		" } ";
	//specify vertex source code to vetex shader object
	glShaderSource(gVertexShaderObject,
		1,
		(GLchar **)&vertexShaderSourceCode,
		NULL);
	//compile vertex shader code
	glCompileShader(gVertexShaderObject);

	//Error checking
	GLint iShaderCompilationStatus = 0;
	GLint iInfoLength = 0;
	GLchar *szInfoLog = NULL;

	glGetShaderiv(gVertexShaderObject,
		GL_COMPILE_STATUS,
		&iShaderCompilationStatus);

	if (iShaderCompilationStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject,
			GL_INFO_LOG_LENGTH,
			&iInfoLength);
		if (iInfoLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLength);

			if (szInfoLog != NULL)
			{
				GLsizei writtened;

				glGetShaderInfoLog(gVertexShaderObject,
					iInfoLength,
					&writtened,
					szInfoLog);
				fwprintf(gpfile, TEXT("ERROR:VERTEX SHADER:%s"), szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(ghwnd);
				exit(0);
			}
		}
	}
	//********************FRAGMENT SHADER*****************
	gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	//write vertex shader code 
	// 
	const GLchar * fragmentShaderSourceCode =
		"#version 430 core" \
		"\n" \
		"out vec4 fragColor;" \
		"uniform int islkeypressed;" \
		"in vec3 difuse_color;" \
		"void main(void)" \
		"{" \
		"if(islkeypressed == 1)" \
		"{" \
		"fragColor = vec4(difuse_color, 1.0);" \
		"}" \
		"else" \
		"{" \
		"fragColor = vec4(1.0, 1.0, 1.0 , 1.0);" \
		"}" \
		"}";
	//specify vertex source code to vetex shader object
	glShaderSource(gFragmentShaderObject,
		1,
		(GLchar **)&fragmentShaderSourceCode,
		NULL);
	//compile vertex shader code
	glCompileShader(gFragmentShaderObject);

	//Error checking
	iShaderCompilationStatus = 0;
	iInfoLength = 0;
	szInfoLog = NULL;

	glGetShaderiv(gFragmentShaderObject,
		GL_COMPILE_STATUS,
		&iShaderCompilationStatus);

	if (iShaderCompilationStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject,
			GL_INFO_LOG_LENGTH,
			&iInfoLength);
		if (iInfoLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLength);

			if (szInfoLog != NULL)
			{
				GLsizei writtened;

				glGetShaderInfoLog(gFragmentShaderObject,
					iInfoLength,
					&writtened,
					szInfoLog);
				fprintf(gpfile, "ERROR:FRAGMENT SHADER:%s", szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(ghwnd);
				exit(0);
			}
		}
	}
	//Create shader program object
	gShaderProgramObject = glCreateProgram();

	//Attach vertex,fragment shader program
	glAttachShader(gShaderProgramObject, gVertexShaderObject);
	glAttachShader(gShaderProgramObject, gFragmentShaderObject);
	//*** PRELINKING BINDING TO VERTEX ATTRIBUTES***
	glBindAttribLocation(gShaderProgramObject,
		AMC_ATTRIBUTES_POSITION,
		"vPosition");

	glBindAttribLocation(gShaderProgramObject,
		AMC_ATTRIBUTES_NORMAL,
		"vNormal");
	//link above shader program
	glLinkProgram(gShaderProgramObject);

	// ***ERROR CHECKING LINKING********
	//Error checking for program linking
	GLint iShaderLinkStatus = 0;
	iInfoLength = 0;
	szInfoLog = NULL;

	glGetProgramiv(gShaderProgramObject,
		GL_LINK_STATUS,
		&iShaderLinkStatus);
	if (iShaderCompilationStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObject,
			GL_INFO_LOG_LENGTH,
			&iInfoLength);
		if (iInfoLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLength);

			if (szInfoLog != NULL)
			{
				GLsizei wr;

				glGetShaderInfoLog(gShaderProgramObject,
					iInfoLength,
					&wr,
					szInfoLog);

				fwprintf(gpfile, TEXT("ERROR:SHADER LINK:%s"), szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(ghwnd);
				exit(0);
			}

		}
	}

	///***POST LINKING GETTING UNIFORMS**

	//getting light uniforms locations
	isLKeyIsPressedUniforms = glGetUniformLocation(gShaderProgramObject, "islkeypressed");

	ldUniform = glGetUniformLocation(gShaderProgramObject, "u_ld");

	kdUniform = glGetUniformLocation(gShaderProgramObject, "u_kd");

	lightPositionUniform = glGetUniformLocation(gShaderProgramObject, "u_light_position");

	//getting transformation matrices uniform locations
	modelMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_model_matrix");

	viewMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_view_matrix");

	projectionUniform = glGetUniformLocation(gShaderProgramObject, "u_projection_matrix");

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
	result_t PushMatrix(list_t *pList, data_t pData);
	result_t PopMatrix(list_t *pList, data_t *pData);

	void uninitialize();

	//fwprintf(gpfile,TEXT("In Display\n"));
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(gShaderProgramObject);

	//declare matrices
	mat4 viewMatrix;
	mat4 modelMatrix;
	mat4 scaleMatirx;

	//make identity matrices
	viewMatrix = mat4::identity();
	modelMatrix = mat4::identity();
	scaleMatirx = mat4::identity();

	//Do transformations...
	modelMatrix = translate(0.0f, 0.0f, -8.0f);
	modelMatrix = modelMatrix * scale(1.5f, 1.5f, 1.5f);
	modelMatrix = modelMatrix * rotate(gAngle , 0.0f,1.0f,0.0f);
	//send neccessary matrix to shader in respective uniforms

	//key events uniforms

	if (isLighting == true)
	{
		glUniform1i(isLKeyIsPressedUniforms, 1);

		glUniform3f(ldUniform, 1.0f, 0.0f, 0.0f);

		glUniform3f(kdUniform, 0.5f, 0.5f, 0.5f);

		glUniform4f(lightPositionUniform, 0.0f, 0.0f, 2.0f, 1.0f);
	}
	else
	{
		glUniform1i(isLKeyIsPressedUniforms, 0);
	}

	///////////////////////First SPhere////////////////////////////////////

	glUniformMatrix4fv(modelMatrixUniform,
						1,
						GL_FALSE,
						modelMatrix);

	glUniformMatrix4fv(viewMatrixUniform,
						1,
						GL_FALSE,
						viewMatrix);

	glUniformMatrix4fv(projectionUniform,
						1,
						GL_FALSE,
						perspectiveProjectionMatrix);

	//pushing sun's model matrix
	result_t ec = PushMatrix(gStack,modelMatrix);
	if (ec != LST_SUCCESS)
	{
		fprintf(gpfile,"Insert model matrix for sun is fail\n");
		uninitialize();
		exit(0);
	}

	//Bind with  vao
	//similarly bind with texture in any
	glBindVertexArray(vao_sphere);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element_vbo);

	

	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);


	glBindVertexArray(0);

	//pop sun matrix
	ec = PopMatrix(gStack, &modelMatrix);
	if (ec != LST_SUCCESS)
	{
		fprintf(gpfile, "faild to pop \n");
		uninitialize();
		exit(0);
	}

	///////////////2nd Sphere earth//////////////////////////

	viewMatrix = mat4::identity();
	//modelMatrix = mat4::identity();
	scaleMatirx = mat4::identity();

	//Do transformations...
	modelMatrix = modelMatrix * translate(1.5f, 0.0f, 0.0f);
	modelMatrix = modelMatrix * rotate(gAngle * 2.0f, 0.0f, 1.0f, 0.0f);
	modelMatrix = modelMatrix * scale(-0.5f,-0.5f,-0.5f);

	if (isLighting == true)
	{

		glUniform3f(ldUniform, 0.0f, 0.0f, 1.0f);

	}

	glUniformMatrix4fv(modelMatrixUniform,
						1,
						GL_FALSE,
						modelMatrix
						);

	glUniformMatrix4fv(viewMatrixUniform,
						1,
						GL_FALSE,
						viewMatrix);

	glUniformMatrix4fv(projectionUniform,
							1,
							GL_FALSE,
							perspectiveProjectionMatrix);

	//push the model matrix sun+earth
	ec = PushMatrix(gStack, modelMatrix);
	if (ec != LST_SUCCESS)
	{
		fprintf(gpfile, "faild to push sun + earth\n");
		uninitialize();
		exit(0);
	}

	glBindVertexArray(vao_sphere);

	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

	glBindVertexArray(0);

	///////////////3rd Sphere MOON/////////////////////////

	viewMatrix = mat4::identity();
	//modelMatrix = mat4::identity();

	//Do transformations...

	//Pop previous stack
	 ec = PopMatrix(gStack, &modelMatrix);
	if (ec != LST_SUCCESS)
	{
		fprintf(gpfile,"faild to pop \n");
		uninitialize();
		exit(0);
	}
	modelMatrix = modelMatrix * translate(1.0f, 0.0f, 0.0f);
	modelMatrix = modelMatrix * rotate(gAngle * 1.0f, 0.0f, 1.0f, 0.0f);

	if (isLighting == true)
	{

		glUniform3f(ldUniform, 1.0f, 1.0f, 1.0f);

	}

	glUniformMatrix4fv(modelMatrixUniform,
		1,
		GL_FALSE,
		modelMatrix
	);

	glUniformMatrix4fv(viewMatrixUniform,
		1,
		GL_FALSE,
		viewMatrix);

	glUniformMatrix4fv(projectionUniform,
		1,
		GL_FALSE,
		perspectiveProjectionMatrix);

	

	glBindVertexArray(vao_sphere);

	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

	glBindVertexArray(0);

	glUseProgram(0);
	SwapBuffers(ghdc);

	gAngle = gAngle + 0.03f;
	//fwprintf(gpfile,TEXT("End OF display\n"));
}

void uninitialize(void)
{
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
	////shader uninitialize code
	GLsizei shaderCount;
	GLsizei shaderNumber;

	if (gShaderProgramObject)
	{
		glUseProgram(gShaderProgramObject);

		glGetProgramiv(gShaderProgramObject,
			GL_ATTACHED_SHADERS,
			&shaderCount);


		GLuint *pShaders = (GLuint*)malloc(sizeof(GLuint)*shaderCount);

		if (pShaders)
		{
			glGetAttachedShaders(gShaderProgramObject,
				shaderCount,
				&shaderCount,
				pShaders);

			for (shaderNumber = 0; shaderNumber < shaderCount; shaderNumber++)
			{
				glDetachShader(gShaderProgramObject,
					pShaders[shaderNumber]);

				glDeleteShader(pShaders[shaderNumber]);

				pShaders[shaderNumber] = 0;
			}
			free(pShaders);
		}

		glDeleteProgram(gShaderProgramObject);

		gShaderProgramObject = 0;

		glUseProgram(0);

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

	//deleteStack
	DestroyList(&gStack);
}

//Stack method
result_t PushMatrix(list_t *pList, data_t pData)
{
	return InsertAtBegining(pList, pData);
}

result_t PopMatrix(list_t *pList, data_t *pData)
{
	return ExamineAndDeleteBegining(pList, pData);
}

BOOL isStackEmpty(list_t *pList)
{
	return TRUE;
}
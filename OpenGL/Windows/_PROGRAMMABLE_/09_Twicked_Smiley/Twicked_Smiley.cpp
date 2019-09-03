#include<Windows.h>
#include<stdio.h>
#include<GL/glew.h>
#include<gl/GL.h>
#include"vmath.h"
#include"TwickedSmiley.h"

using namespace vmath;

#pragma comment(lib,"openGL32.lib")
#pragma comment(lib,"glew32.lib")

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

FILE *gpfile = NULL;

LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);

enum
{
	AMC_ATTRIBUTES_POSITION = 0,
	AMC_ATTRIBUTES_COLOR, 
	AMC_ATTRIBUTES_NORMAL, 
	AMC_ATTRIBUTES_TEXCOORD0 
};

//GLOBAL VARIABLES
bool bFullScreen=false;
DWORD dwstyle;
WINDOWPLACEMENT wpPrev={sizeof(WINDOWPLACEMENT)};
HWND ghwnd=NULL;
bool gbActiveWindow=false;
HDC ghdc=NULL;
HGLRC ghrc=NULL;

GLuint texture_smiley;

GLuint gShaderProgramObject;
//declare vao's according to shapes
GLuint vao_Triangle;
GLuint vao_Rectangle;

//declare vbo's according to Arrays(position,light etc)
GLuint vbo_Rectangle;
GLuint vbo_texture_smiley;

GLuint mvpUniform;
GLuint sampler_Uniform;
mat4 perspectiveProjectionMatrix;

//
int keyPressed = 1;

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpszCmdLine,int iCmdShow)
{
	//FUnction declaration
	int initialize(void);
	void display(void);
	
	//variable decl
	int iret=0;
	bool bdone=false;

	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[]=TEXT("MYWINDOW ");

	if(fopen_s(&gpfile, "log.txt","w")!=0)
	{
		MessageBox(NULL, TEXT("Cant create log"),TEXT("ERROR!!!"),MB_OK);
	
	}
	else
	{
		fwprintf(gpfile, TEXT("log file created\n"));
	}

	wndclass.cbSize=sizeof(WNDCLASSEX);
	wndclass.style=CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra=0;
	wndclass.cbWndExtra=0;
	wndclass.lpfnWndProc=WndProc;
	wndclass.hInstance=hInstance;
	wndclass.hIcon=LoadIcon(NULL,IDI_APPLICATION);
	wndclass.hCursor=LoadCursor(NULL,IDC_ARROW);
	wndclass.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszClassName=szAppName;
	wndclass.lpszMenuName=NULL;
	wndclass.hIconSm=LoadIcon(NULL,IDI_APPLICATION);

	RegisterClassEx(&wndclass);

	//create window

	hwnd=CreateWindowEx(WS_EX_APPWINDOW,
						szAppName,
						TEXT("TWICKED_TEXTURE_SMILEY-SHUBHAM"),
						WS_OVERLAPPEDWINDOW |WS_CLIPCHILDREN | WS_CLIPCHILDREN |WS_VISIBLE,
						100,
						100,
						WIN_WIDTH,
						WIN_HEIGHT,
						NULL,
						NULL,
						hInstance,
						NULL);
	ghwnd=hwnd;
	iret=initialize();
	//handling return values and create log
	if(iret == -1)
	{
		fwprintf(gpfile,TEXT("CHoice pixel format failed!!\n"));
		DestroyWindow(hwnd);
	}
	else if(iret == -2)
	{
		fwprintf(gpfile,TEXT("SetPixelFormat failed!! \n"));
		DestroyWindow(hwnd);
	}
	else if(iret == -3)
	{
		fwprintf(gpfile,TEXT("create context failed\n"));
		DestroyWindow(hwnd);
	}
	else if(iret == -4)
	{
		fwprintf(gpfile,TEXT("wgl make current failed!!\n"));
		DestroyWindow(hwnd);
	
	}
	else
	{
		fwprintf(gpfile,TEXT("Initialization Successful"));
		
	
	}

	ShowWindow(hwnd,iCmdShow);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);
	//call in game loop UpdateWindow(hwnd);

	while(bdone == false)
	{
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
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
			if(gbActiveWindow == true)
			{
				//here call update
				
			}
			display();

		}

	}
	return((int)msg.wParam);


}
LRESULT CALLBACK WndProc(HWND hwnd,UINT iMsg,WPARAM wParam,LPARAM lParam)
{
	//FUnction Declarations
	void resize(int ,int);
	
	void uninitialize(void);

	void toogle_screen(void);

	switch(iMsg)
	{	

		
	case WM_KEYDOWN:

		switch(wParam)
		{

			//key press
		case 0x31:			//1
		case VK_NUMPAD1:
			keyPressed= 1;
			
			break;

		case 0x32:
		case VK_NUMPAD2:
			keyPressed = 2;
			break;

		case 0x33:
		case VK_NUMPAD3:
			
			keyPressed = 3;
			break;

		case 0x34:
		case VK_NUMPAD4:

			keyPressed = 4;
			break;

		case 0x46:
		//	MessageBox(hwnd,TEXT("P BUTTON PRESSED!!"),TEXT("BUTTON P"),MB_OK);
			toogle_screen();
		
			break;
		case VK_ESCAPE:
			if(bFullScreen == true)				//We should exit from fullscreen and then destroy the window.
		{
			SetWindowLong(ghwnd,
						  GWL_STYLE,
						  dwstyle | 
						  WS_OVERLAPPEDWINDOW);
				
			SetWindowPlacement(ghwnd,&wpPrev);
	
			SetWindowPos(ghwnd,
						HWND_TOP,
						0,0,0,0,
						SWP_NOZORDER |
						SWP_FRAMECHANGED |
						SWP_NOMOVE | 
						SWP_NOSIZE|
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
			resize(LOWORD(lParam),HIWORD(lParam));
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
	return(DefWindowProc(hwnd,iMsg,wParam,lParam));
}
//User Defined Functions

void toogle_screen(void)
{
	//MONITORINFO mi;
	

	if(bFullScreen == false)
	{
		dwstyle=GetWindowLong(ghwnd, GWL_STYLE);

		if(dwstyle & WS_OVERLAPPEDWINDOW)
		{
			MONITORINFO	mi= {sizeof(MONITORINFO)};
			
			if(GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd,MONITORINFOF_PRIMARY),&mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE,dwstyle&~WS_OVERLAPPEDWINDOW);

				SetWindowPos(ghwnd, 
							 HWND_TOP,
							 mi.rcMonitor.left,
							 mi.rcMonitor.top,
							 mi.rcMonitor.right-mi.rcMonitor.left,
							 mi.rcMonitor.bottom-mi.rcMonitor.top,
							 SWP_NOZORDER | SWP_FRAMECHANGED );
					
			}	
		}
		ShowCursor(FALSE);
		bFullScreen=true;
	}
	else
	{
		SetWindowLong(ghwnd,
					  GWL_STYLE,
					  dwstyle | 
					  WS_OVERLAPPEDWINDOW);
			
		SetWindowPlacement(ghwnd,&wpPrev);

		SetWindowPos(ghwnd,
					HWND_TOP,
					0,0,0,0,
					SWP_NOZORDER |
					SWP_FRAMECHANGED |
					SWP_NOMOVE | 
					SWP_NOSIZE|
					SWP_NOOWNERZORDER);

		ShowCursor(TRUE);
		bFullScreen=false;
	}
}

int initialize(void)
{
	void resize(int,int);
	void uninitialize(void);
	BOOL loadTexture(GLuint *texture, TCHAR imageresourceId[]);
	//shader objects
	GLuint gVertexShaderObject;
	GLuint gFragmentShaderObject;

	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;
	GLenum result;
	//code
	ZeroMemory(&pfd,sizeof(PPIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL |PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 32;
	ghdc = GetDC(ghwnd);
	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);

	
	if(iPixelFormatIndex == 0)
	{
		return -1;
	}
	if(SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE)
	{
		return -2;
	}
	ghrc = wglCreateContext(ghdc);
	if(ghrc == NULL)
	{
		return -3;
	}
	if(wglMakeCurrent(ghdc, ghrc) == FALSE)
	{
		return -4;	
	}
	///programmable Pipeline changes..
	result = glewInit();
	if(result != GLEW_OK)
	{
		fwprintf(gpfile,TEXT("glewInit failed"));
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
		 "in vec2 vTexcoord;" \
		 "out vec2 out_texcoord;" \
		 "uniform mat4 u_mvp_matrix;" \
		 "void main(void)" \
		 "{" \
		 "gl_Position = u_mvp_matrix * vPosition;" \
		 "out_texcoord = vTexcoord;" \
		 " } " ;
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

	if(iShaderCompilationStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject,
						GL_INFO_LOG_LENGTH,
						&iInfoLength);
		if(iInfoLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLength);

			if(szInfoLog != NULL)
			{
				GLsizei writtened;

				glGetShaderInfoLog(gVertexShaderObject,
									iInfoLength,
									&writtened,
									szInfoLog);
				fwprintf(gpfile,TEXT("ERROR:VERTEX SHADER:%s"),szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(ghwnd);
				exit(0);
			}
		}
	}
	//********************FRAGMENT SHADER*****************
	gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	fwprintf(gpfile,TEXT("Before\n"));
	//write vertex shader code 
	// 
	GLchar * fragmentShaderSourceCode = 
		"#version 430 core" \
		"\n" \
		"in vec2  out_texcoord;" \
		"uniform sampler2D u_Sampler;" \
		"out vec4 fragColor;" \
		"void main(void)" \
		"{" \
		"fragColor = texture(u_Sampler, out_texcoord);" \
		"}" ;
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

	if(iShaderCompilationStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject,
						GL_INFO_LOG_LENGTH,
						&iInfoLength);
		if(iInfoLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLength);

			if(szInfoLog != NULL)
			{
				GLsizei writtened;

				glGetShaderInfoLog(gFragmentShaderObject,
									iInfoLength,
									&writtened,
									szInfoLog);
				fwprintf(gpfile,TEXT("ERROR:FRAGMENT SHADER:%s"),szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(ghwnd);
				exit(0);
			}
		}
	}
	fwprintf(gpfile,TEXT("After\n"));
	//fclose(gpfile);
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
						 AMC_ATTRIBUTES_TEXCOORD0,
							"vTexcoord");

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
		if(iShaderCompilationStatus == GL_FALSE)
		{
			glGetProgramiv(gShaderProgramObject,
							GL_INFO_LOG_LENGTH,
							&iInfoLength);
			if(iInfoLength > 0)
			{
				szInfoLog = (GLchar *)malloc(iInfoLength);

				if (szInfoLog != NULL)
				{
					GLsizei wr;

					glGetShaderInfoLog(gShaderProgramObject,
						iInfoLength,
						&wr,
						szInfoLog);

					fwprintf(gpfile,TEXT("ERROR:SHADER LINK:%s"),szInfoLog);
					free(szInfoLog);
					uninitialize();
					DestroyWindow(ghwnd);
					exit(0); 
				}
				
			}
		}
	///***POST LINKING GETTING UNIFORMS**
	mvpUniform = glGetUniformLocation(gShaderProgramObject,
										"u_mvp_matrix");

	sampler_Uniform = glGetUniformLocation(gShaderProgramObject,
											"u_Sampler");

	fwprintf(gpfile,TEXT("Post link success!!\n"));
	//fclose(gpfile);
	const GLfloat rectangleVertices[] =
									{
										1.0f, 1.0f, 0.0f,
										-1.0f, 1.0f, 0.0f,
										-1.0f, -1.0f, 0.0f,
										1.0f, -1.0f, 0.0f
									};


	/*const GLfloat smileyTextureVertices [] =
									{
										1.0f, 1.0f,
										0.0f, 1.0f,
										0.0f, 0.0f,
										1.0f, 0.0f
									};*/
	//create vao for rectangle...
	//START:
	glGenVertexArrays(1, &vao_Rectangle);
	
	glBindVertexArray(vao_Rectangle);

	glGenBuffers(1, &vbo_Rectangle);

	glBindBuffer(GL_ARRAY_BUFFER,
				 vbo_Rectangle);

	glBufferData(GL_ARRAY_BUFFER,
				sizeof(rectangleVertices),
				 rectangleVertices,
				 GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTES_POSITION,
							3,
							GL_FLOAT,
							GL_FALSE,
							0,
							NULL);

	glEnableVertexAttribArray(AMC_ATTRIBUTES_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//texture smiley
	glGenBuffers(1, &vbo_texture_smiley);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_texture_smiley);

	glBufferData(GL_ARRAY_BUFFER,
				8 * sizeof(GL_FLOAT),
				NULL,
				GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTES_TEXCOORD0,
							2,
							GL_FLOAT,
							GL_FALSE,
							0,
							NULL);

	glEnableVertexAttribArray(AMC_ATTRIBUTES_TEXCOORD0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
	//END:
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);
	fwprintf(gpfile,TEXT("Before LoadTexture \n"));
	
	loadTexture(&texture_smiley, MAKEINTRESOURCE(IDBITMAP_SMILEY));
	fwprintf(gpfile,TEXT("After loadtexture \n"));
	
	glEnable(GL_TEXTURE_2D);
	perspectiveProjectionMatrix = mat4::identity();
	
	resize(WIN_WIDTH, WIN_HEIGHT);
	
	return 0;
}

void resize(int width, int height)
{
	if(height == 0)
	{
		height = 1;
	}
	glViewport(0, 0, (GLsizei)width,(GLsizei)height);

	perspectiveProjectionMatrix =
		perspective(45.0f, (GLfloat)width/(GLfloat)height, 0.1f, 100.0f);
	
}

void display(void)
{
	//fwprintf(gpfile,TEXT("In Display\n"));
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(gShaderProgramObject);

	 mat4 modelViewMatrix;
	 mat4 modelViewProjectionMatrix;
	 GLfloat  smileyTextureVertices[8];

	 modelViewMatrix = mat4::identity();
	 modelViewProjectionMatrix = mat4::identity();

	 modelViewMatrix = translate(0.0f, 0.0f, -4.0f);

	 modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	 glUniformMatrix4fv(mvpUniform,
						1,
						GL_FALSE,
						modelViewProjectionMatrix);

	//binding texture array
	 if(keyPressed == 1)
	 {
		 smileyTextureVertices[0] = 1.0f;
		  smileyTextureVertices[1] = 1.0f;
		  smileyTextureVertices[2] = 0.0f;
		  smileyTextureVertices[3] = 1.0f;
		  smileyTextureVertices[4] = 0.0f;
		  smileyTextureVertices[5] = 0.0f;
		  smileyTextureVertices[6] = 1.0f;
		  smileyTextureVertices[7] = 0.0f;
	 }
	 else if(keyPressed == 2)
	 {
		  smileyTextureVertices[0] = 0.5f;
		  smileyTextureVertices[1] = 0.5f;
		  smileyTextureVertices[2] = 0.0f;
		  smileyTextureVertices[3] = 0.5f;
		  smileyTextureVertices[4] = 0.0f;
		  smileyTextureVertices[5] = 0.0f;
		  smileyTextureVertices[6] = 0.5f;
		  smileyTextureVertices[7] = 0.0f;
		 
	 }
	 else if(keyPressed == 3)
	 {
		  smileyTextureVertices[0] = 2.0f;
		  smileyTextureVertices[1] = 2.0f;
		  smileyTextureVertices[2] = 0.0f;
		  smileyTextureVertices[3] = 2.0f;
		  smileyTextureVertices[4] = 0.0f;
		  smileyTextureVertices[5] = 0.0f;
		  smileyTextureVertices[6] = 2.0f;
		  smileyTextureVertices[7] = 0.0f;
	 }
	 else
	 {
		  smileyTextureVertices[0] = 0.5f;
		  smileyTextureVertices[1] = 0.5f;
		  smileyTextureVertices[2] = 0.5f;
		  smileyTextureVertices[3] = 0.5f;
		  smileyTextureVertices[4] = 0.5f;
		  smileyTextureVertices[5] = 0.5f;
		  smileyTextureVertices[6] = 0.5f;
		  smileyTextureVertices[7] = 0.5f;
	 }
	 glBindBuffer(GL_ARRAY_BUFFER, vbo_texture_smiley);

		 glBufferData(GL_ARRAY_BUFFER,
					sizeof(smileyTextureVertices),
					smileyTextureVertices,
					GL_STATIC_DRAW);
	 //
	 glActiveTexture(GL_TEXTURE0);

	 glBindTexture(GL_TEXTURE_2D, texture_smiley);

	 glUniform1i(sampler_Uniform,
				0);

	 glBindVertexArray(vao_Rectangle);

	 glDrawArrays(GL_TRIANGLE_FAN,
					0,
					4);

	 glBindVertexArray(0);

	 glUseProgram(0);
	SwapBuffers(ghdc);
	//fwprintf(gpfile,TEXT("End OF display\n"));
}

void uninitialize(void)
{

	if(bFullScreen == true)
	{
		SetWindowLong(ghwnd,
					  GWL_STYLE,
					  dwstyle | 
					  WS_OVERLAPPEDWINDOW);
			
		SetWindowPlacement(ghwnd,&wpPrev);

		SetWindowPos(ghwnd,
					HWND_TOP,
					0,0,0,0,
					SWP_NOZORDER |
					SWP_FRAMECHANGED |
					SWP_NOMOVE | 
					SWP_NOSIZE|
					SWP_NOOWNERZORDER);

		ShowCursor(TRUE);
	}
	if(wglGetCurrentContext() == ghrc)
	{
		wglMakeCurrent(NULL, NULL);
	}

	if(ghrc)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
	}

	if(ghdc)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}
	
}

BOOL loadTexture(GLuint *texture, TCHAR imageresourceId[])
{
	//VAR DECLA
	HBITMAP hBitmap = NULL;
	BITMAP bmp;
	BOOL bStatus = FALSE;

	//CODE
	hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), imageresourceId,
		IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);

	if (hBitmap)
	{
		bStatus = TRUE;

		GetObject(hBitmap, sizeof(BITMAP), &bmp);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

		glGenTextures(1, texture);

		glBindTexture(GL_TEXTURE_2D, *texture);

		glTexParameteri(GL_TEXTURE_2D,
			GL_TEXTURE_MAG_FILTER,
			GL_LINEAR);

		glTexParameteri(GL_TEXTURE_2D,
			GL_TEXTURE_MIN_FILTER,
			GL_LINEAR_MIPMAP_LINEAR);

		
		glTexImage2D(GL_TEXTURE_2D,
						0,
						GL_RGB,
						bmp.bmWidth,
						bmp.bmHeight,
						0,
						GL_BGR,
						GL_UNSIGNED_BYTE,
						bmp.bmBits);
		
		glGenerateMipmap(GL_TEXTURE_2D);

		DeleteObject(hBitmap);
	}

	return bStatus;
}

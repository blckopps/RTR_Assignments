#include<Windows.h>
#include<stdio.h>
#include<math.h>
#include<gl/GL.h>
#include<gl/GLU.h>

#pragma comment(lib,"openGL32.lib")
#pragma comment(lib,"glu32.lib")

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

//angles
float angle_x = 0.0f;
float angle_z = 0.0f;
float angle_y = 0.0f;

bool  bLigtht = false;
int key_press = 0;

GLfloat LightAmbient [] = {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat LightDiffuse [] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat LightPosition [] = {0.0f, 0.0f, 0.0f, 1.0f};

//variables
GLfloat light_model_ambient[] = {0.2f, 0.2f, 0.2f, 1.0f};
GLfloat light_model_local_viewer[] = {0.0f};

GLUquadric *quadric[24];

FILE *gpfile = NULL;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//GLOBAL VARIABLES
bool bFullScreen = false;
DWORD dwstyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
HWND ghwnd = NULL;
bool gbActiveWindow = false;
HDC ghdc = NULL;
HGLRC ghrc = NULL;

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
		fprintf(gpfile, "log file created\n");
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
		TEXT("My APP-SHUBHAM"),
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
		fprintf(gpfile, "CHoice pixel format failed!!\n");
		DestroyWindow(hwnd);
	}
	else if (iret == -2)
	{
		fprintf(gpfile, "SetPixelFormat failed!! \n");
		DestroyWindow(hwnd);
	}
	else if (iret == -3)
	{
		fprintf(gpfile, "create context failed\n");
		DestroyWindow(hwnd);
	}
	else if (iret == -4)
	{
		fprintf(gpfile, "wgl make current failed!!\n");
		DestroyWindow(hwnd);

	}
	else
	{
		fprintf(gpfile, "Initialization Successfull!!\n");


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
				if (bLigtht == false)
				{
					bLigtht = true;
					glEnable(GL_LIGHTING);
				
				}
				else
				{
					bLigtht = false;
					glDisable(GL_LIGHTING);
				}
				break;

			case 'X':
			case 'x':
				key_press = 1;
				angle_x = 0.0f;
				break;

			case 'Y':
			case 'y':
				
				key_press = 2;
				angle_y = 0.0f;
				break;

			case 'Z':
			case 'z':
				
				key_press = 3;
				angle_z = 0.0f;
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

	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;

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
	glShadeModel(GL_SMOOTH);

	glEnable(GL_DEPTH_TEST);
	
	glDepthFunc(GL_LEQUAL);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);

	//configuration of light using config. parameters
	glLightfv(GL_LIGHT0,
				GL_AMBIENT,
				LightAmbient);

	glLightfv(GL_LIGHT0,
			GL_DIFFUSE,
			LightDiffuse);

	glLightfv(GL_LIGHT0,
			GL_POSITION,
			LightPosition);

	//light models
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT,light_model_ambient);

	glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER,light_model_local_viewer);
	glEnable(GL_LIGHT0);
	for(int i=0; i<24; i++)
	{
		quadric[i] = gluNewQuadric();
	}
	glClearColor(0.25f, 0.25f, 0.25f, 0.25f);
	glClearDepth(1.0f);

	resize(WIN_WIDTH, WIN_HEIGHT);

	return 0;
}

void resize(int width, int height)
{
	
	if(height == 0)
	{
		height=1;
	}

	glViewport(0, 0, (GLsizei)width,(GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (width<=height)
	{
		glOrtho(0.0f,
				(15.5f),
				0.0f,
				(15.5f *((GLfloat)height/(GLfloat)width)),
				-10.0f,
				10.0f);
				
	}
	else
	{
		glOrtho(0.0f,
				(15.5f *((GLfloat)height/(GLfloat)width)),
				0.0f,
				15.5f,
				-10.0f,
				10.0f);
	
	}
}

void display(void)
{
	void Draw24Spheres(void);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	if(key_press == 1)
	{
		glRotatef(angle_x, 1.0f, 0.0f, 0.0f);
		LightPosition[0] = angle_x;
		glLightfv(GL_LIGHT0,
			GL_POSITION,
				LightPosition);
		glEnable(GL_LIGHT0);
	}
	else if(key_press == 2)
	{
		glRotatef(angle_y, 0.0f, 1.0f, 0.0f);
		LightPosition[1] = angle_y;
		glLightfv(GL_LIGHT0,
			GL_POSITION,
				LightPosition);
		glEnable(GL_LIGHT0);

	}
	else if(key_press == 3)
	{
		glRotatef(angle_z, 0.0f, 0.0f, 1.0f);
		LightPosition[2] = angle_z;
		glLightfv(GL_LIGHT0,
			GL_POSITION,
				LightPosition);
		glEnable(GL_LIGHT0);
	}

	/*glLightfv(GL_LIGHT0,
			GL_POSITION,
			LightPosition);*/

	Draw24Spheres();

	SwapBuffers(ghdc);

}

void uninitialize(void)
{
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

void update()
{
	if (angle_x == 360.0f)
	{
		angle_x = 0.0f;
	}
	else
	{
		angle_x = angle_x + 0.2f;
	}
	if (angle_y == 360.0f)
	{
		angle_y = 0.0f;
	}
	else
	{
		angle_y = angle_y + 0.2f;
	}
	if(angle_z == 360.0f)
	{
		angle_z = 0.0f;
	}
	else
	{
		angle_z = angle_z + 0.2f;
	}
		
}

void Draw24Spheres(void)
{
	//START:common block
	GLfloat MatAmb[4];
	GLfloat MatDiff[4];
	GLfloat MatSpec[4];
	GLfloat MatShini[1];

	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	//END:common block
	//START:1st sphere on first coll
	MatAmb[0] = 0.0215f;
	MatAmb[1] = 0.1745f;
	MatAmb[2] = 0.0215f;
	MatAmb[3] = 1.0f;

	glLightfv(GL_LIGHT0,
			GL_AMBIENT,
			MatAmb);

	MatDiff[0]= 0.07568f;
	MatDiff[1]= 0.61424f;
	MatDiff[2]= 0.07568f;
	MatDiff[3]= 1.0f;

	glLightfv(GL_LIGHT0,
			GL_DIFFUSE,
			MatDiff);

	MatSpec[0]= 0.633f;
	MatSpec[1]= 0.727811f;
	MatSpec[2]= 0.633f;
	MatSpec[3]= 1.0f;

	glLightfv(GL_LIGHT0,
			GL_SPECULAR,
			MatSpec);

	MatShini[0]= 0.6f*128.0f;
	
	glLightfv(GL_LIGHT0,
			GL_SHININESS,
			MatShini);


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(1.0f, 14.0f, 0.0f);	//0 to 15 ortho

	gluSphere(quadric[0], 1.0f, 30, 30);
	//END:1st sphere
	//2nd sphere on first colmn
	
	MatAmb[0] = 0.135f;
	MatAmb[1] = 0.2225f;
	MatAmb[2] = 0.1575f;
	MatAmb[3] = 1.0f;

	glLightfv(GL_LIGHT0,
			GL_AMBIENT,
			MatAmb);

	MatDiff[0]= 0.54f;
	MatDiff[1]= 0.89f;
	MatDiff[2]= 0.63f;
	MatDiff[3]= 1.0f;

	glLightfv(GL_LIGHT0,
			GL_DIFFUSE,
			MatDiff);

	MatSpec[0]= 0.316228f;
	MatSpec[1]= 0.316228f;
	MatSpec[2]= 0.316228f;
	MatSpec[3]= 1.0f;

	glLightfv(GL_LIGHT0,
			GL_SPECULAR,
			MatSpec);

	MatShini[0] = 0.1f *128.0f;
	
	glLightfv(GL_LIGHT0,
			GL_SHININESS,
			MatShini);


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(1.0f, 11.5f, 0.0f);	

	gluSphere(quadric[1], 1.0f, 30, 30);
	//END:2nd sphere

	//3rd sphere on 1st coll
	
	MatAmb[0] = 0.05375f;
	MatAmb[1] = 0.05f;
	MatAmb[2] = 0.06625f;
	MatAmb[3] = 1.0f;

	glLightfv(GL_LIGHT0,
			GL_AMBIENT,
			MatAmb);

	MatDiff[0]= 0.18275f;
	MatDiff[1]= 0.17f;
	MatDiff[2]= 0.22525f;
	MatDiff[3]= 1.0f;

	glLightfv(GL_LIGHT0,
			GL_DIFFUSE,
			MatDiff);

	MatSpec[0]= 0.332741f;
	MatSpec[1]= 0.328634f;
	MatSpec[2]= 0.346435f;
	MatSpec[3]= 1.0f;

	glLightfv(GL_LIGHT0,
			GL_SPECULAR,
			MatSpec);

	MatShini[0]= 0.3f*128.0f;
	
	glLightfv(GL_LIGHT0,
			GL_SHININESS,
			MatShini);


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(1.0f, 9.0f, 0.0f);	

	gluSphere(quadric[2], 1.0f, 30, 30);
	//end

	//4th sphere on 1st coll
	
	MatAmb[0] = 0.25f;
	MatAmb[1] = 0.20725f;
	MatAmb[2] = 0.20725f;
	MatAmb[3] = 1.0f;

	glLightfv(GL_LIGHT0,
			GL_AMBIENT,
			MatAmb);

	MatDiff[0]= 1.0f;
	MatDiff[1]= 0.829f;
	MatDiff[2]= 0.829f;
	MatDiff[3]= 1.0f;

	glLightfv(GL_LIGHT0,
			GL_DIFFUSE,
			MatDiff);

	MatSpec[0]= 0.296648f;
	MatSpec[1]= 0.296648f;
	MatSpec[2]= 0.296648f;
	MatSpec[3]= 1.0f;

	glLightfv(GL_LIGHT0,
			GL_SPECULAR,
			MatSpec);

	MatShini[0] = 0.088f * 128.0f;
	
	glLightfv(GL_LIGHT0,
			GL_SHININESS,
			MatShini);


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(1.0f, 6.5f, 0.0f);	

	gluSphere(quadric[3], 1.0f, 30, 30);
	//end:

	//5th sphere on 1st coll
	
	MatAmb[0] = 0.1745f;
	MatAmb[1] = 0.01175f;
	MatAmb[2] = 0.01175f;
	MatAmb[3] = 1.0f;

	glLightfv(GL_LIGHT0,
			GL_AMBIENT,
			MatAmb);

	MatDiff[0]= 0.61424f;
	MatDiff[1]= 0.04136f;
	MatDiff[2]= 0.04136f;
	MatDiff[3]= 1.0f;

	glLightfv(GL_LIGHT0,
			GL_DIFFUSE,
			MatDiff);

	MatSpec[0]= 0.727811f;
	MatSpec[1]= 0.626959f;
	MatSpec[2]= 0.626959f;
	MatSpec[3]= 1.0f;

	glLightfv(GL_LIGHT0,
			GL_SPECULAR,
			MatSpec);

	MatShini[0]= 0.6f * 128.0f;
	
	glLightfv(GL_LIGHT0,
			GL_SHININESS,
			MatShini);


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(1.0f, 4.0f, 0.0f);	

	gluSphere(quadric[4], 1.0f, 30, 30);
	//end:

	//6th sphere on 1st coll
	
	MatAmb[0] = 0.1f;
	MatAmb[1] = 0.18725f;
	MatAmb[2] = 0.1745f;
	MatAmb[3] = 1.0f;

	glLightfv(GL_LIGHT0,
			GL_AMBIENT,
			MatAmb);

	MatDiff[0]= 0.396f;
	MatDiff[1]= 0.74151f;
	MatDiff[2]= 0.69102f;
	MatDiff[3]= 1.0f;

	glLightfv(GL_LIGHT0,
			GL_DIFFUSE,
			MatDiff);

	MatSpec[0]= 0.297254f;
	MatSpec[1]= 0.30829f;
	MatSpec[2]= 0.30829f;
	MatSpec[3]= 1.0f;

	glLightfv(GL_LIGHT0,
			GL_SPECULAR,
			MatSpec);

	MatShini[0]= 0.1f*128.0f;
	
	glLightfv(GL_LIGHT0,
			GL_SHININESS,
			MatShini);


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(1.0f, 1.5f, 0.0f);	

	gluSphere(quadric[5], 1.0f, 30, 30);
	//end:
					/////////////************2nd coll*******/////
	//1st sphere 2nd coll
	MatAmb[0] = 0.329412f;
	MatAmb[1] = 0.223529f;
	MatAmb[2] = 0.027451f;
	MatAmb[3] = 1.0f;

	glLightfv(GL_LIGHT0,
			GL_AMBIENT,
			MatAmb);

	MatDiff[0]= 0.780392f;
	MatDiff[1]= 0.568627f;
	MatDiff[2]= 0.113725f;
	MatDiff[3]= 1.0f;

	glLightfv(GL_LIGHT0,
			GL_DIFFUSE,
			MatDiff);

	MatSpec[0]= 0.992157f;
	MatSpec[1]= 0.941176f;
	MatSpec[2]= 0.807843f;
	MatSpec[3]= 1.0f;

	glLightfv(GL_LIGHT0,
			GL_SPECULAR,
			MatSpec);

	MatShini[0]= 0.21794872f*128.0f;
	
	glLightfv(GL_LIGHT0,
			GL_SHININESS,
			MatShini);


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(3.5f, 14.0f, 0.0f);	//0 to 15 ortho

	gluSphere(quadric[6], 1.0f, 30, 30);
	//END:1st sphere
	//2nd sphere on 2nd colmn
	
	MatAmb[0] = 0.2125f;
	MatAmb[1] = 0.1275f;
	MatAmb[2] = 0.054f;
	MatAmb[3] = 1.0f;

	glLightfv(GL_LIGHT0,
			GL_AMBIENT,
			MatAmb);

	MatDiff[0]= 0.714f;
	MatDiff[1]= 0.4284f;
	MatDiff[2]= 0.18144f;
	MatDiff[3]= 1.0f;

	glLightfv(GL_LIGHT0,
			GL_DIFFUSE,
			MatDiff);

	MatSpec[0]= 0.393548f;
	MatSpec[1]= 0.271906f;
	MatSpec[2]= 0.166721f;
	MatSpec[3]= 1.0f;

	glLightfv(GL_LIGHT0,
			GL_SPECULAR,
			MatSpec);

	MatShini[0]= 0.2f*128.0f;
	
	glLightfv(GL_LIGHT0,
			GL_SHININESS,
			MatShini);


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(3.5f, 11.5f, 0.0f);	

	gluSphere(quadric[7], 1.0f, 30, 30);
	//END:2nd sphere

	//3rd sphere on 2nd coll
	
	MatAmb[0] = 0.25f;
	MatAmb[1] = 0.25f;
	MatAmb[2] = 0.25f;
	MatAmb[3] = 1.0f;

	glLightfv(GL_LIGHT0,
			GL_AMBIENT,
			MatAmb);

	MatDiff[0]= 0.4f;
	MatDiff[1]= 0.4f;
	MatDiff[2]= 0.4f;
	MatDiff[3]= 1.0f;

	glLightfv(GL_LIGHT0,
			GL_DIFFUSE,
			MatDiff);

	MatSpec[0]= 0.774597f;
	MatSpec[1]= 0.774597f;
	MatSpec[2]= 0.774597f;
	MatSpec[3]= 1.0f;

	glLightfv(GL_LIGHT0,
			GL_SPECULAR,
			MatSpec);

	MatShini[0]= 0.6f*128.0f;
	
	glLightfv(GL_LIGHT0,
			GL_SHININESS,
			MatShini);


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(3.5f, 9.0f, 0.0f);	

	gluSphere(quadric[8], 1.0f, 30, 30);
	//end

	//4th sphere on 2nd coll
	
	MatAmb[0] = 0.19125f;
	MatAmb[1] = 0.0735f;
	MatAmb[2] = 0.0225f;
	MatAmb[3] = 1.0f;

	glLightfv(GL_LIGHT0,
			GL_AMBIENT,
			MatAmb);

	MatDiff[0]= 0.7038f;
	MatDiff[1]= 0.27048f;
	MatDiff[2]= 0.0828f;
	MatDiff[3]= 1.0f;

	glLightfv(GL_LIGHT0,
			GL_DIFFUSE,
			MatDiff);

	MatSpec[0]= 0.256777f;
	MatSpec[1]= 0.137622f;
	MatSpec[2]= 0.086014f;
	MatSpec[3]= 1.0f;

	glLightfv(GL_LIGHT0,
			GL_SPECULAR,
			MatSpec);

	MatShini[0]= 0.1f*128.0f;
	
	glLightfv(GL_LIGHT0,
			GL_SHININESS,
			MatShini);


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(3.5f, 6.5f, 0.0f);	

	gluSphere(quadric[9], 1.0f, 30, 30);
	//end:

	//5th sphere on 2nd coll
	
	MatAmb[0] = 0.24725f;
	MatAmb[1] = 0.1995f;
	MatAmb[2] = 0.0745f;
	MatAmb[3] = 1.0f;

	glLightfv(GL_LIGHT0,
			GL_AMBIENT,
			MatAmb);

	MatDiff[0]= 0.75164f;
	MatDiff[1]= 0.60648f;
	MatDiff[2]= 0.22648f;
	MatDiff[3]= 1.0f;

	glLightfv(GL_LIGHT0,
			GL_DIFFUSE,
			MatDiff);

	MatSpec[0]= 0.628281f;
	MatSpec[1]= 0.555802f;
	MatSpec[2]= 0.366065f;
	MatSpec[3]= 1.0f;

	glLightfv(GL_LIGHT0,
			GL_SPECULAR,
			MatSpec);

	MatShini[0]= 0.4f*128.0f;
	
	glLightfv(GL_LIGHT0,
			GL_SHININESS,
			MatShini);


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(3.5f, 4.0f, 0.0f);	

	gluSphere(quadric[10], 1.0f, 30, 30);
	//end:

	//6th sphere on 2nd coll
	
	MatAmb[0] = 0.19225f;
	MatAmb[1] = 0.19225f;
	MatAmb[2] = 0.19225f;
	MatAmb[3] = 1.0f;

	glLightfv(GL_LIGHT0,
			GL_AMBIENT,
			MatAmb);

	MatDiff[0]= 0.50754f;
	MatDiff[1]= 0.50754f;
	MatDiff[2]= 0.50754f;
	MatDiff[3]= 1.0f;

	glLightfv(GL_LIGHT0,
			GL_DIFFUSE,
			MatDiff);

	MatSpec[0]= 0.508273f;
	MatSpec[1]= 0.508273f;
	MatSpec[2]= 0.508273f;
	MatSpec[3]= 1.0f;

	glLightfv(GL_LIGHT0,
			GL_SPECULAR,
			MatSpec);

	MatShini[0]= 0.4f*128.0f;
	
	glLightfv(GL_LIGHT0,
			GL_SHININESS,
			MatShini);


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(3.5f, 1.5f, 0.0f);	

	gluSphere(quadric[11], 1.0f, 30, 30);
	//end:
				/////***********Start of 3rd coll*******/////////
	//1st sphere on 3rd coll
	MatAmb[0] = 0.0f;
	MatAmb[1] = 0.0f;
	MatAmb[2] = 0.0f;
	MatAmb[3] = 1.0f;

	glLightfv(GL_LIGHT0,
			GL_AMBIENT,
			MatAmb);

	MatDiff[0]= 0.01f;
	MatDiff[1]= 0.01f;
	MatDiff[2]= 0.01f;
	MatDiff[3]= 1.0f;

	glLightfv(GL_LIGHT0,
			GL_DIFFUSE,
			MatDiff);

	MatSpec[0]= 0.50f;
	MatSpec[1]= 0.50f;
	MatSpec[2]= 0.50f;
	MatSpec[3]= 1.0f;

	glLightfv(GL_LIGHT0,
			GL_SPECULAR,
			MatSpec);

	MatShini[0]= 0.25f*128.0f;
	
	glLightfv(GL_LIGHT0,
			GL_SHININESS,
			MatShini);


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(6.0f, 14.0f, 0.0f);	//0 to 15 ortho

	gluSphere(quadric[12], 1.0f, 30, 30);
	//END:1st sphere
	//2nd sphere on 3rd colmn
	
	MatAmb[0] = 0.0f;
	MatAmb[1] = 0.1f;
	MatAmb[2] = 0.06f;
	MatAmb[3] = 1.0f;

	glLightfv(GL_LIGHT0,
			GL_AMBIENT,
			MatAmb);

	MatDiff[0]= 0.0f;
	MatDiff[1]= 0.50980392f;
	MatDiff[2]= 0.50980392f;
	MatDiff[3]= 1.0f;

	glLightfv(GL_LIGHT0,
			GL_DIFFUSE,
			MatDiff);

	MatSpec[0]= 0.50196078f;
	MatSpec[1]= 0.50196078f;
	MatSpec[2]= 0.50196078f;
	MatSpec[3]= 1.0f;

	glLightfv(GL_LIGHT0,
			GL_SPECULAR,
			MatSpec);

	MatShini[0]= 0.25f*128.0f;
	
	glLightfv(GL_LIGHT0,
			GL_SHININESS,
			MatShini);


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(6.0f, 11.5f, 0.0f);	

	gluSphere(quadric[13], 1.0f, 30, 30);
	//END:2nd sphere

	//3rd sphere on 3rd coll
	
	MatAmb[0] = 0.0f;
	MatAmb[1] = 0.0f;
	MatAmb[2] = 0.0f;
	MatAmb[3] = 1.0f;

	glLightfv(GL_LIGHT0,
			GL_AMBIENT,
			MatAmb);

	MatDiff[0]= 0.1f;
	MatDiff[1]= 0.35f;
	MatDiff[2]= 0.1f;
	MatDiff[3]= 1.0f;

	glLightfv(GL_LIGHT0,
			GL_DIFFUSE,
			MatDiff);

	MatSpec[0]= 0.45f;
	MatSpec[1]= 0.55f;
	MatSpec[2]= 0.45f;
	MatSpec[3]= 1.0f;

	glLightfv(GL_LIGHT0,
			GL_SPECULAR,
			MatSpec);

	MatShini[0]= 0.25f*128.0f;
	

	glLightfv(GL_LIGHT0,
			GL_SHININESS,
			MatShini);


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(6.0f, 9.0f, 0.0f);	

	gluSphere(quadric[14], 1.0f, 30, 30);
	//end

	//4th sphere on 3rd coll
	
	MatAmb[0] = 0.0f;
	MatAmb[1] = 0.0f;
	MatAmb[2] = 0.0f;
	MatAmb[3] = 1.0f;

	glLightfv(GL_LIGHT0,
			GL_AMBIENT,
			MatAmb);

	MatDiff[0]= 0.5f;
	MatDiff[1]= 0.0f;
	MatDiff[2]= 0.0f;
	MatDiff[3]= 1.0f;

	glLightfv(GL_LIGHT0,
			GL_DIFFUSE,
			MatDiff);

	MatSpec[0]= 0.7f;
	MatSpec[1]= 0.6f;
	MatSpec[2]= 0.6f;
	MatSpec[3]= 1.0f;

	glLightfv(GL_LIGHT0,
			GL_SPECULAR,
			MatSpec);

	MatShini[0]= 0.25f*128.0f;

	glLightfv(GL_LIGHT0,
			GL_SHININESS,
			MatShini);


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(6.0f, 6.5f, 0.0f);	

	gluSphere(quadric[15], 1.0f, 30, 30);
	//end:

	//5th sphere on 3rd coll
	
	MatAmb[0] = 0.0f;
	MatAmb[1] = 0.0f;
	MatAmb[2] = 0.0f;
	MatAmb[3] = 1.0f;

	glLightfv(GL_LIGHT0,
			GL_AMBIENT,
			MatAmb);

	MatDiff[0]= 0.55f;
	MatDiff[1]= 0.55f;
	MatDiff[2]= 0.55f;
	MatDiff[3]= 1.0f;

	glLightfv(GL_LIGHT0,
			GL_DIFFUSE,
			MatDiff);

	MatSpec[0]= 0.70f;
	MatSpec[1]= 0.70f;
	MatSpec[2]= 0.70f;
	MatSpec[3]= 1.0f;

	glLightfv(GL_LIGHT0,
			GL_SPECULAR,
			MatSpec);

	MatShini[0]= 0.25f*128.0f;

	glLightfv(GL_LIGHT0,
			GL_SHININESS,
			MatShini);


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(6.0f, 4.0f, 0.0f);	

	gluSphere(quadric[16], 1.0f, 30, 30);
	//end:

	//6th sphere on 3rd coll
	
	MatAmb[0] = 0.0f;
	MatAmb[1] = 0.0f;
	MatAmb[2] = 0.0f;
	MatAmb[3] = 1.0f;

	glLightfv(GL_LIGHT0,
			GL_AMBIENT,
			MatAmb);

	MatDiff[0]= 0.5f;
	MatDiff[1]= 0.5f;
	MatDiff[2]= 0.0f;
	MatDiff[3]= 1.0f;

	glLightfv(GL_LIGHT0,
			GL_DIFFUSE,
			MatDiff);

	MatSpec[0]= 0.60f;
	MatSpec[1]= 0.60f;
	MatSpec[2]= 0.50f;
	MatSpec[3]= 1.0f;

	glLightfv(GL_LIGHT0,
			GL_SPECULAR,
			MatSpec);

	MatShini[0]= 0.25f*128.0f;

	glLightfv(GL_LIGHT0,
			GL_SHININESS,
			MatShini);


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(6.0f, 1.5f, 0.0f);	

	gluSphere(quadric[17], 1.0f, 30, 30);
	//end:

					//////*******4th coll start******///////////////
	//1st sphere on 4th coll
	MatAmb[0] = 0.02f;
	MatAmb[1] = 0.02f;
	MatAmb[2] = 0.02f;
	MatAmb[3] = 1.0f;

	glLightfv(GL_LIGHT0,
			GL_AMBIENT,
			MatAmb);

	MatDiff[0]= 0.01f;
	MatDiff[1]=  0.01f;
	MatDiff[2]=  0.01f;
	MatDiff[3]= 1.0f;

	glLightfv(GL_LIGHT0,
			GL_DIFFUSE,
			MatDiff);

	MatSpec[0]= 0.4f;
	MatSpec[1]= 0.4f;
	MatSpec[2]= 0.4f;
	MatSpec[3]= 1.0f;

	glLightfv(GL_LIGHT0,
			GL_SPECULAR,
			MatSpec);

	MatShini[0]= 0.078125f*128.0f;

	glLightfv(GL_LIGHT0,
			GL_SHININESS,
			MatShini);


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(8.5f, 14.0f, 0.0f);	//0 to 15 ortho

	gluSphere(quadric[18], 1.0f, 30, 30);
	//END:1st sphere
	//2nd sphere on 4th colmn
	
	MatAmb[0] = 0.0f;
	MatAmb[1] = 0.05f;
	MatAmb[2] = 0.05f;
	MatAmb[3] = 1.0f;

	glLightfv(GL_LIGHT0,
			GL_AMBIENT,
			MatAmb);

	MatDiff[0]= 0.4f;
	MatDiff[1]= 0.5f;
	MatDiff[2]= 0.5f;
	MatDiff[3]= 1.0f;

	glLightfv(GL_LIGHT0,
			GL_DIFFUSE,
			MatDiff);

	MatSpec[0]= 0.04f;
	MatSpec[1]= 0.7f;
	MatSpec[2]= 0.7f;
	MatSpec[3]= 1.0f;

	glLightfv(GL_LIGHT0,
			GL_SPECULAR,
			MatSpec);

	MatShini[0]= 0.078125f*128.0f;

	glLightfv(GL_LIGHT0,
			GL_SHININESS,
			MatShini);


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(8.5f, 11.5f, 0.0f);	

	gluSphere(quadric[19], 1.0f, 30, 30);
	//END:2nd sphere

	//3rd sphere on 4th coll
	
	MatAmb[0] = 0.0f;
	MatAmb[1] = 0.05f;
	MatAmb[2] = 0.0f;
	MatAmb[3] = 1.0f;

	glLightfv(GL_LIGHT0,
			GL_AMBIENT,
			MatAmb);

	MatDiff[0]= 0.4f;
	MatDiff[1]= 0.5f;
	MatDiff[2]= 0.4f;
	MatDiff[3]= 1.0f;

	glLightfv(GL_LIGHT0,
			GL_DIFFUSE,
			MatDiff);

	MatSpec[0]= 0.04f;
	MatSpec[1]= 0.7f;
	MatSpec[2]= 0.04f;
	MatSpec[3]= 1.0f;

	glLightfv(GL_LIGHT0,
			GL_SPECULAR,
			MatSpec);

	MatShini[0]= 0.078125f*128.0f;

	glLightfv(GL_LIGHT0,
			GL_SHININESS,
			MatShini);


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(8.5f, 9.0f, 0.0f);	

	gluSphere(quadric[20], 1.0f, 30, 30);
	//end

	//4th sphere on 4th coll
	
	MatAmb[0] = 0.05f;
	MatAmb[1] = 0.0f;
	MatAmb[2] = 0.0f;
	MatAmb[3] = 1.0f;

	glLightfv(GL_LIGHT0,
			GL_AMBIENT,
			MatAmb);

	MatDiff[0]= 0.5f;
	MatDiff[1]= 0.4f;
	MatDiff[2]= 0.4f;
	MatDiff[3]= 1.0f;

	glLightfv(GL_LIGHT0,
			GL_DIFFUSE,
			MatDiff);

	MatSpec[0]= 0.7f;
	MatSpec[1]= 0.04f;
	MatSpec[2]= 0.04f;
	MatSpec[3]= 1.0f;

	glLightfv(GL_LIGHT0,
			GL_SPECULAR,
			MatSpec);

	MatShini[0]= 0.078125f*128.0f;

	glLightfv(GL_LIGHT0,
			GL_SHININESS,
			MatShini);


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(8.5f, 6.5f, 0.0f);	

	gluSphere(quadric[21], 1.0f, 30, 30);
	//end:

	//5th sphere on 4th coll
	
	MatAmb[0] = 0.5f;
	MatAmb[1] = 0.5f;
	MatAmb[2] = 0.5f;
	MatAmb[3] = 1.0f;

	glLightfv(GL_LIGHT0,
			GL_AMBIENT,
			MatAmb);

	MatDiff[0]= 0.5f;
	MatDiff[1]= 0.5f;
	MatDiff[2]= 0.5f;
	MatDiff[3]= 1.0f;

	glLightfv(GL_LIGHT0,
			GL_DIFFUSE,
			MatDiff);

	MatSpec[0]= 0.7f;
	MatSpec[1]= 0.7f;
	MatSpec[2]= 0.7f;
	MatSpec[3]= 1.0f;

	glLightfv(GL_LIGHT0,
			GL_SPECULAR,
			MatSpec);

	MatShini[0]= 0.078125f*128.0f;

	glLightfv(GL_LIGHT0,
			GL_SHININESS,
			MatShini);


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(8.5f, 4.0f, 0.0f);	

	gluSphere(quadric[22], 1.0f, 30, 30);
	//end:

	//6th sphere on 4th coll
	
	MatAmb[0] = 0.05f;
	MatAmb[1] = 0.05f;
	MatAmb[2] = 0.0f;
	MatAmb[3] = 1.0f;

	glLightfv(GL_LIGHT0,
			GL_AMBIENT,
			MatAmb);

	MatDiff[0]= 0.5f;
	MatDiff[1]= 0.5f;
	MatDiff[2]= 0.4f;
	MatDiff[3]= 1.0f;

	glLightfv(GL_LIGHT0,
			GL_DIFFUSE,
			MatDiff);

	MatSpec[0]= 0.7f;
	MatSpec[1]= 0.7f;
	MatSpec[2]= 0.7f;
	MatSpec[3]= 1.0f;

	glLightfv(GL_LIGHT0,
			GL_SPECULAR,
			MatSpec);

	MatShini[0]= 0.078125f*128.0f;

	glLightfv(GL_LIGHT0,
			GL_SHININESS,
			MatShini);


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(8.5f, 1.5f, 0.0f);	

	gluSphere(quadric[23], 1.0f, 30, 30);
	//end:
	///
}
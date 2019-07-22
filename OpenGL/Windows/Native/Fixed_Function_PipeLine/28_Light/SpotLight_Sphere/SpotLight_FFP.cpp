#include<Windows.h>
#include<stdio.h>
#include<math.h>
#include<gl/GL.h>
#include<gl/GLU.h>

#pragma comment(lib,"openGL32.lib")
#pragma comment(lib,"glu32.lib")

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

GLUquadric *quadric = NULL;

float angle = 0.0f;
bool  bLigtht = false;

GLfloat LightAmbient [] = {0.2f, 0.2f, 0.2f, 1.0f};
GLfloat LightDiffuse [] = { 1.0f, 1.0f, 1.0f, 1.0f };

GLfloat LightPosition [] = { 1.0f, 1.0f, 0.0f, 1.0f };
GLfloat LightSpeculaar [] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat spot_direction [] = {-1.0, -1.0, 0.0f};

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
		TEXT("My APP_SPOTLIGHT-SHUBHAM"),
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

	glLightfv(GL_LIGHT0,
			 GL_SPECULAR,
			 LightSpeculaar);
	//spot
	glLightf(GL_LIGHT0,
			GL_CONSTANT_ATTENUATION,
			1.5f);

	glLightf(GL_LIGHT0,
			GL_LINEAR_ATTENUATION,
			0.5f);

	glLightf(GL_LIGHT0,
			GL_QUADRATIC_ATTENUATION,
			0.2f);

	glLightfv(GL_LIGHT0,
			GL_SPOT_DIRECTION,
			spot_direction);

	glLightf(GL_LIGHT0,
			GL_SPOT_CUTOFF,
			40.0f);

	/*glLightf(GL_LIGHT0,
			GL_SPOT_EXPONENT,
			3.0f);*/

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);
	glEnable(GL_LIGHT0);
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
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f,
		(GLfloat)width / (GLfloat)height,
		0.1f,
		100.0f);
}

void display(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -1.0f);
	glRotatef(angle, 1.0f, 1.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	quadric = gluNewQuadric();

	gluSphere(quadric, 0.2f, 30, 30);
	
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
	if (angle == 360)
	{
		angle = 0.0f;
	}
	angle = angle + 0.02f;
}
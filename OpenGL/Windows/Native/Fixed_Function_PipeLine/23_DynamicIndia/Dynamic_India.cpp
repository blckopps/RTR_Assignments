#include<Windows.h>
#include<stdio.h>
#include<math.h>
#include<gl/GL.h>
#include<gl/GLU.h>
#include"Sound.h"

#pragma comment(lib,"openGL32.lib")
#pragma comment(lib,"glu32.lib")

#define WIN_WIDTH 800
#define WIN_HEIGHT 600
#define num_points 1000

float Ix=-1.0f;
float Ax=2.0f;
float Ny=2.0f;
float I2y=-1.3f;
//colors
float Sg=0.0f;
float Sb=0.0f;
float Sr=0.0f;

float gr=0.0f;
float gg=0.0f;
float gb=0.0f;

int flag=0;
int flagN=0;
int flagD=0;
int flagi2=0;
int flagP=0;
int flagOfA=0;
//plane
float p1_x=-3.0f;
float p1_y=3.0f;
float angle=270.0f;
float angle2=0.0f;
FILE *gpfile = NULL;

LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);

//GLOBAL VARIABLES
bool bFullScreen=false;
DWORD dwstyle;
WINDOWPLACEMENT wpPrev={sizeof(WINDOWPLACEMENT)};
HWND ghwnd=NULL;
bool gbActiveWindow=false;
HDC ghdc=NULL;
HGLRC ghrc=NULL;
HINSTANCE hInst=NULL;


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
		fprintf(gpfile, "log file created\n");
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
						TEXT("My APP-SHUBHAM"),
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
	hInst=hInstance;
	iret=initialize();
	//handling return values and create log
	if(iret == -1)
	{
		fprintf(gpfile,"CHoice pixel format failed!!\n");
		DestroyWindow(hwnd);
	}
	else if(iret == -2)
	{
		fprintf(gpfile,"SetPixelFormat failed!! \n");
		DestroyWindow(hwnd);
	}
	else if(iret == -3)
	{
		fprintf(gpfile,"create context failed\n");
		DestroyWindow(hwnd);
	}
	else if(iret == -4)
	{
		fprintf(gpfile,"wgl make current failed!!\n");
		DestroyWindow(hwnd);
	
	}
	else
	{
		fprintf(gpfile,"Initialization Successfull!!\n");
		
	
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

	case WM_CREATE:
		PlaySound(MAKEINTRESOURCE(soundplay),hInst,SND_ASYNC|SND_FILENAME |SND_RESOURCE);
		break;
	case WM_KEYDOWN:

		switch(wParam)
		{
		//case 0x46:
		////	MessageBox(hwnd,TEXT("P BUTTON PRESSED!!"),TEXT("BUTTON P"),MB_OK);
		//	toogle_screen();
		//
		//	break;
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
		//MessageBox(hwnd,TEXT("This is WM_DESTROY!!"),TEXT("In Wm_DESTROY"),MB_OK);
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

	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;

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

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	
	if(PlaySound(MAKEINTRESOURCE(soundplay),hInst,SND_ASYNC|SND_FILENAME |SND_RESOURCE)==false)
	{
		fprintf(gpfile,"PlaySound UNsccessfull!!\n");
	}
	//resize(WIN_WIDTH, WIN_HEIGHT);
	toogle_screen();
	return 0;
}

void resize(int width, int height)
{
	if(height == 0)
	{
		height = 1;
	}
	glViewport(0, 0, (GLsizei)width,(GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f,
		(GLfloat)width/(GLfloat)height,	
					0.1f,
					100.0f);
}

void display(void)
{
	
	void Box();
	
	void I();
	void N();
	void D();
	void I2();
	void A();
	void Aflag();
	void plane();
	void plane_upper();
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//glTranslatef(0.0f,0.0f,-2.0f);
	//Box();
	PlaySound(MAKEINTRESOURCE("soundplay"),NULL,SND_ASYNC | SND_RESOURCE);
	//PlaySound((LPCWSTR)"KarChaleHumFinal.wav",NULL,SND_ASYNC | SND_RESOURCE);
	I();
	if(flagN == 2)
	{
		//fprintf(gpfile, "N() created\n");
		N();
	}
	if(flagD == 4)
	{
		D();
	}
	if(flagi2 == 3)
	{
		I2();
	}
	if(flag==1)
	{
		//Aflag();
		if(flagOfA == 6)
		{
			Aflag();
		}
		A();
	}

	if(flagP==5)
	{
		
		glLoadIdentity();
		glTranslatef(p1_x,0.0f,-4.5f);
		
		plane();
		
		glLoadIdentity();
		glTranslatef(p1_x,p1_y,-4.5f);
		glRotatef(angle,0.0f,0.0f,1.0f);
		plane_upper();
		glLoadIdentity();
		glTranslatef(p1_x,-p1_y,-4.5f);
		glRotatef(-angle,0.0f,0.0f,1.0f);
		plane_upper();
	}
	
	SwapBuffers(ghdc);
	////translation
	if(Ix<=0 )
	{
		Ix=Ix+0.0001f;
	}
	if(Ax>=1.1f && flag==1)
	{
		Ax=Ax-0.0001f;
	}
	if(Ny>=0.0f && flagN==2)
	{
		Ny=Ny-0.00015f;
	}
	if(I2y<=0 && flagi2==3)
	{
		I2y=I2y+0.00015f;
	}
	if(flagD == 4)
	{
		if(Sr>=1.0f && Sg>=0.6f && Sb>=0.2f)
		{
			flagP=5;
		}
			if(Sr<=1.0f)
			{
				Sr=Sr+0.0005f;
			}
			
			if(Sg<=0.6f)
			{
				Sg=Sg+0.0001f;
			}
			if(Sb<=0.2f)
			{
				Sb=Sb+0.0001f;
			}
			if(gr<=0.07f)
			{
				//0.07f,0.533f,0.02f
				gr=gr+0.0001f;
			}
			if(gg<=0.533f)
			{
				gg=gg+0.0004f;
			}
			if(gb<=0.02f)
			{
				gb=gb+0.0001f;
			}

	}
	//plane translation
	if(flagP ==5 )
	{
		if(p1_x<=0.0f)
		{
			p1_x=p1_x+0.0005f;
			p1_y=p1_y-0.0005f;
			angle=angle+0.015f;
		}
		if(p1_x>0.0f && p1_x<2.5f)
		{
			p1_x=p1_x+0.0007f;
			//p1_y=0.0f;
		}
		if(p1_x>=2.5f)
		{
			p1_y=p1_y-0.0005f;
				p1_x=p1_x+0.0005f;
				flagOfA=6;
				angle=angle-0.02f;
		}
		
		
	}
	
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
void Box()
{
	glBegin(GL_LINES);
	glVertex2f(0.75f,0.5f);
	glVertex2f(-0.75f,0.5f);

	glVertex2f(-0.75f,0.5f);
	glVertex2f(-0.75f,-0.5f);
	
	glVertex2f(-0.75f,-0.5f);
	glVertex2f(0.75f,-0.5f);

	glVertex2f(0.75f,-0.5f);
	glVertex2f(0.75f,0.5f);
	glEnd();
}
void I()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(Ix, 0.0f, -2.0f);
	glLineWidth(5.0f);
	if(Ix >= 0)
	{
		flag=1;
	}
	glBegin(GL_LINES);
	glColor3f(1.0f,0.6f,0.2f);//saffron
	
	glVertex2f(-0.79f,0.5f);//   -
	glVertex2f(-0.70f,0.5f);
		
	glVertex2f(-0.745f,0.5f);//  |
	glColor3f(0.07f,0.533f,0.02f);//green
	glVertex2f(-0.745f,-0.5f);

	glVertex2f(-0.79f,-0.5f);//  _
	glVertex2f(-0.70f,-0.5f);
	glEnd();
}
void N()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.3f, Ny, -2.0f);
	glLineWidth(30.0f);
	if(Ny <= 0.0f)
	{
		flagi2=3;
	}
	glBegin(GL_LINES);
	
	glColor3f(1.0f,0.6f,0.2f);//saffron
	glVertex2f(-0.65f,0.51f);//			|
	glColor3f(0.07f,0.533f,0.02f);	//green
	glVertex2f(-0.65f,-0.51f);
		
	glColor3f(1.0f,0.6f,0.2f);
	glVertex2f(-0.65f,0.51f);//		/
	glColor3f(0.07f,0.533f,0.02f);
	glVertex2f(-0.55f,-0.51f);

	glColor3f(1.0f,0.6f,0.2f);
	glVertex2f(-0.55f,0.51f);
	glColor3f(0.07f,0.533f,0.02f);
	glVertex2f(-0.55f,-0.51f);//		|
	glEnd();

}
void D()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.6f, 0.0f, -2.0f);
	glLineWidth(30.0f);
	
	glBegin(GL_LINES);
	
	glColor3f(Sr,Sg,Sb);//saffron
	glVertex2f(-0.45f,0.51f);//		Straight line of D
	glColor3f(gr,gg,gb);		//green
	glVertex2f(-0.45f,-0.51f);
		
	glColor3f(Sr,Sg,Sb);
	glVertex2f(-0.50f,0.51f);//upper top of D
	glVertex2f(-0.38f,0.51f);

	glColor3f(gr,gg,gb);
	glVertex2f(-0.50f,-0.51f);//lower base of D
	glVertex2f(-0.38f,-0.51f);

	glColor3f(Sr,Sg,Sb);
	glVertex2f(-0.39f,0.51f);//		Curve line of D
	glColor3f(gr,gg,gb);
	glVertex2f(-0.39f,-0.51f);

	glEnd();

}
void I2()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.9f, I2y, -2.0f);
	glLineWidth(100);
	if(I2y >= 0)
	{
		flagD=4;
	}

	glBegin(GL_LINES);
	glColor3f(1.0f,0.6f,0.2f);//saffron

	glVertex2f(-0.34f,0.5f);//   -
	glVertex2f(-0.26f,0.5f);
		
	glVertex2f(-0.30f,0.5f);//  |
	glColor3f(0.07f,0.533f,0.02f);//green
	glVertex2f(-0.30f,-0.5f);

	glVertex2f(-0.34f,-0.5f);//  _
	glVertex2f(-0.26f,-0.5f);
	glEnd();
}

void A()
{
	void Aflag();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(Ax, 0.0f, -2.0f);
	glLineWidth(30.0f);
	if(Ax <= 1.1f)
	{
		flagN=2;
	}
	glBegin(GL_LINES);
	
	glColor3f(1.0f,0.6f,0.2f);
	glVertex2f(-0.13f,0.51f);//		/
	glColor3f(0.07f,0.533f,0.02f);
	glVertex2f(-0.19f,-0.51f);
	
	glColor3f(1.0f,0.6f,0.2f);
	glVertex2f(-0.13f,0.51f);
	glColor3f(0.07f,0.533f,0.02f);
	glVertex2f(-0.07f,-0.51f);//	

	
	glEnd();
}
void Aflag()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(Ax, 0.0f, -2.0f);
	glLineWidth(2.0f);
	
	glBegin(GL_LINES);
		glColor3f(0.07f,0.533f,0.02f);
		glVertex2f(-0.16f,0.01f);
		glVertex2f(-0.10f,0.01f);
		//white
		glColor3f(1.0f,1.0f,1.0f);
		glVertex2f(-0.16f,0.014f);
		glVertex2f(-0.10f,0.014f);

		glColor3f(1.0f,0.6f,0.02f);
		glVertex2f(-0.16f,0.016f);
		glVertex2f(-0.10f,0.016f);

		glEnd();	
	
}
void plane()
{
	glBegin(GL_POLYGON);
	//glColor3f(176.0f/255.0f, 224.0f / 255.0f, 230.0f / 255.0f);//powder blue
	glColor3f(70.0f / 255.0f, 130.0f / 255.0f, 180.0f / 255.0f);//steel blue
	glVertex2f(0.0f, 0.1f);
	glVertex2f(-0.05f, 0.05f);
	glVertex2f(-0.05f, -0.05f);
	glVertex2f(0.0f, -0.1f);
	glVertex2f(-0.05f, -0.2f);
	glVertex2f(0.0f, -0.25f);
	glVertex2f(0.2f, -0.1f);
	glVertex2f(0.4f, -0.05f);
	glVertex2f(0.45f, 0.0f);
	glVertex2f(0.4f, 0.05f);
	glVertex2f(0.2f, 0.1f);
	glVertex2f(0.0f, 0.25f);
	glVertex2f(-0.05f, 0.2f);

	glEnd();
	//Flag
	glBegin(GL_QUAD_STRIP);
	glColor3f(0.071f, 0.533f, 0.027f);//green
	glVertex2f(-0.2f, -0.05f);
	glVertex2f(-0.05f, -0.05f);
	glColor3f(1.0f, 1.0f, 1.0f);//white
	glVertex2f(-0.2f, 0.0f);
	glVertex2f(-0.05f, 0.0f);
	glColor3f(1.0f, 0.6f, 0.2f);//saffron
	glVertex2f(-0.2f, 0.05f);
	glVertex2f(-0.05f, 0.05f);
	glEnd();
	

	
	//F
	glTranslatef(0.3f, 0.0f, 0.0f);
	glScalef(0.1f, 0.05f, 0.0f);
	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.6f, 0.2f);//saffron
	glVertex2f(-0.0f, 0.75f);
	glVertex2f(-0.60f, 0.75f);
	glColor3f(0.071f, 0.533f, 0.027f);//green
	glVertex2f(-0.60f, -0.75f);
	glVertex2f(0.0f, -0.75f);

	glColor3f(70.0f / 255.0f, 130.0f / 255.0f, 180.0f / 255.0f);//steel blue

	glVertex2f(0.0f, 0.60f);
	glVertex2f(-0.45f, 0.60f);
	glVertex2f(-0.45f, 0.1f);
	glVertex2f(0.0f, 0.1f);

	glVertex2f(0.0f, -0.05f);
	glVertex2f(-0.45f, -0.05f);
	glVertex2f(-0.45f, -0.75f);
	glVertex2f(0.0f, -0.75f);

	glVertex2f(0.0f, 0.15f);
	glVertex2f(-0.2f, 0.15f);
	glVertex2f(-0.2f, -0.75f);
	glVertex2f(0.0f, -0.75f);

	glEnd();
	//glTranslatef(-0.3f, 0.0f, 0.0f);
	//I
	glTranslatef(-0.1f,0.0f,0.0f);
	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.6f, 0.2f);//saffron
	glVertex2f(-1.75f, 0.75f);
	glVertex2f(-1.75f, 0.65f);
	glVertex2f(-1.25f, 0.65f);
	glVertex2f(-1.25f, 0.75f);


	glVertex2f(-1.35f, 0.65f);
	glVertex2f(-1.65f, 0.65f);
	glColor3f(0.071f, 0.533f, 0.027f);//green
	glVertex2f(-1.65f, -0.65f);
	glVertex2f(-1.35f, -0.65f);

	glVertex2f(-1.75f, -0.65f);
	glVertex2f(-1.75f, -0.75f);
	glVertex2f(-1.25f, -0.75f);
	glVertex2f(-1.25f, -0.65f);
	glEnd();
	//A
	glTranslatef(-2.0f,0.0f,0.0f);
	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.6f, 0.2f);//saffron
	glVertex2f(1.2f, 0.75f);
	glVertex2f(1.05f, 0.75f);
	glColor3f(0.066f, 0.533f, 0.027f);//green
	glVertex2f(0.85f, -0.75f);
	glVertex2f(1.5f, -0.75f);
	glEnd();
	glColor3f(70.0f / 255.0f, 130.0f / 255.0f, 180.0f / 255.0f);
	glBegin(GL_TRIANGLES);
	glVertex2f(1.13f, 0.5f);
	glVertex2f(1.0f, -0.75f);
	glVertex2f(1.35f, -0.75f);
	glEnd();


}

void plane_upper()
{
	glBegin(GL_POLYGON);
	//glColor3f(176.0f/255.0f, 224.0f / 255.0f, 230.0f / 255.0f);//powder blue
	glColor3f(70.0f / 255.0f, 130.0f / 255.0f, 180.0f / 255.0f);//steel blue
	glVertex2f(0.0f, 0.1f);
	glVertex2f(-0.05f, 0.05f);
	glVertex2f(-0.05f, -0.05f);
	glVertex2f(0.0f, -0.1f);
	glVertex2f(-0.05f, -0.2f);
	glVertex2f(0.0f, -0.25f);
	glVertex2f(0.2f, -0.1f);
	glVertex2f(0.4f, -0.05f);
	glVertex2f(0.45f, 0.0f);
	glVertex2f(0.4f, 0.05f);
	glVertex2f(0.2f, 0.1f);
	glVertex2f(0.0f, 0.25f);
	glVertex2f(-0.05f, 0.2f);

	glEnd();
	//Flag
	glBegin(GL_QUAD_STRIP);
	glColor3f(0.071f, 0.533f, 0.027f);//green
	glVertex2f(-0.2f, -0.05f);
	glVertex2f(-0.05f, -0.05f);
	glColor3f(1.0f, 1.0f, 1.0f);//white
	glVertex2f(-0.2f, 0.0f);
	glVertex2f(-0.05f, 0.0f);
	glColor3f(1.0f, 0.6f, 0.2f);//saffron
	glVertex2f(-0.2f, 0.05f);
	glVertex2f(-0.05f, 0.05f);
	glEnd();
	

	
	//F
	glTranslatef(0.3f, 0.0f, 0.0f);
	glScalef(0.1f, 0.05f, 0.0f);
	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.6f, 0.2f);//saffron
	glVertex2f(-0.0f, 0.75f);
	glVertex2f(-0.60f, 0.75f);
	glColor3f(0.071f, 0.533f, 0.027f);//green
	glVertex2f(-0.60f, -0.75f);
	glVertex2f(0.0f, -0.75f);

	glColor3f(70.0f / 255.0f, 130.0f / 255.0f, 180.0f / 255.0f);//steel blue

	glVertex2f(0.0f, 0.60f);
	glVertex2f(-0.45f, 0.60f);
	glVertex2f(-0.45f, 0.1f);
	glVertex2f(0.0f, 0.1f);

	glVertex2f(0.0f, -0.05f);
	glVertex2f(-0.45f, -0.05f);
	glVertex2f(-0.45f, -0.75f);
	glVertex2f(0.0f, -0.75f);

	glVertex2f(0.0f, 0.15f);
	glVertex2f(-0.2f, 0.15f);
	glVertex2f(-0.2f, -0.75f);
	glVertex2f(0.0f, -0.75f);

	glEnd();
	//glTranslatef(-0.3f, 0.0f, 0.0f);
	//I
	glTranslatef(-0.1f,0.0f,0.0f);
	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.6f, 0.2f);//saffron
	glVertex2f(-1.75f, 0.75f);
	glVertex2f(-1.75f, 0.65f);
	glVertex2f(-1.25f, 0.65f);
	glVertex2f(-1.25f, 0.75f);


	glVertex2f(-1.35f, 0.65f);
	glVertex2f(-1.65f, 0.65f);
	glColor3f(0.071f, 0.533f, 0.027f);//green
	glVertex2f(-1.65f, -0.65f);
	glVertex2f(-1.35f, -0.65f);

	glVertex2f(-1.75f, -0.65f);
	glVertex2f(-1.75f, -0.75f);
	glVertex2f(-1.25f, -0.75f);
	glVertex2f(-1.25f, -0.65f);
	glEnd();
	//A
	glTranslatef(-2.0f,0.0f,0.0f);
	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.6f, 0.2f);//saffron
	glVertex2f(1.2f, 0.75f);
	glVertex2f(1.05f, 0.75f);
	glColor3f(0.066f, 0.533f, 0.027f);//green
	glVertex2f(0.85f, -0.75f);
	glVertex2f(1.5f, -0.75f);
	glEnd();
	glColor3f(70.0f / 255.0f, 130.0f / 255.0f, 180.0f / 255.0f);
	glBegin(GL_TRIANGLES);
	glVertex2f(1.13f, 0.5f);
	glVertex2f(1.0f, -0.75f);
	glVertex2f(1.35f, -0.75f);
	glEnd();


}
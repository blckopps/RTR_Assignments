#include<Windows.h>
#include<stdio.h>
#include<gl/GL.h>
#include<stdlib.h>
#include<gl/GLU.h>	

#pragma comment(lib,"openGL32.lib")
#pragma comment(lib,"glu32.lib")

#define WIN_WIDTH 800
#define WIN_HEIGHT 600
FILE *gpfile = NULL;
//DS
struct node
{
	int data;
	int col;
	struct node *next;
	float r;
	float g;
	float b;
};


	#define RED 1
	#define GREEN 2
	#define BLUE 3
	#define CYAN 4
	#define YELLOW 5
	#define MAG 6


struct node *head ,*usednode;
int count = 0;
char ch;
///
LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);

//GLOBAL VARIABLES
bool bFullScreen=false;
DWORD dwstyle;
WINDOWPLACEMENT wpPrev={sizeof(WINDOWPLACEMENT)};
HWND ghwnd=NULL;
bool gbActiveWindow=false;
HDC ghdc=NULL;
HGLRC ghrc=NULL;

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
						TEXT("LRU-SHUBHAM"),
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
	void insert_node(int);
	void display();

	void resize(int ,int);
	
	void uninitialize(void);

	void toogle_screen(void);

	switch(iMsg)
	{	

		case WM_CHAR:
			switch (wParam)
			{
			case 'R':
			case 'r':
				fprintf(gpfile,"page added by pressing R button\n");
				insert_node(RED);
				break;

			case 'G':
			case 'g':
				fprintf(gpfile,"page added by pressing g button\n");
				insert_node(GREEN);
				break;


			case 'b':
			case 'B':
				fprintf(gpfile,"page added by pressing b button\n");
				insert_node(BLUE);
				break;


			case 'C':
			case 'c':
				insert_node(CYAN);
				break;

			case 'M':
			case 'm':
				insert_node(MAG);
				break;

			case 'Y':
			case 'y':
				insert_node(YELLOW);
				break;
			}
		break;

	case WM_KEYDOWN:

		switch(wParam)
		{
		case 0x46:
		//	MessageBox(hwnd,TEXT("P BUTTON PRESSED!!"),TEXT("BUTTON P"),MB_OK);
			toogle_screen();
		
			//////


			/////
			break;
		case VK_ESCAPE:
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
	//
	head = (struct node *)malloc(sizeof(struct  node*));
	head = NULL;
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
	//
	void insert_node();
	void displayN();
	//
	glClear(GL_COLOR_BUFFER_BIT);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	
	
	displayN();

	SwapBuffers(ghdc);
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
//DS CALLS
void insert_node(int colval)
{
	void check_replace(int, int);
	fprintf(gpfile,"insert node start:\n");
	if(count >=3)
	{
		
		check_replace(colval);
		
	}
	
	struct node *temp = (struct node *)malloc(sizeof(struct  node*));
	count++;
	temp->data =count;
	temp->col = colval;
	//////////////////////////////////////////////
	if(colval == RED)
	{
		temp->r=1.0f;
		temp->g=0.0f;
		temp->b=0.0f;
		
	}
	else if( colval == GREEN)
	{
		temp->r=0.0f;
		temp->g=1.0f;
		temp->b=0.0f;
	}
	else if(colval == BLUE)
	{
		temp->r=0.0f;
		temp->g=0.0f;
		temp->b=1.0f;
	}
	

	
	////////////////////////////////////////////
	if(head == NULL)
	{
		
		head=temp;
		temp->next=NULL;	
	}
	else
	{
		temp->next=head;
		head=temp;
		
	}
	
	fprintf(gpfile,"insert node END::\n");
}
//
void displayN()
{
	void rect_display(int);
	if(head == NULL)
	{
		
	}
	//fprintf(gpfile,"Displaying list start:\n");
	struct node *temp =NULL;
	 temp = head;
	 glTranslatef(0.0f,0.0f,-3.0f);
	 while(temp != NULL)
	 {
		 
		 glColor3f(temp->r, temp->g, temp->b);
		 glBegin(GL_QUADS);
			//
			rect_display(temp->data);

		glEnd();

	 	temp=temp->next;
		
	 }
	 
	 
}

//
void check_replace(int color,int place)
{
	
	
	struct node *temp,*temp1;
	temp=head;
	temp1=head;
	///
	fprintf(gpfile,"\nIn repla`ce");
	///if page not present in list continue//
	while(temp ->next != NULL)
	{	

		
		//fwprintf(gpfile,TEXT("temp->cal %d color= %d"),temp->col,RED);
		
		if(temp->col == RED)
		{
			temp->b=1.0f;
			
			return;
			
		}
		temp=temp->next;
	}
	
		
}

void rect_display(int place)
{
	switch (place)
	{
	case 1:
		glVertex3f(1.0f, 1.0f, 0.0f);
	
		glVertex3f(0.8f, 1.0f, 0.0f);
	
		glVertex3f(0.8f,-1.0f, 0.0f);
	
		glVertex3f(1.0f, -1.0f, 0.0f);
	
		break;

		case 2:
		glVertex3f(0.8f, 1.0f, 0.0f);
	
		glVertex3f(0.6f, 1.0f, 0.0f);
	
		glVertex3f(0.6f,-1.0f, 0.0f);
	
		glVertex3f(0.8f, -1.0f, 0.0f);
	
		break;

		case 3:
		glVertex3f(0.6f, 1.0f, 0.0f);
	
		glVertex3f(0.4f, 1.0f, 0.0f);
	
		glVertex3f(0.4f,-1.0f, 0.0f);
	
		glVertex3f(0.6f, -1.0f, 0.0f);
	
		break;
	default:
		break;
	}
}
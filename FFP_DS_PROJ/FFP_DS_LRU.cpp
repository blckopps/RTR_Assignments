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
//
 HFONT hFont;
unsigned int base;
float angle = 10.0f;           
GLYPHMETRICSFLOAT gmf[256]; 

#define RED 1
#define GREEN 2
#define BLUE 3
#define CYAN 4
#define YELLOW 5
#define MAG 6
	
#define pageLimit 3
//Font
unsigned int listBase;

struct node *head ,*usednode;
int count = 0;
char ch;
bool isPageFound = false;
static int replaceCount = 1;

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
	//	ShowCursor(FALSE);
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

		//ShowCursor(TRUE);
		bFullScreen=false;
	}
}

int initialize(void)
{
	void resize(int,int);
	unsigned int CreateOutlineFont(char *,int ,float);
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;
	//
	head = (struct node *)calloc(1,sizeof(struct  node));
	
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

	
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

	glShadeModel(GL_SMOOTH);                    
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_LIGHT0); 
	//glEnable(GL_LIGHTING); 
	glEnable(GL_COLOR_MATERIAL); 
	
	char *fontname = "Arial";

	listBase = CreateOutlineFont(fontname, 1, 0.10f);
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
	void PrintString(unsigned int,char *);
	//
	char *text = "Start:";
	char *text1 = "if pageCount < pageSize";
	char *text2 =	"add page";
	char *text3 =	"else if";
	char *text4 =	"page already present";
			
	char *text5 =	"else";
	char *text6 =	"replace LRU page";
	char *text7 =	"End:";
	

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//font render
	
	 glTranslatef(-9.5f, 3.5f, -15.0f);
	 glColor3f(0.3f, 0.0f, 0.0f);
     PrintString(listBase, text);
	 //
	 glMatrixMode(GL_MODELVIEW);
	 glLoadIdentity();

	 glTranslatef(-6.0f, 2.5f, -16.0f);
	 PrintString(listBase, text1);
	 //
	  glMatrixMode(GL_MODELVIEW);
	 glLoadIdentity();

	 glTranslatef(-8.3f, 1.5f, -16.0f);
	 PrintString(listBase, text2);
	 //
	  glMatrixMode(GL_MODELVIEW);
	 glLoadIdentity();

	 glTranslatef(-9.9f, 0.5f, -16.0f);
	 PrintString(listBase, text3);
	 //
	 glMatrixMode(GL_MODELVIEW);
	 glLoadIdentity();

	 glTranslatef(-5.7f, -0.5f, -16.0f);
	 PrintString(listBase, text4);
	 //
	 glMatrixMode(GL_MODELVIEW);
	 glLoadIdentity();

	 glTranslatef(-10.3f, -1.5f, -16.0f);
	 PrintString(listBase, text5);
	 //
	 glMatrixMode(GL_MODELVIEW);
	 glLoadIdentity();

	 glTranslatef(-6.3f, -2.5f, -16.0f);
	 PrintString(listBase, text6);
	 //
	 glMatrixMode(GL_MODELVIEW);
	 glLoadIdentity();

	 glTranslatef(-10.3f, -4.0f, -16.0f);
	 PrintString(listBase, text7);
	 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BITS);
	 glMatrixMode(GL_MODELVIEW);
	 glLoadIdentity();
	//
	displayN();
	
	SwapBuffers(ghdc);
	angle+=0.2f;
	
}

void uninitialize(void)
{
	void ClearFont(unsigned int base);

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

	ClearFont(listBase);
}
//DS CALLS
void insert_node(int colval)
{
	void check_replace(int);
	fprintf(gpfile,"insert node start:\n");
	if(count >=3)
	{
		
		check_replace(colval);
		return;	
	}
	
	
	struct node *temp = (struct node *)calloc(1,sizeof(struct  node));
	count++;
	temp->data = count;
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
	else if(colval == CYAN)
	{
		temp->r=0.0f;
		temp->g=1.0f;
		temp->b=1.0f;
	}
	else if(colval == MAG)
	{
		temp->r=1.0f;
		temp->g=0.0f;
		temp->b=1.0f;
	}
	else if(colval == YELLOW)
	{
		temp->r=1.0f;
		temp->g=1.0f;
		temp->b=0.0f;
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
	void rect_display(int,struct node*);
	if(head == NULL)
	{
		return;
	}
	fprintf(gpfile,"DisplayN():Displaying list start:\n");
	struct node *temp =NULL;
	 temp = head;
	 
	 while(temp != NULL)
	 {
		  glMatrixMode(GL_MODELVIEW);
		  glLoadIdentity();
		
		 switch (temp->data)
		 {
			 case 1:
				 glTranslatef(3.0f ,0.0f ,-6.0f);
				 glBegin(GL_QUADS);
		
				rect_display(temp->data,temp);

				glEnd();
				 break;

			 case 2:
				 glTranslatef(2.0f ,0.0f ,-6.0f);
				 glBegin(GL_QUADS);
		
				rect_display(temp->data,temp);

				glEnd();
				 break;

			 case 3:
				 glTranslatef(1.0f ,0.0f ,-6.0f);
				 glBegin(GL_QUADS);
		
				rect_display(temp->data,temp);

				glEnd();
				 break;

			 default:
				 break;
		 }
		
		
		
	 temp=temp->next;	
		
	 }
	 
	 
}

//
void check_replace(int color)
{
	
	
	struct node *temp,*temp1,*temp3,*prevtemp,*nexttemp;
	temp = head;
	prevtemp = head;
	//temp1=head;
	///
	fprintf(gpfile,"\nIn replace\n");
	///if page not present in list continue//
	while(temp != NULL)
	{	

		if(temp->col == color)
		{
			//if page already present
			isPageFound = true;

		}
		temp=temp->next;
	}
	//page is new
	fprintf(gpfile,"New page adding\n");
	struct node *newtemp= (struct node *)calloc(1,sizeof(struct  node));

	newtemp->col = color;
	newtemp->data = replaceCount;

	if(color == CYAN)
	{
		fprintf(gpfile,"Adding cyan\n");
		newtemp->r=1.0f;
		newtemp->g=0.0f;
		newtemp->b=1.0f;
		
	}
	else if( color == MAG)
	{
		newtemp->r=0.0f;
		newtemp->g=1.0f;
		newtemp->b=1.0f;
	}
	else if(color == YELLOW)
	{
		newtemp->r=1.0f;
		newtemp->g=1.0f;
		newtemp->b=0.0f;
	}
	else if(color == RED)
	{
		newtemp->r = 1.0f;
		newtemp->g = 0.0f;
		newtemp->b = 0.0f;
	}
	else if(color == GREEN)
	{
		newtemp->r = 0.0f;
		newtemp->g = 1.0f;
		newtemp->b = 0.0f;
	}
	else if(color == BLUE)
	{
		newtemp->r = 0.0f;
		newtemp->g = 0.0f;
		newtemp->b = 1.0f;
	}
	fprintf(gpfile,"head data=%d\n",head->data);
	temp = head;

	while(temp != NULL)
	{
		if(temp->data == replaceCount)
		{
			break;
		}
		temp = temp->next;

	}
	//if replace count and temp->count both are 3
	//START:
	if (replaceCount == pageLimit )
	{
		nexttemp = temp->next;

		newtemp->next = nexttemp;

		head = newtemp;

		if(temp != NULL)
		{
			free(temp);
			temp=NULL;
		}

		replaceCount = 1;
		return; 
	}
	//END
	//go to previous of temp
	while(prevtemp->next != temp)
	{
		prevtemp = prevtemp->next;
	}


  	fprintf(gpfile,"temp->data=%d\n",temp->data); //last node
	
	
	prevtemp->next = newtemp;

	if(temp ->next != NULL)
	{
		fwprintf(gpfile,TEXT("temp->next != NULL"));
		newtemp->next = temp->next;
		fclose(gpfile);
	}
	else
	{
		fwprintf(gpfile,TEXT("temp->next == NULL"));
		newtemp->next = NULL;
		
	}

	//delete temp
	if(temp != NULL)
	{
		fprintf(gpfile,"free() succesfull\n");
		
		free(temp);
		
	}
	
	replaceCount++;
	fprintf(gpfile,"newtemp data= %d",newtemp->data);
	
}

void rect_display(int place, struct node* temp)
{
	//fwprintf(gpfile,TEXT("rect_dsiplay()..\n"));
	
			//TOP
	glColor3f(0.1f,0.1f,0.1f);		//RED
	glVertex3f(0.2f,1.0f,-1.0f);					//RT
	glVertex3f(-0.2f,1.0f,-1.0f);					//LT
	glVertex3f(-0.2f,1.0f,1.0f);					//LB
	glVertex3f(0.2f,1.0f,1.0f);					//RB
	//bottom
	glColor3f(0.1f,0.1f,0.1f);		//green
	glVertex3f(0.2f,-1.0f,-1.0f);					//RT
	glVertex3f(-0.2f,-1.0f,-1.0f);					//LT
	glVertex3f(-0.2f,-1.0f,1.0f);					//LB
	glVertex3f(0.2f,-1.0f,1.0f);
	////Front
	glColor3f(temp->r,temp->g,temp->b);		//blue
	glVertex3f(0.2f,1.0f,1.0f);					//RT
	glVertex3f(-0.2f,1.0f,1.0f);					//LT
	glVertex3f(-0.2f,-1.0f,1.0f);					//LB
	glVertex3f(0.2f,-1.0f,1.0f);
	//back
	glColor3f(0.1f,0.1f,0.1f);		//cyan
	glVertex3f(0.2f,1.0f,-1.0f);					//RT
	glVertex3f(-0.2f,1.0f,-1.0f);					//LT
	glVertex3f(-0.2f,-1.0f,-1.0f);					//LB
	glVertex3f(0.2f,-1.0f,-1.0f);

	//Right
	glColor3f(0.1f,0.1f,0.1f);		//magneta
	glVertex3f(0.2f,1.0f,-1.0f);					//RT
	glVertex3f(0.2f,1.0f,1.0f);					//LT
	glVertex3f(0.2f,-1.0f,1.0f);					//LB
	glVertex3f(0.2f,-1.0f,-1.0f);

	//left
	glColor3f(0.1f,0.1f,0.1f);		//Yeloow
	glVertex3f(-0.2f,1.0f,1.0f);					//RT
	glVertex3f(-0.2f,1.0f,-1.0f);					//LT
	glVertex3f(-0.2f,-1.0f,-1.0f);					//LB
	glVertex3f(-0.2f,-1.0f,1.0f);
	
}

void PrintString(unsigned int base, char *str)
{
	float length = 0;

	int indx;

	if(str == NULL)
	{
		//return;
		exit(0);
	}

	for(indx = 0; indx < (int)strlen(str);indx++ )
	{
		length += gmf[str[indx]].gmfCellIncX;
	}

	glTranslatef(-length/2.0f, 0.0f, 0.0f); 

	
	glPushAttrib(GL_LIST_BIT);
		glListBase(base);
		glCallLists(strlen(str) ,GL_UNSIGNED_BYTE, str);
	glPopAttrib();
}

unsigned int CreateOutlineFont(char *fontName, int fontSize, float depth)
{
	HFONT hFont;
	unsigned int base;

	base = glGenLists(256);

	for (int i = 0; i < 256; i++)
	{
		gmf[i].gmfBlackBoxX = 50.0f;
		gmf[i].gmfBlackBoxY = 25.0f;
	}
	if(strcmp(fontName,"symbol") == 0)
	{
		hFont = CreateFont(fontSize, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, SYMBOL_CHARSET,
			OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_DONTCARE | DEFAULT_PITCH, (LPCTSTR)fontName);

	}
	else
	{
		hFont = CreateFont(fontSize, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, 
			OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_DONTCARE | DEFAULT_PITCH, (LPCTSTR)fontName);
	}

	if(!hFont)
	{
		exit(0);
	}

	SelectObject(ghdc, hFont);

	wglUseFontOutlines(ghdc, 0, 255, base, 0.0f, depth, WGL_FONT_POLYGONS, (LPGLYPHMETRICSFLOAT)gmf);

	return base;

}

void ClearFont(unsigned int base)
{
	glDeleteLists(base, 256);
}

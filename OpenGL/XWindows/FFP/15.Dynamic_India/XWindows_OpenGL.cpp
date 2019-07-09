#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<memory.h>

//xwindows headers
#include<X11/Xlib.h>
#include<X11/Xutil.h>
#include<X11/XKBlib.h>
#include<X11/keysym.h>

//OpenGL headers
#include<GL/gl.h>
#include<GL/glu.h>
#include<GL/glx.h>

//namespace
using namespace std;
//

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
//global variables
bool bFullScreen = false;
Display *gpdisplay = NULL;
XVisualInfo *gpXVisualInfo  = NULL;
Colormap gColormap;
Window gWindow;

int giWindowWidth = 600;
int giWindowHeight = 800;

FILE *gpfile = NULL;

static GLXContext gGlxContext;

//entry point function
int main(void)
{
	gpfile = fopen( "log.txt","w");
        if(gpfile == NULL)
	{
		printf("ERROR: creating log file\n");
		exit(0);
	
	}
     //function prototype
        void CreateWindow(void);
        void Uninitialize(void);
        void ToogleFullScreen(void);
	//OpenGL Functions proto..
	void initialize(void);
	void resize(int, int);
	void display(void);
	        
        //variabal 
        int winWidth = giWindowWidth;
        int winHeight =  giWindowHeight;
        bool bDone = false;
        char keys[26];
        //code
        CreateWindow();
        
        initialize();
        //MessageLoop 
        XEvent event;
        KeySym keysym;
        while(!bDone)
        {
		while(XPending(gpdisplay))
		{
		       XNextEvent(gpdisplay, &event); 
		       
		       switch(event.type)
		       {
		                case MapNotify:
		                
		                break;
		                
		                case KeyPress:
		                        keysym = XkbKeycodeToKeysym(gpdisplay, event.xkey.keycode, 0, 0);
		                        
		                        switch(keysym)
		                        {
		                                
		                                case XK_Escape:
		                                	bDone = true;
		                                        Uninitialize();
		                                        exit(0);
		                                        
		                                break;
		                                
		                                default:
		                                
		                                break;
		                                
		                                
		                        } 
		                        //LOOKUP
		                        //START:
		                        XLookupString( &event.xkey,
		                        		keys,
		                        		sizeof(keys),
		                        		NULL,
		                        		NULL);
		                        
		                        switch(keys[0])
		                        {
		                        	case 'f':
		                                case 'F':
		                                
		                                        if(bFullScreen == false)
		                                        {
		                                                ToogleFullScreen();
		                                                bFullScreen = true;
		                                        }
		                                        else
		                                        {
		                                                ToogleFullScreen();
		                                                bFullScreen = false;
		                                        }       
		                                                
		                                break;
		                        }
		                        //END
		                                  
		                break;
		                
		                case ButtonPress:
		                        switch(event.xbutton.button)
		                        {
		                                case 1:         //LEFT MOUSE BUTTON
		                                break;
		                                
		                                case 2:         //MIDLE SCROLL BUTTON
		                                
		                                break;
		                                
		                                case 3:         //RIGHT MOUSE BUTTON
		                                
		                                break;
		                        }
		                break;
		                
		                case MotionNotify:              //WM_MOUSE_MOVE
		                        
		                break;
		                
		                case ConfigureNotify:
		                        winWidth = event.xconfigure.width;
		                        
		                        winHeight = event.xconfigure.height;
		                        
		                        resize(winWidth, winHeight);				//RESIZE
		                break;
		                
		                case Expose:                    //WM_PAINT
		                        
		                break;
		                
		                case DestroyNotify:
		                        
		                break;
		                
		                case 33:
		                	bDone = true;
		                        Uninitialize();
		                        exit(0);                                     
		                break;
		                
		                default:
		                
		                break;
		       }
		}  
		//CALL UPDATE AND DISPLAY ...
		//update();
		display();
	}        
        Uninitialize();
        fprintf(gpfile,"uninitialize call 178");
        exit(0);
        
}

///
void CreateWindow(void)
{
	fprintf(gpfile,"CreateWindow Start");
        //function prototypes
        void Uninitialize();
        
        //variables
        XSetWindowAttributes winattribs;
        int defaultScreen;
        int defaultDepth;
        int styleMask;
        
        static int frameBufferAttributes[] = 
        {
        	GLX_RGBA,
        	GLX_DOUBLEBUFFER,
        	GLX_RED_SIZE, 8,
        	GLX_GREEN_SIZE,8,
        	GLX_BLUE_SIZE,8,
        	GLX_ALPHA_SIZE,8,
        	
        	None
        	
        };
        //Code
        gpdisplay = XOpenDisplay(NULL);                 //NULL for defaault display
        if(gpdisplay == NULL)
        {
                printf("ERROR:UNABLE TO OPEN CONNECTION.....\n");
                fprintf(gpfile,"call from 212\n");
                Uninitialize();
                
                exit(1);
        }
        
        defaultScreen = XDefaultScreen(gpdisplay);
        
        defaultDepth = DefaultDepth(gpdisplay, defaultScreen);	//No Need now...
        //>>
        gpXVisualInfo = glXChooseVisual(gpdisplay,
        				defaultScreen,
        				frameBufferAttributes);
        if(gpXVisualInfo == NULL)
        {
        	printf("ERROR:glxChoiceVisual failed...Exiting...");
        	fprintf(gpfile,"call from 227\ns");
        	Uninitialize();
        	exit(0);
        }
        //>>
        winattribs.border_pixel = 0;
        winattribs.background_pixmap = 0;
        winattribs.colormap = XCreateColormap(gpdisplay,
                                              RootWindow(gpdisplay, gpXVisualInfo->screen),
                                              gpXVisualInfo->visual,
                                              AllocNone);       //dont allocate mem for color map,we saved it on below line
        gColormap =   winattribs.colormap;                                   
        winattribs.background_pixel = BlackPixel(gpdisplay, defaultScreen);
        
        
        winattribs.event_mask = ExposureMask | VisibilityChangeMask |
                                ButtonPressMask | KeyPressMask | PointerMotionMask |
                                StructureNotifyMask;
                                
        styleMask = CWBorderPixel | CWBackPixel | CWEventMask | CWColormap;
        
        gWindow = XCreateWindow(gpdisplay,
                                RootWindow(gpdisplay, gpXVisualInfo->screen),
                                0,
                                0,
                                giWindowWidth,
                                giWindowHeight,
                                0,
                                gpXVisualInfo->depth,
                                InputOutput,
                                gpXVisualInfo->visual,
                                styleMask,
                                &winattribs);
                                
         if(!gWindow)
         {
                printf("ERROR: XCreateWiow Failed....");
                fprintf(gpfile,"call from 264\n");
                Uninitialize();
                exit(1);                       
         }                                             
         XStoreName(gpdisplay, gWindow, "Dynamic India");
         
         Atom windowManagerDelete = XInternAtom(gpdisplay, "WM_DELETE_WINDOW",True);
         XSetWMProtocols(gpdisplay, gWindow, &windowManagerDelete, 1);
         
         XMapWindow(gpdisplay, gWindow);
         fprintf(gpfile,"CreateWindow END");
}

void ToogleFullScreen(void)
{
	
        //variables
        Atom wm_state;
        Atom fullscreen;
        XEvent xev ={0};
        
        //code
        wm_state = XInternAtom(gpdisplay, "_NET_WM_STATE",False);
        memset(&xev, 0, sizeof(xev));
        
        xev.type = ClientMessage;
        xev.xclient.window = gWindow;
        xev.xclient.message_type = wm_state;
        xev.xclient.format = 32;
        xev.xclient.data.l[0] = bFullScreen ? 0 :1;
        
        fullscreen = XInternAtom(gpdisplay, "_NET_WM_STATE_FULLSCREEN",False);
        xev.xclient.data.l[1] = fullscreen;
        
        XSendEvent(gpdisplay,
                   RootWindow(gpdisplay, gpXVisualInfo->screen),
                   False,
                   StructureNotifyMask,
                   &xev);
}

void Uninitialize(void)
{
	fprintf(gpfile,"In uninitialize\n");
	
        if(gWindow)
        {
                XDestroyWindow(gpdisplay, gWindow);
        }
        if(gColormap)
        {
               XFreeColormap(gpdisplay, gColormap);
        }
        if(gpXVisualInfo)
        {
                free(gpXVisualInfo);
                gpXVisualInfo = NULL;
        }
        if(gpdisplay)
        {
                XCloseDisplay(gpdisplay);
                gpdisplay = NULL;
        }
}
//
void initialize(void)
{
	fprintf(gpfile,"initialize START\n");
	void ToogleFullScreen(void);
	void Uninitialize(void);
	void resize(int, int);
	
	gGlxContext = glXCreateContext(	gpdisplay,
					gpXVisualInfo,
					NULL,
					True);
	glXMakeCurrent( gpdisplay,
			gWindow,
			gGlxContext);
			
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	resize(giWindowWidth, giWindowHeight );
	ToogleFullScreen();
			
	fprintf(gpfile,"initialize END\n");
}
//
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
	fprintf(gpfile,"resize END\n");
}

//
void display()
{
	fprintf(gpfile,"Display START\n");
	
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
	
	I();
	if(flagN == 2)
	{
		
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
	
	glXSwapBuffers(gpdisplay, gWindow);
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
	glXSwapBuffers(gpdisplay, gWindow);
	fprintf(gpfile,"Display END\n");
}

void I()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(Ix, 0.0f, -2.0f);
	glLineWidth(30.0f);
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

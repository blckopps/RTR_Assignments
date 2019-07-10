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


bool  bLight = false;

GLUquadric *quadric = NULL;

float angle_Light_Zero = 0.0f;
float angle_Light_One = 0.0f;
float angle_Light_Two = 0.0f;

bool  bLigtht = false;

GLfloat lightPosition_zero[4];
GLfloat lightPosition_one[4];
GLfloat lightPosition_two[4];

GLfloat LightAmbientZero[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat LightDiffuseZero[] = { 1.0f, 0.0f, 0.0f, 1.0f };
GLfloat LigthSpecularZero[] = {1.0f, 0.0f, 0.0f, 1.0f};

GLfloat LightAmbientOne[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat LightDiffuseOne[] = { 0.0f, 1.0f, 0.0f, 1.0f };
GLfloat LigthSpecularOne[] = {0.0f, 1.0f, 0.0f, 1.0f};

GLfloat LightAmbientTwo[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat LightDiffuseTwo[] = { 0.0f, 0.0f, 1.0f, 1.0f };
GLfloat LigthSpecularTwo[] = {0.0f, 0.0f, 1.0f, 1.0f};
//Material 
GLfloat MaterialAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat MaterialDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat MaterialPosition[] = { 100.0f, 100.0f, 100.0f, 1.0f };
GLfloat MaterialSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat MaterialShiness[] = {128.0f }; //128.0f


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
	void update(void);  
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
		                                
		                                case 'L':
		                                case 'l':
		                                	if(bLight == false)
		                                	{
		                                		glEnable(GL_LIGHTING);
		                                		bLight = true;
		                                	}
		                                	else
		                                	{
		                                		glDisable(GL_LIGHTING);
		                                		bLight = false;
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
		                
		                case 33: //Escape
		                	bDone = true;
		                        Uninitialize();
		                        exit(0);                                     
		                break;
		                
		                default:
		                
		                break;
		       }
		}  
		//CALL UPDATE AND DISPLAY ...
		update();
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
        	GLX_DEPTH_SIZE,24,
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
         XStoreName(gpdisplay, gWindow, "Three_Moving_Lights");
         
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
	void Uninitialize(void);
	void resize(int, int);
	
	gGlxContext = glXCreateContext(	gpdisplay,
					gpXVisualInfo,
					NULL,
					True);
	glXMakeCurrent( gpdisplay,
			gWindow,
			gGlxContext);
			
	glShadeModel(GL_SMOOTH);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHT0);
	glDepthFunc(GL_LEQUAL);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);
	//
	//configuration of light using config. parameters
	glLightfv(GL_LIGHT0,
		GL_AMBIENT,
		LightAmbientZero);

	glLightfv(GL_LIGHT0,
		GL_DIFFUSE,
		LightDiffuseZero);

	glLightfv(GL_LIGHT0,
			GL_SPECULAR,
			LigthSpecularZero);
	glEnable(GL_LIGHT0);
	//2nd array prop
	glLightfv(GL_LIGHT1,
		GL_AMBIENT,
		LightAmbientOne);

	glLightfv(GL_LIGHT1,
		GL_DIFFUSE,
		LightDiffuseOne);

	glLightfv(GL_LIGHT1,
			GL_SPECULAR,
			LigthSpecularOne);
	glEnable(GL_LIGHT1);
	//3rd Light
	glLightfv(GL_LIGHT2,
		GL_AMBIENT,
		LightAmbientTwo);

	glLightfv(GL_LIGHT2,
		GL_DIFFUSE,
		LightDiffuseTwo);

	glLightfv(GL_LIGHT2,
			GL_SPECULAR,
			LigthSpecularTwo);
	glEnable(GL_LIGHT2);
	//setting material property
	glMaterialfv(GL_FRONT,
		GL_AMBIENT,
		MaterialAmbient);

	glMaterialfv(GL_FRONT,
		GL_DIFFUSE,
		MaterialDiffuse);

	glMaterialfv(GL_FRONT,
		GL_SPECULAR,
		MaterialSpecular);

	glMaterialfv(GL_FRONT,
		GL_SHININESS,
		MaterialShiness);
	lightPosition_zero[3]=1.0f;
	lightPosition_one[3]=1.0f;
	lightPosition_two[3]=1.0f;

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);


	resize(giWindowWidth, giWindowHeight );
	
		
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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();

	gluLookAt(0.0f, 0.0f, 1.0f,
			  0.0f, 0.0f, 0.0f,
			  0.0f, 1.0f,0.0f);
	//Light 1 
	//start:
	glPushMatrix();
	glRotatef(angle_Light_Zero, 1.0f, 0.0f, 0.0f);
	lightPosition_zero[1]=angle_Light_Zero;
	glLightfv(GL_LIGHT0,
			GL_POSITION,
			lightPosition_zero);

	glPopMatrix();
	//ENd
	//Light 2
	//start:
	glPushMatrix();
	glRotatef(angle_Light_One, 0.0f, 1.0f, 0.0f);
	lightPosition_one[0]=angle_Light_One;
	glLightfv(GL_LIGHT1,
			GL_POSITION,
			lightPosition_one);

	glPopMatrix();
	//ENd
	//Light 3
	//start:
	glPushMatrix();
	glRotatef(angle_Light_Two, 0.0f, 0.0f, 1.0f);
	lightPosition_two[0]=angle_Light_Two;
	glLightfv(GL_LIGHT2,
			GL_POSITION,
			lightPosition_two);

	glPopMatrix();
	//End
	//////
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	quadric = gluNewQuadric();

	gluSphere(quadric, 0.2f, 30, 30);
	glPopMatrix();
	glXSwapBuffers(gpdisplay, gWindow);
	
	fprintf(gpfile,"Display END\n");
}

void update(void) 
{
	if (angle_Light_Zero == 360)
	{
		angle_Light_Zero = 0.0f;
	}
	angle_Light_Zero = angle_Light_Zero + 0.02f;
	//
	if (angle_Light_One == 360)
	{
		angle_Light_One = 0.0f;
	}
	angle_Light_One = angle_Light_One + 0.02f;
	//
	if (angle_Light_Two == 360)
	{
		angle_Light_Two = 0.0f;
	}
	angle_Light_Two = angle_Light_Two + 0.02f;
}

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

//SOIL HEADERS
#include<SOIL/SOIL.h>

//namespace
using namespace std;

//global variables

GLuint stonetexture;

bool bFullScreen = false;
Display *gpdisplay = NULL;
XVisualInfo *gpXVisualInfo  = NULL;
Colormap gColormap;
Window gWindow;
float angle = 0.0f;

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
         XStoreName(gpdisplay, gWindow, "Stone_Texture_pyramid");
         
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
	bool LoadTexture(GLuint *texture, const char *path);
	
	gGlxContext = glXCreateContext(	gpdisplay,
					gpXVisualInfo,
					NULL,
					True);
	glXMakeCurrent( gpdisplay,
			gWindow,
			gGlxContext);
	//Depth related changes//
	glShadeModel(GL_SMOOTH);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_TEXTURE_2D);
	LoadTexture(&stonetexture,"Stone.png");
	//	
		
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
	//fprintf(gpfile,"Display START\n");
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);

	/////////////pyramid////////////////////
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -6.0f);

	glRotatef(angle, 0.0f, 1.0f, 0.0f);
	glBindTexture(GL_TEXTURE_2D, stonetexture);
	glBegin(GL_TRIANGLES);
	//1St
	glTexCoord2f(0.5f, 1.0f);					//R 
	glVertex3f(0.0f, 1.0f, 0.0f);				//APEX

	glTexCoord2f(0.0f, 0.0f);					//G 
	glVertex3f(-1.0f, -1.0f, 1.0f);				//left bottom

	glTexCoord2f(1.0f, 0.0f);					//B
	glVertex3f(1.0f, -1.0f, 1.0f);				//Rigth BOttom

	//2nd
	glTexCoord2f(0.5f, 1.0f);					//R 
	glVertex3f(0.0f, 1.0f, 0.0f);				//APEX

	glTexCoord2f(1.0f, 0.0f);					//B 
	glVertex3f(1.0f, -1.0f, 1.0f);				//left bottom

	glTexCoord2f(0.0f, 0.0f);					//G
	glVertex3f(1.0f, -1.0f, -1.0f);				//Right BOttom
	//3rd
	glTexCoord2f(0.5f, 1.0f);					//R 
	glVertex3f(0.0f, 1.0f, 0.0f);				//APEX

	glTexCoord2f(1.0f, 0.0f);					//G 
	glVertex3f(1.0f, -1.0f, -1.0f);				//left bottom

	glTexCoord2f(0.0f, 0.0f);					//B
	glVertex3f(-1.0f, -1.0f, -1.0f);				//Rigth BOttom
	//4th
	glTexCoord2f(0.5f, 1.0f);					//R 
	glVertex3f(0.0f, 1.0f, 0.0f);				//APEX

	glTexCoord2f(0.0f, 0.0f);					//b
	glVertex3f(-1.0f, -1.0f, -1.0f);				//left bottom

	glTexCoord2f(1.0f, 0.0f);					//g
	glVertex3f(-1.0f, -1.0f, 1.0f);				//Rigth BOttom
	glEnd();

	glXSwapBuffers(gpdisplay, gWindow);
	//fprintf(gpfile,"Display END\n");
}

void update(void)
{
	if(angle >= 360.0f)
	{
		angle=0.0f;
	}
	else
	{
		angle=angle+0.05f;
	}
}

//texture
bool LoadTexture(GLuint *texture, const char *path)
{
	//varibale
	bool result = false;
	int width, height;
	
	unsigned char* imagedata = NULL;
	
	imagedata = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGB);
	
	if(imagedata == NULL)
	{
		fprintf(gpfile,"imagedata null\n");
		exit(0);
	}
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

		glGenTextures(1, texture);

		glBindTexture(GL_TEXTURE_2D, *texture);

		glTexParameteri(GL_TEXTURE_2D,
			GL_TEXTURE_MAG_FILTER,
			GL_LINEAR);

		glTexParameteri(GL_TEXTURE_2D,
			GL_TEXTURE_MIN_FILTER,
			GL_LINEAR_MIPMAP_LINEAR);

		gluBuild2DMipmaps(GL_TEXTURE_2D,
			3,
			width,
			height,
			GL_RGB,
			GL_UNSIGNED_BYTE,
			imagedata);
			
	SOIL_free_image_data(imagedata);
}

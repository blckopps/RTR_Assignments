#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<memory.h>
#include<GL/glew.h>

//xwindows headers
#include<X11/Xlib.h>
#include<X11/Xutil.h>
#include<X11/XKBlib.h>
#include<X11/keysym.h>
#include"vmath.h"

//OpenGL headers
#include<GL/gl.h>
#include<GL/glu.h>
#include<GL/glx.h>

//namespace
using namespace std;
using namespace vmath;

//global variables

bool bFullScreen = false;
Display *gpdisplay = NULL;
XVisualInfo *gpXVisualInfo  = NULL;
Colormap gColormap;
Window gWindow;

int giWindowWidth = 600;
int giWindowHeight = 800;
//
typedef GLXContext (* glXCreateContextAttribsARBProc)(Display *,
						     GLXFBConfig,
						     GLXContext,
						     Bool,
						     const int *);

glXCreateContextAttribsARBProc glXCreateContextAttribsARB = NULL;

GLXFBConfig gGlxFBConfig;
GLXContext gGlxContext;

//shaders object
GLuint gVertexShaderObject;
GLuint gFragmentShaderObject;
GLuint gShaderProgramObject;


GLfloat angle_cube = 0.0f;
GLfloat angle_pyramid = 0.0f;

//declare vao's according to shapes
GLuint vao_pyramid;
GLuint vao_cube;

//declare vbo's according to Arrays(position,color etc)
GLuint vbo_pyramid_position;
GLuint vbo_cube_position;

GLuint vbo_pyramid_color;
GLuint vbo_cube_color;


GLuint mvpUniform;
mat4 perspectiveProjectionMatrix;

//enum
enum
{
	AMC_ATTRIBUTES_POSITION = 0,
	AMC_ATTRIBUTES_COLOR, 
	AMC_ATTRIBUTES_NORMAL, 
	AMC_ATTRIBUTES_TEXCOORD0 
};

FILE *gpfile = NULL;

//static GLXContext gGlxContext;

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
	//variables
	GLXFBConfig *pGlxFBConfig = NULL;
	GLXFBConfig bestGlxFBConfig;
	XVisualInfo *pTempXVisualInfo = NULL;
	int iNumberOfFBConfigs = 0;
        //function prototypes
        void Uninitialize();
        
        //variables
        XSetWindowAttributes winattribs;
        int defaultScreen;
        int defaultDepth;
        int styleMask;
        
        static int frameBufferAttributes[] = 
        {
        	GLX_X_RENDERABLE, True,
        	GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
        	GLX_RENDER_TYPE, GLX_RGBA_BIT,
        	GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
        	GLX_DOUBLEBUFFER, True,
        	GLX_RED_SIZE, 8,
        	GLX_GREEN_SIZE,8,
        	GLX_BLUE_SIZE,8,
        	GLX_ALPHA_SIZE,8,
        	GLX_DEPTH_SIZE,24,
        	GLX_STENCIL_SIZE,8,
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
        
        //Retrive all FBConfig driver has
        pGlxFBConfig = glXChooseFBConfig(gpdisplay,
        				 defaultScreen,
        				 frameBufferAttributes,
        				 &iNumberOfFBConfigs);
        
        fprintf(gpfile,"Number of FBConfigs: %d\n",iNumberOfFBConfigs);
        
        int bestFrameBufferConfig = -1;
        int bestNumberOfSamples = -1;
        int worstFrameBufferConfig = -1;
        int worstNumberOfSamples = 999;
        
        for(int i=0; i < iNumberOfFBConfigs; i++)
        {
        	//for each FBCongig get temporary visual info it is just used to check
        	//capability..
        	pTempXVisualInfo = glXGetVisualFromFBConfig( gpdisplay,
        						     pGlxFBConfig[i]);
        						     
		if(pTempXVisualInfo)
		{
			//Get number of sample buffers from respective FBConfig
			int sampleBuffers, samples;
			
			glXGetFBConfigAttrib(gpdisplay,
					     pGlxFBConfig[i],
					     GLX_SAMPLE_BUFFERS,
					     &sampleBuffers);
					     
			//get number of samples from respective
			glXGetFBConfigAttrib(gpdisplay,
					     pGlxFBConfig[i],
					     GLX_SAMPLES,
					     &samples);
					     
			//more number of samples, sampleBuffers more eligible Fbconfig is..
			
			if(bestFrameBufferConfig < 0 ||
			   sampleBuffers && samples > bestNumberOfSamples)
			{
				bestFrameBufferConfig = i;
				bestNumberOfSamples = samples;
			}
			if(worstFrameBufferConfig < 0 ||
			   !sampleBuffers || samples < worstNumberOfSamples)
			{
				worstFrameBufferConfig = i;
				worstNumberOfSamples = samples;
			}
		}
		XFree(pTempXVisualInfo);
        }
        
        //Now assign found best one...
        bestGlxFBConfig = pGlxFBConfig[bestFrameBufferConfig];
        
        gGlxFBConfig = bestGlxFBConfig;
        
        XFree(pGlxFBConfig);
        
        
        //>>
        gpXVisualInfo = glXGetVisualFromFBConfig(gpdisplay,
        					 bestGlxFBConfig);
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
         XStoreName(gpdisplay, gWindow, "3D Animation");
         
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
	
	if (vbo_pyramid_position)
	{
		glDeleteBuffers(1, &vbo_pyramid_position);
		vbo_pyramid_position = 0;
	}
	if (vbo_pyramid_color)
	{
		glDeleteBuffers(1, &vbo_pyramid_color);
		vbo_pyramid_color = 0;
	}
        
        if (vbo_cube_position)
	{
		glDeleteBuffers(1, &vbo_cube_position);
		vbo_cube_position = 0;
	}
	if (vbo_cube_color)
	{
		glDeleteBuffers(1, &vbo_cube_color);
		vbo_cube_color = 0;
	}
        
	if (vao_pyramid)
	{
		glDeleteVertexArrays(1, &vao_pyramid);
		vao_pyramid = 0;
	}
	
	if(vao_cube)
	{
	        glDeleteVertexArrays(1, &vao_cube);
		vao_cube = 0;
	}

	//shader uninitialize code
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
	
	GLenum result;
	
	glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)glXGetProcAddressARB((GLubyte *) "glXCreateContextAttribsARB");
	
	if(glXCreateContextAttribsARB == NULL)
	{
		fprintf(gpfile,"glXCreateContextAttribARB is NULL\n");
		printf("glXCreateContextAttribARB is null Exiting..\n");
		Uninitialize();
		exit(0);	
	}

	const int attribs[] = {
			  	GLX_CONTEXT_MAJOR_VERSION_ARB,4,
			  	GLX_CONTEXT_MINOR_VERSION_ARB,5,
			  	GLX_CONTEXT_PROFILE_MASK_ARB,GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
			  	None
			  };
	
	gGlxContext = glXCreateContextAttribsARB(gpdisplay,
						gGlxFBConfig,
						0,
						True,
						attribs);
						
	if(!gGlxContext)
	{
		fprintf(gpfile,"Not Obtained required context...\n");
		const int attribs[] =
			  {
			  	GLX_CONTEXT_MAJOR_VERSION_ARB,1,
			  	GLX_CONTEXT_MINOR_VERSION_ARB,0,
			  	
			  	None
			  };

		gGlxContext = glXCreateContextAttribsARB(gpdisplay,
						gGlxFBConfig,
						0,
						True,
						attribs);
	}
	
	//check obtained context is H/W context or not
	if(!glXIsDirect(gpdisplay,gGlxContext))
	{
		fprintf(gpfile,"Required context is not H/W render context\n");
	}
	else
	{
		fprintf(gpfile,"Required context is  H/W render context\n");
	}
	
	glXMakeCurrent( gpdisplay,
			gWindow,
			gGlxContext);
	
	//programmable changes		
	result = glewInit();
	if(result != GLEW_OK)
	{
		fprintf(gpfile,"glewInit failed\n");
		Uninitialize();
		
	}
	//************VERTEX SHADER***************************
	//Define vertex shader Object
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	//write vertex shader code
	 const GLchar* vertexShaderSourceCode = 
		"#version 430 core" \
		 "\n" \
		 "in vec4 vPosition;" \
		 "in vec4 vColor;" \
		 "out vec4 out_color;" \
		 "uniform mat4 u_mvp_matrix;" \
		 "void main(void)" \
		 "{" \
		 "	gl_Position = u_mvp_matrix * vPosition;" \
		 "	out_color = vColor;" \
		 "}" ;
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
				fprintf(gpfile,"ERROR:VERTEX SHADER:%s",szInfoLog);
				free(szInfoLog);
				Uninitialize();
				
				exit(0);
			}
		}
	}
	//********************FRAGMENT SHADER*****************
	gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	//write vertex shader code
	const GLchar * fragmentShaderSourceCode = 
		"#version 430 core" \
		"\n" \
		"out vec4 fragColor;" \
		"in vec4 out_color;" \
		"void main(void)" \
		"{" \
		"	fragColor = out_color;" \
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
				fprintf(gpfile,"ERROR:FRAGMENT SHADER:%s",szInfoLog);
				free(szInfoLog);
				Uninitialize();
				
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
						AMC_ATTRIBUTES_COLOR,
						"vColor");
											
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

					fprintf(gpfile,"ERROR:SHADER LINK:%s",szInfoLog);
					free(szInfoLog);
					Uninitialize();
					exit(0); 
				}
				
			}
		}
	
	///***POST LINKING GETTING UNIFORMS**
	mvpUniform = glGetUniformLocation(gShaderProgramObject,
		                                "u_mvp_matrix");
		
	//arrays

	fprintf(gpfile,"Post link success!!\n");
	const GLfloat pyramid_vertices[]=
						{
							//1st		
							0.0f, 1.0f, 0.0f,
							-1.0f, -1.0f, 1.0f,				
							1.0f, -1.0f, 1.0f,
							//2nd
		
							0.0f, 1.0f, 0.0f,
							1.0f, -1.0f, 1.0f,
							1.0f, -1.0f, -1.0f,				
							//3rd
							0.0f, 1.0f, 0.0f,				
							1.0f, -1.0f, -1.0f,				
							-1.0f, -1.0f, -1.0f,
							//4th
							0.0f, 1.0f, 0.0f,				
							-1.0f, -1.0f, -1.0f,
							-1.0f, -1.0f, 1.0f
						};

	const GLfloat pyramid_color [] =
								{
									1.0f, 0.0f, 0.0f,
									0.0f, 1.0f, 0.0f,
									0.0f, 0.0f, 1.0f,

									1.0f, 0.0f, 0.0f,
									0.0f, 1.0f, 0.0f,
									0.0f, 0.0f, 1.0f,

									1.0f, 0.0f, 0.0f,
									0.0f, 1.0f, 0.0f,
									0.0f, 0.0f, 1.0f,

									1.0f, 0.0f, 0.0f,
									0.0f, 1.0f, 0.0f,
									0.0f, 0.0f, 1.0f
								};

	const GLfloat cube_vertices[] =
									{
										//TOP
										 1.0f,1.0f,-1.0f,
										 -1.0f,1.0f,-1.0f,
										 -1.0f,1.0f,1.0f,					
										 1.0f,1.0f,1.0f,					
										//bottom
	 
										 1.0f,-1.0f,-1.0f,
										 -1.0f,-1.0f,-1.0f,				
										 -1.0f,-1.0f,1.0f,
										 1.0f, -1.0f, 1.0f,
										////Front
	 
										 1.0f,1.0f,1.0f,					
										 -1.0f,1.0f,1.0f,				
										 -1.0f,-1.0f,1.0f,				
										 1.0f,-1.0f,1.0f,
										//back
	 
										 1.0f,1.0f,-1.0f,				
										 -1.0f,1.0f,-1.0f,				
										 -1.0f,-1.0f,-1.0f,				
										 1.0f,-1.0f,-1.0f,

										//Right
	 
										 1.0f,1.0f,-1.0f,				
										 1.0f,1.0f,1.0f,					
										 1.0f,-1.0f,1.0f,				
										 1.0f,-1.0f,-1.0f,

										//left
	 
										 -1.0f,1.0f,1.0f,					
										 -1.0f,1.0f,-1.0f,					
										 -1.0f,-1.0f,-1.0f,					
										 -1.0f,-1.0f,1.0f
									};


	const GLfloat cube_color[] =
								{
									1.0f, 0.0f, 0.0f,
									1.0f, 0.0f, 0.0f,
									1.0f, 0.0f, 0.0f,
									1.0f, 0.0f, 0.0f,

									0.0f, 1.0f, 0.0f,
									0.0f, 1.0f, 0.0f,
									0.0f, 1.0f, 0.0f,
									0.0f, 1.0f, 0.0f,

									0.0f, 0.0f, 1.0f,
									0.0f, 0.0f, 1.0f,
									0.0f, 0.0f, 1.0f,
									0.0f, 0.0f, 1.0f,

									1.0f, 1.0f, 0.0f,
									1.0f, 1.0f, 0.0f,
						        		1.0f, 1.0f, 0.0f,
									1.0f, 1.0f, 0.0f,

									1.0f, 0.0f, 1.0f,
									1.0f, 0.0f, 1.0f,
									1.0f, 0.0f, 1.0f,
									1.0f, 0.0f, 1.0f,

									0.0f, 1.0f, 1.0f,
									0.0f, 1.0f, 1.0f,
									0.0f, 1.0f, 1.0f,
									0.0f, 1.0f, 1.0f,
								};
	//create vao for pyramid....
	//START:
	glGenVertexArrays(1, &vao_pyramid);
	

	glBindVertexArray(vao_pyramid);  //vao triangle bind
	

	glGenBuffers(1, &vbo_pyramid_position);

	glBindBuffer(GL_ARRAY_BUFFER,
				vbo_pyramid_position);

	glBufferData(GL_ARRAY_BUFFER,
				 sizeof(pyramid_vertices),
				 pyramid_vertices,
				 GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTES_POSITION,
							3,
							GL_FLOAT,
							GL_FALSE,
							0,
							NULL
							);

	glEnableVertexAttribArray(AMC_ATTRIBUTES_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	////color vbo triangle
	glGenBuffers(1, &vbo_pyramid_color);

	glBindBuffer(GL_ARRAY_BUFFER,
				 vbo_pyramid_color);

	glBufferData(GL_ARRAY_BUFFER,
				sizeof(pyramid_color),
				pyramid_color,
				GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTES_COLOR,
							3,
							GL_FLOAT,
							GL_FALSE,
							0,
							NULL);

	glEnableVertexAttribArray(AMC_ATTRIBUTES_COLOR);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);						//vao pyramid unbind...
	////END
	//////create vao for cube...
	//////START:
		glGenVertexArrays(1, &vao_cube);
	
		glBindVertexArray(vao_cube);

		glGenBuffers(1, &vbo_cube_position);	//vbo position attach cube

		glBindBuffer(GL_ARRAY_BUFFER,
					 vbo_cube_position);

		glBufferData(GL_ARRAY_BUFFER,
					 sizeof(cube_vertices),
					 cube_vertices,
					 GL_STATIC_DRAW);

		glVertexAttribPointer(AMC_ATTRIBUTES_POSITION,
								3,
								GL_FLOAT,
								GL_FALSE,
								0,
								NULL);

		glEnableVertexAttribArray(AMC_ATTRIBUTES_POSITION);

		glBindBuffer(GL_ARRAY_BUFFER, 0);		//Unbind vbo position py
		//cube vbo color
		glGenBuffers(1, &vbo_cube_color);

		glBindBuffer(GL_ARRAY_BUFFER,
					 vbo_cube_color);

		glBufferData(GL_ARRAY_BUFFER,
					 sizeof(cube_color),
					 cube_color,
					 GL_STATIC_DRAW);

		glVertexAttribPointer(AMC_ATTRIBUTES_COLOR,
								3,
								GL_FLOAT,
								GL_FALSE,
								0,
								NULL);

		glEnableVertexAttribArray(AMC_ATTRIBUTES_COLOR);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(0);
	
	///END
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);
        
        perspectiveProjectionMatrix = mat4::identity();

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
	
	perspectiveProjectionMatrix = perspective(45.0f,(GLfloat)width/(GLfloat)height, 0.1f, 100.0f);
	
	
	fprintf(gpfile,"resize END\n");
}

//
void display()
{
	//fprintf(gpfile,"Display START\n");
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(gShaderProgramObject);
	//write all code here
	//declare matrix
         mat4 modelViewMatrix;
	 mat4 modelViewProjectionMatrix;
	 mat4 translateMatrix;
	 mat4 rotationMatrix;
	//triangle
	//make identity matrix
	 modelViewMatrix = mat4::identity();
	 modelViewProjectionMatrix = mat4::identity();
	 translateMatrix = mat4::identity();
	 rotationMatrix = mat4::identity();

	 //Do transformations...
	 modelViewMatrix = translate(-1.8f,0.0f,-6.0f);
	 rotationMatrix = rotate(angle_pyramid, 0.0f , 1.0f, 0.0f);

	 modelViewProjectionMatrix = perspectiveProjectionMatrix  * modelViewMatrix;

	 modelViewProjectionMatrix = modelViewProjectionMatrix * rotationMatrix;
	 //send neccessary matrix to shader in respective uniforms

	 glUniformMatrix4fv(mvpUniform,				
						1,						
						GL_FALSE,				
						modelViewProjectionMatrix
						);

	 ////Bind with  vao
	 ////similarly bind with texture in any
	 glBindVertexArray(vao_pyramid);
	 
	 //draw neccessaary matrices
	 glDrawArrays(GL_TRIANGLES,    //what to draw from given array
					0,				//from where to start
					12);				//How many to draw from start

	
	 glBindVertexArray(0);
	

	//cube...
	 modelViewMatrix = mat4::identity();
	 modelViewProjectionMatrix = mat4::identity();
	 rotationMatrix = mat4::identity();

	 modelViewMatrix = translate(2.2f, 0.0f, -6.0f);
	 rotationMatrix = rotate(angle_cube, 0.0f, 0.0f, 1.0f);
	 rotationMatrix = rotationMatrix * rotate(angle_cube, 0.0f, 1.0f, 0.0f);
	 rotationMatrix = rotationMatrix * rotate(angle_cube, 1.0f, 0.0f, 0.0f);

	 modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	 modelViewProjectionMatrix = modelViewProjectionMatrix * rotationMatrix;

	 glUniformMatrix4fv(mvpUniform,
						1,
						GL_FALSE,
						modelViewProjectionMatrix);
		
	 glBindVertexArray(vao_cube);

	 glDrawArrays(GL_TRIANGLE_FAN,
					0,
					4);

	  glDrawArrays(GL_TRIANGLE_FAN,
					4,
					4);
	  
	   glDrawArrays(GL_TRIANGLE_FAN,
					8,
					4);

	    glDrawArrays(GL_TRIANGLE_FAN,
					12,
					4);

		 glDrawArrays(GL_TRIANGLE_FAN,
					16,
					4);

		  glDrawArrays(GL_TRIANGLE_FAN,
					20,
					4);

	 glBindVertexArray(0);

	glUseProgram(0);
	glXSwapBuffers(gpdisplay, gWindow);
	//fprintf(gpfile,"Display END\n");
}

void update()
{
	if(angle_cube == 360.0f)
	{
		angle_cube = 0.0f;
		angle_pyramid = 0.0f;
	}
	else
	{
		angle_cube += 0.11f;
		angle_pyramid += 0.11f;
	}
}

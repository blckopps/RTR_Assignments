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

//soil header
#include<SOIL/SOIL.h>

//namespace
using namespace std;
using namespace vmath;

//global variables


Display *gpdisplay = NULL;
XVisualInfo *gpXVisualInfo  = NULL;
Colormap gColormap;
Window gWindow;

int giWindowWidth = 600;
int giWindowHeight = 800;

//
bool bFullScreen = false;
bool bIsLighting = false;
bool bIsAnimation = false;

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
GLuint vao;
GLuint vbo;
GLuint vbo_normal;
GLfloat angle_cube = 0.0f;

GLuint mvUniform;
GLuint projectionUnform;
GLuint ldUniform;
GLuint kdUniform;
GLuint lightPositionUniform;
GLuint isLKeyIsPressed;

mat4 perspectiveProjectionMatrix;
mat4 modelViewMatrix;
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
		                                
		                                case 'l':
		                                case 'L':
			                                if (bIsLighting == false)
			                                {
				                                bIsLighting = true;

			                                }
			                                else
			                                {
				                                bIsLighting = false;
			                                }
			                                break;
			                                
			                                case 'a':
		                                        case'A':
			                                if (bIsAnimation == false)
			                                {
				                                bIsAnimation = true;

			                                }
			                                else
			                                {
				                                bIsAnimation = false;
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
         XStoreName(gpdisplay, gWindow, "CUBE DIFUSE LIGHT");
         
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
	
	if (vbo)
	{
		glDeleteBuffers(1, &vbo);
		vbo = 0;
	}

	if (vao)
	{
		glDeleteVertexArrays(1, &vao);
		vao = 0;
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
	
	//shader objects
	GLuint gVertexShaderObject;
	GLuint gFragmentShaderObject;
	
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
		"in vec3 vNormal;" \
		"uniform mat4 u_mv_matrix;" \
		"uniform mat4 u_projection_matrix;" \
		"uniform int u_islkeypressed;" \
		"uniform vec3 u_ld;" \
		"uniform vec3 u_kd;" \
		"uniform vec4 u_light_position;" \
		"out vec3 difuse_color;" \
		"void main(void)" \
		"{" \
		"if(u_islkeypressed == 1)" \
		"{" \
		"vec4 eye_coordinates = u_mv_matrix * vPosition;" \
		"mat3 normal_matrix = mat3(transpose(inverse(u_mv_matrix)));" \
		"vec3 tnorm = normalize(normal_matrix * vNormal);" \
		"vec3 s = normalize(vec3(u_light_position - eye_coordinates));" \
		"difuse_color = u_ld * u_kd * dot(s, tnorm);" \
		"}" \
		"gl_Position =  u_projection_matrix *  u_mv_matrix * vPosition;" \
		" } ";
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
		"in vec3 difuse_color;" \
		"uniform int u_islkeypressed;" \
		"void main(void)" \
		"{" \
		"if(u_islkeypressed == 1)" \
		"{" \
		"fragColor = vec4(difuse_color, 1.0f);" \
		"}" \
		"else" \
		"{" \
		"fragColor = vec4(1.0, 1.0, 1.0, 1.0f);" \
		"}" \
		"}";
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
				AMC_ATTRIBUTES_NORMAL,
				"vNormal");
											
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
	mvUniform = glGetUniformLocation(gShaderProgramObject,
		                                "u_mv_matrix");

	projectionUnform = glGetUniformLocation(gShaderProgramObject,
		                                "u_projection_matrix");

	isLKeyIsPressed = glGetUniformLocation(gShaderProgramObject,
		                                "u_islkeypressed");

	ldUniform = glGetUniformLocation(gShaderProgramObject,
		                                "u_ld");


	kdUniform = glGetUniformLocation(gShaderProgramObject,
		                                "u_kd");

	lightPositionUniform = glGetUniformLocation(gShaderProgramObject,
		                                        "u_light_position");	
	//arrays

	fprintf(gpfile,"Post link success!!\n");

	const GLfloat cubeVertices[] =
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

	const GLfloat normal_vertices[] =
	{
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,

		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,

		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,

		0.0f, 0.0f,-1.0f,
		0.0f, 0.0f,-1.0f,
		0.0f, 0.0f,-1.0f,
		0.0f, 0.0f,-1.0f,

		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f
	};
	//create vao for rectangle...
	//START:
	glGenVertexArrays(1, &vao);

		glBindVertexArray(vao);

		glGenBuffers(1, &vbo);

		glBindBuffer(GL_ARRAY_BUFFER,
			vbo);

		glBufferData(GL_ARRAY_BUFFER,
						sizeof(cubeVertices),
						cubeVertices,
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

	//bind with normal vbo
	glGenBuffers(1, &vbo_normal);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_normal);

	glBufferData(GL_ARRAY_BUFFER,
					sizeof(normal_vertices),
					normal_vertices,
					GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTES_NORMAL,
							3,
							GL_FLOAT,
							GL_FALSE,
							0,
							NULL);

	glEnableVertexAttribArray(AMC_ATTRIBUTES_NORMAL);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);

	perspectiveProjectionMatrix = mat4::identity();
	modelViewMatrix = mat4::identity();

        
	resize(giWindowWidth, giWindowHeight );

	fprintf(gpfile,"initialize END\n");
}
//
void resize(int width, int height)
{
	if (height == 0)
	{
		height = 1;
	}
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	perspectiveProjectionMatrix =
		perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
	
	fprintf(gpfile,"resize END\n");
}

//
void display()
{
	//fprintf(gpfile,"Display START\n");
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(gShaderProgramObject);

	mat4 rotationMatrix;

	modelViewMatrix = mat4::identity();
   
	rotationMatrix = mat4::identity();

	//Do transformations...
	modelViewMatrix = modelViewMatrix * translate(0.0f, 0.0f, -5.0f);


	rotationMatrix = rotate(angle_cube, 0.0f, 0.0f, 1.0f);
	rotationMatrix = rotationMatrix * rotate(angle_cube, 0.0f, 1.0f, 0.0f);
	rotationMatrix = rotationMatrix * rotate(angle_cube, 1.0f, 0.0f, 0.0f);

	modelViewMatrix = modelViewMatrix * rotationMatrix;
	

	if (bIsLighting == true)
	{
		glUniform1i(isLKeyIsPressed, 1);

		glUniform3f(ldUniform, 1.0f, 1.0f, 1.0f);

		glUniform3f(kdUniform, 0.5f, 0.5f, 0.5f);

		glUniform4f(lightPositionUniform, 0.0f, 0.0f, 2.0f, 1.0f);

	}
	else
	{
		glUniform1i(isLKeyIsPressed, 0);
	}

	glUniformMatrix4fv(mvUniform,
						1,
						GL_FALSE,
						modelViewMatrix
						);

	glUniformMatrix4fv(projectionUnform,
						1,
						GL_FALSE,
						perspectiveProjectionMatrix
						);

	//Bind with  vao
	//similarly bind with texture in any
	glBindVertexArray(vao);

	//draw neccessaary matrices
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
	if (bIsAnimation == true)
	{
		if (angle_cube == 360.0f)
		{
			angle_cube = 0.0f;

		}
		else
		{
			angle_cube += 0.05f;

		}
	}
}


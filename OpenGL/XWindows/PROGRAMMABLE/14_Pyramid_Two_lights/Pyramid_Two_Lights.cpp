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

GLuint vao_pyramid;
GLuint vbo_position_pyramid;
GLuint vbo_normals_pyramid;


//Transformation matrix uniforms
GLuint modelMatrixUniform;
GLuint viewMatrixUniform;
GLuint projectionUniform;
mat4 perspectiveProjectionMatrix;

//
//shader uniforms
GLuint isLKeyIsPressedUniforms;


//light uniforms
GLuint laUniform_Red;
GLuint ldUniform_Red;
GLuint lsUniform_Red;

GLuint laUniform_Blue;
GLuint ldUniform_Blue;
GLuint lsUniform_Blue;

GLuint kaUniform;
GLuint kdUniform;
GLuint ksUniform;

GLuint lightPositionUniform_Red;
GLuint lightPositionUniform_Blue;


GLuint shininessUniform;

//light array RED
float lightAmbient_Red[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
float lightDifuse_Red[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
float lightSpecular_Red[4] = { 1.0f, 0.0f, 0.0f, 1.0f };

//light array BLUE
float lightAmbient_Blue[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
float lightDifuse_Blue[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
float lightSpecular_Blue[4] = { 0.0f, 0.0f, 1.0f, 1.0f };

//material array
float materialAmbient[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
float materialDifuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
float materialSpecular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

float lightPosition_Red[4] = { 100.0f, 100.0f, 100.0f, 1.0f };
float lightPosition_Blue[4] = { -100.0f, 100.0f, 100.0f, 1.0f };

float materialShininess = 128.0f;



bool isLighting = false;
bool isAnimation = false;
GLfloat angle_pyramid = 0.0f;

//sphere var
float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_textures[764];
unsigned short sphere_elements[2280];
GLsizei gNumVertices, gNumElements;
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
			                                if (isLighting == false)
			                                {
				                                isLighting = true;

			                                }
			                                else
			                                {
				                                isLighting = false;
			                                }
			                                break;

			                              case 'A':
										  case 'a':
												if (isAnimation == false)
												{
													isAnimation = true;

												}
												else
												{
													isAnimation = false;
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
		
		display();
		update();
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
         XStoreName(gpdisplay, gWindow, "PYRAMID TWO LIGHTS");
         
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
	
		if (vbo_normals_pyramid)
	{
		glDeleteVertexArrays(1, &vbo_normals_pyramid);
		vbo_normals_pyramid = 0;
	}

	if (vbo_position_pyramid)
	{
		glDeleteVertexArrays(1, &vbo_position_pyramid);
		vbo_position_pyramid = 0;
	}
	////shader uninitialize code
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
		"uniform mat4 u_model_matrix;" \
		"uniform mat4 u_view_matrix;" \
		"uniform mat4 u_projection_matrix;" \
		"uniform int islkeypressed;" \
		"uniform vec3 u_la_red;" \
		"uniform vec3 u_ld_red;" \
		"uniform vec3 u_ls_red;" \
		"uniform vec3 u_la_blue;" \
		"uniform vec3 u_ld_blue;" \
		"uniform vec3 u_ls_blue;" \
		"uniform vec3 u_ka;" \
		"uniform vec3 u_kd;" \
		"uniform vec3 u_ks;" \
		"uniform float u_shininess;" \
		"uniform vec4 u_light_position_red;" \
		"uniform vec4 u_light_position_blue;" \
		"out vec3 phong_ads_light;" \
		"void main(void)" \
		"{" \
			"if(islkeypressed == 1)" \
			"{" \
				"vec4 eye_coordinates = u_view_matrix *  u_model_matrix  * vPosition;" \
				"vec3 tnorm = normalize(mat3(u_view_matrix * u_model_matrix) * vNormal);" \

				"vec3 light_direction_red = normalize(vec3(u_light_position_red - eye_coordinates));" \
				"vec3 light_direction_blue = normalize(vec3(u_light_position_blue - eye_coordinates));" \

				"float tn_dot_ldirection_red = max(dot(light_direction_red, tnorm), 0);" \
				"float tn_dot_ldirection_blue = max(dot(light_direction_blue, tnorm), 0);" \

				"vec3 reflection_vector_red = reflect(-light_direction_red, tnorm);" \
				"vec3 reflection_vector_blue = reflect(-light_direction_blue, tnorm);" \

				"vec3 viewer_vector = normalize(vec3(-eye_coordinates));" \

				"vec3 ambient_red = u_la_red * u_ka;" \
				"vec3 ambient_blue = u_la_blue * u_ka;" \
				
				"vec3 difuse_red = u_ld_red * u_kd * tn_dot_ldirection_red;" \
				"vec3 difuse_blue = u_ld_blue * u_kd * tn_dot_ldirection_blue;" \

				"vec3 specular_red = u_ls_red * u_ks * pow(max(dot(reflection_vector_red,viewer_vector),0),u_shininess);" \
				"vec3 specular_blue = u_ls_blue * u_ks * pow(max(dot(reflection_vector_blue,viewer_vector),0),u_shininess);" \
				
				"vec3 redLight = ambient_red + difuse_red + specular_red;" \
				"vec3 blueLight = ambient_blue + difuse_blue + specular_blue;" \

				"phong_ads_light = redLight + blueLight;" \
			"}" \
			"else" \
			"{" \
				"phong_ads_light = vec3(1.0, 1.0, 1.0);" \
			"}" \
			"gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" \
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
		"uniform int islkeypressed;" \
		"in vec3 phong_ads_light;" \
		"void main(void)" \
		"{" \
			"fragColor = vec4(phong_ads_light, 1.0);" \
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
	//getting light uniforms locations
	isLKeyIsPressedUniforms = glGetUniformLocation(gShaderProgramObject,"islkeypressed");

	//RED light uniforms
	laUniform_Red = glGetUniformLocation(gShaderProgramObject, "u_la_red");
	ldUniform_Red = glGetUniformLocation(gShaderProgramObject, "u_ld_red");
	lsUniform_Red = glGetUniformLocation(gShaderProgramObject, "u_ls_red");

	//BLUE light unifomrs
	laUniform_Blue = glGetUniformLocation(gShaderProgramObject, "u_la_blue");
	ldUniform_Blue = glGetUniformLocation(gShaderProgramObject, "u_ld_blue");
	lsUniform_Blue = glGetUniformLocation(gShaderProgramObject, "u_ls_blue");

	kaUniform = glGetUniformLocation(gShaderProgramObject, "u_ka");
	kdUniform = glGetUniformLocation(gShaderProgramObject, "u_kd");
	ksUniform = glGetUniformLocation(gShaderProgramObject, "u_ks");

	shininessUniform = glGetUniformLocation(gShaderProgramObject, "u_shininess");

	lightPositionUniform_Red = glGetUniformLocation(gShaderProgramObject, "u_light_position_red");
	lightPositionUniform_Blue = glGetUniformLocation(gShaderProgramObject, "u_light_position_blue");

	//getting transformation matrices uniform locations
	modelMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_model_matrix");

	viewMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_view_matrix");

	projectionUniform = glGetUniformLocation(gShaderProgramObject, "u_projection_matrix");
	//arrays

	fprintf(gpfile,"Post link success!!\n");

	//VAO AND VBO
	glGenVertexArrays(1, &vao_pyramid);

	glBindVertexArray(vao_pyramid);

	//position
	const GLfloat pyramid_vertices[] =
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

	glGenBuffers(1, &vbo_position_pyramid);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_pyramid);

	glBufferData(GL_ARRAY_BUFFER,
					sizeof(pyramid_vertices),
					pyramid_vertices,
					GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTES_POSITION,
							3,
							GL_FLOAT,
							GL_FALSE,
							0,
							NULL);

	glEnableVertexAttribArray(AMC_ATTRIBUTES_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//normal 
	const GLfloat pyramid_Normals[] =
	{
				0.0f, 0.447214f, 0.894427f,
				0.0f, 0.447214f, 0.894427f,
				0.0f, 0.447214f, 0.894427f,
				0.894427f, 0.447214f, 0.0f,
				0.894427f, 0.447214f, 0.0f,
				0.894427f, 0.447214f, 0.0f,
				0.0f ,0.447214f, -0.894427f,
				0.0f ,0.447214f, -0.894427f,
				0.0f ,0.447214f, -0.894427f,
				-0.894427f, 0.447214f, 0.0f,
				-0.894427f, 0.447214f, 0.0f,
				-0.894427f, 0.447214f, 0.0f

	};
	glGenBuffers(1, &vbo_normals_pyramid);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_normals_pyramid);

	glBufferData(GL_ARRAY_BUFFER,
					sizeof(pyramid_Normals),
					pyramid_Normals,
					GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTES_NORMAL,
							3,
							GL_FLOAT,
							GL_FALSE,
							0,
							NULL);

	glEnableVertexAttribArray(AMC_ATTRIBUTES_NORMAL);

	glBindBuffer(GL_ARRAY_BUFFER, 0);


	//unbind vao
	glBindVertexArray(0);

	//START:

	//END:
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

	//declare matrices
	mat4 viewMatrix;
	mat4 modelMatrix;
	mat4 rotateMatrix;

	//make identity matrices
	viewMatrix = mat4::identity();
	modelMatrix = mat4::identity();
	rotateMatrix = mat4::identity();

	//Do transformations...
	modelMatrix = translate(0.0f, 0.0f, -6.0f);
	rotateMatrix = rotate(angle_pyramid, 0.0f, 1.0f, 0.0f);
	modelMatrix = modelMatrix * rotateMatrix;

	//send neccessary matrix to shader in respective uniforms

	//key events uniforms

	if (isLighting == true)
	{
		glUniform1i(isLKeyIsPressedUniforms, 1);

		glUniform1f(shininessUniform, materialShininess);

		//RED light
		glUniform3fv(laUniform_Red, 1, lightAmbient_Red);
		glUniform3fv(ldUniform_Red, 1, lightDifuse_Red);
		glUniform3fv(lsUniform_Red, 1, lightSpecular_Red);

		//Blue
		glUniform3fv(laUniform_Blue, 1, lightAmbient_Blue);
		glUniform3fv(ldUniform_Blue, 1, lightDifuse_Blue);
		glUniform3fv(lsUniform_Blue, 1, lightSpecular_Blue);

		//material
		glUniform3fv(kaUniform, 1, materialAmbient);
		glUniform3fv(kdUniform, 1, materialDifuse);
		glUniform3fv(ksUniform, 1, materialSpecular);

		glUniform4fv(lightPositionUniform_Red, 1, lightPosition_Red);
		glUniform4fv(lightPositionUniform_Blue, 1, lightPosition_Blue);
	}
	else
	{
		glUniform1i(isLKeyIsPressedUniforms, 0);
	}

	glUniformMatrix4fv(modelMatrixUniform,
						1,
						GL_FALSE,
						modelMatrix
						);

	glUniformMatrix4fv(viewMatrixUniform,
						1,
						GL_FALSE,
						viewMatrix
						);

	glUniformMatrix4fv(projectionUniform,
						1,
						GL_FALSE,
						perspectiveProjectionMatrix);
	//Bind with  vao
	//similarly bind with texture in any
	glBindVertexArray(vao_pyramid);
	
	glDrawArrays(GL_TRIANGLES,    //what to draw from given array
					0,				//from where to start
					12);

	glBindVertexArray(0);

	glUseProgram(0);
	glXSwapBuffers(gpdisplay, gWindow);
	//fprintf(gpfile,"Display END\n");
}

void update(void)
{
	if (angle_pyramid >= 360.0f)
	{
		angle_pyramid = 0.0f;
	}
	else
	{
		angle_pyramid += 0.03f;
	}
}


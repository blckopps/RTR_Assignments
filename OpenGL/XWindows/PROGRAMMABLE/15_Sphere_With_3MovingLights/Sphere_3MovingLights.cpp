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
#include"Sphere.h"
//#pragma comment(lib,"Sphere.lib")
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
GLfloat angle_X = 0.0f;

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

GLuint gShaderProgramObject_PV;
GLuint gShaderProgramObject_PF;

GLuint vao_sphere;
GLuint vbo_position_vbo;
GLuint vbo_normals_vbo;
GLuint vbo_element_vbo;

//Transformation matrix uniforms
GLuint modelMatrixUniform_PV;
GLuint viewMatrixUniform_PV;
GLuint projectionUniform_PV;

GLuint modelMatrixUniform_PF;
GLuint viewMatrixUniform_PF;
GLuint projectionUniform_PF;
mat4 perspectiveProjectionMatrix;

/////////////*******shader uniforms for per vertex*******///////////////////

GLuint isLKeyIsPressedUniforms_PV;
GLuint shininessUniform_PV;
//RED COLOR
GLuint laUniform_PV_RED;
GLuint ldUniform_PV_RED;
GLuint lsUniform_PV_RED;

GLuint lightPositionUniform_PV_RED;
//GREEN COLOR
GLuint laUniform_PV_GREEN;
GLuint ldUniform_PV_GREEN;
GLuint lsUniform_PV_GREEN;

GLuint lightPositionUniform_PV_GREEN;
//BLUE COLOR
GLuint laUniform_PV_BLUE;
GLuint ldUniform_PV_BLUE;
GLuint lsUniform_PV_BLUE;

GLuint lightPositionUniform_PV_BLUE;;
//Material Per Vertex 
GLuint kaUniform_PV;
GLuint kdUniform_PV;
GLuint ksUniform_PV;

////////////******shader uniforms for per fragment****/////////////
GLuint isLKeyIsPressedUniforms_PF;
GLuint shininessUniform_PF;
//RED COLOR
GLuint laUniform_PF_RED;
GLuint ldUniform_PF_RED;
GLuint lsUniform_PF_RED;

GLuint lightPositionUniform_PF_RED;
//GREEN COLOR
GLuint laUniform_PF_GREEN;
GLuint ldUniform_PF_GREEN;
GLuint lsUniform_PF_GREEN;

GLuint lightPositionUniform_PF_GREEN;
//BLUE COLOR
GLuint laUniform_PF_BLUE;
GLuint ldUniform_PF_BLUE;
GLuint lsUniform_PF_BLUE;

GLuint kaUniform_PF;
GLuint kdUniform_PF;
GLuint ksUniform_PF;

GLuint lightPositionUniform_PF_BLUE;;

//light array Red
float lightAmbient_Red[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
float lightDifuse_Red[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
float lightSpecular_Red[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
//light array Green
//
float lightAmbient_Green[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
float lightDifuse_Green[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
float lightSpecular_Green[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
//light array Blue
float lightAmbient_Blue[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
float lightDifuse_Blue[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
float lightSpecular_Blue[4] = { 0.0f, 0.0f, 1.0f, 1.0f };

//material array
float materialAmbient[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
float materialDifuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
float materialSpecular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

float lightPosition_Red[4];
float lightPosition_Green[4];
float lightPosition_Blue[4];

float materialShininess = 50.0f;


bool isLighting = false;
bool isPerVertex = true;
bool isPerFragment = false;
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
        void update(void);
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
		                                
					                        if (isPerFragment == false)
											{
												isPerFragment = true;

												isPerVertex = false;

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
			                                
			                                case 'v':
											case 'V':
												if (isPerVertex == false)
												{
													isPerVertex = true;
													isPerFragment = false;

												}
												break;

											case 'q':
											case 'Q':
												bDone = true;
		                                        Uninitialize();
		                                        exit(0);
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
         XStoreName(gpdisplay, gWindow, "SPHERE 3 MOVING LIGHTS");
         
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

	int uninitialize2(GLuint);

	if (vbo_element_vbo)
	{
		glDeleteBuffers(1, &vbo_element_vbo);
		vbo_element_vbo = 0;
	}

	if (vbo_normals_vbo)
	{
		glDeleteVertexArrays(1, &vbo_normals_vbo);
		vbo_normals_vbo = 0;
	}

	if (vbo_position_vbo)
	{
		glDeleteVertexArrays(1, &vbo_position_vbo);
		vbo_position_vbo = 0;
	}
	//	////shader uninitialize code
	int ret = 0;

	ret = uninitialize2(gShaderProgramObject_PV);
	ret = uninitialize2(gShaderProgramObject_PF);

	if (ret != 0)
	{
		fprintf(gpfile, "\nFaild uninitialize2\n");
	}
	else
	{
		fprintf(gpfile, " \nuninitialize2 successfull\n");
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

int uninitialize2(GLuint shaderObject)
{
	GLsizei shaderCount;
	GLsizei shaderNumber;

	if (shaderObject)
	{
		glUseProgram(shaderObject);

		glGetProgramiv(shaderObject,
			GL_ATTACHED_SHADERS,
			&shaderCount);


		GLuint *pShaders = (GLuint*)malloc(sizeof(GLuint)*shaderCount);

		if (pShaders)
		{
			glGetAttachedShaders(shaderObject,
				shaderCount,
				&shaderCount,
				pShaders);

			for (shaderNumber = 0; shaderNumber < shaderCount; shaderNumber++)
			{
				glDetachShader(shaderObject,
					pShaders[shaderNumber]);

				glDeleteShader(pShaders[shaderNumber]);

				pShaders[shaderNumber] = 0;
			}
			free(pShaders);
		}
		else
		{
			fprintf(gpfile, "\nUninitialize2 : Failed to alloc memory\n");
			return 1;
		}

		glDeleteProgram(shaderObject);

		shaderObject = 0;

		glUseProgram(0);
	}
	return 0;
}
//
void initialize(void)
{
	fprintf(gpfile,"initialize START\n");
	void Uninitialize(void);
	void resize(int, int);
	
	//shader objects for per vertex and per fragment
	GLuint gVertexShaderObject_PV;
	GLuint gVertexShaderObject_PF;

	GLuint gFragmentShaderObject_PV;
	GLuint gFragmentShaderObject_PF;

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
	gVertexShaderObject_PV = glCreateShader(GL_VERTEX_SHADER);
	gVertexShaderObject_PF = glCreateShader(GL_VERTEX_SHADER);
	//write vertex shader code
	 const GLchar* vertexShaderSourceCode_PV =
		"#version 430 core" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec3 vNormal;" \

		"uniform mat4 u_model_matrix;" \
		"uniform mat4 u_view_matrix;" \
		"uniform mat4 u_projection_matrix;" \
		"uniform int islkeypressed_PV;" \
		//RED
		"uniform vec3 u_la_PV_RED;" \
		"uniform vec3 u_ld_PV_RED;" \
		"uniform vec3 u_ls_PV_RED;" \
		//GREEN
		"uniform vec3 u_la_PV_GREEN;" \
		"uniform vec3 u_ld_PV_GREEN;" \
		"uniform vec3 u_ls_PV_GREEN;" \
		//BLUE
		"uniform vec3 u_la_PV_BLUE;" \
		"uniform vec3 u_ld_PV_BLUE;" \
		"uniform vec3 u_ls_PV_BLUE;" \
		//Material
		"uniform vec3 u_ka_PV;" \
		"uniform vec3 u_kd_PV;" \
		"uniform vec3 u_ks_PV;" \

		"uniform float u_shininess_PV;" \

		"uniform vec4 u_light_position_PV_RED;" \
		"uniform vec4 u_light_position_PV_GREEN;" \
		"uniform vec4 u_light_position_PV_BLUE;" \

		"out vec3 phong_ads_light_PV;" \
		"void main(void)" \
		"{" \
			"if(islkeypressed_PV == 1)" \
			"{" \
				"vec4 eye_coordinates = u_view_matrix *  u_model_matrix  * vPosition;" \
				"vec3 tnorm = normalize(mat3(u_view_matrix * u_model_matrix) * vNormal);" \

				"vec3 light_direction_RED = normalize(vec3(u_light_position_PV_RED - eye_coordinates));" \
				"vec3 light_direction_BLUE = normalize(vec3(u_light_position_PV_BLUE - eye_coordinates));" \
				"vec3 light_direction_GREEN = normalize(vec3(u_light_position_PV_GREEN - eye_coordinates));" \

				"float tn_dot_ldirection_RED = max(dot(light_direction_RED, tnorm), 0);" \
				"float tn_dot_ldirection_GREEN = max(dot(light_direction_GREEN, tnorm), 0);" \
				"float tn_dot_ldirection_BLUE = max(dot(light_direction_BLUE, tnorm), 0);" \

				"vec3 reflection_vector_RED = reflect(-light_direction_RED, tnorm);" \
				"vec3 reflection_vector_GREEN = reflect(-light_direction_GREEN, tnorm);" \
				"vec3 reflection_vector_BLUE = reflect(-light_direction_BLUE, tnorm);" \

				"vec3 viewer_vector = normalize(vec3(-eye_coordinates));" \

				"vec3 ambient_RED = u_la_PV_RED * u_ka_PV;" \
				"vec3 ambient_GREEN = u_la_PV_GREEN * u_ka_PV;" \
				"vec3 ambient_BLUE = u_la_PV_BLUE * u_ka_PV;" \

				"vec3 difuse_RED = u_ld_PV_RED * u_kd_PV * tn_dot_ldirection_RED;" \
				"vec3 difuse_GREEN = u_ld_PV_GREEN * u_kd_PV * tn_dot_ldirection_GREEN;" \
				"vec3 difuse_BLUE = u_ld_PV_BLUE * u_kd_PV * tn_dot_ldirection_BLUE;" \

				"vec3 specular_RED = u_ls_PV_RED * u_ks_PV * pow(max(dot(reflection_vector_RED, viewer_vector),0),u_shininess_PV);" \
				"vec3 specular_GREEN = u_ls_PV_GREEN * u_ks_PV * pow(max(dot(reflection_vector_GREEN, viewer_vector),0),u_shininess_PV);" \
				"vec3 specular_BLUE = u_ls_PV_BLUE * u_ks_PV * pow(max(dot(reflection_vector_BLUE, viewer_vector),0),u_shininess_PV);" \
				
				"vec3 redLight = ambient_RED + difuse_RED + specular_RED;" \
				"vec3 blueLight = ambient_BLUE + difuse_BLUE + specular_BLUE;" \
				"vec3 greenLight = ambient_GREEN + difuse_GREEN + specular_GREEN;" \

				"phong_ads_light_PV = redLight + blueLight + greenLight;" \
			"}" \
			"else" \
			"{" \
				"phong_ads_light_PV = vec3(1.0, 1.0, 1.0);" \
			"}" \
			"gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" \
		" } ";
	//vertex shader for per fragment..
	const GLchar* vertexShaderSourceCode_PF =
		"#version 430 core" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec3 vNormal;" \

		"uniform mat4 u_model_matrix;" \
		"uniform mat4 u_view_matrix;" \
		"uniform mat4 u_projection_matrix;" \
		"uniform int islkeypressed_PF;" \

		"uniform vec4 u_light_position_PF_RED;" \
		"uniform vec4 u_light_position_PF_GREEN;" \
		"uniform vec4 u_light_position_PF_BLUE;" \

		"out vec3 tnorm_PF;" \
		
		"out vec3 light_direction_PF_RED;" \
		"out vec3 light_direction_PF_GREEN;" \
		"out vec3 light_direction_PF_BLUE;" \

		"out vec3 viewer_vector_PF;" \
		"void main(void)" \
		"{" \
			"if(islkeypressed_PF == 1)" \
			"{" \
				"vec4 eye_coordinates = u_view_matrix *  u_model_matrix  * vPosition;" \

				"tnorm_PF = mat3(u_view_matrix * u_model_matrix) * vNormal;" \

				"light_direction_PF_RED = vec3(u_light_position_PF_RED - eye_coordinates);" \
				"light_direction_PF_GREEN = vec3(u_light_position_PF_GREEN - eye_coordinates);" \
				"light_direction_PF_BLUE = vec3(u_light_position_PF_BLUE - eye_coordinates);" \

				"viewer_vector_PF = vec3(-eye_coordinates.xyz);" \
			"}" \
			"gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" \
		" } ";
	//specify vertex source code to vetex shader object
	glShaderSource(gVertexShaderObject_PV,
					1,
					(GLchar **)&vertexShaderSourceCode_PV,
					NULL);

	glShaderSource(gVertexShaderObject_PF,
					1,
					(GLchar **)&vertexShaderSourceCode_PF,
					NULL);
	//compile vertex shader code
	glCompileShader(gVertexShaderObject_PV);

	//Error checking
	GLint iShaderCompilationStatus = 0;
	GLint iInfoLength = 0;
	GLchar *szInfoLog = NULL;

	glGetShaderiv(gVertexShaderObject_PV,
					GL_COMPILE_STATUS,
					&iShaderCompilationStatus);

	if(iShaderCompilationStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject_PV,
						GL_INFO_LOG_LENGTH,
						&iInfoLength);
		if(iInfoLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLength);

			if(szInfoLog != NULL)
			{
				GLsizei writtened;

				glGetShaderInfoLog(gVertexShaderObject_PV,
									iInfoLength,
									&writtened,
									szInfoLog);
				fprintf(gpfile,"ERROR:VERTEX SHADER PV:%s",szInfoLog);
				free(szInfoLog);
				Uninitialize();
				
				exit(0);
			}
		}
	}

	

	//compile vertex shader code per fragment
	glCompileShader(gVertexShaderObject_PF);

	iShaderCompilationStatus = 0;
	iInfoLength = 0;
	szInfoLog = NULL;

	glGetShaderiv(gVertexShaderObject_PF,
					GL_COMPILE_STATUS,
					&iShaderCompilationStatus);

	if(iShaderCompilationStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject_PF,
						GL_INFO_LOG_LENGTH,
						&iInfoLength);
		if(iInfoLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLength);

			if(szInfoLog != NULL)
			{
				GLsizei writtened;

				glGetShaderInfoLog(gVertexShaderObject_PF,
									iInfoLength,
									&writtened,
									szInfoLog);
				fprintf(gpfile,"ERROR:VERTEX SHADER PF:%s",szInfoLog);
				free(szInfoLog);
				Uninitialize();
				
				exit(0);
			}
		}
	}

	//********************FRAGMENT SHADER*****************
	gFragmentShaderObject_PV = glCreateShader(GL_FRAGMENT_SHADER);
	gFragmentShaderObject_PF = glCreateShader(GL_FRAGMENT_SHADER);
	//write vertex shader code
	const GLchar * fragmentShaderSourceCode_PV =
		"#version 430 core" \
		"\n" \
		"out vec4 fragColor;" \
		"in vec3 phong_ads_light_PV;" \
		"void main(void)" \
		"{" \
			
			"fragColor = vec4(phong_ads_light_PV, 1.0);" \
			
		"}";
	//fragment shader for per fragment.
	const GLchar * fragmentShaderSourceCode_PF =
		"#version 430 core" \
		"\n" \
		"out vec4 fragColor;" \
		"uniform int islkeypressed_PF;" \

		"uniform vec3 u_la_PF_RED;" \
		"uniform vec3 u_la_PF_GREEN;" \
		"uniform vec3 u_la_PF_BLUE;" \

		"uniform vec3 u_ld_PF_RED;" \
		"uniform vec3 u_ld_PF_GREEN;" \
		"uniform vec3 u_ld_PF_BLUE;" \

		"uniform vec3 u_ls_PF_RED;" \
		"uniform vec3 u_ls_PF_GREEN;" \
		"uniform vec3 u_ls_PF_BLUE;" \

		"uniform vec3 u_ka_PF;" \
		"uniform vec3 u_kd_PF;" \
		"uniform vec3 u_ks_PF;" \

		"in vec3 tnorm_PF;" \

		"in vec3 light_direction_PF_RED;" \
		"in vec3 light_direction_PF_GREEN;" \
		"in vec3 light_direction_PF_BLUE;" \

		"in vec3 viewer_vector_PF;" \
		"uniform float u_shininess_PF;" \
		"void main(void)" \
		"{" \
			"if(islkeypressed_PF == 1)" \
			"{" \
				"vec3 tnorm_normalized = normalize(tnorm_PF);" \

				"vec3 light_direction_normalized_RED = normalize(light_direction_PF_RED);" \
				"vec3 light_direction_normalized_GREEN = normalize(light_direction_PF_GREEN);" \
				"vec3 light_direction_normalized_BLUE = normalize(light_direction_PF_BLUE);" \

				"vec3 viewer_vector_normalized = normalize(viewer_vector_PF);" \

				"float tn_dot_ldirection_RED = max(dot(light_direction_normalized_RED, tnorm_normalized), 0);" \
				"float tn_dot_ldirection_GREEN = max(dot(light_direction_normalized_GREEN, tnorm_normalized), 0);" \
				"float tn_dot_ldirection_BLUE = max(dot(light_direction_normalized_BLUE, tnorm_normalized), 0);" \

				"vec3 reflection_vector_RED = reflect(-light_direction_normalized_RED, tnorm_normalized);" \
				"vec3 reflection_vector_GREEN = reflect(-light_direction_normalized_GREEN, tnorm_normalized);" \
				"vec3 reflection_vector_BLUE = reflect(-light_direction_normalized_BLUE, tnorm_normalized);" \

				"vec3 ambient_RED = u_la_PF_RED * u_ka_PF;" \
				"vec3 ambient_GREEN = u_la_PF_GREEN * u_ka_PF;" \
				"vec3 ambient_BLUE = u_la_PF_BLUE * u_ka_PF;" \

				"vec3 difuse_RED = u_ld_PF_RED * u_kd_PF * tn_dot_ldirection_RED;" \
				"vec3 difuse_GREEN = u_ld_PF_GREEN * u_kd_PF * tn_dot_ldirection_GREEN;" \
				"vec3 difuse_BLUE = u_ld_PF_BLUE * u_kd_PF * tn_dot_ldirection_BLUE;" \

				"vec3 specular_RED = u_ls_PF_RED * u_ks_PF * pow(max(dot(reflection_vector_RED, viewer_vector_normalized),0),u_shininess_PF);" \
				"vec3 specular_GREEN = u_ls_PF_GREEN * u_ks_PF * pow(max(dot(reflection_vector_GREEN, viewer_vector_normalized),0),u_shininess_PF);" \
				"vec3 specular_BLUE = u_ls_PF_BLUE * u_ks_PF * pow(max(dot(reflection_vector_BLUE, viewer_vector_normalized),0),u_shininess_PF);" \

				"vec3 redLight = ambient_RED + difuse_RED + specular_RED;" \
				"vec3 greenLight = ambient_GREEN + difuse_GREEN + specular_GREEN;" \
				"vec3 blueLight = ambient_BLUE + difuse_BLUE + specular_BLUE;" \


				"vec3 phong_light_pf = redLight + greenLight + blueLight;" \
				"fragColor = vec4(phong_light_pf, 1.0);" \

			"}" \
			"else" \
			"{" \
				"fragColor = vec4(1.0, 1.0 , 1.0 , 1.0);" \
			"}" \
		"}";
	//specify vertex source code to vetex shader object
	glShaderSource(gFragmentShaderObject_PV,
					1,
					(GLchar **)&fragmentShaderSourceCode_PV,
					NULL);

	glShaderSource(gFragmentShaderObject_PF,
					1,
					(GLchar **)&fragmentShaderSourceCode_PF,
					NULL);
	//compile vertex shader code
	glCompileShader(gFragmentShaderObject_PV);

	//Error checking
	 iShaderCompilationStatus = 0;
	 iInfoLength = 0;
	 szInfoLog = NULL;

	 glGetShaderiv(gFragmentShaderObject_PV,
					GL_COMPILE_STATUS,
					&iShaderCompilationStatus);

	if(iShaderCompilationStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject_PV,
						GL_INFO_LOG_LENGTH,
						&iInfoLength);
		if(iInfoLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLength);

			if(szInfoLog != NULL)
			{
				GLsizei writtened;

				glGetShaderInfoLog(gFragmentShaderObject_PV,
									iInfoLength,
									&writtened,
									szInfoLog);
				fprintf(gpfile,"ERROR:FRAGMENT SHADER PV:%s",szInfoLog);
				free(szInfoLog);
				Uninitialize();
				
				exit(0);
			}
		}
	}

	glCompileShader(gFragmentShaderObject_PF);

	iShaderCompilationStatus = 0;
	 iInfoLength = 0;
	 szInfoLog = NULL;

	 glGetShaderiv(gFragmentShaderObject_PF,
					GL_COMPILE_STATUS,
					&iShaderCompilationStatus);
	 if(iShaderCompilationStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject_PF,
						GL_INFO_LOG_LENGTH,
						&iInfoLength);
		if(iInfoLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLength);

			if(szInfoLog != NULL)
			{
				GLsizei writtened;

				glGetShaderInfoLog(gFragmentShaderObject_PF,
									iInfoLength,
									&writtened,
									szInfoLog);
				fprintf(gpfile,"ERROR:FRAGMENT SHADER PF:%s",szInfoLog);
				free(szInfoLog);
				Uninitialize();
				
				exit(0);
			}
		}
	}
	//Create shader program object
	gShaderProgramObject_PV = glCreateProgram();
	gShaderProgramObject_PF = glCreateProgram();

	//Attach vertex,fragment shader program
	glAttachShader(gShaderProgramObject_PV, gVertexShaderObject_PV);
	glAttachShader(gShaderProgramObject_PV, gFragmentShaderObject_PV);
	glAttachShader(gShaderProgramObject_PF, gVertexShaderObject_PF);
	glAttachShader(gShaderProgramObject_PF, gFragmentShaderObject_PF);

        //*** PRELINKING BINDING TO VERTEX ATTRIBUTES***
	glBindAttribLocation(gShaderProgramObject_PV,
						AMC_ATTRIBUTES_POSITION,
						"vPosition");

	glBindAttribLocation(gShaderProgramObject_PV,
						AMC_ATTRIBUTES_NORMAL,
						"vNormal");

	glBindAttribLocation(gShaderProgramObject_PF,
						AMC_ATTRIBUTES_POSITION,
						"vPosition");

	glBindAttribLocation(gShaderProgramObject_PF,
						AMC_ATTRIBUTES_NORMAL,
						"vNormal");	
											
	//link above shader program
	glLinkProgram(gShaderProgramObject_PV);

	glLinkProgram(gShaderProgramObject_PF);
	// ***ERROR CHECKING LINKING********
	//Error checking for program linking
		GLint iShaderLinkStatus = 0;
		iInfoLength = 0;
		szInfoLog = NULL;

		glGetProgramiv(gShaderProgramObject_PV,
							GL_LINK_STATUS,
							&iShaderLinkStatus);
		if(iShaderCompilationStatus == GL_FALSE)
		{
			glGetProgramiv(gShaderProgramObject_PV,
							GL_INFO_LOG_LENGTH,
							&iInfoLength);
			if(iInfoLength > 0)
			{
				szInfoLog = (GLchar *)malloc(iInfoLength);

				if (szInfoLog != NULL)
				{
					GLsizei wr;

					glGetShaderInfoLog(gShaderProgramObject_PV,
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
	
		iShaderLinkStatus = 0;
		iInfoLength = 0;
		szInfoLog = NULL;

		glGetProgramiv(gShaderProgramObject_PF,
							GL_LINK_STATUS,
							&iShaderLinkStatus);
		if(iShaderCompilationStatus == GL_FALSE)
		{
			glGetProgramiv(gShaderProgramObject_PF,
							GL_INFO_LOG_LENGTH,
							&iInfoLength);
			if(iInfoLength > 0)
			{
				szInfoLog = (GLchar *)malloc(iInfoLength);

				if (szInfoLog != NULL)
				{
					GLsizei wr;

					glGetShaderInfoLog(gShaderProgramObject_PF,
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
	//getting light uniforms locations
	isLKeyIsPressedUniforms_PV = glGetUniformLocation(gShaderProgramObject_PV, "islkeypressed_PV");
	isLKeyIsPressedUniforms_PF = glGetUniformLocation(gShaderProgramObject_PF, "islkeypressed_PF");

	//PER VERTEX
	
	laUniform_PV_RED	= glGetUniformLocation(gShaderProgramObject_PV, "u_la_PV_RED");
	laUniform_PV_GREEN  = glGetUniformLocation(gShaderProgramObject_PV, "u_la_PV_GREEN");
	laUniform_PV_BLUE   = glGetUniformLocation(gShaderProgramObject_PV, "u_la_PV_BLUE");
	
	ldUniform_PV_RED	= glGetUniformLocation(gShaderProgramObject_PV, "u_ld_PV_RED");
	ldUniform_PV_GREEN  = glGetUniformLocation(gShaderProgramObject_PV, "u_ld_PV_GREEN");
	ldUniform_PV_BLUE	= glGetUniformLocation(gShaderProgramObject_PV, "u_ld_PV_BLUE");

	lsUniform_PV_RED	 = glGetUniformLocation(gShaderProgramObject_PV, "u_ls_PV_RED");
	lsUniform_PV_GREEN   = glGetUniformLocation(gShaderProgramObject_PV, "u_ls_PV_GREEN");
	lsUniform_PV_BLUE	 = glGetUniformLocation(gShaderProgramObject_PV, "u_ls_PV_BLUE");

	kaUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_ka_PV");
	kdUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_kd_PV");
	ksUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_ks_PV");

	shininessUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_shininess_PV");

	lightPositionUniform_PV_RED = glGetUniformLocation(gShaderProgramObject_PV, "u_light_position_PV_RED");
	lightPositionUniform_PV_GREEN = glGetUniformLocation(gShaderProgramObject_PV, "u_light_position_PV_GREEN");
	lightPositionUniform_PV_BLUE = glGetUniformLocation(gShaderProgramObject_PV, "u_light_position_PV_BLUE");

	//PER FRAG
	laUniform_PF_RED = glGetUniformLocation(gShaderProgramObject_PF, "u_la_PF_RED");
	laUniform_PF_GREEN = glGetUniformLocation(gShaderProgramObject_PF, "u_la_PF_GREEN");
	laUniform_PF_BLUE = glGetUniformLocation(gShaderProgramObject_PF, "u_la_PF_BLUE");

	ldUniform_PF_RED = glGetUniformLocation(gShaderProgramObject_PF, "u_ld_PF_RED");
	ldUniform_PF_GREEN = glGetUniformLocation(gShaderProgramObject_PF, "u_ld_PF_GREEN");
	ldUniform_PF_BLUE = glGetUniformLocation(gShaderProgramObject_PF, "u_ld_PF_BLUE");

	lsUniform_PF_RED = glGetUniformLocation(gShaderProgramObject_PF, "u_ls_PF_RED");
	lsUniform_PF_GREEN = glGetUniformLocation(gShaderProgramObject_PF, "u_ls_PF_GREEN");
	lsUniform_PF_BLUE = glGetUniformLocation(gShaderProgramObject_PF, "u_ls_PF_BLUE");

	
	kaUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_ka_PF");
	kdUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_kd_PF");
	ksUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_ks_PF");

	
	shininessUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_shininess_PF");

	
	lightPositionUniform_PF_RED = glGetUniformLocation(gShaderProgramObject_PF, "u_light_position_PF_RED");
	lightPositionUniform_PF_GREEN = glGetUniformLocation(gShaderProgramObject_PF, "u_light_position_PF_GREEN");
	lightPositionUniform_PF_BLUE = glGetUniformLocation(gShaderProgramObject_PF, "u_light_position_PF_BLUE");

	//getting transformation matrices uniform locations
	//PV
	modelMatrixUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_model_matrix");

	viewMatrixUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_view_matrix");

	projectionUniform_PV = glGetUniformLocation(gShaderProgramObject_PV, "u_projection_matrix");

	//PF
	modelMatrixUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_model_matrix");

	viewMatrixUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_view_matrix");

	projectionUniform_PF = glGetUniformLocation(gShaderProgramObject_PF, "u_projection_matrix");
	//arrays

	fprintf(gpfile,"Post link success!!\n");

	//sphere 
	getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
	gNumVertices = getNumberOfSphereVertices();
	gNumElements = getNumberOfSphereElements();


	//VAO AND VBO
	glGenVertexArrays(1, &vao_sphere);

	glBindVertexArray(vao_sphere);

	//position
	glGenBuffers(1, &vbo_position_vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_vbo);

	glBufferData(GL_ARRAY_BUFFER,
					sizeof(sphere_vertices),
					sphere_vertices,
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

	glGenBuffers(1, &vbo_normals_vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_normals_vbo);

	glBufferData(GL_ARRAY_BUFFER,
					sizeof(sphere_normals),
					sphere_normals,
					GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTES_NORMAL,
							3,
							GL_FLOAT,
							GL_FALSE,
							0,
							NULL);

	glEnableVertexAttribArray(AMC_ATTRIBUTES_NORMAL);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// element vbo
	glGenBuffers(1, &vbo_element_vbo);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element_vbo);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

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

	//declare matrices
	mat4 viewMatrix;
	mat4 modelMatrix;

	//make identity matrices
	viewMatrix = mat4::identity();
	modelMatrix = mat4::identity();

	//Do transformations...
	modelMatrix = translate(0.0f, 0.0f, -3.0f);

	//send neccessary matrix to shader in respective uniforms

	if (isPerVertex == true)
	{
		glUseProgram(gShaderProgramObject_PV);
		//fprintf(gpfile,"PER VERTEX\n");
		if (isLighting == true)
		{
			glUniform1i(isLKeyIsPressedUniforms_PV, 1);

			glUniform1f(shininessUniform_PV, materialShininess);

			glUniform3fv(laUniform_PV_RED, 1, lightAmbient_Red);
			glUniform3fv(laUniform_PV_BLUE, 1, lightAmbient_Blue);
			glUniform3fv(laUniform_PV_GREEN, 1, lightAmbient_Green);

			glUniform3fv(ldUniform_PV_RED, 1, lightDifuse_Red);
			glUniform3fv(ldUniform_PV_GREEN, 1, lightDifuse_Green);
			glUniform3fv(ldUniform_PV_BLUE, 1, lightDifuse_Blue);

			glUniform3fv(lsUniform_PV_RED, 1, lightSpecular_Red);
			glUniform3fv(lsUniform_PV_GREEN, 1, lightSpecular_Green);
			glUniform3fv(lsUniform_PV_BLUE, 1, lightSpecular_Blue);


			glUniform3fv(kaUniform_PV, 1, materialAmbient);
			glUniform3fv(kdUniform_PV, 1, materialDifuse);
			glUniform3fv(ksUniform_PV, 1, materialSpecular);

			glUniform4fv(lightPositionUniform_PV_RED, 1, lightPosition_Red);
			glUniform4fv(lightPositionUniform_PV_GREEN, 1, lightPosition_Green);
			glUniform4fv(lightPositionUniform_PV_BLUE, 1, lightPosition_Blue);
		}
		else
		{
			glUniform1i(isLKeyIsPressedUniforms_PV, 0);
		}
		glUniformMatrix4fv(modelMatrixUniform_PV,
							1,
							GL_FALSE,
							modelMatrix);

		glUniformMatrix4fv(viewMatrixUniform_PV,
							1,
							GL_FALSE,
							viewMatrix);

		glUniformMatrix4fv(projectionUniform_PV,
							1,
							GL_FALSE,
							perspectiveProjectionMatrix);

		glUseProgram(gShaderProgramObject_PV);

	}
	else
	{
		glUseProgram(gShaderProgramObject_PF);
		//fprintf(gpfile, "PER FRAGMENT\n");
		if (isLighting == true)
		{
			glUniform1i(isLKeyIsPressedUniforms_PF, 1);

			glUniform1f(shininessUniform_PF, materialShininess);

			glUniform3fv(laUniform_PF_RED, 1, lightAmbient_Red);
			glUniform3fv(laUniform_PF_GREEN, 1, lightAmbient_Green);
			glUniform3fv(laUniform_PF_BLUE, 1, lightAmbient_Blue);

			glUniform3fv(ldUniform_PF_RED, 1, lightDifuse_Red);
			glUniform3fv(ldUniform_PF_GREEN, 1, lightDifuse_Green);
			glUniform3fv(ldUniform_PF_BLUE, 1, lightDifuse_Blue);

			glUniform3fv(lsUniform_PF_RED, 1, lightSpecular_Red);
			glUniform3fv(lsUniform_PF_GREEN, 1, lightSpecular_Green);
			glUniform3fv(lsUniform_PF_BLUE, 1, lightSpecular_Blue);

			glUniform3fv(kaUniform_PF, 1, materialAmbient);
			glUniform3fv(kdUniform_PF, 1, materialDifuse);
			glUniform3fv(ksUniform_PF, 1, materialSpecular);

			glUniform4fv(lightPositionUniform_PF_RED, 1, lightPosition_Red);
			glUniform4fv(lightPositionUniform_PF_GREEN, 1, lightPosition_Green);
			glUniform4fv(lightPositionUniform_PF_BLUE, 1, lightPosition_Blue);
		}
		else
		{
			glUniform1i(isLKeyIsPressedUniforms_PF, 0);
		}
		//
		glUniformMatrix4fv(modelMatrixUniform_PF,
							1,
							GL_FALSE,
							modelMatrix
							);

		glUniformMatrix4fv(viewMatrixUniform_PF,
							1,
							GL_FALSE,
							viewMatrix);

		glUniformMatrix4fv(projectionUniform_PF,
							1,
							GL_FALSE,
							perspectiveProjectionMatrix);


	}


	//similarly bind with texture in any
	glBindVertexArray(vao_sphere);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_element_vbo);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

	glBindVertexArray(0);

	glUseProgram(0);
	glXSwapBuffers(gpdisplay, gWindow);
	//fprintf(gpfile,"Display END\n");
}

void update()
{
	if (angle_X >= 360.0f)
	{
		angle_X = 0.0f;
	}
	else
	{
		angle_X += 0.005f;
	}

	lightPosition_Red[0] = 0.0f;
	lightPosition_Red[1] = 100.0f*sin(angle_X);
	lightPosition_Red[2] = 100.0f *cos(angle_X);
	lightPosition_Red[3] = 1.0f;

	lightPosition_Green[0] = 100.0f*cos(angle_X);
	lightPosition_Green[1] = 0.0f;
	lightPosition_Green[2] = 100.0f*sin(angle_X);
	lightPosition_Green[3] = 1.0f;

	lightPosition_Blue[0] = 100 * cos(angle_X);
	lightPosition_Blue[1] = 100 * sin(angle_X);
	lightPosition_Blue[2] = 0.0f;
	lightPosition_Blue[3] = 1.0f;
}

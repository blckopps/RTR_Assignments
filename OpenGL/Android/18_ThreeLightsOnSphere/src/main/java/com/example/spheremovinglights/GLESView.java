package com.example.spheremovinglights;

import android.opengl.GLSurfaceView;
import android.opengl.GLES31;
import javax.microedition.khronos.opengles.GL10;
import javax.microedition.khronos.egl.EGLConfig;
import java.util.*;
//texture
import android.graphics.BitmapFactory;
import android.graphics.Bitmap;
import android.opengl.GLUtils;          //Teximage2D

//for opengl buffers
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.nio.ShortBuffer;
//matrix maths
import android.opengl.Matrix;

import android.content.Context;
import android.view.Gravity;
import android.graphics.Color;
import android.view.MotionEvent;
import android.view.GestureDetector;
import android.view.GestureDetector.OnGestureListener;
import android.view.GestureDetector.OnDoubleTapListener;

public class GLESView extends GLSurfaceView implements GLSurfaceView.Renderer ,OnDoubleTapListener,OnGestureListener
{
	private GestureDetector gestureDetector;
	private final Context context;
	
	//variables
	private int vertexShaderObject_PV;
	private int fragmentShaderObject_PV;

	private int vertexShaderObject_PF;
	private int fragmentShaderObject_PF;

	private int shaderProgramObject_PV;
	private int shaderProgramObject_PF;

	private int[] vao_sphere = new int[1];
	private int[] vbo_sphere_position = new int[1];
	
	private int[] vbo_sphere_normal = new int[1];
	

    private int[] vbo_sphere_element = new int[1];

	int numVertices,numElements;	
	//Uniforms 
	private int mUniform_PV;
	private int vUniform_PV;
	private int projectionUniform_PV;
	

	private int mUniform_PF;
	private int vUniform_PF;
	private int projectionUniform_PF;
	
	////
	private int sampler_Uniform;
	
	//For PV
	//RED
	private int laUniform_PV_RED;
	private int ldUniform_PV_RED;
	private int lsUniform_PV_RED;
	//BLUE
	private int laUniform_PV_BLUE;
	private int ldUniform_PV_BLUE;
	private int lsUniform_PV_BLUE;
	//GREEN
	private int laUniform_PV_GREEN;
	private int ldUniform_PV_GREEN;
	private int lsUniform_PV_GREEN;

	private int kaUniform_PV;
	private int kdUniform_PV;
	private int ksUniform_PV;

	private int shininessUniform_PV;

	private int lightPositionUniform_PV_RED;
	private int lightPositionUniform_PV_GREEN;
	private int lightPositionUniform_PV_BLUE;

	private int isLKeyIsPressed_PV;
	/////////**For PF**///////////
	//RED
	private int laUniform_PF_RED;
	private int ldUniform_PF_RED;
	private int lsUniform_PF_RED;
	//BLUE
	private int laUniform_PF_BLUE;
	private int ldUniform_PF_BLUE;
	private int lsUniform_PF_BLUE;
	//GREEN
	private int laUniform_PF_GREEN;
	private int ldUniform_PF_GREEN;
	private int lsUniform_PF_GREEN;

	private int kaUniform_PF;
	private int kdUniform_PF;
	private int ksUniform_PF;

	private int shininessUniform_PF;

	private int lightPositionUniform_PF_RED;
	private int lightPositionUniform_PF_GREEN;
	private int lightPositionUniform_PF_BLUE;

	private int isLKeyIsPressed_PF;

	//END
	private float[] perspectiveProjectionMatrix = new float[16];   //4*4 matrix

	//Light arrays
	//RED
	private float lightAmbient_Red[] = new float[ ]{ 0.0f, 0.0f, 0.0f, 0.0f};
	private float lightDifuse_Red[] = new float[ ] { 1.0f, 0.0f, 0.0f, 1.0f };
	private float lightSpecular_Red[] = new float[ ] {1.0f, 0.0f, 0.0f, 1.0f };
	//GREEN
	private float lightAmbient_Green[] = new float[ ]{ 0.0f, 0.0f, 0.0f, 0.0f};
	private float lightDifuse_Green[] = new float[ ] { 0.0f, 1.0f, 0.0f, 1.0f };
	private float lightSpecular_Green[] = new float[ ] {0.0f, 1.0f, 0.0f, 1.0f };
	//BLUE
	private float lightAmbient_Blue[] = new float[ ]{ 0.0f, 0.0f, 0.0f, 0.0f};
	private float lightDifuse_Blue[] = new float[ ] { 0.0f, 0.0f, 1.0f, 1.0f };
	private float lightSpecular_Blue[] = new float[ ] {0.0f, 0.0f, 1.0f, 1.0f };

	//material array
	private float materialAmbient[] = new float[ ]{0.0f, 0.0f, 0.0f, 0.0f};
	private float materialDifuse[] = new float[ ]{1.0f, 1.0f, 1.0f, 1.0f };
	private float materialSpecular[] = new float[  ]{1.0f, 1.0f, 1.0f, 1.0f};

	private float lightPosition_Red[] = new float[4];
	private float lightPosition_Green[] = new float[4];
	private float lightPosition_Blue[] = new float[4];

	private float materialShininess = 50.0f;	
	//
	Boolean bFullScreen = false;
	Boolean bIsLighting = false;
	Boolean bPerVertex = true;
	Boolean bPerFragment = false;
	
	double angle = 0.0f;
	//
	
	
	public GLESView(Context drawingContext)
	{
		super(drawingContext);
		
		context = drawingContext;
		//
		gestureDetector = new GestureDetector(drawingContext, this, null, false);
		//drawingContext:it's like global enviroment 
		//this:who is going to handle gesture
		//null:other is not going to handle
		//false:internally used by android
		gestureDetector.setOnDoubleTapListener(this);
		
		setEGLContextClientVersion(3);
		
		setRenderer(this);
		
		setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
	}
	
	//methods OnDoubleTapListner
	@Override
	public boolean onTouchEvent(MotionEvent event)
	{
		int eventaction = event.getAction(); //not needed
		if(!gestureDetector.onTouchEvent(event))
			super.onTouchEvent(event);
		return(true);
	}
	
	 @Override 
	 public boolean onDoubleTap(MotionEvent event)		//Enable Lighting
	 {
		if(bIsLighting)
		{
			bIsLighting = false;
		}
		else
		{
			bIsLighting = true;
		}
			
		System.out.println("RTR: Double tap");
		 return(true);
	 }
	 
	 @Override 
	 public boolean onDoubleTapEvent(MotionEvent event)
	 {
		 
		 return(true);
	 }
	 
	 @Override
	 public boolean onSingleTapConfirmed(MotionEvent event)
	 {
		 if(bPerVertex == true)
		 {
		 	bPerVertex = false;	
		 	bPerFragment = true;
		 } 
		 else
		 {
		 	bPerVertex = true;
		 	bPerFragment = false;
		 }
		 
		 return(true);
	 }
	 
	 
	 //On GestureListner
	 @Override
	 public boolean onDown(MotionEvent event)
	 {
		 return(true);
		 
	 }
	 //
	 @Override
	 public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY)
	 {
		 return(true);
	 }
	 
	 @Override
	 public void onLongPress(MotionEvent e)
	 {
		 
			
		 System.out.println("RTR: On long press");
	 }
	 
	 @Override 
	 public boolean onScroll(MotionEvent e1, MotionEvent e2, float disX, float disY)
	 {
		 Uninitialize();
		 System.exit(0);
		 return(true);
	 }
	 
	 @Override 
	 public void onShowPress(MotionEvent e)
	 {
		 
	 }
	 
	 @Override
	 public boolean onSingleTapUp(MotionEvent e)
	 {
		 return(true);
	 }
	 //implements GLSurfaceView.Renderer
	 @Override
	 public void onSurfaceCreated(GL10 gl,EGLConfig config)
	 {
		 String version = gl.glGetString(GL10.GL_VERSION);
                 String shadingVersion = gl.glGetString(GLES31.GL_SHADING_LANGUAGE_VERSION);	 
		 System.out.println("RTR Opengl version: " + version);
		 System.out.println("RTR: Shading language version " + shadingVersion);
		 
		 initialize();
		 System.out.println("RTR: After Initialize()");
	 }
	 @Override
	 public void onSurfaceChanged(GL10 unused, int width, int height)  //resize
	 {
		 System.out.println("RTR: onSurfaceChanged()");
		 
		 resize(width, height);
	 }
	 
	 @Override
	 public void onDrawFrame(GL10 unused) //Display
	 {
		 display();
		 
		 System.out.println("RTR: onDrawFrame()");
	 }
	 
	 //our custom methods
	
	 private void initialize()
	 {
		 System.out.println("RTR: In start Initialize()");
		 
		 ///////////////*****For Per Fragment*********//////////////////////
		 vertexShaderObject_PF = GLES31.glCreateShader(GLES31.GL_VERTEX_SHADER);
		 
		final String vertexShaderSourceCode_PF = String.format
		                                (
		                                        "#version 310 es" +
		                                         "\n" +
												"in vec4 vPosition;" +
											"in vec3 vNormal;" +

											"uniform mat4 u_model_matrix;" +
											"uniform mat4 u_view_matrix;" +
											"uniform mat4 u_projection_matrix;" +
											"uniform float islkeypressed_PF;" +

											"uniform vec4 u_light_position_PF_RED;" +
											"uniform vec4 u_light_position_PF_GREEN;" +
											"uniform vec4 u_light_position_PF_BLUE;" +

											"out vec3 tnorm_PF;" +
											
											"out vec3 light_direction_PF_RED;" +
											"out vec3 light_direction_PF_GREEN;" +
											"out vec3 light_direction_PF_BLUE;" +

											"out vec3 viewer_vector_PF;" +
											"void main(void)" +
											"{" +
												"if(islkeypressed_PF == 1.0)" +
												"{" +
													"vec4 eye_coordinates = u_view_matrix *  u_model_matrix  * vPosition;" +

													"tnorm_PF = mat3(u_view_matrix * u_model_matrix) * vNormal;" +

													"light_direction_PF_RED = vec3(u_light_position_PF_RED - eye_coordinates);" +
													"light_direction_PF_GREEN = vec3(u_light_position_PF_GREEN - eye_coordinates);" +
													"light_direction_PF_BLUE = vec3(u_light_position_PF_BLUE - eye_coordinates);" +

													"viewer_vector_PF = vec3(-eye_coordinates.xyz);" +
												"}" +
												"gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" +
											" } "
		                                );
		                                
         GLES31.glShaderSource(vertexShaderObject_PF, vertexShaderSourceCode_PF);
		                 
		 GLES31.glCompileShader(vertexShaderObject_PF);
		 
		 //Vertex shader error checking
		 int[] iShaderCompileStatus = new int[1];
		 int[] iInfoLogLength = new int[1];
		 String szInfoLog = null;
		 
		 GLES31.glGetShaderiv(vertexShaderObject_PF,
		                      GLES31.GL_COMPILE_STATUS,
		                      iShaderCompileStatus,
		                      0);
		                      
		  if(iShaderCompileStatus[0] == GLES31.GL_FALSE)
		  {
		        GLES31.glGetShaderiv(vertexShaderObject_PF,
		                             GLES31.GL_INFO_LOG_LENGTH,
		                             iInfoLogLength,
		                             0 );
		                             
                        if(iInfoLogLength[0] > 0)
                        {
                                szInfoLog = GLES31.glGetShaderInfoLog(vertexShaderObject_PF);
                                
                                System.out.println("RTR: vertex shader PF ERROR: " + szInfoLog);
                                //uninitialize();
                                System.exit(0);
                        }
		        
		  }
		 
		 System.out.println("RTR: After Vertex Shader PF..");

        	///////////////////////////////////////****Fragment Shader *******////////////////////////////////
        	
		 
		 fragmentShaderObject_PF = GLES31.glCreateShader(GLES31.GL_FRAGMENT_SHADER);
		 
		 final String fragmentShaderSourceCode_PF = String.format
		                                (
		                                        "#version 310 es" +
		                                        "\n" +
		                                        "precision highp float;" +
												"out vec4 fragColor;" +
												"uniform float islkeypressed_PF;" +

												"uniform vec3 u_la_PF_RED;" +
												"uniform vec3 u_la_PF_GREEN;" +
												"uniform vec3 u_la_PF_BLUE;" +

												"uniform vec3 u_ld_PF_RED;" +
												"uniform vec3 u_ld_PF_GREEN;" +
												"uniform vec3 u_ld_PF_BLUE;" +

												"uniform vec3 u_ls_PF_RED;" +
												"uniform vec3 u_ls_PF_GREEN;" +
												"uniform vec3 u_ls_PF_BLUE;" +

												"uniform vec3 u_ka_PF;" +
												"uniform vec3 u_kd_PF;" +
												"uniform vec3 u_ks_PF;" +

												"in vec3 tnorm_PF;" +

												"in vec3 light_direction_PF_RED;" +
												"in vec3 light_direction_PF_GREEN;" +
												"in vec3 light_direction_PF_BLUE;" +

												"in vec3 viewer_vector_PF;" +
												"uniform float u_shininess_PF;" +
												"void main(void)" +
												"{" +
													"if(islkeypressed_PF == 1.0)" +
													"{" +
														"vec3 tnorm_normalized = normalize(tnorm_PF);" +

														"vec3 light_direction_normalized_RED = normalize(light_direction_PF_RED);" +
														"vec3 light_direction_normalized_GREEN = normalize(light_direction_PF_GREEN);" +
														"vec3 light_direction_normalized_BLUE = normalize(light_direction_PF_BLUE);" +

														"vec3 viewer_vector_normalized = normalize(viewer_vector_PF);" +

														"float tn_dot_ldirection_RED = max(dot(light_direction_normalized_RED, tnorm_normalized), 0.0);" +
														"float tn_dot_ldirection_GREEN = max(dot(light_direction_normalized_GREEN, tnorm_normalized), 0.0);" +
														"float tn_dot_ldirection_BLUE = max(dot(light_direction_normalized_BLUE, tnorm_normalized), 0.0);" +

														"vec3 reflection_vector_RED = reflect(-light_direction_normalized_RED, tnorm_normalized);" +
														"vec3 reflection_vector_GREEN = reflect(-light_direction_normalized_GREEN, tnorm_normalized);" +
														"vec3 reflection_vector_BLUE = reflect(-light_direction_normalized_BLUE, tnorm_normalized);" +

														"vec3 ambient_RED = u_la_PF_RED * u_ka_PF;" +
														"vec3 ambient_GREEN = u_la_PF_GREEN * u_ka_PF;" +
														"vec3 ambient_BLUE = u_la_PF_BLUE * u_ka_PF;" +

														"vec3 difuse_RED = u_ld_PF_RED * u_kd_PF * tn_dot_ldirection_RED;" +
														"vec3 difuse_GREEN = u_ld_PF_GREEN * u_kd_PF * tn_dot_ldirection_GREEN;" +
														"vec3 difuse_BLUE = u_ld_PF_BLUE * u_kd_PF * tn_dot_ldirection_BLUE;" +

														"vec3 specular_RED = u_ls_PF_RED * u_ks_PF * pow(max(dot(reflection_vector_RED, viewer_vector_normalized),0.0),u_shininess_PF);" +
														"vec3 specular_GREEN = u_ls_PF_GREEN * u_ks_PF * pow(max(dot(reflection_vector_GREEN, viewer_vector_normalized),0.0),u_shininess_PF);" +
														"vec3 specular_BLUE = u_ls_PF_BLUE * u_ks_PF * pow(max(dot(reflection_vector_BLUE, viewer_vector_normalized),0.0),u_shininess_PF);" +

														"vec3 redLight = ambient_RED + difuse_RED + specular_RED;" +
														"vec3 greenLight = ambient_GREEN + difuse_GREEN + specular_GREEN;" +
														"vec3 blueLight = ambient_BLUE + difuse_BLUE + specular_BLUE;" +


														"vec3 phong_light_pf = redLight + greenLight + blueLight;" +
														"fragColor = vec4(phong_light_pf, 1.0);" +

													"}" +
													"else" +
													"{" +
														"fragColor = vec4(1.0, 1.0 , 1.0 , 1.0);" +
													"}" +
												"}"
		                                );
		 
		 GLES31.glShaderSource(fragmentShaderObject_PF, fragmentShaderSourceCode_PF);
		 
		 GLES31.glCompileShader(fragmentShaderObject_PF);
		 
		 //Error checking for fragment shader 
		  iShaderCompileStatus[0] = 0;
		  iInfoLogLength[0] = 0;
		  szInfoLog = null;
		  
		  
		 GLES31.glGetShaderiv(fragmentShaderObject_PF,
		                      GLES31.GL_COMPILE_STATUS,
		                      iShaderCompileStatus,
		                      0);
		                      
                if(iShaderCompileStatus[0] == GLES31.GL_FALSE)
                {
                        GLES31.glGetShaderiv(fragmentShaderObject_PF,
                                             GLES31.GL_INFO_LOG_LENGTH,
                                             iInfoLogLength,
                                             0);
                                             
                        if(iInfoLogLength[0] > 0)
                        {
                            szInfoLog = GLES31.glGetShaderInfoLog(fragmentShaderObject_PF);    
                            
                            System.out.println("RTR: fragment shader PF ERROR: " + szInfoLog);
                            //uninitialize();
                            System.exit(0);
                        }
                        
                }
                System.out.println("RTR: After Fragment Shader..");
                
		//Create ShaderProgramObject and attach above shaders
		
		     shaderProgramObject_PF = GLES31.glCreateProgram();
		     
		     GLES31.glAttachShader(shaderProgramObject_PF, vertexShaderObject_PF);
		     GLES31.glAttachShader(shaderProgramObject_PF, fragmentShaderObject_PF);
		     
		     
		 //*** PRELINKING BINDING TO VERTEX ATTRIBUTES***
		 GLES31.glBindAttribLocation(shaderProgramObject_PF,
		                             GLESMacros.AMC_ATTRIBUTE_POSITION,
		                             "vPosition");

		GLES31.glBindAttribLocation(shaderProgramObject_PF,
									GLESMacros.AMC_ATTRIBUTE_NORMAL,
									"vNormal");

			System.out.println("RTR: shader pre link program successfull for PF : " );
                //Link above program
                GLES31.glLinkProgram(shaderProgramObject_PF);
                
                //ERROR checking for Linking
                
                int[] iShaderLinkStatus = new int[1];
                iInfoLogLength[0] = 0;
				szInfoLog = null;
		
		System.out.println( "RTR: iShaderLinkStatus for PF before " + iShaderLinkStatus[0]);

		GLES31.glGetProgramiv(shaderProgramObject_PF,
		                      GLES31.GL_LINK_STATUS,
		                      iShaderLinkStatus,
		                      0);

		

		  if(iShaderLinkStatus[0] == GLES31.GL_FALSE)
		  {
		        System.out.println( "RTR: iShaderLinkStatus for PF after " + iShaderLinkStatus[0]);
		        GLES31.glGetProgramiv(shaderProgramObject_PF,
		                              GLES31.GL_INFO_LOG_LENGTH,
		                              iInfoLogLength,
		                              0 );
		                              
                        if(iInfoLogLength[0] > 0)
                        {
                              szInfoLog = GLES31.glGetProgramInfoLog(shaderProgramObject_PF);
                              
                              System.out.println( "RTR: iInfoLogLength " + iInfoLogLength[0]);
                               System.out.println("RTR: shader program PF ERROR : " + szInfoLog);
                                //uninitialize();
                                System.exit(0);
                        }
		  }
		  
		 ///***POST LINKING GETTING UNIFORMS**
		 
		mUniform_PF= GLES31.glGetUniformLocation(shaderProgramObject_PF, "u_model_matrix");
		
		vUniform_PF = GLES31.glGetUniformLocation(shaderProgramObject_PF, "u_view_matrix");

		projectionUniform_PF = GLES31.glGetUniformLocation(shaderProgramObject_PF, "u_projection_matrix");

		isLKeyIsPressed_PF = GLES31.glGetUniformLocation(shaderProgramObject_PF,
												"islkeypressed_PF");

		laUniform_PF_RED = GLES31.glGetUniformLocation(shaderProgramObject_PF, "u_la_PF_RED");
		laUniform_PF_GREEN = GLES31.glGetUniformLocation(shaderProgramObject_PF, "u_la_PF_GREEN");
		laUniform_PF_BLUE = GLES31.glGetUniformLocation(shaderProgramObject_PF, "u_la_PF_BLUE");

		ldUniform_PF_RED = GLES31.glGetUniformLocation(shaderProgramObject_PF, "u_ld_PF_RED");
		ldUniform_PF_GREEN = GLES31.glGetUniformLocation(shaderProgramObject_PF, "u_ld_PF_GREEN");
		ldUniform_PF_BLUE = GLES31.glGetUniformLocation(shaderProgramObject_PF, "u_ld_PF_BLUE");

		lsUniform_PF_RED = GLES31.glGetUniformLocation(shaderProgramObject_PF, "u_ls_PF_RED");
		lsUniform_PF_GREEN = GLES31.glGetUniformLocation(shaderProgramObject_PF, "u_ls_PF_GREEN");
		lsUniform_PF_BLUE = GLES31.glGetUniformLocation(shaderProgramObject_PF, "u_ls_PF_BLUE");

		kaUniform_PF = GLES31.glGetUniformLocation(shaderProgramObject_PF, "u_ka_PF");
		kdUniform_PF = GLES31.glGetUniformLocation(shaderProgramObject_PF, "u_kd_PF");
		ksUniform_PF = GLES31.glGetUniformLocation(shaderProgramObject_PF, "u_ks_PF");

		shininessUniform_PF = GLES31.glGetUniformLocation(shaderProgramObject_PF, "u_shininess_PF");


		lightPositionUniform_PF_RED = GLES31.glGetUniformLocation(shaderProgramObject_PF,
												"u_light_position_PF_RED");
		lightPositionUniform_PF_GREEN = GLES31.glGetUniformLocation(shaderProgramObject_PF,
												"u_light_position_PF_GREEN");
		lightPositionUniform_PF_BLUE= GLES31.glGetUniformLocation(shaderProgramObject_PF,
												"u_light_position_PF_BLUE");
		
		System.out.println("RTR: After Post Linking per fragment");
		

		 ////////////////////****For per Vertex***********////////////////////////////////////////////////////////
	      //// ////////////////////////// //*********Vertex shader *********//////
	      
		 vertexShaderObject_PV = GLES31.glCreateShader(GLES31.GL_VERTEX_SHADER);
		 
		final String vertexShaderSourceCode = String.format
		                                (
		                                        "#version 310 es" +
		                                         "\n" +
												"in vec4 vPosition;" +
											"in vec3 vNormal;" +

											"uniform mat4 u_model_matrix;" +
											"uniform mat4 u_view_matrix;" +
											"uniform mat4 u_projection_matrix;" +
											"uniform float islkeypressed_PV;" +
											//RED
											"uniform vec3 u_la_PV_RED;" +
											"uniform vec3 u_ld_PV_RED;" +
											"uniform vec3 u_ls_PV_RED;" +
											//GREEN
											"uniform vec3 u_la_PV_GREEN;" +
											"uniform vec3 u_ld_PV_GREEN;" +
											"uniform vec3 u_ls_PV_GREEN;" +
											//BLUE
											"uniform vec3 u_la_PV_BLUE;" +
											"uniform vec3 u_ld_PV_BLUE;" +
											"uniform vec3 u_ls_PV_BLUE;" +
											//Material
											"uniform vec3 u_ka_PV;" +
											"uniform vec3 u_kd_PV;" +
											"uniform vec3 u_ks_PV;" +

											"uniform float u_shininess_PV;" +

											"uniform vec4 u_light_position_PV_RED;" +
											"uniform vec4 u_light_position_PV_GREEN;" +
											"uniform vec4 u_light_position_PV_BLUE;" +

											"out vec3 phong_ads_light_PV;" +
											"void main(void)" +
											"{" +
												"if(islkeypressed_PV == 1.0)" +
												"{" +
													"vec4 eye_coordinates = u_view_matrix *  u_model_matrix  * vPosition;" +
													"vec3 tnorm = normalize(mat3(u_view_matrix * u_model_matrix) * vNormal);" +

													"vec3 light_direction_RED = normalize(vec3(u_light_position_PV_RED - eye_coordinates));" +
													"vec3 light_direction_BLUE = normalize(vec3(u_light_position_PV_BLUE - eye_coordinates));" +
													"vec3 light_direction_GREEN = normalize(vec3(u_light_position_PV_GREEN - eye_coordinates));" +

													"float tn_dot_ldirection_RED = max(dot(light_direction_RED, tnorm), 0.0);" +
													"float tn_dot_ldirection_GREEN = max(dot(light_direction_GREEN, tnorm), 0.0);" +
													"float tn_dot_ldirection_BLUE = max(dot(light_direction_BLUE, tnorm), 0.0);" +

													"vec3 reflection_vector_RED = reflect(-light_direction_RED, tnorm);" +
													"vec3 reflection_vector_GREEN = reflect(-light_direction_GREEN, tnorm);" +
													"vec3 reflection_vector_BLUE = reflect(-light_direction_BLUE, tnorm);" +

													"vec3 viewer_vector = normalize(vec3(-eye_coordinates));" +

													"vec3 ambient_RED = u_la_PV_RED * u_ka_PV;" +
													"vec3 ambient_GREEN = u_la_PV_GREEN * u_ka_PV;" +
													"vec3 ambient_BLUE = u_la_PV_BLUE * u_ka_PV;" +

													"vec3 difuse_RED = u_ld_PV_RED * u_kd_PV * tn_dot_ldirection_RED;" +
													"vec3 difuse_GREEN = u_ld_PV_GREEN * u_kd_PV * tn_dot_ldirection_GREEN;" +
													"vec3 difuse_BLUE = u_ld_PV_BLUE * u_kd_PV * tn_dot_ldirection_BLUE;" +

													"vec3 specular_RED = u_ls_PV_RED * u_ks_PV * pow(max(dot(reflection_vector_RED, viewer_vector),0.0),u_shininess_PV);" +
													"vec3 specular_GREEN = u_ls_PV_GREEN * u_ks_PV * pow(max(dot(reflection_vector_GREEN, viewer_vector),0.0),u_shininess_PV);" +
													"vec3 specular_BLUE = u_ls_PV_BLUE * u_ks_PV * pow(max(dot(reflection_vector_BLUE, viewer_vector),0.0),u_shininess_PV);" +
													
													"vec3 redLight = ambient_RED + difuse_RED + specular_RED;" +
													"vec3 blueLight = ambient_BLUE + difuse_BLUE + specular_BLUE;" +
													"vec3 greenLight = ambient_GREEN + difuse_GREEN + specular_GREEN;" +

													"phong_ads_light_PV = redLight + blueLight + greenLight;" +
												"}" +
												"else" +
												"{" +
													"phong_ads_light_PV = vec3(1.0, 1.0, 1.0);" +
												"}" +
												"gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" +
											" } "
		                                );
		                                
         GLES31.glShaderSource(vertexShaderObject_PV, vertexShaderSourceCode);
		                 
		 GLES31.glCompileShader(vertexShaderObject_PV);
		 
		 //Vertex shader error checking
		 iShaderCompileStatus[0] = 0;
		  iInfoLogLength[0] = 0;
		  szInfoLog = null;
		 
		 GLES31.glGetShaderiv(vertexShaderObject_PV,
		                      GLES31.GL_COMPILE_STATUS,
		                      iShaderCompileStatus,
		                      0);
		                      
		  if(iShaderCompileStatus[0] == GLES31.GL_FALSE)
		  {
		        GLES31.glGetShaderiv(vertexShaderObject_PV,
		                             GLES31.GL_INFO_LOG_LENGTH,
		                             iInfoLogLength,
		                             0 );
		                             
                        if(iInfoLogLength[0] > 0)
                        {
                                szInfoLog = GLES31.glGetShaderInfoLog(vertexShaderObject_PV);
                                
                                System.out.println("RTR: vertex shader ERROR: " + szInfoLog);
                                //uninitialize();
                                System.exit(0);
                        }
		        
		  }
		 
		 System.out.println("RTR: After Vertex Shader..");

        	///////////////////////////////////////****Fragment Shader *******////////////////////////////////
        	
		 
		 fragmentShaderObject_PV = GLES31.glCreateShader(GLES31.GL_FRAGMENT_SHADER);
		 
		 final String fragmentShaderSourceCode = String.format
		                                (
		                                        "#version 310 es" +
		                                        "\n" +
		                                        "precision highp float;" +
												"out vec4 fragColor;" +
												"in vec3 phong_ads_light_PV;" +
												"void main(void)" +
												"{" +
													
													"fragColor = vec4(phong_ads_light_PV, 1.0);" +
													
												"}"
		                                );
		 
		 GLES31.glShaderSource(fragmentShaderObject_PV, fragmentShaderSourceCode);
		 
		 GLES31.glCompileShader(fragmentShaderObject_PV);
		 
		 //Error checking for fragment shader 
		  iShaderCompileStatus[0] = 0;
		  iInfoLogLength[0] = 0;
		  szInfoLog = null;
		  
		  
		 GLES31.glGetShaderiv(fragmentShaderObject_PV,
		                      GLES31.GL_COMPILE_STATUS,
		                      iShaderCompileStatus,
		                      0);
		                      
                if(iShaderCompileStatus[0] == GLES31.GL_FALSE)
                {
                        GLES31.glGetShaderiv(fragmentShaderObject_PV,
                                             GLES31.GL_INFO_LOG_LENGTH,
                                             iInfoLogLength,
                                             0);
                                             
                        if(iInfoLogLength[0] > 0)
                        {
                            szInfoLog = GLES31.glGetShaderInfoLog(fragmentShaderObject_PV);    
                            
                            System.out.println("RTR: fragment shader ERROR: " + szInfoLog);
                            //uninitialize();
                            System.exit(0);
                        }
                        
                }
                System.out.println("RTR: After Fragment Shader..");
                
		//Create ShaderProgramObject and attach above shaders
		
		     shaderProgramObject_PV = GLES31.glCreateProgram();
		     
		     GLES31.glAttachShader(shaderProgramObject_PV, vertexShaderObject_PV);
		     GLES31.glAttachShader(shaderProgramObject_PV, fragmentShaderObject_PV);
		     
		     
		 //*** PRELINKING BINDING TO VERTEX ATTRIBUTES***
		 GLES31.glBindAttribLocation(shaderProgramObject_PV,
		                             GLESMacros.AMC_ATTRIBUTE_POSITION,
		                             "vPosition");

		GLES31.glBindAttribLocation(shaderProgramObject_PV,
									GLESMacros.AMC_ATTRIBUTE_NORMAL,
									"vNormal");

			System.out.println("RTR: shader pre link program successfull : " );
                //Link above program
                GLES31.glLinkProgram(shaderProgramObject_PV);
                
                //ERROR checking for Linking
                
                 iShaderLinkStatus[0] = 0;
                iInfoLogLength[0] = 0;
				szInfoLog = null;
		
		System.out.println( "RTR: iShaderLinkStatus before " + iShaderLinkStatus[0]);

		GLES31.glGetProgramiv(shaderProgramObject_PV,
		                      GLES31.GL_LINK_STATUS,
		                      iShaderLinkStatus,
		                      0);

		

		  if(iShaderLinkStatus[0] == GLES31.GL_FALSE)
		  {
		        System.out.println( "RTR: iShaderLinkStatus after " + iShaderLinkStatus[0]);
		        GLES31.glGetProgramiv(shaderProgramObject_PV,
		                              GLES31.GL_INFO_LOG_LENGTH,
		                              iInfoLogLength,
		                              0 );
		                              
                        if(iInfoLogLength[0] > 0)
                        {
                              szInfoLog = GLES31.glGetProgramInfoLog(shaderProgramObject_PV);
                              
                              System.out.println( "RTR: iInfoLogLength " + iInfoLogLength[0]);
                               System.out.println("RTR: shader program ERROR : " + szInfoLog);
                                //uninitialize();
                                System.exit(0);
                        }
		  }
		  
		 ///***POST LINKING GETTING UNIFORMS**
		 
		mUniform_PV= GLES31.glGetUniformLocation(shaderProgramObject_PV, "u_model_matrix");
		
		vUniform_PV = GLES31.glGetUniformLocation(shaderProgramObject_PV, "u_view_matrix");

		projectionUniform_PV = GLES31.glGetUniformLocation(shaderProgramObject_PV, "u_projection_matrix");

		isLKeyIsPressed_PV = GLES31.glGetUniformLocation(shaderProgramObject_PV,
												"islkeypressed_PV");

		laUniform_PV_RED = GLES31.glGetUniformLocation(shaderProgramObject_PV, "u_la_PV_RED");
		ldUniform_PV_RED= GLES31.glGetUniformLocation(shaderProgramObject_PV, "u_ld_PV_RED");
		lsUniform_PV_RED = GLES31.glGetUniformLocation(shaderProgramObject_PV, "u_ls_PV_RED");

		laUniform_PV_GREEN = GLES31.glGetUniformLocation(shaderProgramObject_PV, "u_la_PV_GREEN");
		ldUniform_PV_GREEN = GLES31.glGetUniformLocation(shaderProgramObject_PV, "u_ld_PV_GREEN");
		lsUniform_PV_GREEN = GLES31.glGetUniformLocation(shaderProgramObject_PV, "u_ls_PV_GREEN");

		laUniform_PV_BLUE = GLES31.glGetUniformLocation(shaderProgramObject_PV, "u_la_PV_BLUE");
		ldUniform_PV_BLUE = GLES31.glGetUniformLocation(shaderProgramObject_PV, "u_ld_PV_BLUE");
		lsUniform_PV_BLUE = GLES31.glGetUniformLocation(shaderProgramObject_PV, "u_ls_PV_BLUE");

		kaUniform_PV = GLES31.glGetUniformLocation(shaderProgramObject_PV, "u_ka_PV");
		kdUniform_PV = GLES31.glGetUniformLocation(shaderProgramObject_PV, "u_kd_PV");
		ksUniform_PV = GLES31.glGetUniformLocation(shaderProgramObject_PV, "u_ks_PV");

		shininessUniform_PV = GLES31.glGetUniformLocation(shaderProgramObject_PV, "u_shininess_PV");


		lightPositionUniform_PV_RED = GLES31.glGetUniformLocation(shaderProgramObject_PV,
												"u_light_position_PV_RED");
		lightPositionUniform_PV_GREEN = GLES31.glGetUniformLocation(shaderProgramObject_PV,
												"u_light_position_PV_GREEN");
		lightPositionUniform_PV_BLUE = GLES31.glGetUniformLocation(shaderProgramObject_PV,
												"u_light_position_PV_BLUE");
		
		System.out.println("RTR: After Post Linking");
		
		 //Sphere call
			Sphere sphere=new Sphere();

	        float sphere_vertices[]=new float[1146];
	        float sphere_normals[]=new float[1146];
	        float sphere_textures[]=new float[764];
	        short sphere_elements[]=new short[2280];

	        sphere.getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);

	        numVertices = sphere.getNumberOfSphereVertices();
	        numElements = sphere.getNumberOfSphereElements();
		


		////
		
                // vao
        GLES31.glGenVertexArrays(1,vao_sphere,0);
        GLES31.glBindVertexArray(vao_sphere[0]);
        
        // position vbo
        GLES31.glGenBuffers(1,vbo_sphere_position,0);
        GLES31.glBindBuffer(GLES31.GL_ARRAY_BUFFER,vbo_sphere_position[0]);
        
        ByteBuffer byteBuffer=ByteBuffer.allocateDirect(sphere_vertices.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());
        FloatBuffer verticesBuffer=byteBuffer.asFloatBuffer();
        verticesBuffer.put(sphere_vertices);
        verticesBuffer.position(0);
        
        GLES31.glBufferData(GLES31.GL_ARRAY_BUFFER,
                            sphere_vertices.length * 4,
                            verticesBuffer,
                            GLES31.GL_STATIC_DRAW);
        
        GLES31.glVertexAttribPointer(GLESMacros.AMC_ATTRIBUTE_POSITION,
                                     3,
                                     GLES31.GL_FLOAT,
                                     false,0,0);
        
        GLES31.glEnableVertexAttribArray(GLESMacros.AMC_ATTRIBUTE_POSITION);
        
        GLES31.glBindBuffer(GLES31.GL_ARRAY_BUFFER,0);
        
        // normal vbo
        GLES31.glGenBuffers(1,vbo_sphere_normal,0);
        GLES31.glBindBuffer(GLES31.GL_ARRAY_BUFFER,vbo_sphere_normal[0]);
        
        byteBuffer=ByteBuffer.allocateDirect(sphere_normals.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());
        verticesBuffer=byteBuffer.asFloatBuffer();
        verticesBuffer.put(sphere_normals);
        verticesBuffer.position(0);
        
        GLES31.glBufferData(GLES31.GL_ARRAY_BUFFER,
                            sphere_normals.length * 4,
                            verticesBuffer,
                            GLES31.GL_STATIC_DRAW);
        
        GLES31.glVertexAttribPointer(GLESMacros.AMC_ATTRIBUTE_NORMAL,
                                     3,
                                     GLES31.GL_FLOAT,
                                     false,0,0);
        
        GLES31.glEnableVertexAttribArray(GLESMacros.AMC_ATTRIBUTE_NORMAL);
        
        GLES31.glBindBuffer(GLES31.GL_ARRAY_BUFFER,0);
        
        // element vbo
        GLES31.glGenBuffers(1,vbo_sphere_element,0);
        GLES31.glBindBuffer(GLES31.GL_ELEMENT_ARRAY_BUFFER,vbo_sphere_element[0]);
        
        byteBuffer=ByteBuffer.allocateDirect(sphere_elements.length * 2);
        byteBuffer.order(ByteOrder.nativeOrder());
        ShortBuffer elementsBuffer=byteBuffer.asShortBuffer();
        elementsBuffer.put(sphere_elements);
        elementsBuffer.position(0);
        
        GLES31.glBufferData(GLES31.GL_ELEMENT_ARRAY_BUFFER,
                            sphere_elements.length * 2,
                            elementsBuffer,
                            GLES31.GL_STATIC_DRAW);
        
        GLES31.glBindBuffer(GLES31.GL_ELEMENT_ARRAY_BUFFER,0);

        GLES31.glBindVertexArray(0);
                 
        System.out.println("RTR: After vbo_sphere_normal");

		/////////////////
         GLES31.glEnable(GLES31.GL_DEPTH_TEST);
                 
         GLES31.glDepthFunc(GLES31.GL_LEQUAL);

		 GLES31.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		 
		 Matrix.setIdentityM(perspectiveProjectionMatrix, 0);
		 
		 System.out.println("RTR: End  Initialize()");
	 }
		
	private void resize(int width, int height)
	{
		
		if(height == 0)
	        {
		        height = 1;
	        }
	        GLES31.glViewport(0, 0, width, height);
        
			
		Matrix.perspectiveM(perspectiveProjectionMatrix, 0 ,45.0f,(float) width/(float)height, 0.1f, 100.0f);        
			
	        
		System.out.println("RTR: In resize()");
	}
	
	private void display()
	{
		System.out.println("RTR: Start of display()");
		
		
		float[] modelMatrix = new float[16];
		float[] viewMatrix = new float[16];
		float[] translateMatrix = new float[16];
                
		 
                
		GLES31.glClear(GLES31.GL_COLOR_BUFFER_BIT | GLES31.GL_DEPTH_BUFFER_BIT);
		
		//set to identity matrix
		Matrix.setIdentityM(modelMatrix, 0);
		Matrix.setIdentityM(viewMatrix, 0);
		Matrix.setIdentityM(translateMatrix, 0);
		
		
		Matrix.translateM(modelMatrix, 0 ,translateMatrix, 0, 0.0f, 0.0f, -2.0f);
		
		if(bPerVertex)
		{
			GLES31.glUseProgram(shaderProgramObject_PV);
		//light uniform 
			if (bIsLighting == true)
			{
				GLES31.glUniform1f(isLKeyIsPressed_PV, 1.0f);

				GLES31.glUniform1f(shininessUniform_PV, materialShininess);

				GLES31.glUniform3fv(laUniform_PV_RED, 1, lightAmbient_Red, 0);
				GLES31.glUniform3fv(laUniform_PV_BLUE, 1, lightAmbient_Blue, 0);
				GLES31.glUniform3fv(laUniform_PV_GREEN, 1, lightAmbient_Green, 0);

				GLES31.glUniform3fv(ldUniform_PV_RED, 1, lightDifuse_Red, 0);
				GLES31.glUniform3fv(ldUniform_PV_GREEN, 1, lightDifuse_Green, 0);
				GLES31.glUniform3fv(ldUniform_PV_BLUE, 1, lightDifuse_Blue, 0);

				GLES31.glUniform3fv(lsUniform_PV_RED, 1, lightSpecular_Red, 0);
				GLES31.glUniform3fv(lsUniform_PV_GREEN, 1, lightSpecular_Green, 0);
				GLES31.glUniform3fv(lsUniform_PV_BLUE, 1, lightSpecular_Blue, 0);

				GLES31.glUniform3fv(kaUniform_PV, 1, materialAmbient, 0);
				GLES31.glUniform3fv(kdUniform_PV, 1, materialDifuse, 0);
				GLES31.glUniform3fv(ksUniform_PV, 1, materialSpecular, 0);


				GLES31.glUniform4fv(lightPositionUniform_PV_RED, 1, lightPosition_Red, 0);
				GLES31.glUniform4fv(lightPositionUniform_PV_GREEN, 1, lightPosition_Green, 0);
				GLES31.glUniform4fv(lightPositionUniform_PV_BLUE, 1, lightPosition_Blue, 0);

			}
			else
			{
				GLES31.glUniform1f(isLKeyIsPressed_PV, 0.0f);
			}
			GLES31.glUniformMatrix4fv(mUniform_PV,				
									1,				
									false,				
									modelMatrix,
									0 );

		GLES31.glUniformMatrix4fv(vUniform_PV,				
									1,				
									false,				
									viewMatrix,
									0 );
		
		GLES31.glUniformMatrix4fv(projectionUniform_PV,				
									1,				
									false,				
									perspectiveProjectionMatrix,
									0 );
		 }
		 
		 if(bPerFragment)
		 {
		 	GLES31.glUseProgram(shaderProgramObject_PF);

		 	if (bIsLighting == true)
			{
				GLES31.glUniform1f(isLKeyIsPressed_PF, 1.0f);

				GLES31.glUniform1f(shininessUniform_PF, materialShininess);

				GLES31.glUniform3fv(laUniform_PF_RED, 1, lightAmbient_Red, 0);
				GLES31.glUniform3fv(laUniform_PF_GREEN, 1, lightAmbient_Green, 0);
				GLES31.glUniform3fv(laUniform_PF_BLUE, 1, lightAmbient_Blue, 0);

				GLES31.glUniform3fv(ldUniform_PF_RED, 1, lightDifuse_Red, 0);
				GLES31.glUniform3fv(ldUniform_PF_GREEN, 1, lightDifuse_Green, 0);
				GLES31.glUniform3fv(ldUniform_PF_BLUE, 1, lightDifuse_Blue, 0);

				GLES31.glUniform3fv(lsUniform_PF_RED, 1, lightSpecular_Red, 0);
				GLES31.glUniform3fv(lsUniform_PF_GREEN, 1, lightSpecular_Green, 0);
				GLES31.glUniform3fv(lsUniform_PF_BLUE, 1, lightSpecular_Blue, 0);

				GLES31.glUniform3fv(kaUniform_PF, 1, materialAmbient, 0);
				GLES31.glUniform3fv(kdUniform_PF, 1, materialDifuse, 0);
				GLES31.glUniform3fv(ksUniform_PF, 1, materialSpecular, 0);


				GLES31.glUniform4fv(lightPositionUniform_PF_RED, 1, lightPosition_Red, 0);
				GLES31.glUniform4fv(lightPositionUniform_PF_GREEN, 1, lightPosition_Green, 0);
				GLES31.glUniform4fv(lightPositionUniform_PF_BLUE, 1, lightPosition_Blue, 0);

			}
			else
			{
				GLES31.glUniform1f(isLKeyIsPressed_PF, 0.0f);
			}
			GLES31.glUniformMatrix4fv(mUniform_PF,				
									1,				
									false,				
									modelMatrix,
									0 );

		GLES31.glUniformMatrix4fv(vUniform_PF,				
									1,				
									false,				
									viewMatrix,
									0 );
		
		GLES31.glUniformMatrix4fv(projectionUniform_PF,				
									1,				
									false,				
									perspectiveProjectionMatrix,
									0 );
		 }
		
		

		// bind vao
        GLES31.glBindVertexArray(vao_sphere[0]);
        
        // *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
        GLES31.glBindBuffer(GLES31.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element[0]);

        GLES31.glDrawElements(GLES31.GL_TRIANGLES, numElements, GLES31.GL_UNSIGNED_SHORT, 0);
        
        // unbind vao
        GLES31.glBindVertexArray(0);
		
		requestRender();

		if (angle >= 360.0f)
		{
			angle = 0.0f;
		}
		else
		{
			angle += 0.5f;
		}

		float radians = (float)Math.toRadians(angle); 
		float sinVal = (float)Math.sin(radians);
		float cosVal = (float)Math.cos(radians);

		lightPosition_Red[0] = 0.0f;
		lightPosition_Red[1] = 100.0f * sinVal;
		lightPosition_Red[2] = 100.0f * cosVal;
		lightPosition_Red[3] = 1.0f;

		lightPosition_Green[0] = 100.0f*cosVal;
		lightPosition_Green[1] = 0.0f;
		lightPosition_Green[2] = 100.0f*sinVal;
		lightPosition_Green[3] = 1.0f;

		lightPosition_Blue[0] = 100 * cosVal;
		lightPosition_Blue[1] = 100 * sinVal;
		lightPosition_Blue[2] = 0.0f;
		lightPosition_Blue[3] = 1.0f;
		
		System.out.println("RTR: End of display()");
	}

		private void Uninitialize()
		{
		
	                                
	                    if (vbo_sphere_normal[0] != 0)
		                {
			                GLES31.glDeleteBuffers(1, vbo_sphere_normal , 0);
			                
		                }
		                
	                                
	                    if (vbo_sphere_position[0] != 0)
		                {
			                GLES31.glDeleteBuffers(1, vbo_sphere_position , 0);
			                
		                }

		                if (vbo_sphere_element[0] != 0)
		                {
			                GLES31.glDeleteBuffers(1, vbo_sphere_element , 0);
			                
		                }

		                if (vao_sphere[0] != 0)
		                {
			                GLES31.glDeleteVertexArrays(1, vao_sphere, 0);
			                
		                }
	                 
	            //per vertex       
		       GLES31.glUseProgram(shaderProgramObject_PV);
		       
		       GLES31.glDetachShader(shaderProgramObject_PV, GLES31.GL_FRAGMENT_SHADER );
		       
		       GLES31.glDeleteShader(fragmentShaderObject_PV);
		       
		        GLES31.glDetachShader(shaderProgramObject_PV, GLES31.GL_VERTEX_SHADER );
		       
		       GLES31.glDeleteShader(vertexShaderObject_PV);
		       
		       GLES31.glDeleteProgram(shaderProgramObject_PV);
		       
		       GLES31.glUseProgram(0);
		       
		       //per fragment
		       GLES31.glUseProgram(shaderProgramObject_PF);
		       
		       GLES31.glDetachShader(shaderProgramObject_PF, GLES31.GL_FRAGMENT_SHADER );
		       
		       GLES31.glDeleteShader(fragmentShaderObject_PF);
		       
		        GLES31.glDetachShader(shaderProgramObject_PF, GLES31.GL_VERTEX_SHADER );
		       
		       GLES31.glDeleteShader(vertexShaderObject_PF);
		       
		       GLES31.glDeleteProgram(shaderProgramObject_PF);
		       
		       GLES31.glUseProgram(0);
		       
		       System.out.println("Uninitialize successfull");

		}
    }

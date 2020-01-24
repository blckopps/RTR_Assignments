package com.example.spheres;

import android.opengl.GLSurfaceView;
import android.opengl.GLES31;
import javax.microedition.khronos.opengles.GL10;
import javax.microedition.khronos.egl.EGLConfig;
import java.lang.Math;
import java.util.*;		//collections
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
	//Uniforms for PV
	private int mUniform;
	private int vUniform;
	private int projectionUniform;
	
	private int gWidth, gHeight;
	////
	private int sampler_Uniform;
	
	//For PV
	private int laUniform_PV;
	private int ldUniform_PV;
	private int lsUniform_PV;

	private int kaUniform_PV;
	private int kdUniform_PV;
	private int ksUniform_PV;

	private int shininessUniform_PV;

	private int lightPositionUniform_PV;


	private int isLKeyIsPressed_PV;
	//For PF
	private int laUniform_PF;
	private int ldUniform_PF;
	private int lsUniform_PF;

	private int kaUniform_PF;
	private int kdUniform_PF;
	private int ksUniform_PF;

	private int shininessUniform_PF;

	private int lightPositionUniform_PF;


	private int isLKeyIsPressed_PF;

	private int NUMOFSPHERES = 24;
	private float angle_X =0.0f;
	private int iRotation = 0;

	private float[] perspectiveProjectionMatrix = new float[16];   //4*4 matrix

	//Light arrays
	private float lightAmbient[] = new float[ ]{ 0.0f, 0.0f, 0.0f, 0.0f};
	private float lightDifuse[] = new float[ ] { 1.0f, 1.0f, 1.0f, 1.0f };
	private float lightSpecular[] = new float[ ] {1.0f, 1.0f, 1.0f, 1.0f };

	//material array
	private float materialAmbient[] = new float[ ]{0.0f, 0.0f, 0.0f, 0.0f};
	private float materialDifuse[] = new float[ ]{1.0f, 1.0f, 1.0f, 1.0f };
	private float materialSpecular[] = new float[  ]{1.0f, 1.0f, 1.0f, 1.0f};

	private float lightPosition[] = new float[ ]{100.0f, 100.0f, 100.0f, 1.0f};

	private float materialShininess = 128.0f;	
	//
	Boolean bFullScreen = false;
	Boolean bIsLighting = false;
	Boolean bPerVertex = true;
	Boolean bPerFragment = false;
	
	//
	final float[] cube_vertices = new float[12];
	
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
		 bPerFragment = !bPerFragment;
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
		 if(iRotation == 2)
		 {
		 	iRotation = 0;
		 }
		 else
		 {
		 	iRotation++;
		 }
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
		 System.out.println("RTR: Opengl version: " + version);
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
												"uniform vec4 u_light_position_PF;" +

												"out vec3 tnorm_PF;" +
												"out vec3 light_direction_PF;" +
												"out vec3 viewer_vector_PF;" +
												"void main(void)" +
												"{" +
													"if(islkeypressed_PF == 1.0)" +
													"{" +
														"vec4 eye_coordinates = u_view_matrix *  u_model_matrix  * vPosition;" +
														"tnorm_PF = mat3(u_view_matrix * u_model_matrix) * vNormal;" +
														"light_direction_PF = vec3(u_light_position_PF - eye_coordinates);" +
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
												"uniform vec3 u_la_PF;" +
												"uniform vec3 u_ld_PF;" +
												"uniform vec3 u_ls_PF;" +
												"uniform vec3 u_ka_PF;" +
												"uniform vec3 u_kd_PF;" +
												"uniform vec3 u_ks_PF;" +
												"in vec3 tnorm_PF;" +
												"in vec3 light_direction_PF;" +
												"in vec3 viewer_vector_PF;" +
												"uniform float u_shininess_PF;" +
												"void main(void)" +
												"{" +
													"if(islkeypressed_PF == 1.0)" +
													"{" +
														"vec3 tnorm_normalized = normalize(tnorm_PF);" +
														"vec3 light_direction_normalized = normalize(light_direction_PF);" +
														"vec3 viewer_vector_normalized = normalize(viewer_vector_PF);" +
														"float tn_dot_ldirection = max(dot(light_direction_normalized, tnorm_normalized), 0.0);" +
														"vec3 reflection_vector = reflect(-light_direction_normalized, tnorm_normalized);" +
														"vec3 ambient = u_la_PF * u_ka_PF;" +
														"vec3 difuse = u_ld_PF * u_kd_PF * tn_dot_ldirection;" +
														"vec3 specular = u_ls_PF * u_ks_PF * pow(max(dot(reflection_vector,viewer_vector_normalized),0.0),u_shininess_PF);" +
														"vec3 phong_light_pf = ambient + difuse + specular;" +
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
		 
		

		isLKeyIsPressed_PF = GLES31.glGetUniformLocation(shaderProgramObject_PF,
												"islkeypressed_PF");

		laUniform_PF = GLES31.glGetUniformLocation(shaderProgramObject_PF, "u_la_PF");
		ldUniform_PF = GLES31.glGetUniformLocation(shaderProgramObject_PF, "u_ld_PF");
		lsUniform_PF = GLES31.glGetUniformLocation(shaderProgramObject_PF, "u_ls_PF");

		kaUniform_PF = GLES31.glGetUniformLocation(shaderProgramObject_PF, "u_ka_PF");
		kdUniform_PF = GLES31.glGetUniformLocation(shaderProgramObject_PF, "u_kd_PF");
		ksUniform_PF = GLES31.glGetUniformLocation(shaderProgramObject_PF, "u_ks_PF");

		shininessUniform_PF = GLES31.glGetUniformLocation(shaderProgramObject_PF, "u_shininess_PF");


		lightPositionUniform_PF = GLES31.glGetUniformLocation(shaderProgramObject_PF,
												"u_light_position_PF");
		
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
												"uniform vec3 u_la_PV;" +
												"uniform vec3 u_ld_PV;" +
												"uniform vec3 u_ls_PV;" +
												"uniform vec3 u_ka_PV;" +
												"uniform vec3 u_kd_PV;" +
												"uniform vec3 u_ks_PV;" +
												"uniform float u_shininess_PV;" +
												"uniform vec4 u_light_position_PV;" +
												"out vec3 phong_ads_light_PV;" +
												"void main(void)" +
												"{" +
													"if(islkeypressed_PV == 1.0)" +
													"{" +
														"vec4 eye_coordinates = u_view_matrix *  u_model_matrix  * vPosition;" +
														"vec3 tnorm = normalize(mat3(u_view_matrix * u_model_matrix) * vNormal);" +
														"vec3 light_direction = normalize(vec3(u_light_position_PV - eye_coordinates));" +
														"float tn_dot_ldirection = max(dot(light_direction, tnorm), 0.0);" +
														"vec3 reflection_vector = reflect(-light_direction, tnorm);" +
														"vec3 viewer_vector = normalize(vec3(-eye_coordinates));" +
														"vec3 ambient = u_la_PV * u_ka_PV;" +
														"vec3 difuse = u_ld_PV * u_kd_PV * tn_dot_ldirection;" +
														"vec3 specular = u_ls_PV * u_ks_PV * pow(max(dot(reflection_vector,viewer_vector),0.0),u_shininess_PV);" +
														"phong_ads_light_PV = ambient + difuse + specular;" +
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
												"uniform float islkeypressed_PV;" +
												"in vec3 phong_ads_light_PV;" +
												"void main(void)" +
												"{" +
													"if(islkeypressed_PV == 1.0)" +
													"{" +
														"fragColor = vec4(phong_ads_light_PV, 1.0);" +
													"}" +
													"else" +
													"{" +
														"fragColor = vec4(phong_ads_light_PV, 1.0);" +
													"}" +
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
		 
		mUniform= GLES31.glGetUniformLocation(shaderProgramObject_PV, "u_model_matrix");
		
		vUniform = GLES31.glGetUniformLocation(shaderProgramObject_PV, "u_view_matrix");

		projectionUniform = GLES31.glGetUniformLocation(shaderProgramObject_PV, "u_projection_matrix");

		isLKeyIsPressed_PV = GLES31.glGetUniformLocation(shaderProgramObject_PV,
												"islkeypressed_PV");

		laUniform_PV = GLES31.glGetUniformLocation(shaderProgramObject_PV, "u_la_PV");
		ldUniform_PV = GLES31.glGetUniformLocation(shaderProgramObject_PV, "u_ld_PV");
		lsUniform_PV = GLES31.glGetUniformLocation(shaderProgramObject_PV, "u_ls_PV");

		kaUniform_PV = GLES31.glGetUniformLocation(shaderProgramObject_PV, "u_ka_PV");
		kdUniform_PV = GLES31.glGetUniformLocation(shaderProgramObject_PV, "u_kd_PV");
		ksUniform_PV = GLES31.glGetUniformLocation(shaderProgramObject_PV, "u_ks_PV");

		shininessUniform_PV = GLES31.glGetUniformLocation(shaderProgramObject_PV, "u_shininess_PV");


		lightPositionUniform_PV = GLES31.glGetUniformLocation(shaderProgramObject_PV,
												"u_light_position_PV");
		
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

		 GLES31.glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
		 
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

        gWidth = width;
        gHeight = height;
			
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
		
		if(!bPerFragment)
		{
			GLES31.glUseProgram(shaderProgramObject_PV);
		//light uniform 
			if (bIsLighting == true)
			{
				GLES31.glUniform1f(isLKeyIsPressed_PV, 1.0f);

				//GLES31.glUniform1f(shininessUniform_PV, materialShininess);

				GLES31.glUniform3fv(laUniform_PV, 1, lightAmbient, 0);
				GLES31.glUniform3fv(ldUniform_PV, 1, lightDifuse, 0);
				GLES31.glUniform3fv(lsUniform_PV, 1, lightSpecular, 0);

				//GLES31.glUniform3fv(kaUniform_PV, 1, materialAmbient, 0);
				//GLES31.glUniform3fv(kdUniform_PV, 1, materialDifuse, 0);
				//GLES31.glUniform3fv(ksUniform_PV, 1, materialSpecular, 0);


				GLES31.glUniform4fv(lightPositionUniform_PV, 1, lightPosition, 0);

			}
			else
			{
				GLES31.glUniform1f(isLKeyIsPressed_PV, 0.0f);
			}
		 }
		 else
		 {
		 	GLES31.glUseProgram(shaderProgramObject_PF);

		 	if (bIsLighting == true)
			{
				GLES31.glUniform1f(isLKeyIsPressed_PF, 1.0f);

				//GLES31.glUniform1f(shininessUniform_PF, materialShininess);

				GLES31.glUniform3fv(laUniform_PF, 1, lightAmbient, 0);
				GLES31.glUniform3fv(ldUniform_PF, 1, lightDifuse, 0);
				GLES31.glUniform3fv(lsUniform_PF, 1, lightSpecular, 0);

				// GLES31.glUniform3fv(kaUniform_PF, 1, materialAmbient, 0);
				// GLES31.glUniform3fv(kdUniform_PF, 1, materialDifuse, 0);
				// GLES31.glUniform3fv(ksUniform_PF, 1, materialSpecular, 0);


				GLES31.glUniform4fv(lightPositionUniform_PF, 1, lightPosition, 0);

			}
			else
			{
				GLES31.glUniform1f(isLKeyIsPressed_PF, 0.0f);
			}
		 }
		
		// GLES31.glUniformMatrix4fv(mUniform,				
		// 							1,				
		// 							false,				
		// 							modelMatrix,
		// 							0 );

		GLES31.glUniformMatrix4fv(vUniform,				
									1,				
									false,				
									viewMatrix,
									0 );
		
		GLES31.glUniformMatrix4fv(projectionUniform,				
									1,				
									false,				
									perspectiveProjectionMatrix,
									0 );
		int xPos = 0;
		int yPos = gHeight - gHeight / 6;

		for(int i = 1; i<= NUMOFSPHERES; i++)
		{

			getCurrentArray(i);
			
			Matrix.setIdentityM(modelMatrix, 0);

			Matrix.translateM(modelMatrix, 0 ,translateMatrix, 0, 0.0f, 0.0f, -2.0f);

			if(!bPerFragment)
			{
				
				if (bIsLighting == true)
				{
					
					GLES31.glUniform1f(shininessUniform_PV, materialShininess);


					GLES31.glUniform3fv(kaUniform_PV, 1, materialAmbient, 0);
					GLES31.glUniform3fv(kdUniform_PV, 1, materialDifuse, 0);
					GLES31.glUniform3fv(ksUniform_PV, 1, materialSpecular, 0);

				}
			 }
			 else
			 {
			 	if (bIsLighting == true)
				{
					
					GLES31.glUniform1f(shininessUniform_PF, materialShininess);


					GLES31.glUniform3fv(kaUniform_PF, 1, materialAmbient, 0);
					GLES31.glUniform3fv(kdUniform_PF, 1, materialDifuse, 0);
					GLES31.glUniform3fv(ksUniform_PF, 1, materialSpecular, 0);

				}
			 }

			 GLES31.glUniformMatrix4fv(mUniform,				
										1,				
										false,				
										modelMatrix,
										0 );

			 GLES31.glViewport(xPos, yPos, gWidth/4, gHeight/6);

			// bind vao
	        GLES31.glBindVertexArray(vao_sphere[0]);
	        
	        // *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
	        GLES31.glBindBuffer(GLES31.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element[0]);

	        GLES31.glDrawElements(GLES31.GL_TRIANGLES, numElements, GLES31.GL_UNSIGNED_SHORT, 0);
	        
	        // unbind vao
	        GLES31.glBindVertexArray(0);

		     if (i % 6 == 0)
			{
				xPos += gWidth / 4;
				yPos = gHeight - gHeight / 6;
			}
			else
			{
				yPos -= gHeight / 6;
			}

		}
		requestRender();
		
			if (angle_X >= 360.0f)
			{
				angle_X = 0.0f;
			}
			else
			{
				angle_X += 0.005f;
			}
			if (iRotation == 0)
			{
				lightPosition[0] = 0.0f;
				lightPosition[1] = 100.0f*(float)Math.sin(angle_X);
				lightPosition[2] = 100.0f *(float)Math.cos(angle_X);
				lightPosition[3] = 1.0f; 
			}
			else if (iRotation == 1)
			{
				lightPosition[0] = 100.0f*(float)Math.sin(angle_X);
				lightPosition[1] = 0.0f;
				lightPosition[2] = 100.0f *(float)Math.cos(angle_X);
				lightPosition[3] = 1.0f;
			}
			else
			{
				lightPosition[0] = 100.0f*(float)Math.sin(angle_X);
				lightPosition[1] = 100.0f *(float)Math.cos(angle_X);
				lightPosition[2] = 0.0f;
				lightPosition[3] = 1.0f;
			}

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

		private void getCurrentArray(int index)
		{
				if(index == 1)
				{
					materialAmbient[0] =  (0.0215f);
					materialAmbient[1] =  (0.1745f);
					materialAmbient[2] =  (0.0215f);
					////materialAmbient[3] =  (1.0f);
					
					materialDifuse[0] =  (0.07568f);
					materialDifuse[1] =  (0.61424f);
					materialDifuse[2] =  (0.07568f);
					//materialDifuse[3] =  (1.0f);
					
					materialSpecular[0] =  (0.633f);
					materialSpecular[1] =  (0.727811f);
					materialSpecular[2] =  (0.633f);
					//materialSpecular[3] =  (1.0f);
									
					materialShininess =  (0.6f*128.0f);
				}
				else if(index == 2)
				{
					materialAmbient[0] =  (0.135f);
					materialAmbient[1] =  (0.2225f);
					materialAmbient[2] =  (0.1575f);
					//materialAmbient[3] =  (1.0f);
					
					materialDifuse[0] =  (0.54f);
					materialDifuse[1] =  (0.89f);
					materialDifuse[2] =  (0.63f);
					//materialDifuse[3] =  (1.0f);
					
					materialSpecular[0] =  (0.316228f);
					materialSpecular[1] =  (0.316228f);
					materialSpecular[2] =  (0.316228f);
					//materialSpecular[3] =  (1.0f);
					
					materialShininess =  (0.1f*128.0f);
				}
				else if(index == 3)
				{
					materialAmbient[0] =  (0.05375f);
					materialAmbient[1] =  (0.05f);
					materialAmbient[2] =  (0.06625f);
					//materialAmbient[3] =  (1.0f);
					
					materialDifuse[0] =  (0.18275f);
					materialDifuse[1] =  (0.17f);
					materialDifuse[2] =  (0.22525f);
					//materialDifuse[3] =  (1.0f);
					
					materialSpecular[0] =  (0.332741f);
					materialSpecular[1] =  (0.328634f);
					materialSpecular[2] =  (0.346435f);
					//materialSpecular[3] =  (1.0f);
					
					materialShininess =  (0.3f*128.0f);
				}
				else if(index == 4)
				{
					materialAmbient[0] =  (0.25f);
				materialAmbient[1] =  (0.20725f);
				materialAmbient[2] =  (0.20725f);
				//materialAmbient[3] =  (1.0f);
				
				materialDifuse[0] =  (1.0f);
				materialDifuse[1] =  (0.829f);
				materialDifuse[2] =  (0.829f);
				//materialDifuse[3] =  (1.0f);
				
				materialSpecular[0] =  (0.296648f);
				materialSpecular[1] =  (0.296648f);
				materialSpecular[2] =  (0.296648f);
				//materialSpecular[3] =  (1.0f);
				
				materialShininess =  (0.88f*128.0f);
				}
				else if(index == 5)
				{
					materialAmbient[0] =  (0.1745f);
					materialAmbient[1] =  (0.01175f);
					materialAmbient[2] =  (0.01175f);
					//materialAmbient[3] =  (1.0f);
					
					materialDifuse[0] =  (0.61424f);
					materialDifuse[1] =  (0.04136f);
					materialDifuse[2] =  (0.04136f);
					//materialDifuse[3] =  (1.0f);
					
					materialSpecular[0] =  (0.727811f);
					materialSpecular[1] =  (0.626959f);
					materialSpecular[2] =  (0.626959f);
					//materialSpecular[3] =  (1.0f);
					
					materialShininess =  (0.6f* 128.0f);
				}
				else if(index == 6)
				{
					materialAmbient[0] =  (0.1f);
				materialAmbient[1] =  (0.18725f);
				materialAmbient[2] =  (0.1745f);
				//materialAmbient[3] =  (1.0f);
				
				materialDifuse[0] =  (0.396f);
				materialDifuse[1] =  (0.74151f);
				materialDifuse[2] =  (0.69102f);
				//materialDifuse[3] =  (1.0f);
				
				materialSpecular[0] =  (0.297254f);
				materialSpecular[1] =  (0.30829f);
				materialSpecular[2] =  (0.30829f);
				//materialSpecular[3] =  (1.0f);
				
				materialShininess =  (0.1f*128.0f);
				}
				else if(index == 7)
				{
					materialAmbient[0] =  (0.329412f);
					materialAmbient[1] =  (0.223529f);
					materialAmbient[2] =  (0.027451f);
					//materialAmbient[3] =  (1.0f);

					materialDifuse[0] =  (0.780392f);
					materialDifuse[1] =  (0.568627f);
					materialDifuse[2] =  (0.113725f);
					//materialDifuse[3] =  (1.0f);

					materialSpecular[0] =  (0.992157f);
					materialSpecular[1] =  (0.941176f);
					materialSpecular[2] =  (0.807843f);
					//materialSpecular[3] =  (1.0f);

					materialShininess =  (0.21794872f*128.0f);
				}
				else if(index == 8)
				{
					materialAmbient[0] =  (0.2125f);
					materialAmbient[1] =  (0.1275f);
					materialAmbient[2] =  (0.054f);
					//materialAmbient[3] =  (1.0f);
				
					materialDifuse[0] =  (0.714f);
					materialDifuse[1] =  (0.4284f);
					materialDifuse[2] =  (0.18144f);
					//materialDifuse[3] =  (1.0f);
				
					materialSpecular[0] =  (0.393548f);
					materialSpecular[1] =  (0.271906f);
					materialSpecular[2] =  (0.166721f);
					//materialSpecular[3] =  (1.0f);
				
					materialShininess =  (0.2f*128.0f);
				}
				else if(index == 9)
				{
					materialAmbient[0] =  (0.25f);
					materialAmbient[1] =  (0.25f);
					materialAmbient[2] =  (0.25f);
					//materialAmbient[3] =  (1.0f);
				
					materialDifuse[0] =  (0.4f);
					materialDifuse[1] =  (0.4f);
					materialDifuse[2] =  (0.4f);
					//materialDifuse[3] =  (1.0f);
				
					materialSpecular[0] =  (0.774597f);
					materialSpecular[1] =  (0.774597f);
					materialSpecular[2] =  (0.774597f);
					//materialSpecular[3] =  (1.0f);
				
					materialShininess =  (0.6f*128.0f);
				}
				else if(index == 10)
				{
					materialAmbient[0] =  (0.19125f);
					materialAmbient[1] =  (0.0735f);
					materialAmbient[2] =  (0.0225f);
					//materialAmbient[3] =  (1.0f);
				
					materialDifuse[0] =  (0.7038f);
					materialDifuse[1] =  (0.27048f);
					materialDifuse[2] =  (0.0828f);
					//materialDifuse[3] =  (1.0f);
				
					materialSpecular[0] =  (0.256777f);
					materialSpecular[1] =  (0.137622f);
					materialSpecular[2] =  (0.086014f);
					//materialSpecular[3] =  (1.0f);
				
					materialShininess =  (0.1f*128.0f);
				}
				else if(index == 11)
				{
					materialAmbient[0] =  (0.24725f);
					materialAmbient[1] =  (0.1995f);
					materialAmbient[2] =  (0.0745f);
					//materialAmbient[3] =  (1.0f);
				
					materialDifuse[0] =  (0.75164f);
					materialDifuse[1] =  (0.60648f);
					materialDifuse[2] =  (0.22648f);
					//materialDifuse[3] =  (1.0f);
				
					materialSpecular[0] =  (0.628281f);
					materialSpecular[1] =  (0.555802f);
					materialSpecular[2] =  (0.366065f);
					//materialSpecular[3] =  (1.0f);
				
					materialShininess =  (0.4f*128.0f);
				}
				else if(index == 12)
				{
					materialAmbient[0] =  (0.19225f);
					materialAmbient[1] =  (0.19225f);
					materialAmbient[2] =  (0.19225f);
					//materialAmbient[3] =  (1.0f);

					materialDifuse[0] =  (0.50754f);
					materialDifuse[1] =  (0.50754f);
					materialDifuse[2] =  (0.50754f);
					//materialDifuse[3] =  (1.0f);

					materialSpecular[0] =  (0.508273f);
					materialSpecular[1] =  (0.508273f);
					materialSpecular[2] =  (0.508273f);
					//materialSpecular[3] =  (1.0f);

					materialShininess =  (0.4f*128.0f);
				}
				else if(index == 13)
				{
					materialAmbient[0] =  (0.0f);
					materialAmbient[1] =  (0.0f);
					materialAmbient[2] =  (0.0f);
					//materialAmbient[3] =  (1.0f);
				
					materialDifuse[0] =  (0.01f);
					materialDifuse[1] =  (0.01f);
					materialDifuse[2] =  (0.01f);
					//materialDifuse[3] =  (1.0f);
				
					materialSpecular[0] =  (0.50f);
					materialSpecular[1] =  (0.50f);
					materialSpecular[2] =  (0.50f);
					//materialSpecular[3] =  (1.0f);
				
					materialShininess =  (0.25f*128.0f);
				}
				else if(index == 14)
				{
					materialAmbient[0] =  (0.0f);
					materialAmbient[1] =  (0.1f);
					materialAmbient[2] =  (0.06f);
					//materialAmbient[3] =  (1.0f);
				
					materialDifuse[0] =  (0.0f);
					materialDifuse[1] =  (0.50980392f);
					materialDifuse[2] =  (0.50980392f);
					//materialDifuse[3] =  (1.0f);
				
					materialSpecular[0] =  (0.50196078f);
					materialSpecular[1] =  (0.50196078f);
					materialSpecular[2] =  (0.50196078f);
					//materialSpecular[3] =  (1.0f);
				
					materialShininess =  (0.25f*128.0f);
				}
				else if(index == 15)
				{
					materialAmbient[0] =  (0.0f);
					materialAmbient[1] =  (0.0f);
					materialAmbient[2] =  (0.0f);
					//materialAmbient[3] =  (1.0f);
				
					materialDifuse[0] =  (0.1f);
					materialDifuse[1] =  (0.35f);
					materialDifuse[2] =  (0.1f);
					//materialDifuse[3] =  (1.0f);
				
					materialSpecular[0] =  (0.45f);
					materialSpecular[1] =  (0.55f);
					materialSpecular[2] =  (0.45f);
					//materialSpecular[3] =  (1.0f);
				
					materialShininess =  (0.25f*128.0f);
				}
				else if(index == 16)
				{
					materialAmbient[0] =  (0.0f);
					materialAmbient[1] =  (0.0f);
					materialAmbient[2] =  (0.0f);
					//materialAmbient[3] =  (1.0f);
				
					materialDifuse[0] =  (0.5f);
					materialDifuse[1] =  (0.0f);
					materialDifuse[2] =  (0.0f);
					//materialDifuse[3] =  (1.0f);
				
					materialSpecular[0] =  (0.7f);
					materialSpecular[1] =  (0.6f);
					materialSpecular[2] =  (0.6f);
					//materialSpecular[3] =  (1.0f);
				
					materialShininess =  (0.25f*128.0f);
				}
				else if(index == 17)
				{
					materialAmbient[0] =  (0.0f);
					materialAmbient[1] =  (0.0f);
					materialAmbient[2] =  (0.0f);
					//materialAmbient[3] =  (1.0f);
				
					materialDifuse[0] =  (0.55f);
					materialDifuse[1] =  (0.55f);
					materialDifuse[2] =  (0.55f);
					//materialDifuse[3] =  (1.0f);
				
					materialSpecular[0] =  (0.70f);
					materialSpecular[1] =  (0.70f);
					materialSpecular[2] =  (0.70f);
					//materialSpecular[3] =  (1.0f);
				
					materialShininess =  (0.25f*128.0f);
				}
				else if(index == 18)
				{
					materialAmbient[0] =  (0.0f);
					materialAmbient[1] =  (0.0f);
					materialAmbient[2] =  (0.0f);
					//materialAmbient[3] =  (1.0f);
				
					materialDifuse[0] =  (0.5f);
					materialDifuse[1] =  (0.5f);
					materialDifuse[2] =  (0.0f);
					//materialDifuse[3] =  (1.0f);
				
					materialSpecular[0] =  (0.60f);
					materialSpecular[1] =  (0.60f);
					materialSpecular[2] =  (0.50f);
					//materialSpecular[3] =  (1.0f);
				
					materialShininess =  (0.25f*128.0f);
				}
				else if(index == 19)
				{
					materialAmbient[0] =  (0.02f);
					materialAmbient[1] =  (0.02f);
					materialAmbient[2] =  (0.02f);
					//materialAmbient[3] =  (1.0f);
				
					materialDifuse[0] =  (0.01f);
					materialDifuse[1] =  (0.01f);
					materialDifuse[2] =  (0.01f);
					//materialDifuse[3] =  (1.0f);
				
					materialSpecular[0] =  (0.4f);
					materialSpecular[1] =  (0.4f);
					materialSpecular[2] =  (0.4f);
					//materialSpecular[3] =  (1.0f);
				
					materialShininess =  (0.078125f*128.0f);
				}
				else if(index == 20)
				{
					materialAmbient[0] =  (0.0f);
					materialAmbient[1] =  (0.05f);
					materialAmbient[2] =  (0.05f);
					//materialAmbient[3] =  (1.0f);
				
					materialDifuse[0] =  (0.4f);
					materialDifuse[1] =  (0.5f);
					materialDifuse[2] =  (0.5f);
					//materialDifuse[3] =  (1.0f);
				
					materialSpecular[0] =  (0.04f);
					materialSpecular[1] =  (0.7f);
					materialSpecular[2] =  (0.7f);
					//materialSpecular[3] =  (1.0f);
				
					materialShininess =  (0.078125f*128.0f);
				}
				else if(index == 21)
				{
					materialAmbient[0] =  (0.0f);
					materialAmbient[1] =  (0.05f);
					materialAmbient[2] =  (0.0f);
					//materialAmbient[3] =  (1.0f);
				
					materialDifuse[0] =  (0.4f);
					materialDifuse[1] =  (0.5f);
					materialDifuse[2] =  (0.4f);
					//materialDifuse[3] =  (1.0f);
				
					materialSpecular[0] =  (0.04f);
					materialSpecular[1] =  (0.7f);
					materialSpecular[2] =  (0.04f);
					//materialSpecular[3] =  (1.0f);
				
					materialShininess =  (0.078125f*128.0f);
				}
				else if(index == 22)
				{
					materialAmbient[0] =  (0.05f);
					materialAmbient[1] =  (0.0f);
					materialAmbient[2] =  (0.0f);
					//materialAmbient[3] =  (1.0f);
				
					materialDifuse[0] =  (0.5f);
					materialDifuse[1] =  (0.4f);
					materialDifuse[2] =  (0.4f);
					//materialDifuse[3] =  (1.0f);
				
					materialSpecular[0] =  (0.7f);
					materialSpecular[1] =  (0.04f);
					materialSpecular[2] =  (0.04f);
					//materialSpecular[3] =  (1.0f);
				
					materialShininess =  (0.078125f*128.0f);
				}
				else if(index == 23)
				{
					materialAmbient[0] =  (0.5f);
					materialAmbient[1] =  (0.5f);
					materialAmbient[2] =  (0.5f);
					//materialAmbient[3] =  (1.0f);

					materialDifuse[0] =  (0.5f);
					materialDifuse[1] =  (0.5f);
					materialDifuse[2] =  (0.5f);
					//materialDifuse[3] =  (1.0f);

					materialSpecular[0] =  (0.7f);
					materialSpecular[1] =  (0.7f);
					materialSpecular[2] =  (0.7f);
					//materialSpecular[3] =  (1.0f);

					materialShininess =  (0.078125f*128.0f);
				}
				else if(index == 24)
				{
					materialAmbient[0] =  (0.05f);
					materialAmbient[1] =  (0.05f);
					materialAmbient[2] =  (0.0f);
					//materialAmbient[3] =  (1.0f);
				
					materialDifuse[0] =  (0.5f);
					materialDifuse[1] =  (0.5f);
					materialDifuse[2] =  (0.4f);
					//materialDifuse[3] =  (1.0f);
				
					materialSpecular[0] =  (0.7f);
					materialSpecular[1] =  (0.7f);
					materialSpecular[2] =  (0.7f);
					//materialSpecular[3] =  (1.0f);
				
					materialShininess =  (0.078125f*128.0f);
				}


		}

    }

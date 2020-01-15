package com.example.sphereadspvpf;

import android.opengl.GLSurfaceView;
import android.opengl.GLES31;
import javax.microedition.khronos.opengles.GL10;
import javax.microedition.khronos.egl.EGLConfig;

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

	private float materialShininess = 50.0f;	
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
		 if(!bPerVertex)
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
		 
		 if(!bPerFragment)
		 {
		 		bPerFragment = true;
		 		bPerVertex = false; 
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

				GLES31.glUniform3fv(laUniform_PV, 1, lightAmbient, 0);
				GLES31.glUniform3fv(ldUniform_PV, 1, lightDifuse, 0);
				GLES31.glUniform3fv(lsUniform_PV, 1, lightSpecular, 0);

				GLES31.glUniform3fv(kaUniform_PV, 1, materialAmbient, 0);
				GLES31.glUniform3fv(kdUniform_PV, 1, materialDifuse, 0);
				GLES31.glUniform3fv(ksUniform_PV, 1, materialSpecular, 0);


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

				GLES31.glUniform1f(shininessUniform_PF, materialShininess);

				GLES31.glUniform3fv(laUniform_PF, 1, lightAmbient, 0);
				GLES31.glUniform3fv(ldUniform_PF, 1, lightDifuse, 0);
				GLES31.glUniform3fv(lsUniform_PF, 1, lightSpecular, 0);

				GLES31.glUniform3fv(kaUniform_PF, 1, materialAmbient, 0);
				GLES31.glUniform3fv(kdUniform_PF, 1, materialDifuse, 0);
				GLES31.glUniform3fv(ksUniform_PF, 1, materialSpecular, 0);


				GLES31.glUniform4fv(lightPositionUniform_PF, 1, lightPosition, 0);

			}
			else
			{
				GLES31.glUniform1f(isLKeyIsPressed_PF, 0.0f);
			}
		 }
		
		GLES31.glUniformMatrix4fv(mUniform,				
									1,				
									false,				
									modelMatrix,
									0 );

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

		// bind vao
        GLES31.glBindVertexArray(vao_sphere[0]);
        
        // *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
        GLES31.glBindBuffer(GLES31.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element[0]);

        GLES31.glDrawElements(GLES31.GL_TRIANGLES, numElements, GLES31.GL_UNSIGNED_SHORT, 0);
        
        // unbind vao
        GLES31.glBindVertexArray(0);
		
		requestRender();
		
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

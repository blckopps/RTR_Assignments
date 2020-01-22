package com.example.pyramidlight;

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
	private int vertexShaderObject;
	private int fragmentShaderObject;
	private int shaderProgramObject;
	
	private int[] vao = new int[1];
	private int[] vbo_position = new int[1];
	
	private int[] vbo_normal = new int[1];
	
	
	//Uniforms
	private int modelMatrixUniform;
	private int viewMatrixUniform;
	private int projectionUniform;

	private int sampler_Uniform;
	
	//light uniforms
	private int laUniform_Red;
	
	private int ldUniform_Red;
	
	private int lsUniform_Red;

	
	private int laUniform_Blue;
	
	private int ldUniform_Blue;
	
	private int lsUniform_Blue;

	
	private int kaUniform;
	
	private int kdUniform;
	
	private int ksUniform;

	
	private int lightPositionUniform_Red;
	
	private int lightPositionUniform_Blue;

	private int isLKeyIsPressed;
	
	private int shininessUniform;

	private float[] perspectiveProjectionMatrix = new float[16];   //4*4 matrix
	
	//
	Boolean bFullScreen = false;
	Boolean bIsLighting = false;
	Boolean bIsAnimation = false;
	
	float angle = 0.0f;
	//
	final float[] pyramid_vertices = new float[12];

	//light
	//light array RED
	float[] lightAmbient_Red = new float[] { 0.0f, 0.0f, 0.0f, 0.0f };
	float[] lightDifuse_Red = new float[]{ 1.0f, 0.0f, 0.0f, 1.0f };
	float[] lightSpecular_Red = new float[]{ 1.0f, 0.0f, 0.0f, 1.0f };

	//light array BLUE
	float[] lightAmbient_Blue = new float[]{ 0.0f, 0.0f, 0.0f, 0.0f };
	float[] lightDifuse_Blue = new float[]{ 0.0f, 0.0f, 1.0f, 1.0f };
	float[] lightSpecular_Blue = new float[]{ 0.0f, 0.0f, 1.0f, 1.0f };

	//material array
	float[] materialAmbient = new float[]{ 0.0f, 0.0f, 0.0f, 0.0f };
	float[] materialDifuse = new float[]{ 1.0f, 1.0f, 1.0f, 1.0f };
	float[] materialSpecular = new float[]{ 1.0f, 1.0f, 1.0f, 1.0f };

	float[] lightPosition_Red = new float[]{ 100.0f, 100.0f, 100.0f, 1.0f };
	float[] lightPosition_Blue = new float[]{ -100.0f, 100.0f, 100.0f, 1.0f };

	float materialShininess = 128.0f;
	
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
		 
		 if(bIsAnimation)
		 {
		 	bIsAnimation = false;	
		 }
		 else
		 {
		 	bIsAnimation = true;

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
		 
	      //// / //*********Vertex shader*********//////
	      
		 vertexShaderObject = GLES31.glCreateShader(GLES31.GL_VERTEX_SHADER);
		 
		final String vertexShaderSourceCode = String.format
		                                (
		                                     "#version 310 es" +
											"\n" +
											"in vec4 vPosition;" +
											"in vec3 vNormal;" +
											"uniform mat4 u_model_matrix;" +
											"uniform mat4 u_view_matrix;" +
											"uniform mat4 u_projection_matrix;" +
											"uniform int islkeypressed;" +
											"uniform vec3 u_la_red;" +
											"uniform vec3 u_ld_red;" +
											"uniform vec3 u_ls_red;" +
											"uniform vec3 u_la_blue;" +
											"uniform vec3 u_ld_blue;" +
											"uniform vec3 u_ls_blue;" +
											"uniform vec3 u_ka;" +
											"uniform vec3 u_kd;" +
											"uniform vec3 u_ks;" +
											"uniform float u_shininess;" +
											"uniform vec4 u_light_position_red;" +
											"uniform vec4 u_light_position_blue;" +
											"out vec3 phong_ads_light;" +
											"void main(void)" +
											"{" +
												"if(islkeypressed == 1)" +
												"{" +
													"vec4 eye_coordinates = u_view_matrix *  u_model_matrix  * vPosition;" +
													"vec3 tnorm = normalize(mat3(u_view_matrix * u_model_matrix) * vNormal);" +

													"vec3 light_direction_red = normalize(vec3(u_light_position_red - eye_coordinates));" +
													"vec3 light_direction_blue = normalize(vec3(u_light_position_blue - eye_coordinates));" +

													"float tn_dot_ldirection_red = max(dot(light_direction_red, tnorm), 0.0);" +
													"float tn_dot_ldirection_blue = max(dot(light_direction_blue, tnorm), 0.0);" +

													"vec3 reflection_vector_red = reflect(-light_direction_red, tnorm);" +
													"vec3 reflection_vector_blue = reflect(-light_direction_blue, tnorm);" +

													"vec3 viewer_vector = normalize(vec3(-eye_coordinates));" +

													"vec3 ambient_red = u_la_red * u_ka;" +
													"vec3 ambient_blue = u_la_blue * u_ka;" +
													
													"vec3 difuse_red = u_ld_red * u_kd * tn_dot_ldirection_red;" +
													"vec3 difuse_blue = u_ld_blue * u_kd * tn_dot_ldirection_blue;" +

													"vec3 specular_red = u_ls_red * u_ks * pow(max(dot(reflection_vector_red,viewer_vector),0.0),u_shininess);" +
													"vec3 specular_blue = u_ls_blue * u_ks * pow(max(dot(reflection_vector_blue,viewer_vector),0.0),u_shininess);" +
													
													"vec3 redLight = ambient_red + difuse_red + specular_red;" +
													"vec3 blueLight = ambient_blue + difuse_blue + specular_blue;" +

													"phong_ads_light = redLight + blueLight;" +
												"}" +
												"else" +
												"{" +
													"phong_ads_light = vec3(1.0, 1.0, 1.0);" +
												"}" +
												"gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" +
											" } "
		                                );
		                                
         GLES31.glShaderSource(vertexShaderObject, vertexShaderSourceCode);
		                 
		 GLES31.glCompileShader(vertexShaderObject);
		 
		 //Vertex shader error checking
		 int[] iShaderCompileStatus = new int[1];
		 int[] iInfoLogLength = new int[1];
		 String szInfoLog = null;
		 
		 GLES31.glGetShaderiv(vertexShaderObject,
		                      GLES31.GL_COMPILE_STATUS,
		                      iShaderCompileStatus,
		                      0);
		                      
		  if(iShaderCompileStatus[0] == GLES31.GL_FALSE)
		  {
		        GLES31.glGetShaderiv(vertexShaderObject,
		                             GLES31.GL_INFO_LOG_LENGTH,
		                             iInfoLogLength,
		                             0 );
		                             
                        if(iInfoLogLength[0] > 0)
                        {
                                szInfoLog = GLES31.glGetShaderInfoLog(vertexShaderObject);
                                
                                System.out.println("RTR: vertex shader ERROR: " + szInfoLog);
                                //uninitialize();
                                System.exit(0);
                        }
		        
		  }
		 
		 System.out.println("RTR: After Vertex Shader..");

        	///////////////////////////////////////****Fragment Shader *******////////////////////////////////
        	
		 
		 fragmentShaderObject = GLES31.glCreateShader(GLES31.GL_FRAGMENT_SHADER);
		 
		 final String fragmentShaderSourceCode = String.format
		                                (
		                                        "#version 310 es" +
												"\n" +
												"precision highp float;" +
												"out vec4 fragColor;" +
												"uniform int islkeypressed;" +
												"in vec3 phong_ads_light;" +
												"void main(void)" +
												"{" +
													"fragColor = vec4(phong_ads_light, 1.0);" +
												"}"
		                                );
		 
		 GLES31.glShaderSource(fragmentShaderObject, fragmentShaderSourceCode);
		 
		 GLES31.glCompileShader(fragmentShaderObject);
		 
		 //Error checking for fragment shader 
		  iShaderCompileStatus[0] = 0;
		  iInfoLogLength[0] = 0;
		  szInfoLog = null;
		  
		  
		 GLES31.glGetShaderiv(fragmentShaderObject,
		                      GLES31.GL_COMPILE_STATUS,
		                      iShaderCompileStatus,
		                      0);
		                      
                if(iShaderCompileStatus[0] == GLES31.GL_FALSE)
                {
                        GLES31.glGetShaderiv(fragmentShaderObject,
                                             GLES31.GL_INFO_LOG_LENGTH,
                                             iInfoLogLength,
                                             0);
                                             
                        if(iInfoLogLength[0] > 0)
                        {
                            szInfoLog = GLES31.glGetShaderInfoLog(fragmentShaderObject);    
                            
                            System.out.println("RTR: fragment shader ERROR: " + szInfoLog);
                            //uninitialize();
                            System.exit(0);
                        }
                        
                }
                System.out.println("RTR: After Fragment Shader..");
                
		//Create ShaderProgramObject and attach above shaders
		
		     shaderProgramObject = GLES31.glCreateProgram();
		     
		     GLES31.glAttachShader(shaderProgramObject, vertexShaderObject);
		     GLES31.glAttachShader(shaderProgramObject, fragmentShaderObject);
		     
		     
		 //*** PRELINKING BINDING TO VERTEX ATTRIBUTES***
		 GLES31.glBindAttribLocation(shaderProgramObject,
		                             GLESMacros.AMC_ATTRIBUTE_POSITION,
		                             "vPosition");

		GLES31.glBindAttribLocation(shaderProgramObject,
									GLESMacros.AMC_ATTRIBUTE_NORMAL,
									"vNormal");

			System.out.println("RTR: shader pre link program successfull : " );
                //Link above program
                GLES31.glLinkProgram(shaderProgramObject);
                
                //ERROR checking for Linking
                
                int[] iShaderLinkStatus = new int[1];
                iInfoLogLength[0] = 0;
				szInfoLog = null;
		
		System.out.println( "RTR: iShaderLinkStatus before " + iShaderLinkStatus[0]);

		GLES31.glGetProgramiv(shaderProgramObject,
		                      GLES31.GL_LINK_STATUS,
		                      iShaderLinkStatus,
		                      0);

		

		  if(iShaderLinkStatus[0] == GLES31.GL_FALSE)
		  {
		        System.out.println( "RTR: iShaderLinkStatus after " + iShaderLinkStatus[0]);
		        GLES31.glGetProgramiv(shaderProgramObject,
		                              GLES31.GL_INFO_LOG_LENGTH,
		                              iInfoLogLength,
		                              0 );
		                              
                        if(iInfoLogLength[0] > 0)
                        {
                              szInfoLog = GLES31.glGetProgramInfoLog(shaderProgramObject);
                              
                              System.out.println( "RTR: iInfoLogLength " + iInfoLogLength[0]);
                               System.out.println("RTR: shader program ERROR : " + szInfoLog);
                                //uninitialize();
                                System.exit(0);
                        }
		  }
		  
		 ///***POST LINKING GETTING UNIFORMS**
		 
		modelMatrixUniform = GLES31.glGetUniformLocation(shaderProgramObject, "u_model_matrix");

		viewMatrixUniform = GLES31.glGetUniformLocation(shaderProgramObject, "u_view_matrix");
		
		projectionUniform = GLES31.glGetUniformLocation(shaderProgramObject, "u_projection_matrix");

		isLKeyIsPressed = GLES31.glGetUniformLocation(shaderProgramObject,
												"islkeypressed");
		//

		//getting light uniforms locations
			
			//RED light uniforms
			laUniform_Red = GLES31.glGetUniformLocation(shaderProgramObject, "u_la_red");
			ldUniform_Red = GLES31.glGetUniformLocation(shaderProgramObject, "u_ld_red");
			lsUniform_Red = GLES31.glGetUniformLocation(shaderProgramObject, "u_ls_red");

			//BLUE light unifomrs
			laUniform_Blue = GLES31.glGetUniformLocation(shaderProgramObject, "u_la_blue");
			ldUniform_Blue = GLES31.glGetUniformLocation(shaderProgramObject, "u_ld_blue");
			lsUniform_Blue = GLES31.glGetUniformLocation(shaderProgramObject, "u_ls_blue");

			kaUniform = GLES31.glGetUniformLocation(shaderProgramObject, "u_ka");
			kdUniform = GLES31.glGetUniformLocation(shaderProgramObject, "u_kd");
			ksUniform = GLES31.glGetUniformLocation(shaderProgramObject, "u_ks");

			shininessUniform = GLES31.glGetUniformLocation(shaderProgramObject, "u_shininess");

			lightPositionUniform_Red = GLES31.glGetUniformLocation(shaderProgramObject, "u_light_position_red");
			lightPositionUniform_Blue = GLES31.glGetUniformLocation(shaderProgramObject, "u_light_position_blue");

		System.out.println("RTR: After Post Linking");
		
		 final float[] pyramid_vertices = new float[]
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

		final float[] normals = new float[]
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
		
		
                GLES31.glGenVertexArrays(1, vao, 0);
                
                GLES31.glBindVertexArray(vao[0]);
                
                GLES31.glGenBuffers(1, vbo_position, 0);
                
                GLES31.glBindBuffer(GLES31.GL_ARRAY_BUFFER, vbo_position[0]);
                
                //create buffer data from our array
                
                //1.allocate buffer directly from native memory
                ByteBuffer byteBuffer = ByteBuffer.allocateDirect(pyramid_vertices.length * 4); //4for float
                
                //arrange the buffer in native byte order
                byteBuffer.order(ByteOrder.nativeOrder());
                
                //create float type buffer & convert our byte buffer type buffer
                
                FloatBuffer positionBuffer = byteBuffer.asFloatBuffer();
                
                //now put array into cooked arraay
                
		positionBuffer.put(pyramid_vertices);
		
		//set arrays at zeroth position of buffer
		//interleaved array
		positionBuffer.position(0);
		
		
		GLES31.glBufferData(GLES31.GL_ARRAY_BUFFER,
		                    pyramid_vertices.length*4,
		                    positionBuffer,
		                    GLES31.GL_STATIC_DRAW );
		                    
		GLES31.glVertexAttribPointer(GLESMacros.AMC_ATTRIBUTE_POSITION,
		                                3,
		                                GLES31.GL_FLOAT,
		                                false,
		                                0,
		                                0);
		                                
                GLES31.glEnableVertexAttribArray(GLESMacros.AMC_ATTRIBUTE_POSITION);
                
                GLES31.glBindBuffer(GLES31.GL_ARRAY_BUFFER, 0);

                System.out.println("RTR: After VBO_POSITION");

		//Noraml buffer cube

		GLES31.glGenBuffers(1, vbo_normal, 0);

		 GLES31.glBindBuffer(GLES31.GL_ARRAY_BUFFER, vbo_normal[0]);

                System.out.println("RTR: Start to convert byteBuff to floatBuff");
		 ByteBuffer byteBufferTex = ByteBuffer.allocateDirect(normals.length * 4);

		 byteBufferTex.order(ByteOrder.nativeOrder());

		 FloatBuffer normalBuffer = byteBufferTex.asFloatBuffer();

		 normalBuffer.put(normals);

		 normalBuffer.position(0);
                
		GLES31.glBufferData(GLES31.GL_ARRAY_BUFFER,
						    normals.length * 4,
						    normalBuffer,
						    GLES31.GL_STATIC_DRAW);
				
		GLES31.glVertexAttribPointer(GLESMacros.AMC_ATTRIBUTE_NORMAL,
		                                3,
		                                GLES31.GL_FLOAT,
		                                false,
		                                0,
		                                0);


		GLES31.glEnableVertexAttribArray(GLESMacros.AMC_ATTRIBUTE_NORMAL);

		GLES31.glBindBuffer(GLES31.GL_ARRAY_BUFFER, 0);

          GLES31.glBindVertexArray(0);
                 
        System.out.println("RTR: After vbo_normal");

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
		float[] rotateMatrix = new float[16];
                
		 GLES31.glUseProgram(shaderProgramObject);
                
		GLES31.glClear(GLES31.GL_COLOR_BUFFER_BIT | GLES31.GL_DEPTH_BUFFER_BIT);
		
		
		//set to identity matrix
		Matrix.setIdentityM(modelMatrix, 0);
		Matrix.setIdentityM(viewMatrix, 0);
		Matrix.setIdentityM(translateMatrix, 0);
		Matrix.setIdentityM(rotateMatrix, 0);
		
		
		Matrix.translateM(modelMatrix, 0 ,translateMatrix, 0, 0.0f, 0.0f, -6.0f);
		Matrix.setRotateM(rotateMatrix, 0 , angle, 0.0f, 1.0f, 0.0f);

		Matrix.multiplyMM(modelMatrix, 0,
						  modelMatrix, 0,
						  rotateMatrix, 0);

		//light uniform 
		if (bIsLighting == true)
		{
			GLES31.glUniform1i(isLKeyIsPressed, 1);

			GLES31.glUniform1f(shininessUniform, materialShininess);

			//RED light
			GLES31.glUniform3fv(laUniform_Red, 1, lightAmbient_Red, 0);
			GLES31.glUniform3fv(ldUniform_Red, 1, lightDifuse_Red, 0);
			GLES31.glUniform3fv(lsUniform_Red, 1, lightSpecular_Red, 0);

			//Blue
			GLES31.glUniform3fv(laUniform_Blue, 1, lightAmbient_Blue, 0);
			GLES31.glUniform3fv(ldUniform_Blue, 1, lightDifuse_Blue, 0);
			GLES31.glUniform3fv(lsUniform_Blue, 1, lightSpecular_Blue, 0);

			//material
			GLES31.glUniform3fv(kaUniform, 1, materialAmbient, 0);
			GLES31.glUniform3fv(kdUniform, 1, materialDifuse, 0);
			GLES31.glUniform3fv(ksUniform, 1, materialSpecular, 0);

			GLES31.glUniform4fv(lightPositionUniform_Red, 1, lightPosition_Red, 0);
			GLES31.glUniform4fv(lightPositionUniform_Blue, 1, lightPosition_Blue, 0);

		}
		else
		{
			GLES31.glUniform1i(isLKeyIsPressed, 0);
		}
		 
		
		GLES31.glUniformMatrix4fv(modelMatrixUniform,				
									1,				
									false,				
									modelMatrix,
									0 );
		
		GLES31.glUniformMatrix4fv(viewMatrixUniform,				
									1,				
									false,				
									viewMatrix,
									0 );

		GLES31.glUniformMatrix4fv(projectionUniform,				
									1,				
									false,				
									perspectiveProjectionMatrix,
									0 );

		GLES31.glBindVertexArray(vao[0]);
	        //draw neccessaary matrices
	         GLES31.glDrawArrays(GLES31.GL_TRIANGLES,
									0,
									12);

	        GLES31.glBindVertexArray(0);
		//
	        GLES31.glUseProgram(0);
		
		requestRender();
		if(bIsAnimation)

			angle = angle + 0.5f;
		System.out.println("RTR: End of display()");
	}

		private void Uninitialize()
		{
		
	                      
	          if (vbo_normal[0] != 0)
		      {
			      GLES31.glDeleteBuffers(1, vbo_normal , 0);
			      
		      }
		      
	                      
	           if (vbo_position[0] != 0)
		      {
			      GLES31.glDeleteBuffers(1, vbo_position , 0);
			      
		      }
		      if (vao[0] != 0)
		      {
			      GLES31.glDeleteVertexArrays(1, vao, 0);
			      
		      }
	                        
		       GLES31.glUseProgram(shaderProgramObject);
		       
		       GLES31.glDetachShader(shaderProgramObject, GLES31.GL_FRAGMENT_SHADER );
		       
		       GLES31.glDeleteShader(fragmentShaderObject);
		       
		        GLES31.glDetachShader(shaderProgramObject, GLES31.GL_VERTEX_SHADER );
		       
		       GLES31.glDeleteShader(vertexShaderObject);
		       
		       GLES31.glDeleteProgram(shaderProgramObject);
		       
		       GLES31.glUseProgram(0);
		       
		       System.out.println("Uninitialize successfull");

		}
    }

package com.example.cubedifuse;

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
	private int mvUniform;
	private int projectionUniform;

	private int sampler_Uniform;
	
	private int ldUniform;
	private int kdUniform;
	private int lightPositionUniform;
	private int isLKeyIsPressed;


	private float[] perspectiveProjectionMatrix = new float[16];   //4*4 matrix
	
	//
	Boolean bFullScreen = false;
	Boolean bIsLighting = false;
	Boolean bIsAnimation = false;
	
	float angle_cube = 0.0f;
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
												"uniform mat4 u_mv_matrix;" +
												"uniform mat4 u_projection_matrix;" +
												"uniform int u_islkeypressed;" +
												"uniform vec3 u_ld;" +
												"uniform vec3 u_kd;" +
												"uniform vec4 u_light_position;" +
												"out vec3 difuse_color;" +
												"void main(void)" +
												"{" +
													"if(u_islkeypressed == 1)" +
													"{" +
														"vec4 eye_coordinates = u_mv_matrix * vPosition;" +
														"mat3 normal_matrix = mat3(transpose(inverse(u_mv_matrix)));" +
														"vec3 tnorm = normalize(normal_matrix * vNormal);" +
														"vec3 s = normalize(vec3(u_light_position - eye_coordinates));" +
														"difuse_color = u_ld * u_kd * dot(s, tnorm);" +
													"}" +
													"else" +
													" {" +
														"difuse_color = vec3(1.0, 1.0, 1.0);" +
													" }" +
													"gl_Position =  u_projection_matrix *  u_mv_matrix * vPosition;" +
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
												"in vec3 difuse_color;" +
												"void main(void)" +
												"{" +
													
													"fragColor = vec4(difuse_color, 1.0f);" +
													
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
		 
		mvUniform = GLES31.glGetUniformLocation(shaderProgramObject, "u_mv_matrix");
		
		projectionUniform = GLES31.glGetUniformLocation(shaderProgramObject, "u_projection_matrix");

		isLKeyIsPressed = GLES31.glGetUniformLocation(shaderProgramObject,
												"u_islkeypressed");

		ldUniform = GLES31.glGetUniformLocation(shaderProgramObject,
											"u_ld");


		kdUniform = GLES31.glGetUniformLocation(shaderProgramObject,
										"u_kd");

		lightPositionUniform = GLES31.glGetUniformLocation(shaderProgramObject,
												"u_light_position");
		
		System.out.println("RTR: After Post Linking");
		
		 final float[] cube_vertices = new float[]
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

		final float[] normals = new float[]
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
		
		
                GLES31.glGenVertexArrays(1, vao, 0);
                
                GLES31.glBindVertexArray(vao[0]);
                
                GLES31.glGenBuffers(1, vbo_position, 0);
                
                GLES31.glBindBuffer(GLES31.GL_ARRAY_BUFFER, vbo_position[0]);
                
                //create buffer data from our array
                
                //1.allocate buffer directly from native memory
                ByteBuffer byteBuffer = ByteBuffer.allocateDirect(cube_vertices.length * 4); //4for float
                
                //arrange the buffer in native byte order
                byteBuffer.order(ByteOrder.nativeOrder());
                
                //create float type buffer & convert our byte buffer type buffer
                
                FloatBuffer positionBuffer = byteBuffer.asFloatBuffer();
                
                //now put array into cooked arraay
                
		positionBuffer.put(cube_vertices);
		
		//set arrays at zeroth position of buffer
		//interleaved array
		positionBuffer.position(0);
		
		
		GLES31.glBufferData(GLES31.GL_ARRAY_BUFFER,
		                    cube_vertices.length*4,
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
		
		
		float[] modelViewMatrix = new float[16];
		float[] translateMatrix = new float[16];
		float[] rotateMatrix = new float[16];
                
		 GLES31.glUseProgram(shaderProgramObject);
                
		GLES31.glClear(GLES31.GL_COLOR_BUFFER_BIT | GLES31.GL_DEPTH_BUFFER_BIT);
		
		
		
		
		//set to identity matrix
		Matrix.setIdentityM(modelViewMatrix, 0);
		Matrix.setIdentityM(translateMatrix, 0);
		Matrix.setIdentityM(rotateMatrix, 0);
		
		
		Matrix.translateM(modelViewMatrix, 0 ,translateMatrix, 0, 0.0f, 0.0f, -6.0f);
		Matrix.setRotateM(rotateMatrix, 0 , angle_cube, 1.0f, 1.0f, 1.0f);

		Matrix.multiplyMM(modelViewMatrix, 0,
				  modelViewMatrix, 0,
				  rotateMatrix, 0);
		//light uniform 
		if (bIsLighting == true)
		{
			GLES31.glUniform1i(isLKeyIsPressed, 1);

			GLES31.glUniform3f(ldUniform, 1.0f, 1.0f, 1.0f);

			GLES31.glUniform3f(kdUniform, 0.5f, 0.5f, 0.5f);

			GLES31.glUniform4f(lightPositionUniform, 0.0f, 0.0f, 2.0f, 1.0f);

		}
		else
		{
			GLES31.glUniform1i(isLKeyIsPressed, 0);
		}
		 
		
		GLES31.glUniformMatrix4fv(mvUniform,				
									1,				
									false,				
									modelViewMatrix,
									0 );
		
		GLES31.glUniformMatrix4fv(projectionUniform,				
									1,				
									false,				
									perspectiveProjectionMatrix,
									0 );

		GLES31.glBindVertexArray(vao[0]);
	        //draw neccessaary matrices
	         GLES31.glDrawArrays(GLES31.GL_TRIANGLE_FAN,
									0,
									4);

			GLES31.glDrawArrays(GLES31.GL_TRIANGLE_FAN,
									4,
									4);

			GLES31.glDrawArrays(GLES31.GL_TRIANGLE_FAN,
									8,
									4);

			GLES31.glDrawArrays(GLES31.GL_TRIANGLE_FAN,
									12,
									4);

			GLES31.glDrawArrays(GLES31.GL_TRIANGLE_FAN,
									16,
									4);

			GLES31.glDrawArrays(GLES31.GL_TRIANGLE_FAN,
									20,
									4);			//How many to draw from start

	        GLES31.glBindVertexArray(0);
		//
	        GLES31.glUseProgram(0);
		
		requestRender();
		if(bIsAnimation)

			angle_cube = angle_cube + 0.5f;
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

package com.example.tshader;

import android.opengl.GLSurfaceView;
import android.opengl.GLES32;
import javax.microedition.khronos.opengles.GL10;
import javax.microedition.khronos.egl.EGLConfig;

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
	private int tessellationControlShaderObj;
	private int tessellationEvaluationShaderObj;
	private int shaderProgramObject;
	
	private int[] vao = new int[1];
	private int[] vbo = new int[1];
	
	private int mvpUniform;
	private int lineColorUniform;
	private int numOfSegmentsUniform;
	private int numOfStripsUniform;

	private int gNumOfSegments = 0;

	private float[] perspectiveProjectionMatrix = new float[16];   //4*4 matrix
	
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
	 public boolean onDoubleTap(MotionEvent event)
	 {
			gNumOfSegments--;
			if (gNumOfSegments <= 0)
			{
				gNumOfSegments = 1;
			}
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
		 gNumOfSegments++;
			if (gNumOfSegments >=50)
			{
				gNumOfSegments = 50;
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
		 
		 
	 }
	 
	 @Override 
	 public boolean onScroll(MotionEvent e1, MotionEvent e2, float disX, float disY)
	 {
		 
		 Uninitialize();
		 System.exit(0);
		 return true;
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
                 String shadingVersion = gl.glGetString(GLES32.GL_SHADING_LANGUAGE_VERSION);	 
		 System.out.println("RTR Opengl version: " + version);
		 System.out.println("RTR: Shading language version " + shadingVersion);
		 
		 initialize();
		 System.out.println("RTR: After Initialize()");
	 }
	 @Override
	 public void onSurfaceChanged(GL10 unused, int width, int height)
	 {
		 System.out.println("RTR: onSurfaceChanged()");
		 
		 resize(width, height);
	 }
	 
	 @Override
	 public void onDrawFrame(GL10 unused)
	 {
		 display();
		 
		 System.out.println("RTR: onDrawFrame()");
	 }
	 
	 //our custom methods
	
	 private void initialize()
	 {
		 System.out.println("RTR: In start Initialize()");
		 
	      //// / //*********Vertex shader*********//////
	      
		 vertexShaderObject = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);
		 
		final String vertexShaderSourceCode = String.format
		                                (
		                                        "#version 320 es" +
		                                         "\n" +
		                                         "in vec4 vPosition;" +
		                                         //"uniform mat4 u_mvp_matrix;" +
		                                         "void main(void)" +
		                                         "{" +
		                                        	 "gl_Position = vPosition;" +
		                                         " } " 
		                                );
		                                
                GLES32.glShaderSource(vertexShaderObject, vertexShaderSourceCode);
		                 
		 GLES32.glCompileShader(vertexShaderObject);
		 
		 //Vertex shader error checking
		 int[] iShaderCompileStatus = new int[1];
		 int[] iInfoLogLength = new int[1];
		 String szInfoLog = null;
		 
		 GLES32.glGetShaderiv(vertexShaderObject,
		                      GLES32.GL_COMPILE_STATUS,
		                      iShaderCompileStatus,
		                      0);
		                      
		  if(iShaderCompileStatus[0] == GLES32.GL_FALSE)
		  {
		        GLES32.glGetShaderiv(vertexShaderObject,
		                             GLES32.GL_INFO_LOG_LENGTH,
		                             iInfoLogLength,
		                             0 );
		                             
                        if(iInfoLogLength[0] > 0)
                        {
                                szInfoLog = GLES32.glGetShaderInfoLog(vertexShaderObject);
                                
                                System.out.println("RTR: vertex shader ERROR: " + szInfoLog);
                                //uninitialize();
                                System.exit(0);
                        }
		        
		  }
		 
		  ///////*******Tessellation control shader*****/////////////////

		  tessellationControlShaderObj = GLES32.glCreateShader(GLES32.GL_TESS_CONTROL_SHADER);

		  final String tessControlShaderSourceCode = String.format
	  											(
	  												"#version 320 es" +
													"\n" +
													"layout(vertices=4)out;" +
													"uniform int numberOfSegments;" +
													"uniform int numberOfStrips;" +
													"void main(void)" +
													"{" +
														"gl_out[gl_InvocationID].gl_Position = gl_in" +
														"[gl_InvocationID].gl_Position;" +
														"gl_TessLevelOuter[0] = float(numberOfStrips);" +
														"gl_TessLevelOuter[1] = float(numberOfSegments);" +
													"}"

	  											);

	  			GLES32.glShaderSource(tessellationControlShaderObj, tessControlShaderSourceCode);
		 
				 GLES32.glCompileShader(tessellationControlShaderObj);
				 
				 //Error checking for fragment shader 
				  iShaderCompileStatus[0] = 0;
				  iInfoLogLength[0] = 0;
				  szInfoLog = null;
		 

				  	GLES32.glGetShaderiv(tessellationControlShaderObj,
		                      GLES32.GL_COMPILE_STATUS,
		                      iShaderCompileStatus,
		                      0);
		                      
                if(iShaderCompileStatus[0] == GLES32.GL_FALSE)
                {
                        GLES32.glGetShaderiv(tessellationControlShaderObj,
                                             GLES32.GL_INFO_LOG_LENGTH,
                                             iInfoLogLength,
                                             0);
                                             
                        if(iInfoLogLength[0] > 0)
                        {
                            szInfoLog = GLES32.glGetShaderInfoLog(tessellationControlShaderObj);    
                            
                            System.out.println("RTR: tessellationControlShaderObj shader ERROR: " + szInfoLog);
                            //uninitialize();
                            System.exit(0);
                        }
                        
                }
        //////////****Tessellation Evaluation Shader*****/////////////

          tessellationEvaluationShaderObj = GLES32.glCreateShader(GLES32.GL_TESS_EVALUATION_SHADER);

		  final String tessellationEvaluationShaderSourceCode = String.format
	  											(
	  												"#version 320 es" +
													"\n" +
													"layout(isolines)in;" +
													"uniform mat4 u_mvp_matrix;" +
													"void main(void)" +
													"{" +
														"float u = gl_TessCoord.x;" +
														"vec3 p0 = gl_in[0].gl_Position.xyz;" +
														"vec3 p1 = gl_in[1].gl_Position.xyz;" +
														"vec3 p2 = gl_in[2].gl_Position.xyz;" +
														"vec3 p3 = gl_in[3].gl_Position.xyz;" +
														"float u1 = (1.0 -u);" +
														"float u2 = u * u;" +

														"float b3 = u2 * u;" +
														"float b2 = 3.0 * u2 * u1;" +
														"float b1 = 3.0 * u * u1 * u1;" +
														"float b0 = u1 * u1 * u1;" +
														
														"vec3 p = p0 * b0 + p1 * b1 + p2 * b2 + p3 * b3;" +
														"gl_Position = u_mvp_matrix * vec4(p, 1.0);" +
													"}"

	  											);

	  			GLES32.glShaderSource(tessellationEvaluationShaderObj, tessellationEvaluationShaderSourceCode);
		 
				 GLES32.glCompileShader(tessellationEvaluationShaderObj);
				 
				 //Error checking for fragment shader 
				  iShaderCompileStatus[0] = 0;
				  iInfoLogLength[0] = 0;
				  szInfoLog = null;
		 

				  	GLES32.glGetShaderiv(tessellationEvaluationShaderObj,
					                      GLES32.GL_COMPILE_STATUS,
					                      iShaderCompileStatus,
					                      0);
		                      
                if(iShaderCompileStatus[0] == GLES32.GL_FALSE)
                {
                        GLES32.glGetShaderiv(tessellationEvaluationShaderObj,
                                             GLES32.GL_INFO_LOG_LENGTH,
                                             iInfoLogLength,
                                             0);
                                             
                        if(iInfoLogLength[0] > 0)
                        {
                            szInfoLog = GLES32.glGetShaderInfoLog(tessellationEvaluationShaderObj);    
                            
                            System.out.println("RTR: tessellationEvaluation shader ERROR: " + szInfoLog);
                            //uninitialize();
                            System.exit(0);
                        }
                        
                }

        ////////////****Fragment Shader *******//////////////
        	
		 
		 fragmentShaderObject = GLES32.glCreateShader(GLES32.GL_FRAGMENT_SHADER);
		 
		 final String fragmentShaderSourceCode = String.format
		                                (
		                                        "#version 320 es" +
												"\n" +
												"precision highp float;" +
												"uniform vec4 lineColor;" +
												"out vec4 fragColor;" +
												"void main(void)" +
												"{" +
													"fragColor = lineColor;" +
												"}" 
		                                );
		 
		 GLES32.glShaderSource(fragmentShaderObject, fragmentShaderSourceCode);
		 
		 GLES32.glCompileShader(fragmentShaderObject);
		 
		 //Error checking for fragment shader 
		  iShaderCompileStatus[0] = 0;
		  iInfoLogLength[0] = 0;
		  szInfoLog = null;
		  
		  
		 GLES32.glGetShaderiv(fragmentShaderObject,
		                      GLES32.GL_COMPILE_STATUS,
		                      iShaderCompileStatus,
		                      0);
		                      
                if(iShaderCompileStatus[0] == GLES32.GL_FALSE)
                {
                        GLES32.glGetShaderiv(fragmentShaderObject,
                                             GLES32.GL_INFO_LOG_LENGTH,
                                             iInfoLogLength,
                                             0);
                                             
                        if(iInfoLogLength[0] > 0)
                        {
                            szInfoLog = GLES32.glGetShaderInfoLog(fragmentShaderObject);    
                            
                            System.out.println("RTR: fragment shader ERROR: " + szInfoLog);
                            //uninitialize();
                            System.exit(0);
                        }
                        
                }
                
		//Create ShaderProgramObject and attach above shaders
		
		     shaderProgramObject = GLES32.glCreateProgram();
		     
		     GLES32.glAttachShader(shaderProgramObject, vertexShaderObject);
		     GLES32.glAttachShader(shaderProgramObject, fragmentShaderObject);
		     GLES32.glAttachShader(shaderProgramObject, tessellationEvaluationShaderObj);
		     GLES32.glAttachShader(shaderProgramObject, tessellationControlShaderObj);
		     
		 //*** PRELINKING BINDING TO VERTEX ATTRIBUTES***
		 GLES32.glBindAttribLocation(shaderProgramObject,
		                             GLESMacros.AMC_ATTRIBUTE_POSITION,
		                             "vPosition");

                //Link above program
                GLES32.glLinkProgram(shaderProgramObject);
                
                //ERROR checking for Linking
                
                int[] iShaderLinkStatus = new int[1];
                iInfoLogLength[0] = 0;
				szInfoLog = null;
		
		GLES32.glGetProgramiv(shaderProgramObject,
		                      GLES32.GL_LINK_STATUS,
		                      iShaderLinkStatus,
		                      0);
		                      
		  if(iShaderLinkStatus[0] == GLES32.GL_FALSE)
		  {
		        
		        GLES32.glGetProgramiv(shaderProgramObject,
		                              GLES32.GL_INFO_LOG_LENGTH,
		                              iInfoLogLength,
		                              0 );
		                              
                        if(iInfoLogLength[0] > 0)
                        {
                              szInfoLog = GLES32.glGetShaderInfoLog(shaderProgramObject);
                              
                               System.out.println("RTR: shader program ERROR: " + szInfoLog);
                                //uninitialize();
                                System.exit(0);
                        }
		  }
		  
		 ///***POST LINKING GETTING UNIFORMS**
		 
		mvpUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_mvp_matrix");

		lineColorUniform = GLES32.glGetUniformLocation(shaderProgramObject, "lineColor");

		numOfStripsUniform = GLES32.glGetUniformLocation(shaderProgramObject, "numberOfStrips");

		numOfSegmentsUniform = GLES32.glGetUniformLocation(shaderProgramObject, "numberOfSegments");
		
		 final float[] vertices = new float[]
		                                {
		                                        -1.0f, -1.0f, 
		                                        -0.5f, 1.0f,
		                                         0.5f, -1.0f,
		                                          1.0f, 1.0f
		                                };
		                                
                //create vao
                GLES32.glGenVertexArrays(1, vao, 0);
                
                GLES32.glBindVertexArray(vao[0]);
                
                GLES32.glGenBuffers(1, vbo, 0);
                
                GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo[0]);
                
                //create buffer data from our array
                
                //1.allocate buffer directly from native memory
                ByteBuffer byteBuffer = ByteBuffer.allocateDirect(vertices.length * 8); //4for float
                
                //arrange the buffer in native byte order
                byteBuffer.order(ByteOrder.nativeOrder());
                
                //create float type buffer & convert our byte buffer type buffer
                
                FloatBuffer positionBuffer = byteBuffer.asFloatBuffer();
                
                //now put array into cooked arraay
                
		positionBuffer.put(vertices);
		
		//set arrays at zeroth position of buffe
		//interleaved array
		positionBuffer.position(0);
		
		
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER,
		                    vertices.length*8,
		                    positionBuffer,
		                    GLES32.GL_STATIC_DRAW );
		                    
		GLES32.glVertexAttribPointer(GLESMacros.AMC_ATTRIBUTE_POSITION,
		                                2,
		                                GLES32.GL_FLOAT,
		                                false,
		                                0,
		                                0);
		                                
                GLES32.glEnableVertexAttribArray(GLESMacros.AMC_ATTRIBUTE_POSITION);
                
                GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
                
                GLES32.glBindVertexArray(0);
                 
                 GLES32.glEnable(GLES32.GL_DEPTH_TEST);
                 
                 GLES32.glDepthFunc(GLES32.GL_LEQUAL);
		
		 GLES32.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		 GLES32.glLineWidth(3.0f);

			gNumOfSegments = 1;

		 Matrix.setIdentityM(perspectiveProjectionMatrix, 0);
		 
		 System.out.println("RTR: End  Initialize()");
	 }
		
	private void resize(int width, int height)
	{
		
		if(height == 0)
	        {
		        height = 1;
	        }
	        GLES32.glViewport(0, 0, width, height);
        
			
		Matrix.perspectiveM(perspectiveProjectionMatrix, 0 ,45.0f, (float)width/(float)height, 0.1f, 100.0f);        
			
	        
		System.out.println("RTR: In resize()");
	}
	
	private void display()
	{
		System.out.println("RTR: Start of display()");
		
		
		float[] modelViewMatrix = new float[16];
		float[] modelViewProjectionMatrix = new float[16];
		float[] translateMatrix = new float[16];
		
		GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);
		
		GLES32.glUseProgram(shaderProgramObject);
		
		Matrix.setIdentityM(modelViewMatrix, 0);
		Matrix.setIdentityM(modelViewProjectionMatrix, 0);
		Matrix.setIdentityM(translateMatrix, 0);
		
		Matrix.translateM(modelViewMatrix, 0 ,translateMatrix,0, 0.5f, 0.5f, -2.0f);
		
		
		Matrix.multiplyMM(modelViewProjectionMatrix, 0,
		                 perspectiveProjectionMatrix, 0,
		                 modelViewMatrix, 0);
		                 
		 GLES32.glUniform1i(numOfSegmentsUniform, gNumOfSegments);

		 GLES32.glUniform1i(numOfStripsUniform, 1);

		 if(gNumOfSegments >= 50)
		 {
		 	GLES32.glUniform4f(lineColorUniform,  1.0f, 0.0f, 0.0f, 1.0f);	
		 }
		 else
		 {
		 	GLES32.glUniform4f(lineColorUniform,  1.0f, 1.0f, 0.0f, 1.0f);
		 }

		GLES32.glUniformMatrix4fv(mvpUniform,				
						1,				
						false,				
						modelViewProjectionMatrix,
						0 );
						
		GLES32.glBindVertexArray(vao[0]);

	 //draw neccessaary matrices
	         GLES32.glDrawArrays(GLES32.GL_PATCHES,    //what to draw from given array
					0,				//from where to start
					4);				//How many to draw from start

	        GLES32.glBindVertexArray(0);

	        GLES32.glUseProgram(0);
		
		requestRender();
		System.out.println("RTR: End of display()");
	}
	
	private void Uninitialize()
	{
	
	                
	                if (vbo[0] != 0)
	                {
		                GLES32.glDeleteBuffers(1, vbo , 0);
		                
	                }

	                if (vao[0] != 0)
	                {
		                GLES32.glDeleteVertexArrays(1, vao, 0);
		                
	                }

	       GLES32.glUseProgram(shaderProgramObject);
	       
	       GLES32.glDetachShader(shaderProgramObject, GLES32.GL_FRAGMENT_SHADER );
	       
	       GLES32.glDeleteShader(fragmentShaderObject);
	       
	        GLES32.glDetachShader(shaderProgramObject, GLES32.GL_VERTEX_SHADER );
	       
	       GLES32.glDeleteShader(vertexShaderObject);
	       
	       GLES32.glDeleteProgram(shaderProgramObject);
	       
	       GLES32.glUseProgram(0);
	       
	       System.out.println("Uninitialize successfull");

	}
}

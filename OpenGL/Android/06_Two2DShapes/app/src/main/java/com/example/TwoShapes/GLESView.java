package com.example.TwoShapes;

import android.opengl.GLSurfaceView;
import android.opengl.GLES31;
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
	private int shaderProgramObject;
	
	private int[] vao_triangle = new int[1];
	private int[] vao_rectangle = new int[1];
		
	private int[] vbo_pos_triangle = new int[1];
	private int[] vbo_pos_rectangle = new int[1];
	
	
	private int mvpUniform;
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
	      
		 vertexShaderObject = GLES31.glCreateShader(GLES31.GL_VERTEX_SHADER);
		 
		final String vertexShaderSourceCode = String.format
		                                (
		                                        "#version 310 es" +
		                                         "\n" +
		                                         "in vec4 vPosition;" +
		                                         "uniform mat4 u_mvp_matrix;" +
		                                         "void main(void)" +
		                                         "{" +
		                                         "gl_Position = u_mvp_matrix * vPosition;" +
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
		 
		 
        	////////****Fragment Shader *******//////////
        	
		 
		 fragmentShaderObject = GLES31.glCreateShader(GLES31.GL_FRAGMENT_SHADER);
		 
		 final String fragmentShaderSourceCode = String.format
		                                (
		                                        "#version 310 es" +
		                                        "\n" +
		                                        "precision highp float;" +
		                                        "out vec4 fragColor;" +
		                                        "void main(void)" +
		                                        "{" +
		                                        "fragColor = vec4(1.0, 1.0, 1.0, 1.0);" +
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
                
		//Create ShaderProgramObject and attach above shaders
		
		     shaderProgramObject = GLES31.glCreateProgram();
		     
		     GLES31.glAttachShader(shaderProgramObject, vertexShaderObject);
		     GLES31.glAttachShader(shaderProgramObject, fragmentShaderObject);
		     
		     
		 //*** PRELINKING BINDING TO VERTEX ATTRIBUTES***
		 GLES31.glBindAttribLocation(shaderProgramObject,
		                             GLESMacros.AMC_ATTRIBUTE_POSITION,
		                             "vPosition");

                //Link above program
                GLES31.glLinkProgram(shaderProgramObject);
                
                //ERROR checking for Linking
                
                int[] iShaderLinkStatus = new int[1];
                iInfoLogLength[0] = 0;
		szInfoLog = null;
		
		GLES31.glGetProgramiv(shaderProgramObject,
		                      GLES31.GL_LINK_STATUS,
		                      iShaderLinkStatus,
		                      0);
		                      
		  if(iShaderLinkStatus[0] == GLES31.GL_FALSE)
		  {
		        
		        GLES31.glGetProgramiv(shaderProgramObject,
		                              GLES31.GL_INFO_LOG_LENGTH,
		                              iInfoLogLength,
		                              0 );
		                              
                        if(iInfoLogLength[0] > 0)
                        {
                              szInfoLog = GLES31.glGetShaderInfoLog(shaderProgramObject);
                              
                               System.out.println("RTR: shader program ERROR: " + szInfoLog);
                                //uninitialize();
                                System.exit(0);
                        }
		  }
		  
		 ///***POST LINKING GETTING UNIFORMS**
		 
		mvpUniform = GLES31.glGetUniformLocation(shaderProgramObject, "u_mvp_matrix");
		
		 final float[] triangle_vertices = new float[]
		                                {
		                                        0.0f, 1.0f, 0.0f,
		                                        -1.0f, -1.0f, 0.0f,
		                                        1.0f, -1.0f, 0.0f
		                                };

		final float[] rectangle_vertices = new float[]
						{
							1.0f,1.0f,0.0f,
							-1.0f,1.0f,0.0f,
							-1.0f,-1.0f,0.0f,
							1.0f,-1.0f,0.0f	
						};
		                                
                //create vao trinalge
                GLES31.glGenVertexArrays(1, vao_triangle, 0);
                
                GLES31.glBindVertexArray(vao_triangle[0]);
                
                GLES31.glGenBuffers(1, vbo_pos_triangle, 0);
                
                GLES31.glBindBuffer(GLES31.GL_ARRAY_BUFFER, vbo_pos_triangle[0]);
                
                //create buffer data from our array
                
                //1.allocate buffer directly from native memory
                ByteBuffer byteBuffer = ByteBuffer.allocateDirect(triangle_vertices.length * 4); //4for float
                
                //arrange the buffer in native byte order
                byteBuffer.order(ByteOrder.nativeOrder());
                
                //create float type buffer & convert our byte buffer type buffer
                
                FloatBuffer positionBuffer = byteBuffer.asFloatBuffer();
                
                //now put array into cooked arraay
                
		positionBuffer.put(triangle_vertices);
		
		//set arrays at zeroth position of buffe
		//interleaved array
		positionBuffer.position(0);
		
		
		GLES31.glBufferData(GLES31.GL_ARRAY_BUFFER,
		                    triangle_vertices.length*4,
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
                
                GLES31.glBindVertexArray(0);
                 

		//vao for rectangle
		GLES31.glGenVertexArrays(1, vao_rectangle, 0);

		GLES31.glBindVertexArray(vao_rectangle[0]);

		GLES31.glGenBuffers(1, vbo_pos_rectangle, 0);


		GLES31.glBindBuffer(GLES31.GL_ARRAY_BUFFER, vbo_pos_rectangle[0]);

		ByteBuffer byteBufferRect = ByteBuffer.allocateDirect( rectangle_vertices.length * 4);

		
		byteBufferRect.order(ByteOrder.nativeOrder());

		FloatBuffer positionRectBuffer = byteBufferRect.asFloatBuffer();
		
		positionRectBuffer.put(rectangle_vertices);

		positionRectBuffer.position(0);
		
		GLES31.glBufferData(GLES31.GL_ARRAY_BUFFER,
				    rectangle_vertices.length *4,
				    positionRectBuffer,
				    GLES31.GL_STATIC_DRAW );

		GLES31.glVertexAttribPointer(GLESMacros.AMC_ATTRIBUTE_POSITION,
					      3,
					     GLES31.GL_FLOAT,
					     false,
					     0,
					     0 );

		GLES31.glEnableVertexAttribArray(GLESMacros.AMC_ATTRIBUTE_POSITION);

		GLES31.glBindBuffer(GLES31.GL_ARRAY_BUFFER, 0);

		GLES31.glBindVertexArray(0);
		

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
        
			
		Matrix.perspectiveM(perspectiveProjectionMatrix, 0 ,45.0f, width/height, 0.1f, 100.0f);        
			
	        
		System.out.println("RTR: In resize()");
	}
	
	private void display()
	{
		System.out.println("RTR: Start of display()");
		
		
		float[] modelViewMatrix = new float[16];
		float[] modelViewProjectionMatrix = new float[16];
		float[] translateMatrix = new float[16];
		
		GLES31.glClear(GLES31.GL_COLOR_BUFFER_BIT | GLES31.GL_DEPTH_BUFFER_BIT);
		
		GLES31.glUseProgram(shaderProgramObject);
		
		Matrix.setIdentityM(modelViewMatrix, 0);
		Matrix.setIdentityM(modelViewProjectionMatrix, 0);
		Matrix.setIdentityM(translateMatrix, 0);
		
		Matrix.translateM(modelViewMatrix, 0 ,translateMatrix, 0, 2.5f, 0.0f, -9.0f);
		
		
		Matrix.multiplyMM(modelViewProjectionMatrix, 0,
		                 perspectiveProjectionMatrix, 0,
		                 modelViewMatrix, 0);
		                 
		 
		GLES31.glUniformMatrix4fv(mvpUniform,				
						1,				
						false,				
						modelViewProjectionMatrix,
						0 );
						
		GLES31.glBindVertexArray(vao_triangle[0]);

	        //draw neccessaary matrices
	         GLES31.glDrawArrays(GLES31.GL_TRIANGLES,    //what to draw from given array
					0,				//from where to start
					3);				//How many to draw from start

	        GLES31.glBindVertexArray(0);
		
		//rectangle
		Matrix.setIdentityM(modelViewMatrix, 0);
		Matrix.setIdentityM(modelViewProjectionMatrix, 0);
		Matrix.setIdentityM(translateMatrix, 0);
		
		Matrix.translateM(modelViewMatrix, 0 ,translateMatrix, 0, -2.0f, 0.0f, -9.0f);
		
		
		Matrix.multiplyMM(modelViewProjectionMatrix, 0,
		                 perspectiveProjectionMatrix, 0,
		                 modelViewMatrix, 0);
		                 
		 
		GLES31.glUniformMatrix4fv(mvpUniform,				
						1,				
						false,				
						modelViewProjectionMatrix,
						0 );


		GLES31.glBindVertexArray(vao_rectangle[0]);

		GLES31.glDrawArrays(GLES31.GL_TRIANGLE_FAN, 0 , 4);
		
		GLES31.glBindVertexArray(0);
		
		//
	        GLES31.glUseProgram(0);
		
		requestRender();
		System.out.println("RTR: End of display()");
	}
	
	private void Uninitialize()
	{
	
	       
	                
	                if (vbo_pos_triangle[0] != 0)
	                {
		                GLES31.glDeleteBuffers(1, vbo_pos_triangle , 0);
		                
	                }
                                
                        if (vbo_pos_rectangle[0] != 0)
	                {
		                GLES31.glDeleteBuffers(1, vbo_pos_rectangle , 0);
		                
	                }
	                if (vao_triangle[0] != 0)
	                {
		                GLES31.glDeleteVertexArrays(1, vao_triangle, 0);
		                
	                }
                        
                        if (vao_rectangle[0] != 0)
	                {
		                GLES31.glDeleteVertexArrays(1, vao_rectangle, 0);
		                
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

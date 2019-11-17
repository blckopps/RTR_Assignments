package com.example.checkerboard;

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
	
	private int[] vao_rect = new int[1];
	private int[] vbo_rect = new int[1];
	private int[] vbo_recttwo =new int[1];
	
	private int[] texture_checker= new int[1];
	
	private int[] vbo_texcoord = new int[1];
	
	private int mvpUniform;
	
	final private int checkimageheight = 64;
	final private int checkimagewidth = 64;
	
	byte[] checkimage = new byte[checkimageheight*checkimagewidth*4];
	
	private float[] perspectiveProjectionMatrix = new float[16];   //4*4 matrix
	
	private int sampler_Uniform;
	
	
	//
	final float[] rect_vertices = new float[12];
	
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
							"in vec2 vTexcoord;" +
							"out vec2 out_texcoord;" +
		                                         "uniform mat4 u_mvp_matrix;" +
		                                         "void main(void)" +
		                                         "{" +
		                                         "gl_Position = u_mvp_matrix * vPosition;" +
							 "out_texcoord = vTexcoord;" +
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
        	////////****Fragment Shader *******//////////
        	
		 
		 fragmentShaderObject = GLES31.glCreateShader(GLES31.GL_FRAGMENT_SHADER);
		 
		 final String fragmentShaderSourceCode = String.format
		                                (
		                                        "#version 310 es" +
		                                        "\n" +
							"precision highp float;" +
							"uniform sampler2D u_Sampler;" +
							"in vec2 out_texcoord;" +
		                                        "out vec4 fragColor;" +
		                                        "void main(void)" +
		                                        "{" +
		                                        "fragColor = texture(u_Sampler, out_texcoord);" +
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
					    GLESMacros.AMC_ATTRIBUTE_TEXCOORD0,
						"vTexcoord");

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
		
		sampler_Uniform = GLES31.glGetUniformLocation(shaderProgramObject, "u_Sampler");
		
		System.out.println("RTR: After Post Linking");
		
		 /*final float[] rect_vertices = new float[]
		                                {
		                                	1.0f, 1.0f, 0.0f,
							-1.0f, 1.0f, 0.0f,
                                                        -1.0f, -1.0f, 0.0f,
							1.0f, -1.0f, 0.0f
		                                };
*/
		final float[] texcoord = new float[]
						{
							1.0f, 1.0f,
							0.0f, 1.0f,
							0.0f, 0.0f,
							1.0f, 0.0f
						};
		
		
                GLES31.glGenVertexArrays(1, vao_rect, 0);
                
                GLES31.glBindVertexArray(vao_rect[0]);
                
                GLES31.glGenBuffers(1, vbo_rect, 0);
                
                GLES31.glBindBuffer(GLES31.GL_ARRAY_BUFFER, vbo_rect[0]);
                
                /*//create buffer data from our array
                
                //1.allocate buffer directly from native memory
                ByteBuffer byteBuffer = ByteBuffer.allocateDirect(rect_vertices.length * 4); //4for float
                
                //arrange the buffer in native byte order
                byteBuffer.order(ByteOrder.nativeOrder());
                
                //create float type buffer & convert our byte buffer type buffer
                
                FloatBuffer positionBuffer = byteBuffer.asFloatBuffer();
                
                //now put array into cooked arraay
                
		positionBuffer.put(rect_vertices);
		
		//set arrays at zeroth position of buffe
		//interleaved array
		positionBuffer.position(0);
		*/
		
		GLES31.glBufferData(GLES31.GL_ARRAY_BUFFER,
		                    rect_vertices.length*4,
		                    null,
		                    GLES31.GL_DYNAMIC_DRAW );
		                    
		GLES31.glVertexAttribPointer(GLESMacros.AMC_ATTRIBUTE_POSITION,
		                                3,
		                                GLES31.GL_FLOAT,
		                                false,
		                                0,
		                                0);
		                                
                GLES31.glEnableVertexAttribArray(GLESMacros.AMC_ATTRIBUTE_POSITION);
                
                GLES31.glBindBuffer(GLES31.GL_ARRAY_BUFFER, 0);

                System.out.println("RTR: After VBO_POSITION");
		//texture buffer smiley
		GLES31.glGenBuffers(1, vbo_texcoord, 0);

		 GLES31.glBindBuffer(GLES31.GL_ARRAY_BUFFER, vbo_texcoord[0]);

                System.out.println("RTR: Start to convert byteBuff to floatBuff");
		 ByteBuffer byteBufferTex = ByteBuffer.allocateDirect(texcoord.length * 4);

		 byteBufferTex.order(ByteOrder.nativeOrder());

		 FloatBuffer textureBuffer = byteBufferTex.asFloatBuffer();

		 textureBuffer.put(texcoord);

		 textureBuffer.position(0);
                
		GLES31.glBufferData(GLES31.GL_ARRAY_BUFFER,
				    texcoord.length * 4,
				    textureBuffer,
				    GLES31.GL_STATIC_DRAW);
		
		GLES31.glVertexAttribPointer(GLESMacros.AMC_ATTRIBUTE_TEXCOORD0,
		                                2,
		                                GLES31.GL_FLOAT,
		                                false,
		                                0,
		                                0);


		GLES31.glEnableVertexAttribArray(GLESMacros.AMC_ATTRIBUTE_TEXCOORD0);

		GLES31.glBindBuffer(GLES31.GL_ARRAY_BUFFER, 0);

                GLES31.glBindVertexArray(0);
                 
                System.out.println("RTR: After VBO_texcoorD");
		/////////////////
                 GLES31.glEnable(GLES31.GL_DEPTH_TEST);
                 
                 GLES31.glDepthFunc(GLES31.GL_LEQUAL);
		 GLES31.glEnable(GLES31.GL_TEXTURE_2D);
		 
		 GLES31.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		 
		 //texture load
		 System.out.println("RTR:Before LoadTexture");
		 
		 texture_checker[0] = loadTexture();
		 
		 System.out.println("RTR: After Load Texture");
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
		float[] modelViewProjectionMatrix = new float[16];
		float[] translateMatrix = new float[16];
		
                
                GLES31.glUseProgram(shaderProgramObject);
                
		GLES31.glClear(GLES31.GL_COLOR_BUFFER_BIT | GLES31.GL_DEPTH_BUFFER_BIT);
		
		GLES31.glActiveTexture(GLES31.GL_TEXTURE0);
		
		GLES31.glBindTexture(GLES31.GL_TEXTURE_2D, texture_checker[0]);
		
		GLES31.glUniform1i(sampler_Uniform, 0);
		
		
		
		//set to identity matrix
		Matrix.setIdentityM(modelViewMatrix, 0);
		Matrix.setIdentityM(modelViewProjectionMatrix, 0);
		Matrix.setIdentityM(translateMatrix, 0);
		
		
		
		Matrix.translateM(modelViewMatrix, 0 ,translateMatrix, 0, -1.0f, 0.0f, -4.0f);
		

		Matrix.multiplyMM(modelViewProjectionMatrix, 0,
		                 perspectiveProjectionMatrix, 0,
		                 modelViewMatrix, 0);
		 
		
		GLES31.glUniformMatrix4fv(mvpUniform,				
						1,				
						false,				
						modelViewProjectionMatrix,
						0 );
		
						
		GLES31.glBindVertexArray(vao_rect[0]);
                
                //1st pos
                rect_vertices[0] = 1.0f;
                rect_vertices[1] = 1.0f;
                rect_vertices[2] = 0.0f;
                
                rect_vertices[3] = -1.0f;
                rect_vertices[4] = 1.0f;
                rect_vertices[5] = 0.0f;
                
                rect_vertices[6] = -1.0f;
                rect_vertices[7] = -1.0f; 
                rect_vertices[8] = 0.0f;
                
                rect_vertices[9] = 1.0f;
                rect_vertices[10] = -1.0f; 
                rect_vertices[11] = 0.0f;
                
                GLES31.glBindBuffer(GLES31.GL_ARRAY_BUFFER, vbo_rect[0]);
                
                //create buffer data from our array
                
                //1.allocate buffer directly from native memory
                ByteBuffer byteBuffer = ByteBuffer.allocateDirect(rect_vertices.length * 4); //4for float
                
                //arrange the buffer in native byte order
                byteBuffer.order(ByteOrder.nativeOrder());
                
                //create float type buffer & convert our byte buffer type buffer
                
                FloatBuffer positionBuffer = byteBuffer.asFloatBuffer();
                
                //now put array into cooked arraay
                
		positionBuffer.put(rect_vertices);
		
		//set arrays at zeroth position of buffe
		//interleaved array
		positionBuffer.position(0);
		
		
		GLES31.glBufferData(GLES31.GL_ARRAY_BUFFER,
		                    rect_vertices.length*4,
		                    positionBuffer,
		                    GLES31.GL_STATIC_DRAW );
		                    
                GLES31.glBindBuffer(GLES31.GL_ARRAY_BUFFER, 0);
	        //draw neccessaary matrices
	         GLES31.glDrawArrays(GLES31.GL_TRIANGLE_FAN,    //what to draw from given array
					0,				//from where to start
					4);				//How many to draw from start

	        GLES31.glBindVertexArray(0);
	        
	        //2nd checker
	        
		Matrix.setIdentityM(modelViewMatrix, 0);
		Matrix.setIdentityM(modelViewProjectionMatrix, 0);
		Matrix.setIdentityM(translateMatrix, 0);
		
		
		
		Matrix.translateM(modelViewMatrix, 0 ,translateMatrix, 0, 0.1f, 0.0f, -4.0f);
		

		Matrix.multiplyMM(modelViewProjectionMatrix, 0,
		                 perspectiveProjectionMatrix, 0,
		                 modelViewMatrix, 0);
		 
		
		GLES31.glUniformMatrix4fv(mvpUniform,				
						1,				
						false,				
						modelViewProjectionMatrix,
						0 );
		
						
		GLES31.glBindVertexArray(vao_rect[0]);
                
                //2nd pos
                rect_vertices[0] = 1.0f;
                rect_vertices[1] = -1.0f;
                rect_vertices[2] = 0.0f;
                
                rect_vertices[3] = 1.0f;
                rect_vertices[4] = 1.0f;
                rect_vertices[5] = 0.0f;
                
                rect_vertices[6] = 2.41421f;  
                rect_vertices[7] = 1.0f; 
                rect_vertices[8] = -1.41411f;
                
                rect_vertices[9] = 2.41421f;  
                rect_vertices[10] = -1.0f; 
                rect_vertices[11] = -1.41411f;
                
                GLES31.glBindBuffer(GLES31.GL_ARRAY_BUFFER, vbo_rect[0]);
                
                //create buffer data from our array
                
                //1.allocate buffer directly from native memory
                ByteBuffer byteBufferOne = ByteBuffer.allocateDirect(rect_vertices.length * 4); //4for float
                
                //arrange the buffer in native byte order
                byteBufferOne.order(ByteOrder.nativeOrder());
                
                //create float type buffer & convert our byte buffer type buffer
                
                FloatBuffer positionBufferOne = byteBufferOne.asFloatBuffer();
                
                //now put array into cooked arraay
                
		positionBufferOne.put(rect_vertices);
		
		//set arrays at zeroth position of buffe
		//interleaved array
		positionBufferOne.position(0);
		
		
		GLES31.glBufferData(GLES31.GL_ARRAY_BUFFER,
		                    rect_vertices.length*4,
		                    positionBufferOne,
		                    GLES31.GL_STATIC_DRAW );
		                    
                GLES31.glBindBuffer(GLES31.GL_ARRAY_BUFFER, 0);
	        //draw neccessaary matrices
	         GLES31.glDrawArrays(GLES31.GL_TRIANGLE_FAN,    //what to draw from given array
					0,				//from where to start
					4);				//How many to draw from start

	        GLES31.glBindVertexArray(0);
		//
	        GLES31.glUseProgram(0);
		
		requestRender();

		System.out.println("RTR: End of display()");
	}


        private int loadTexture()
        {
	         
	        int[] texture =new int[1];
                        
                          MakeCheckImage( );
                        
                        ByteBuffer byteBufferChecker = ByteBuffer.allocateDirect(checkimage.length * 4); 
                
                        //arrange the buffer in native byte order
                        byteBufferChecker.order(ByteOrder.nativeOrder());
                        
                        
		        byteBufferChecker.put(checkimage);
		        
		        //set arrays at zeroth position of buffe
		        //interleaved array
		        byteBufferChecker.position(0);         
                       
                       
                        Bitmap bitmap = Bitmap.createBitmap(checkimagewidth,
                                                     checkimageheight,
                                                     Bitmap.Config.ARGB_8888);
                        
                        bitmap.copyPixelsFromBuffer(byteBufferChecker);
                        
                        //
                        
		        GLES31.glPixelStorei(GLES31.GL_UNPACK_ALIGNMENT, 1);

		        GLES31.glGenTextures(1, texture, 0);

		        GLES31.glBindTexture(GLES31.GL_TEXTURE_2D, texture[0]);

		        GLES31.glTexParameteri(GLES31.GL_TEXTURE_2D,
			        GLES31.GL_TEXTURE_MAG_FILTER,
			        GLES31.GL_LINEAR);

		        GLES31.glTexParameteri(GLES31.GL_TEXTURE_2D,
			        GLES31.GL_TEXTURE_MIN_FILTER,
			        GLES31.GL_LINEAR_MIPMAP_LINEAR);

		        
		        GLUtils.texImage2D(GLES31.GL_TEXTURE_2D,
						        0,
						        bitmap,
						        0);
		        
		        
		        GLES31.glGenerateMipmap(GLES31.GL_TEXTURE_2D);
		        
		        GLES31.glBindTexture(GLES31.GL_TEXTURE_2D, 0);

                return (texture[0]);
        }
        
        void MakeCheckImage( )
        {
                
	        for (int i=0; i<checkimageheight; i++)
	        {
		        for (int j=0; j<checkimagewidth; j++)
		        {
			        int c = (((i & 0x8)==0) ^ ((j & 0x8)==0)) ? 1: 0;
                                c = c * 255;        
			        checkimage[(i * checkimageheight * 4) + (j*4)+ 0] = (byte)c;
			        checkimage[(i * checkimageheight * 4) + (j*4)+ 1] = (byte)c;
			        checkimage[(i * checkimageheight * 4) + (j*4)+ 2] = (byte)c;
			        checkimage[(i * checkimageheight * 4) + (j*4)+ 3] = (byte)255;
		        }
	        }
	
        }
        
        
	private void Uninitialize()
	{
	
	       
	                
                                
                        if (vbo_texcoord[0] != 0)
	                {
		                GLES31.glDeleteBuffers(1, vbo_texcoord , 0);
		                
	                }
	                if (vbo_recttwo[0] != 0)
	                {
		                GLES31.glDeleteBuffers(1, vbo_recttwo , 0);
		                
	                }
                                
                        if (vbo_rect[0] != 0)
	                {
		                GLES31.glDeleteBuffers(1, vbo_rect , 0);
		                
	                }
	                if (vao_rect[0] != 0)
	                {
		                GLES31.glDeleteVertexArrays(1, vao_rect, 0);
		                
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

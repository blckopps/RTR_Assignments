//global variables
var canvas = null;
var gl = null;
var bFullScreen=false;
var canvas_original_width;
var canvas_original_height;

const WebGLMacros =
{
	AMC_ATTRIBUTE_VERTEX:0,
	AMC_ATTRIBUTE_COLOR:1,
	AMC_ATTRIBUTE_NORMAL:2,
	AMC_ATTRIBUTE_TEXTURE0:3,
};

// shader objects
var vertexShaderObject;
var fragmentShaderObject;
var shaderProgramObject;

var vao_cube;
var vbo_cube;
var vbo_cube_col;

var vao_pyramid;
var vbo_pyramid;
var vbo_cube_col

var mvpUniform;

var perspectiveProjectionMatrix;

//
var angle = 0.0;

//To start animation
var requestAnimationFrame = 
	window.requestAnimationFrame || null ||
	window.webKitRequestAnimationFrame ||
	window.mozRequestAnimationFrame ||
	window.oRequestAnimationFrame ||
	window.msRequestAnimationFrame;
	
var cancelAnimationFrame = 
	window.cancelAnimationFrame || null ||
	window.webKitCancelAnimationFrame || window.webKitCancelRequestAnimationFrame ||
	window.mozCancelAnimationFrame || window.mozCancelRequestAnimationFrame ||
	window.oCancelRequestAnimationFrame || window.oCancelAnimationFrame ||
	window.msCancelRequestAnimationFrame || window.msCancelAnimationFrame ;
	
function myMain()
{
	//get <canvas> element..
	 canvas = document.getElementById("id_Canvas");
	if(!canvas)
		console.log("Obtaining canvas faild!!!\n");
	else
		console.log("Obtaining canvas Successfull\n");
	
	canvas_original_width = canvas.width;
	canvas_original_height = canvas.height;
	
	//register keyboards keydown and event handler..
	window.addEventListener("keydown",keydown,false);
	window.addEventListener("click",mouseDown,false);
	window.addEventListener("resize",resize, false);
		
		init();
		
		resize();
		draw();
}

function keydown(event)
{
	switch(event.keyCode)
	{
		case 70:
		//alert("F key is pressed");
		toggleFullScreen();
		
		break;

		case 27:
			uninitialize();
			window.close();	//may not work in firefox but works in chrome....
			break;
		
	}
}

function mouseDown()
{
	
}

function toggleFullScreen()
{
	//code.
	var fullscreen_element =
	document.fullscreenElement ||
	document.webKitFullscreenElement ||
	document.mozFullScreenElement ||
	document.msFullscreenElement || 
	null;
	
	if(fullscreen_element == null)
	{
		if(canvas.requestFullscreen)
			canvas.requestFullscreen();
		else if(canvas.mozRequestFullScreen)
			canvas.mozRequestFullScreen();
		else if(canvas.webKitRequestFullscreen)
			canvas.webKitRequestFullscreen();
		else if(canvas.msRequestFullscreen)
			canvas.msRequestFullscreen();
		bFullScreen = true;
	}
	else
	{
		if(document.exitFullscreen)
			document.exitFullscreen();
		else if(document.mozCancelFullScreen)
			document.mozCancelFullScreen();
		else if(document.webKitExitFullscreen)
			webKitExitFullscreen();
		else if(msExitFullscreen)
			document.msExitFullscreen();
			
		bFullScreen = false;	
	}
}

function init()
{
	//code
	//Get WebGL 2.0 context
	gl = canvas.getContext("webgl2");
	if(gl==null)
	{
		console.log("getContext: null context??");
		return;
	}
	else
	{
		console.log("getContext: Successfull\n");

	}
	
	gl.viewportWidth = canvas.width;
	gl.viewportHeight = canvas.height;
	
	//vertexShader code
	var vertexShaderSourceCode = 
	"#version 300 es" +
	"\n" +
	"in vec4 vPosition;" +
	"in vec4 vColor;" +
	"uniform mat4 u_mvp_matrix;" +
	"out vec4 out_color;" +
	"void main(void)" +
	"{" +
		"gl_Position = u_mvp_matrix * vPosition;" +
		"out_color = vColor;" +
	"}";
	
	vertexShaderObject = gl.createShader(gl.VERTEX_SHADER);
	gl.shaderSource(vertexShaderObject,vertexShaderSourceCode);
	gl.compileShader(vertexShaderObject);
	
	if(gl.getShaderParameter(vertexShaderObject,gl.COMPILE_STATUS)==false)
	{
		var error = gl.getShaderInfoLog(vertexShaderObject);
		if(error.length > 0)
		{
			alert("VERTEXT SHADER EROOR");
			uninitialize();
		}
	}
	
	//fragment shader
	var fragmentShaderSourceCode = 
	"#version 300 es" +
	"\n" +
	
	"precision highp float;" +
	"in vec4 out_color;" +
	"out  vec4 FragColor;" +
	"void main(void)" +
	"{" +
		"FragColor = out_color;" +
	"}";
	
	fragmentShaderObject = gl.createShader(gl.FRAGMENT_SHADER);
	gl.shaderSource(fragmentShaderObject,fragmentShaderSourceCode);
	gl.compileShader(fragmentShaderObject);
	
	if(gl.getShaderParameter(fragmentShaderObject,gl.COMPILE_STATUS)==false)
	{
		var error = gl.getShaderInfoLog(fragmentShaderObject);
		if(error.length > 0)
		{
			alert("FRAGMENT SHADER EROOR: " + error);
			uninitialize();
		}
	}
	//shader program
	shaderProgramObject = gl.createProgram();
	gl.attachShader(shaderProgramObject, vertexShaderObject);
	gl.attachShader(shaderProgramObject, fragmentShaderObject);
	
	//prelinking
	gl.bindAttribLocation(shaderProgramObject, WebGLMacros.AMC_ATTRIBUTE_VERTEX, "vPosition");
	gl.bindAttribLocation(shaderProgramObject, WebGLMacros.AMC_ATTRIBUTE_COLOR, "vColor");
	//linking
	gl.linkProgram(shaderProgramObject);
	if(!gl.getProgramParameter(shaderProgramObject, gl.LINK_STATUS))
	{
		var error = gl.getProgramInfoLog(shaderProgramObject);
		if(error.length > 0)
		{
				alert("LINK ERROR : "+ error);
				uninitialize();
		}
	}
	
	//Post Link..getting uniforms locations
	mvpUniform = gl.getUniformLocation(shaderProgramObject, "u_mvp_matrix");

	////////////******Vertices color texcoord, vao vbo*****///////////
	var cube_vertices = new Float32Array([
											//TOP
											1.0,1.0,-1.0,
											-1.0,1.0,-1.0,
											-1.0,1.0,1.0,					
											1.0,1.0,1.0,					
										   //bottom
		
											1.0,-1.0,-1.0,
											-1.0,-1.0,-1.0,				
											-1.0,-1.0,1.0,
											1.0, -1.0, 1.0,
										   ////Front
		
											1.0,1.0,1.0,					
											-1.0,1.0,1.0,				
											-1.0,-1.0,1.0,				
											1.0,-1.0,1.0,
										   //back
		
											1.0,1.0,-1.0,				
											-1.0,1.0,-1.0,				
											-1.0,-1.0,-1.0,				
											1.0,-1.0,-1.0,
   
										   //Right
		
											1.0,1.0,-1.0,				
											1.0,1.0,1.0,					
											1.0,-1.0,1.0,				
											1.0,-1.0,-1.0,
   
										   //left
		
											-1.0,1.0,1.0,					
											-1.0,1.0,-1.0,					
											-1.0,-1.0,-1.0,					
											-1.0,-1.0,1.0

											]);

	var cube_color = new Float32Array([
											1.0, 0.0, 0.0,
											1.0, 0.0, 0.0,
											1.0, 0.0, 0.0,
											1.0, 0.0, 0.0,

											0.0, 1.0, 0.0,
											0.0, 1.0, 0.0,
											0.0, 1.0, 0.0,
											0.0, 1.0, 0.0,

											0.0, 0.0, 1.0,
											0.0, 0.0, 1.0,
											0.0, 0.0, 1.0,
											0.0, 0.0, 1.0,

											1.0, 1.0, 0.0,
											1.0, 1.0, 0.0,
											1.0, 1.0, 0.0,
											1.0, 1.0, 0.0,

											1.0, 0.0, 1.0,
											1.0, 0.0, 1.0,
											1.0, 0.0, 1.0,
											1.0, 0.0, 1.0,

											0.0, 1.0, 1.0,
											0.0, 1.0, 1.0,
											0.0, 1.0, 1.0,
											0.0, 1.0, 1.0
										]);

	var pyramid_vertices = new Float32Array([
												//1st		
												0.0, 1.0, 0.0,
												-1.0, -1.0, 1.0,				
												1.0, -1.0, 1.0,
												//2nd
							
												0.0, 1.0, 0.0,
												1.0, -1.0, 1.0,
												1.0, -1.0, -1.0,				
												//3rd
												0.0, 1.0, 0.0,				
												1.0, -1.0, -1.0,				
												-1.0, -1.0, -1.0,
												//4th
												0.0, 1.0, 0.0,				
												-1.0, -1.0, -1.0,
												-1.0, -1.0, 1.0
											]);

	var pyramid_color = new Float32Array([
											1.0, 0.0, 0.0,
											0.0, 1.0, 0.0,
											0.0, 0.0, 1.0,
											1.0, 0.0, 0.0,
											0.0, 1.0, 0.0,
											0.0, 0.0, 1.0,
											1.0, 0.0, 0.0,
											0.0, 1.0, 0.0,
											0.0, 0.0, 1.0,
											1.0, 0.0, 0.0,
											0.0, 1.0, 0.0,
											0.0, 0.0, 1.0
										])


	//triangle
	//STRAT:											
	vao_cube = gl.createVertexArray();
	gl.bindVertexArray(vao_cube);

	//position
	vbo_cube = gl.createBuffer();

	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_cube);
	gl.bufferData(gl.ARRAY_BUFFER, cube_vertices, gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.AMC_ATTRIBUTE_VERTEX, 3, gl.FLOAT, false , 0, 0);
	gl.enableVertexAttribArray(WebGLMacros.AMC_ATTRIBUTE_VERTEX);

	gl.bindBuffer(gl.ARRAY_BUFFER, null);

	//color
	vbo_cube_col = gl.createBuffer();

	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_cube_col);
	gl.bufferData(gl.ARRAY_BUFFER, cube_color, gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.AMC_ATTRIBUTE_COLOR, 3, gl.FLOAT, false , 0, 0);
	gl.enableVertexAttribArray(WebGLMacros.AMC_ATTRIBUTE_COLOR);

	gl.bindBuffer(gl.ARRAY_BUFFER, null);

	gl.bindVertexArray(null);
	//END:
	
	//rectangle
	//START:
	vao_pyramid = gl.createVertexArray();
	gl.bindVertexArray(vao_pyramid);

	vbo_pyramid = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_pyramid);
	gl.bufferData(gl.ARRAY_BUFFER, pyramid_vertices, gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.AMC_ATTRIBUTE_VERTEX, 3, gl.FLOAT, false , 0, 0);
	gl.enableVertexAttribArray(WebGLMacros.AMC_ATTRIBUTE_VERTEX);

	gl.bindBuffer(gl.ARRAY_BUFFER, null);

	//color
	vbo_cube_col = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_cube_col);
	gl.bufferData(gl.ARRAY_BUFFER, pyramid_color, gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.AMC_ATTRIBUTE_COLOR, 3, gl.FLOAT, false , 0, 0);
	gl.enableVertexAttribArray(WebGLMacros.AMC_ATTRIBUTE_COLOR);

	gl.bindBuffer(gl.ARRAY_BUFFER, null);

	gl.bindVertexArray(null);
	//END:
	gl.enable(gl.DEPTH_TEST);
	//gl.depthFunc(GL_LEQUAL);
	//Set Clear color
	gl.clearColor(0.0, 0.0, 0.0, 1.0); 
	gl.clearDepth(1.0);
	perspectiveProjectionMatrix = mat4.create();
}

function resize()
{
	if(bFullScreen == true)
	{
		canvas.width = window.innerWidth;
		canvas.height = window.innerHeight;
	}
	else
	{
		canvas.width = canvas_original_width;
		canvas.height = canvas_original_height;
	}
	
	gl.viewport(0, 0, canvas.width, canvas.height);

	mat4.perspective(perspectiveProjectionMatrix, 45.0, parseFloat(canvas.width)/parseFloat(canvas.height), 0.1, 100.0);
}

function draw()
{
	gl.clear(gl.COLOR_BUFFER_BIT);
	
	gl.useProgram(shaderProgramObject);

	var modelViewMatrix = mat4.create();
	var modelViewProjectionMatrix = mat4.create();
	
	
	mat4.translate(modelViewMatrix, modelViewMatrix, [2.0, 0.0 , -5.0]);

	mat4.rotateX(modelViewMatrix, modelViewMatrix, degToRad(angle));
	mat4.rotateY(modelViewMatrix, modelViewMatrix, degToRad(angle));
	mat4.rotateZ(modelViewMatrix, modelViewMatrix, degToRad(angle));
	
	mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);
	gl.uniformMatrix4fv(mvpUniform, false, modelViewProjectionMatrix);

	//Draw required shapes
	//STRAT:
	//cube
	gl.bindVertexArray(vao_cube);
	gl.drawArrays(gl.TRIANGLE_FAN,
		0,
		4);

	gl.drawArrays(gl.TRIANGLE_FAN,
			4,
			4);

	gl.drawArrays(gl.TRIANGLE_FAN,
			8,
			4);

	gl.drawArrays(gl.TRIANGLE_FAN,
			12,
			4);

	gl.drawArrays(gl.TRIANGLE_FAN,
			16,
			4);

	gl.drawArrays(gl.TRIANGLE_FAN,
			20,
			4);

	gl.bindVertexArray(null) ;

	gl.bindVertexArray(null) ;
	//END:

	//Draw pyramid
	var modelViewMatrix = mat4.create();
	var modelViewProjectionMatrix = mat4.create();
	
	mat4.translate(modelViewMatrix, modelViewMatrix, [-1.8, 0.0 , -4.0]);

	//mat4.rotateX(modelViewMatrix, modelViewMatrix, degToRad(angle));
	mat4.rotateY(modelViewMatrix, modelViewMatrix, degToRad(angle));
	//mat4.rotateX(modelViewMatrix, modelViewMatrix, degToRad(angle));

	mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);
	gl.uniformMatrix4fv(mvpUniform, false, modelViewProjectionMatrix);

	//Draw required shapes
	//STRAT:
	//triangle
	gl.bindVertexArray(vao_pyramid);
	gl.drawArrays(gl.TRIANGLE_FAN, 0, 12);
	gl.bindVertexArray(null) ;

	gl.bindVertexArray(null) ;
	//END:

	gl.useProgram(null);
	//Animation loop
	requestAnimationFrame(draw, canvas);

	if(angle >= 360.0)
	{
		angle = 0.0;
	}
	else
	{
		angle = angle + 0.5;
	}
}

function uninitialize()
{
	if(vao_cube)
	{
		gl.deleteVertexArray(vao_cube);
		vao_cube = null;
	}

	if(vbo_cube)
	{
		gl.deleteBuffer(vbo_cube);
		vbo_cube = null;
	}

	if(vbo_cube_col)
	{
        gl.deleteBuffer(vbo_cube_col);
		vbo_cube_col = null;
	}
	/////rect
	if(vao_pyramid)
	{
		gl.deleteVertexArray(vao_pyramid);
		vao_pyramid = null;
	}

	if(vbo_pyramid)
	{
		gl.deleteBuffer(vbo_pyramid);
		vbo_pyramid = null;
	}

	if(vbo_cube_col)
	{
        gl.deleteBuffer(vbo_cube_col);
		vbo_cube_col = null;
	}


	if(shaderProgramObject)
	{
			if(fragmentShaderObject)
			{
				gl.detachShader(shaderProgramObject, fragmentShaderObject);
				gl.deleteShader(fragmentShaderObject);
				fragmentShaderObject = null;

			}

			if(vertexShaderObject)
			{
				gl.detachShader(shaderProgramObject, vertexShaderObject);
				gl.deleteShader(vertexShaderObject);
				vertexShaderObject = null;

			}
			gl.deleteProgram(shaderProgramObject);
			shaderProgramObject = null;
	}
}

function degToRad(angle)
{
	return(angle * Math.PI/180);
}
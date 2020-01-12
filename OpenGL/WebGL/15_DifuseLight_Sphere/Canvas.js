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
var vbo_normals_cube;

var vao_pyramid;
var vbo_pyramid;
var vbo_texture_pyramid

var mvpUniform;

var perspectiveProjectionMatrix;

var modelViewMatrixUniform, projectionMatrixUniform;
var ldUniform, kdUniform, lightPositionUniform;
var lKeyPressedUniform ;

var bLkeyPressed = 0;
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
		
	 mesh = new Mesh();
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

			case 76:
				if(bLkeyPressed == 0)
				{
					bLkeyPressed = 1;	
				}
				else
				{
					bLkeyPressed = 0;
				}
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
	"in vec3 vNormal;" +

	"uniform mat4 u_model_view_matrix;" +
	"uniform mat4 u_projection_matrix;" +
	"uniform mediump int u_lkeypressed;" +
	"uniform vec3 u_ld;" +
	"uniform vec3 u_kd;" +
	"uniform vec3 u_light_position;" +

	"out vec3 difuse_light;" +
	"void main(void)" +
	"{" +
		"if(u_lkeypressed == 1)" +
		"{" +
			"vec4 eyeCoord = u_model_view_matrix * vPosition;" +
			"vec3 tnorm = normalize(mat3(u_model_view_matrix) * vNormal);" +
			"vec3 s = normalize(vec3(u_light_position - (eyeCoord).xyz));" +
			"difuse_light = u_ld * u_kd * max(dot(s, tnorm), 0.0);" +
		"}" +
		"else" +
		"{" +
			"difuse_light = vec3(1.0,1.0,1.0);" +
		"}" +
		"gl_Position = u_projection_matrix * u_model_view_matrix * vPosition;" +
	"}";
	
	vertexShaderObject = gl.createShader(gl.VERTEX_SHADER);
	gl.shaderSource(vertexShaderObject,vertexShaderSourceCode);
	gl.compileShader(vertexShaderObject);
	
	if(gl.getShaderParameter(vertexShaderObject,gl.COMPILE_STATUS)==false)
	{
		var error = gl.getShaderInfoLog(vertexShaderObject);
		if(error.length > 0)
		{
			alert("VERTEXT SHADER EROOR" + error);
			uninitialize();
		}
	}
	
	//fragment shader
	var fragmentShaderSourceCode = 
	"#version 300 es" +
	"\n" +
	
	"precision highp float;" +
	"in vec3 difuse_light;" +
	"uniform mediump int u_lkeypressed;" +
	"out  vec4 FragColor;" +
	"void main(void)" +
	"{" +
		"if(u_lkeypressed == 1)" +
		"{" +
			"FragColor = vec4(difuse_light, 1.0);" +
		 "}" +
		 "else" +
		 "{" +
			"FragColor = vec4(difuse_light, 1.0);" +
		  "}" +
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
	gl.bindAttribLocation(shaderProgramObject, WebGLMacros.AMC_ATTRIBUTE_NORMAL, "vNormal");
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
	modelViewMatrixUniform = gl.getUniformLocation(shaderProgramObject, "u_model_view_matrix");
	projectionMatrixUniform = gl.getUniformLocation(shaderProgramObject, "u_projection_matrix");

	lKeyPressedUniform = gl.getUniformLocation(shaderProgramObject, "u_lkeypressed");
	ldUniform = gl.getUniformLocation(shaderProgramObject, "u_ld");
	kdUniform = gl.getUniformLocation(shaderProgramObject,"u_kd");
	lightPositionUniform = gl.getUniformLocation(shaderProgramObject, "u_light_position");

	
	////////////******Vertices color texcoord, vao vbo*****///////////
	 
	makeSphere(mesh, 2.0, 100, 10);
	
	//END:
	
	
	
	gl.depthFunc(gl.LEQUAL);
	//Set Clear color
	gl.clearColor(0.0, 0.0, 0.0, 1.0); 
	gl.clearDepth(1.0);
	gl.enable(gl.DEPTH_TEST);
	//gl.enable(gl.CULL_FACE);

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
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
	
	gl.useProgram(shaderProgramObject);

	var modelViewMatrix = mat4.create();
	
	mat4.translate(modelViewMatrix, modelViewMatrix, [0.0, 0.0 , -7.0]);

	mat4.rotateX(modelViewMatrix, modelViewMatrix, degToRad(angle));
	mat4.rotateY(modelViewMatrix, modelViewMatrix, degToRad(angle));
	mat4.rotateZ(modelViewMatrix, modelViewMatrix, degToRad(angle));
	
	//mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);
	if(bLkeyPressed == 1)
	{
		gl.uniform1i(lKeyPressedUniform, 1);
		gl.uniform3f(ldUniform,1.0,1.0,1.0);
		gl.uniform3f(kdUniform,.5,.5,.5);

		var lightPos = [100.0,100.0,100.0,1.0];
		gl.uniform4fv(lightPositionUniform, lightPos);
		
	}
	else
	{
		gl.uniform1i(lKeyPressedUniform, 0);
	}
	gl.uniformMatrix4fv(modelViewMatrixUniform, false, modelViewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

	//Draw required shapes
	//STRAT:
	//cube
	mesh.draw();
	
	//Animation loop
	requestAnimationFrame(draw, canvas);

	// if(angle >= 360.0)
	// {
	// 	angle = 0.0;
	// }
	// else
	// {
	// 	angle = angle + 0.5;
	// }
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

	if(vbo_texture_pyramid)
	{
        gl.deleteBuffer(vbo_texture_pyramid);
		vbo_texture_pyramid = null;
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

	if(vbo_texture_pyramid)
	{
        gl.deleteBuffer(vbo_texture_pyramid);
		vbo_texture_pyramid = null;
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
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


var vao_pyramid;
var vbo_pyramid;
var vbo_pyramid_color;
var vbo_pyramid_texcoords;

var mUniform;
var vUniform;
var pUniform;

var isLKeyIsPressedUniforms;

var laUniform_Red;
var ldUniform_Red;
var lsUniform_Red;

var laUniform_Blue;
var ldUniform_Blue;
var lsUniform_Blue;

var kaUniform;
var kdUniform;
var ksUniform;

var shininessUniform;

var lightPositionUniform_Red;
var lightPositionUniform_Blue;

var perspectiveProjectionMatrix;


//
var angle = 0.0;
var isLighting = 0;
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

		case 76:
			if(isLighting == 0)
			{
				isLighting = 1;
			}
			else
			{
				isLighting = 0;
			}
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
	"in vec3 vNormal;" +

	"uniform mat4 u_model_matrix;" +
	"uniform mat4 u_view_matrix;" +
	"uniform mat4 u_proj_matrix;" +

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
		"gl_Position = u_proj_matrix * u_view_matrix *  u_model_matrix * vPosition;" +
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
	"in vec3 phong_ads_light;" +
	"out  vec4 FragColor;" +
	"void main(void)" +
	"{" +
		"FragColor = vec4(phong_ads_light, 1.0);" +
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
	gl.bindAttribLocation(shaderProgramObject, WebGLMacros.AMC_ATTRIBUTE_TEXTURE0, "vNormal");
	
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
	mUniform = gl.getUniformLocation(shaderProgramObject, "u_model_matrix");
	vUniform = gl.getUniformLocation(shaderProgramObject, "u_view_matrix");
	pUniform = gl.getUniformLocation(shaderProgramObject, "u_proj_matrix");

	//
	isLKeyIsPressedUniforms = gl.getUniformLocation(shaderProgramObject, "islkeypressed");

	//RED light uniforms
	laUniform_Red = gl.getUniformLocation(shaderProgramObject, "u_la_red");
	ldUniform_Red = gl.getUniformLocation(shaderProgramObject, "u_ld_red");
	lsUniform_Red = gl.getUniformLocation(shaderProgramObject, "u_ls_red");

	//BLUE light unifomrs
	laUniform_Blue = gl.getUniformLocation(shaderProgramObject, "u_la_blue");
	ldUniform_Blue = gl.getUniformLocation(shaderProgramObject, "u_ld_blue");
	lsUniform_Blue = gl.getUniformLocation(shaderProgramObject, "u_ls_blue");

	kaUniform = gl.getUniformLocation(shaderProgramObject, "u_ka");
	kdUniform = gl.getUniformLocation(shaderProgramObject, "u_kd");
	ksUniform = gl.getUniformLocation(shaderProgramObject, "u_ks");

	shininessUniform = gl.getUniformLocation(shaderProgramObject, "u_shininess");

	lightPositionUniform_Red = gl.getUniformLocation(shaderProgramObject, "u_light_position_red");
	lightPositionUniform_Blue = gl.getUniformLocation(shaderProgramObject, "u_light_position_blue");
	
	////////////******Vertices color texcoord, vao vbo*****///////////
	
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
										]);

	var pyramid_tecvoords = new Float32Array([
											0.0, 0.447214, 0.894427,
											0.0, 0.447214, 0.894427,
											0.0, 0.447214, 0.894427,
											0.894427, 0.447214, 0.0,
											0.894427, 0.447214, 0.0,
											0.894427, 0.447214, 0.0,
											0.0 ,0.447214, -0.894427,
											0.0 ,0.447214, -0.894427,
											0.0 ,0.447214, -0.894427,
											-0.894427, 0.447214, 0.0,
											-0.894427, 0.447214, 0.0,
											-0.894427, 0.447214, 0.0
											]);
	//pyramid
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
	vbo_pyramid_color = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_pyramid_color);
	gl.bufferData(gl.ARRAY_BUFFER, pyramid_color, gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.AMC_ATTRIBUTE_COLOR, 3, gl.FLOAT, false , 0, 0);
	gl.enableVertexAttribArray(WebGLMacros.AMC_ATTRIBUTE_COLOR);

	gl.bindBuffer(gl.ARRAY_BUFFER, null);

	//texcoords
	vbo_pyramid_texcoords = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_pyramid_texcoords);
	gl.bufferData(gl.ARRAY_BUFFER, pyramid_tecvoords, gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.AMC_ATTRIBUTE_TEXTURE0, 3, gl.FLOAT, false, 0, 0);
	gl.enableVertexAttribArray(WebGLMacros.AMC_ATTRIBUTE_TEXTURE0);

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


	//Draw pyramid
	var modelMatrix = mat4.create();
	var viewMatrix = mat4.create();
	var modelViewProjectionMatrix = mat4.create();
	
	mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0 , -4.0]);

	//mat4.rotateX(modelMatrix, modelMatrix, degToRad(angle));
	mat4.rotateY(modelMatrix, modelMatrix, degToRad(angle));
	//mat4.rotateX(modelMatrix, modelMatrix, degToRad(angle));

	gl.uniformMatrix4fv(mUniform, false, modelMatrix);
	gl.uniformMatrix4fv(vUniform, false, viewMatrix);
	gl.uniformMatrix4fv(pUniform, false, perspectiveProjectionMatrix);

	if(isLighting == 1)
	{
		gl.uniform1i(isLKeyIsPressedUniforms, 1);

		gl.uniform1f(shininessUniform, 128.0);

		//RED light
		gl.uniform3f(laUniform_Red, 0.0, 0.0, 0.0, 0.0);
		gl.uniform3f(ldUniform_Red, 1.0, 0.0, 0.0, 1.0);
		gl.uniform3f(lsUniform_Red, 1.0, 0.0, 0.0, 1.0 );

		//Blue
		gl.uniform3f(laUniform_Blue, 0.0, 0.0, 0.0, 0.0);
		gl.uniform3f(ldUniform_Blue,0.0, 0.0, 1.0, 1.0);
		gl.uniform3f(lsUniform_Blue,0.0, 0.0, 1.0, 1.0);

		//material
		gl.uniform3f(kaUniform, 0.0, 0.0, 0.0, 0.0);
		gl.uniform3f(kdUniform,  1.0, 1.0, 1.0, 1.0);
		gl.uniform3f(ksUniform,  1.0, 1.0, 1.0, 1.0);

		gl.uniform4f(lightPositionUniform_Red,100.0, 100.0, 100.0, 1.0);
		gl.uniform4f(lightPositionUniform_Blue, -100.0, 100.0, 100.0, 1.0);
	}
	else
	{
		gl.uniform1i(isLKeyIsPressedUniforms, 0);
	}
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

	if(vbo_pyramid_color)
	{
        gl.deleteBuffer(vbo_pyramid_color);
		vbo_pyramid_color = null;
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
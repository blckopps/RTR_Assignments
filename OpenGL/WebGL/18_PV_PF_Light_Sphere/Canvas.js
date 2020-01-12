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


var perspectiveProjectionMatrix;

/////For per Vertex///////////
// shader objects for PV
var vertexShaderObject_PV;
var fragmentShaderObject_PV;
var shaderProgramObject_PV;

var modelViewMatrixUniform_PV, projectionMatrixUniform_PV;


var laUniform_PV,ldUniform_PV, lsUniform_PV;
var kaUniform,kdUniform_PV, ksUniform_PV;
var shininessUniform_PV;
var lightPositionUniform_PV;

var lKeyPressedUniform_PV ;

////////For per Fragment/////////
var vertexShaderObject_PF;
var fragmentShaderObject_PF;
var shaderProgramObject_PF;

var modelViewMatrixUniform_PF, projectionMatrixUniform_PF;


var laUniform_PF,ldUniform_PF, lsUniform_PF;
var kaUniform_PF,kdUniform_PF, ksUniform_PF;
var shininessUniform_PF;
var lightPositionUniform_PF;
//light arrays
var lightAmbient = [ 0.0, 0.0, 0.0];
var lightDifuse = [ 1.0, 1.0, 1.0];
var lightSpecular = [ 1.0, 1.0, 1.0 ];

//material array
var materialAmbient = [ 0.0, 0.0, 0.0 ];
var materialDifuse = [ 1.0, 1.0, 1.0];
var materialSpecular= [ 1.0, 1.0, 1.0];

var lightPosition = [ 100.0, 100.0, 100.0, 1.0];

var materialShininess = 128.0;

var lKeyPressedUniform_PF ;

var bLkeyPressed = 0;
var perVertex = 0;
var perFragment = 0;
//

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
		case 84:					// 'T ' to toogle
		toggleFullScreen();
		
		break;

		case 81:					// 'q' for quit
			uninitialize();
			window.close();	//may not work in firefox but works in chrome....
			break;

			case 76:					// 'L' key   
				if(bLkeyPressed == 0)
				{
					bLkeyPressed = 1;	
				}
				else
				{
					bLkeyPressed = 0;
				}
				break;

				case 70:				//F key for fragment shader
					if(perFragment == 0)
					{
						perFragment = 1;
						perVertex = 0;
					}
					else
					{
						perFragment= 0;
					}
					break;


					case 86:				//V key for vertex shader
					//console.log("V key");
					
						perVertex = 1;
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
	

	/////////////////////////////////Per Vertex/////////////////////////////////
	//vertexShader code
	var vertexShaderSourceCode_PV = 
	"#version 300 es" +
	"\n" +
	"in vec4 vPosition;" +
	"in vec3 vNormal;" +

	"uniform mat4 u_model_view_matrix;" +
	"uniform mat4 u_projection_matrix;" +

	"uniform mediump int u_lkeypressed;" +

	"uniform vec3 u_la;" +
	"uniform vec3 u_ld;" +
	"uniform vec3 u_ls;" +

	"uniform vec3 u_ka;" +
	"uniform vec3 u_kd;" +
	"uniform vec3 u_ks;" +

	"uniform float u_shininess;" +
	"uniform vec4 u_light_position;" +

	"out vec3 ads_light;" +

	"void main(void)" +
	"{" +
		"if(u_lkeypressed == 1)" +
		"{" +
			"vec4 eyeCoord = u_model_view_matrix * vPosition;" +
			"vec3 tnorm = normalize(mat3(u_model_view_matrix) * vNormal);" +
			"vec3 light_direction = normalize(vec3(u_light_position) - (eyeCoord).xyz);" +
			"float tn_dot_ldirection = max(dot(tnorm, light_direction), 0.0);" +
			"vec3 reflection_vector = reflect(-light_direction, tnorm);" +
			"vec3 viewer_vector = normalize(-eyeCoord.xyz);" +
			"vec3 ambient = u_la * u_ka;" +
			"vec3 difuse = u_ld * u_kd * tn_dot_ldirection;" +
			"vec3 specular = u_ls * u_ks * pow(max(dot(reflection_vector, viewer_vector), 0.0), u_shininess);" +
			"ads_light = ambient + difuse + specular;" +
		"}" +
		"else" +
		"{" +
			"ads_light = vec3(1.0,1.0,1.0);" +
		"}" +
		"gl_Position = u_projection_matrix * u_model_view_matrix * vPosition;" +
	"}";
	
	vertexShaderObject_PV = gl.createShader(gl.VERTEX_SHADER);
	gl.shaderSource(vertexShaderObject_PV,vertexShaderSourceCode_PV);
	gl.compileShader(vertexShaderObject_PV);
	
	if(gl.getShaderParameter(vertexShaderObject_PV,gl.COMPILE_STATUS)==false)
	{
		var error = gl.getShaderInfoLog(vertexShaderObject_PV);
		if(error.length > 0)
		{
			alert("VERTEXT SHADER PER VERTEX EROOR" + error);
			uninitialize();
		}
	}
	
	//fragment shader
	var fragmentShaderSourceCode_PV = 
	"#version 300 es" +
	"\n" +
	
	"precision highp float;" +
	"in vec3 ads_light;" +
	"out  vec4 FragColor;" +
	"void main(void)" +
	"{" +
		"FragColor = vec4(ads_light, 1.0);" +
	"}";
	
	fragmentShaderObject_PV = gl.createShader(gl.FRAGMENT_SHADER);
	gl.shaderSource(fragmentShaderObject_PV,fragmentShaderSourceCode_PV);
	gl.compileShader(fragmentShaderObject_PV);
	
	if(gl.getShaderParameter(fragmentShaderObject_PV,gl.COMPILE_STATUS)==false)
	{
		var error = gl.getShaderInfoLog(fragmentShaderObject_PV);
		if(error.length > 0)
		{
			alert("FRAGMENT SHADER PER VERTEX EROOR: " + error);
			uninitialize();
		}
	}
	//shader program
	shaderProgramObject_PV = gl.createProgram();
	gl.attachShader(shaderProgramObject_PV, vertexShaderObject_PV);
	gl.attachShader(shaderProgramObject_PV, fragmentShaderObject_PV);
	
	//prelinking
	gl.bindAttribLocation(shaderProgramObject_PV, WebGLMacros.AMC_ATTRIBUTE_VERTEX, "vPosition");
	gl.bindAttribLocation(shaderProgramObject_PV, WebGLMacros.AMC_ATTRIBUTE_NORMAL, "vNormal");
	//linking
	gl.linkProgram(shaderProgramObject_PV);
	if(!gl.getProgramParameter(shaderProgramObject_PV, gl.LINK_STATUS))
	{
		var error = gl.getProgramInfoLog(shaderProgramObject_PV);
		if(error.length > 0)
		{
				alert("SHADER LINK ERROR PER VERTEX: " + error);
				uninitialize();
		}
	}
	
	//Post Link..getting uniforms locations
	modelViewMatrixUniform_PV = gl.getUniformLocation(shaderProgramObject_PV, "u_model_view_matrix");
	projectionMatrixUniform_PV = gl.getUniformLocation(shaderProgramObject_PV, "u_projection_matrix");

	lKeyPressedUniform_PV = gl.getUniformLocation(shaderProgramObject_PV, "u_lkeypressed");

	laUniform_PV = gl.getUniformLocation(shaderProgramObject_PV, "u_la");
	ldUniform_PV = gl.getUniformLocation(shaderProgramObject_PV, "u_ld");
	lsUniform_PV = gl.getUniformLocation(shaderProgramObject_PV, "u_ls");

	kaUniform_PV = gl.getUniformLocation(shaderProgramObject_PV,"u_ka");
	kdUniform_PV = gl.getUniformLocation(shaderProgramObject_PV,"u_kd");
	ksUniform_PV = gl.getUniformLocation(shaderProgramObject_PV,"u_ks");

	shininessUniform_PV = gl.getUniformLocation(shaderProgramObject_PV, "u_shininess");
	lightPositionUniform_PV = gl.getUniformLocation(shaderProgramObject_PV, "u_light_position");
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////Per Fragment ///////////////////////
	//vertexShader code
	var vertexShaderSourceCode_PF = 
	"#version 300 es" +
	"\n" +
	"in vec4 vPosition;" +
	"in vec3 vNormal;" +

	"uniform mat4 u_model_view_matrix;" +
	"uniform mat4 u_projection_matrix;" +

	"uniform mediump int u_lkeypressed;" +

	"uniform vec4 u_light_position;" +

	"out vec3 tnorm;" +
	"out vec3 light_direction;" +
	"out vec3 viewer_vector;" +

	"void main(void)" +
	"{" +
		"if(u_lkeypressed == 1)" +
		"{" +
			"vec4 eyeCoord = u_model_view_matrix * vPosition;" +

			"tnorm = normalize(mat3(u_model_view_matrix) * vNormal);" +
			"light_direction = normalize(vec3(u_light_position) - (eyeCoord).xyz);" +
			"viewer_vector = normalize(-eyeCoord.xyz);" +

		"}" +
		"gl_Position = u_projection_matrix * u_model_view_matrix * vPosition;" +
	"}";
	
	vertexShaderObject_PF = gl.createShader(gl.VERTEX_SHADER);
	gl.shaderSource(vertexShaderObject_PF,vertexShaderSourceCode_PF);
	gl.compileShader(vertexShaderObject_PF);
	
	if(gl.getShaderParameter(vertexShaderObject_PF,gl.COMPILE_STATUS)==false)
	{
		var error = gl.getShaderInfoLog(vertexShaderObject_PF);
		if(error.length > 0)
		{
			alert("VERTEXT SHADER EROOR" + error);
			uninitialize();
		}
	}
	
	//fragment shader
	var fragmentShaderSourceCode_PF = 
	"#version 300 es" +
	"\n" +
	
	"precision highp float;" +

	"in vec3 tnorm;" +
	"in vec3 light_direction;" +
	"in vec3 viewer_vector;" +

	"uniform vec3 u_la;" +
	"uniform vec3 u_ld;" +
	"uniform vec3 u_ls;" +

	"uniform vec3 u_ka;" +
	"uniform vec3 u_kd;" +
	"uniform vec3 u_ks;" +

	"uniform float u_shininess;" +
	"uniform mediump int u_lkeypressed;" +

	"out  vec4 FragColor;" +
	"void main(void)" +
	"{" +
		"vec3 phong_ads_color;" +

		"if(u_lkeypressed == 1)" +
		"{" +
				"vec3 normalized_tnorm = normalize(tnorm);" +
				"vec3 normalized_light_direction = normalize(light_direction);" +
				"vec3 normalized_viewer_vector = normalize(viewer_vector);" +

				"float tn_dot_ldirection = max(dot(normalized_tnorm, normalized_light_direction), 0.0);" +

				"vec3 reflection_vector = reflect(-normalized_light_direction, normalized_tnorm);" +

				"vec3 ambient = u_la * u_ka;" +
				"vec3 difuse = u_ld * u_kd * tn_dot_ldirection;" +
				"vec3 specular = u_ls * u_ks * pow(max(dot(reflection_vector, normalized_viewer_vector), 0.0), u_shininess);" +

				"phong_ads_color = ambient + difuse + specular;" +
		"}" +
		"else" +
		"{" +
			"phong_ads_color = vec3(1.0, 1.0, 1.0);" +
		"}" +
		"FragColor = vec4(phong_ads_color,1.0);" +
	"}";
	
	fragmentShaderObject_PF = gl.createShader(gl.FRAGMENT_SHADER);
	gl.shaderSource(fragmentShaderObject_PF,fragmentShaderSourceCode_PF);
	gl.compileShader(fragmentShaderObject_PF);
	
	if(gl.getShaderParameter(fragmentShaderObject_PF,gl.COMPILE_STATUS)==false)
	{
		var error = gl.getShaderInfoLog(fragmentShaderObject_PF);
		if(error.length > 0)
		{
			alert("FRAGMENT SHADER EROOR: " + error);
			uninitialize();
		}
	}
	//shader program
	shaderProgramObject_PF = gl.createProgram();
	gl.attachShader(shaderProgramObject_PF, vertexShaderObject_PF);
	gl.attachShader(shaderProgramObject_PF, fragmentShaderObject_PF);
	
	//prelinking
	gl.bindAttribLocation(shaderProgramObject_PF, WebGLMacros.AMC_ATTRIBUTE_VERTEX, "vPosition");
	gl.bindAttribLocation(shaderProgramObject_PF, WebGLMacros.AMC_ATTRIBUTE_NORMAL, "vNormal");
	//linking
	gl.linkProgram(shaderProgramObject_PF);
	if(!gl.getProgramParameter(shaderProgramObject_PF, gl.LINK_STATUS))
	{
		var error = gl.getProgramInfoLog(shaderProgramObject_PF);
		if(error.length > 0)
		{
				alert("LINK ERROR : "+ error);
				uninitialize();
		}
	}
	
	//Post Link..getting uniforms locations
	modelViewMatrixUniform_PF = gl.getUniformLocation(shaderProgramObject_PF, "u_model_view_matrix");
	projectionMatrixUniform_PF = gl.getUniformLocation(shaderProgramObject_PF, "u_projection_matrix");

	lKeyPressedUniform_PF = gl.getUniformLocation(shaderProgramObject_PF, "u_lkeypressed");

	laUniform_PF = gl.getUniformLocation(shaderProgramObject_PF, "u_la");
	ldUniform_PF = gl.getUniformLocation(shaderProgramObject_PF, "u_ld");
	lsUniform_PF = gl.getUniformLocation(shaderProgramObject_PF, "u_ls");

	kaUniform_PF = gl.getUniformLocation(shaderProgramObject_PF,"u_ka");
	kdUniform_PF = gl.getUniformLocation(shaderProgramObject_PF,"u_kd");
	ksUniform_PF = gl.getUniformLocation(shaderProgramObject_PF,"u_ks");

	shininessUniform_PF = gl.getUniformLocation(shaderProgramObject_PF, "u_shininess");
	lightPositionUniform_PF = gl.getUniformLocation(shaderProgramObject_PF, "u_light_position");
	
	////////////******Vertices color texcoord, vao vbo*****///////////
	 
	makeSphere(mesh, 2.0, 100, 10);
	
	//END:
	
	gl.depthFunc(gl.LEQUAL);
	//Set Clear color
	gl.clearColor(0.0, 0.0, 0.0, 1.0); 
	gl.clearDepth(1.0);
	gl.enable(gl.DEPTH_TEST);
	gl.enable(gl.CULL_FACE);

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
	
	var modelViewMatrix = mat4.create();
	
	mat4.translate(modelViewMatrix, modelViewMatrix, [0.0, 0.0 , -7.0]);

	//mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);
	if( perVertex == 1)
	{
		gl.useProgram(shaderProgramObject_PV);

		if(bLkeyPressed == 1)
		{
			gl.uniform1i(lKeyPressedUniform_PV, 1);
			
			gl.uniform3fv(laUniform_PV, lightAmbient);
			gl.uniform3fv(ldUniform_PV, lightDifuse);
			gl.uniform3fv(lsUniform_PV, lightSpecular);
			
			gl.uniform3fv(kaUniform_PV, materialAmbient);
			gl.uniform3fv(kdUniform_PV, materialDifuse);
			gl.uniform3fv(ksUniform_PV, materialSpecular);

			gl.uniform1f(shininessUniform_PV, materialShininess);

			
			gl.uniform4fv(lightPositionUniform_PV, lightPosition);
		}
		else
		{
			gl.uniform1i(lKeyPressedUniform_PV, 0);
		}

		gl.uniformMatrix4fv(modelViewMatrixUniform_PV, false, modelViewMatrix);
		gl.uniformMatrix4fv(projectionMatrixUniform_PV, false, perspectiveProjectionMatrix);
	}
	else
	{
		gl.useProgram(shaderProgramObject_PF);
		if(bLkeyPressed == 1)
		{
			gl.uniform1i(lKeyPressedUniform_PF, 1);
			
			gl.uniform3fv(laUniform_PF, lightAmbient);
			gl.uniform3fv(ldUniform_PF, lightDifuse);
			gl.uniform3fv(lsUniform_PF, lightSpecular);
			
			gl.uniform3fv(kaUniform_PF, materialAmbient);
			gl.uniform3fv(kdUniform_PF, materialDifuse);
			gl.uniform3fv(ksUniform_PF, materialSpecular);

			gl.uniform1f(shininessUniform_PF, materialShininess);

			
			gl.uniform4fv(lightPositionUniform_PF, lightPosition);
		}
		else
		{
			gl.uniform1i(lKeyPressedUniform_PF, 0);
		}
		gl.uniformMatrix4fv(modelViewMatrixUniform_PF, false, modelViewMatrix);
		gl.uniformMatrix4fv(projectionMatrixUniform_PF, false, perspectiveProjectionMatrix);
	}

	//Draw required shapes
	//STRAT:
	//cube
	mesh.draw();
	
	//Animation loop
	requestAnimationFrame(draw, canvas);
}

function uninitialize()
{
	if(shaderProgramObject_PV)
	{
			if(fragmentShaderObject_PV)
			{
				gl.detachShader(shaderProgramObject_PV, fragmentShaderObject_PV);
				gl.deleteShader(fragmentShaderObject_PV);
				fragmentShaderObject_PV = null;

			}

			if(vertexShaderObject_PV)
			{
				gl.detachShader(shaderProgramObject_PV, vertexShaderObject_PV);
				gl.deleteShader(vertexShaderObject_PV);
				vertexShaderObject_PV = null;

			}
			gl.deleteProgram(shaderProgramObject_PV);
			shaderProgramObject_PV = null;
	}
}

function degToRad(angle)
{
	return(angle * Math.PI/180);
}
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
var materialAmbient = [ 0.0, 0.0, 0.0];
var materialDifuse = [ 1.0, 1.0, 1.0];
var materialSpecular= [ 1.0, 1.0, 1.0 ];

var lightPosition = [ ];

var materialShininess= 128 ;

var lKeyPressedUniform_PF ;

var bLkeyPressed = 0;
var perVertex = 0;
var perFragment = 0;
//
var angle_X = 0.0;
var iRotation = 0;
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

					case 88: //x
						iRotation = 0;
					break;

					case 89:
						iRotation = 1;
						break;

						case 90:
							iRotation = 2;
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
	
	//var modelViewMatrix = mat4.create();
	
	//mat4.translate(modelViewMatrix, modelViewMatrix, [0.0, 0.0 , -7.0]);

	//mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);
	

	//Draw required shapes
	//STRAT:
	//cube
	
	 var addX = canvas.width/4;
	 var addY = canvas.height/6;

	 var xPos = 0;
	 var yPos = canvas.height - addY;

	for(var i = 1; i<= 24; i++)
	{
		var modelViewMatrix = mat4.create();
	
		mat4.translate(modelViewMatrix, modelViewMatrix, [0.0, 0.0 , -7.0]);


		//light and materials
		initSphereMaterials(i);

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
		
		

		 gl.viewport(xPos, yPos, canvas.width/4, canvas.height/6);

		mesh.draw();

		if(i % 6 == 0) 
		{
			xPos = xPos + addX;
			var yPos = canvas.height - addY;
		}
		else
		{
			yPos = yPos - addY;
		}
	}
	//Animation loop
	requestAnimationFrame(draw, canvas);
	if (angle_X >= 360.0)
	{
		angle_X = 0.0;
	}
	else
	{
		angle_X += 0.5;
	}
	if (iRotation == 0)
	{
		lightPosition[0] = 0.0;
		lightPosition[1] = 100.0* Math.sin(degToRad(angle_X));
		lightPosition[2] = 100.0 * Math.cos(degToRad(angle_X));
		lightPosition[3] = 1.0; 
	}
	else if (iRotation == 1)
	{
		lightPosition[0] = 100.0* Math.sin(degToRad(angle_X));
		lightPosition[1] = 0.0;
		lightPosition[2] = 100.0 * Math.cos(degToRad(angle_X));
		lightPosition[3] = 1.0;
	}
	else
	{
		lightPosition[0] = 100.0* Math.sin(degToRad(angle_X));
		lightPosition[1] = 100.0 * Math.cos(degToRad(angle_X));
		lightPosition[2] = 0.0;
		lightPosition[3] = 1.0;
	}
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

function initSphereMaterials(index)
{
	if(index == 1)
	{
		materialAmbient[0] = parseFloat(0.0215).toFixed(4);
		materialAmbient[1] = parseFloat(0.1745).toFixed(4);
		materialAmbient[2] = parseFloat(0.0215).toFixed(4);
		////materialAmbient[3] = parseFloat(1.0).toFixed(4);
		
		materialDifuse[0] = parseFloat(0.07568).toFixed(4);
		materialDifuse[1] = parseFloat(0.61424).toFixed(4);
		materialDifuse[2] = parseFloat(0.07568).toFixed(4);
		//materialDifuse[3] = parseFloat(1.0).toFixed(4);
		
		materialSpecular[0] = parseFloat(0.633).toFixed(4);
		materialSpecular[1] = parseFloat(0.727811).toFixed(4);
		materialSpecular[2] = parseFloat(0.633).toFixed(4);
		//materialSpecular[3] = parseFloat(1.0).toFixed(4);
						
		materialShininess = parseFloat(0.6*128.0).toFixed(4);
	}
	else if(index == 2)
	{
		materialAmbient[0] = parseFloat(0.135).toFixed(4);
		materialAmbient[1] = parseFloat(0.2225).toFixed(4);
		materialAmbient[2] = parseFloat(0.1575).toFixed(4);
		//materialAmbient[3] = parseFloat(1.0).toFixed(4);
		
		materialDifuse[0] = parseFloat(0.54).toFixed(4);
		materialDifuse[1] = parseFloat(0.89).toFixed(4);
		materialDifuse[2] = parseFloat(0.63).toFixed(4);
		//materialDifuse[3] = parseFloat(1.0).toFixed(4);
		
		materialSpecular[0] = parseFloat(0.316228).toFixed(4);
		materialSpecular[1] = parseFloat(0.316228).toFixed(4);
		materialSpecular[2] = parseFloat(0.316228).toFixed(4);
		//materialSpecular[3] = parseFloat(1.0).toFixed(4);
		
		materialShininess = parseFloat(0.1 *128.0).toFixed(4);
	}
	else if(index == 3)
	{
		materialAmbient[0] = parseFloat(0.05375).toFixed(4);
		materialAmbient[1] = parseFloat(0.05).toFixed(4);
		materialAmbient[2] = parseFloat(0.06625).toFixed(4);
		//materialAmbient[3] = parseFloat(1.0).toFixed(4);
		
		materialDifuse[0] = parseFloat(0.18275).toFixed(4);
		materialDifuse[1] = parseFloat(0.17).toFixed(4);
		materialDifuse[2] = parseFloat(0.22525).toFixed(4);
		//materialDifuse[3] = parseFloat(1.0).toFixed(4);
		
		materialSpecular[0] = parseFloat(0.332741).toFixed(4);
		materialSpecular[1] = parseFloat(0.328634).toFixed(4);
		materialSpecular[2] = parseFloat(0.346435).toFixed(4);
		//materialSpecular[3] = parseFloat(1.0).toFixed(4);
		
		materialShininess = parseFloat(0.3*128.0).toFixed(4);
	}
	else if(index == 4)
	{
		materialAmbient[0] = parseFloat(0.25).toFixed(4);
	materialAmbient[1] = parseFloat(0.20725).toFixed(4);
	materialAmbient[2] = parseFloat(0.20725).toFixed(4);
	//materialAmbient[3] = parseFloat(1.0).toFixed(4);
	
	materialDifuse[0] = parseFloat(1.0).toFixed(4);
	materialDifuse[1] = parseFloat(0.829).toFixed(4);
	materialDifuse[2] = parseFloat(0.829).toFixed(4);
	//materialDifuse[3] = parseFloat(1.0).toFixed(4);
	
	materialSpecular[0] = parseFloat(0.296648).toFixed(4);
	materialSpecular[1] = parseFloat(0.296648).toFixed(4);
	materialSpecular[2] = parseFloat(0.296648).toFixed(4);
	//materialSpecular[3] = parseFloat(1.0).toFixed(4);
	
	materialShininess = parseFloat(0.88*128.0).toFixed(4);
	}
	else if(index == 5)
	{
		materialAmbient[0] = parseFloat(0.1745).toFixed(4);
		materialAmbient[1] = parseFloat(0.01175).toFixed(4);
		materialAmbient[2] = parseFloat(0.01175).toFixed(4);
		//materialAmbient[3] = parseFloat(1.0).toFixed(4);
		
		materialDifuse[0] = parseFloat(0.61424).toFixed(4);
		materialDifuse[1] = parseFloat(0.04136).toFixed(4);
		materialDifuse[2] = parseFloat(0.04136).toFixed(4);
		//materialDifuse[3] = parseFloat(1.0).toFixed(4);
		
		materialSpecular[0] = parseFloat(0.727811).toFixed(4);
		materialSpecular[1] = parseFloat(0.626959).toFixed(4);
		materialSpecular[2] = parseFloat(0.626959).toFixed(4);
		//materialSpecular[3] = parseFloat(1.0).toFixed(4);
		
		materialShininess = parseFloat(0.6 * 128.0).toFixed(4);
	}
	else if(index == 6)
	{
		materialAmbient[0] = parseFloat(0.1).toFixed(4);
	materialAmbient[1] = parseFloat(0.18725).toFixed(4);
	materialAmbient[2] = parseFloat(0.1745).toFixed(4);
	//materialAmbient[3] = parseFloat(1.0).toFixed(4);
	
	materialDifuse[0] = parseFloat(0.396).toFixed(4);
	materialDifuse[1] = parseFloat(0.74151).toFixed(4);
	materialDifuse[2] = parseFloat(0.69102).toFixed(4);
	//materialDifuse[3] = parseFloat(1.0).toFixed(4);
	
	materialSpecular[0] = parseFloat(0.297254).toFixed(4);
	materialSpecular[1] = parseFloat(0.30829).toFixed(4);
	materialSpecular[2] = parseFloat(0.30829).toFixed(4);
	//materialSpecular[3] = parseFloat(1.0).toFixed(4);
	
	materialShininess = parseFloat(0.1*128.0).toFixed(4);
	}
	else if(index == 7)
	{
		materialAmbient[0] = parseFloat(0.329412).toFixed(4);
		materialAmbient[1] = parseFloat(0.223529).toFixed(4);
		materialAmbient[2] = parseFloat(0.027451).toFixed(4);
		//materialAmbient[3] = parseFloat(1.0).toFixed(4);

		materialDifuse[0] = parseFloat(0.780392).toFixed(4);
		materialDifuse[1] = parseFloat(0.568627).toFixed(4);
		materialDifuse[2] = parseFloat(0.113725).toFixed(4);
		//materialDifuse[3] = parseFloat(1.0).toFixed(4);

		materialSpecular[0] = parseFloat(0.992157).toFixed(4);
		materialSpecular[1] = parseFloat(0.941176).toFixed(4);
		materialSpecular[2] = parseFloat(0.807843).toFixed(4);
		//materialSpecular[3] = parseFloat(1.0).toFixed(4);

		materialShininess = parseFloat(0.21794872*128.0).toFixed(4);
	}
	else if(index == 8)
	{
		materialAmbient[0] = parseFloat(0.2125).toFixed(4);
		materialAmbient[1] = parseFloat(0.1275).toFixed(4);
		materialAmbient[2] = parseFloat(0.054).toFixed(4);
		//materialAmbient[3] = parseFloat(1.0).toFixed(4);
	
		materialDifuse[0] = parseFloat(0.714).toFixed(4);
		materialDifuse[1] = parseFloat(0.4284).toFixed(4);
		materialDifuse[2] = parseFloat(0.18144).toFixed(4);
		//materialDifuse[3] = parseFloat(1.0).toFixed(4);
	
		materialSpecular[0] = parseFloat(0.393548).toFixed(4);
		materialSpecular[1] = parseFloat(0.271906).toFixed(4);
		materialSpecular[2] = parseFloat(0.166721).toFixed(4);
		//materialSpecular[3] = parseFloat(1.0).toFixed(4);
	
		materialShininess = parseFloat(0.2*128.0).toFixed(4);
	}
	else if(index == 9)
	{
		materialAmbient[0] = parseFloat(0.25).toFixed(4);
		materialAmbient[1] = parseFloat(0.25).toFixed(4);
		materialAmbient[2] = parseFloat(0.25).toFixed(4);
		//materialAmbient[3] = parseFloat(1.0).toFixed(4);
	
		materialDifuse[0] = parseFloat(0.4).toFixed(4);
		materialDifuse[1] = parseFloat(0.4).toFixed(4);
		materialDifuse[2] = parseFloat(0.4).toFixed(4);
		//materialDifuse[3] = parseFloat(1.0).toFixed(4);
	
		materialSpecular[0] = parseFloat(0.774597).toFixed(4);
		materialSpecular[1] = parseFloat(0.774597).toFixed(4);
		materialSpecular[2] = parseFloat(0.774597).toFixed(4);
		//materialSpecular[3] = parseFloat(1.0).toFixed(4);
	
		materialShininess = parseFloat(0.6*128.0).toFixed(4);
	}
	else if(index == 10)
	{
		materialAmbient[0] = parseFloat(0.19125).toFixed(4);
		materialAmbient[1] = parseFloat(0.0735).toFixed(4);
		materialAmbient[2] = parseFloat(0.0225).toFixed(4);
		//materialAmbient[3] = parseFloat(1.0).toFixed(4);
	
		materialDifuse[0] = parseFloat(0.7038).toFixed(4);
		materialDifuse[1] = parseFloat(0.27048).toFixed(4);
		materialDifuse[2] = parseFloat(0.0828).toFixed(4);
		//materialDifuse[3] = parseFloat(1.0).toFixed(4);
	
		materialSpecular[0] = parseFloat(0.256777).toFixed(4);
		materialSpecular[1] = parseFloat(0.137622).toFixed(4);
		materialSpecular[2] = parseFloat(0.086014).toFixed(4);
		//materialSpecular[3] = parseFloat(1.0).toFixed(4);
	
		materialShininess = parseFloat(0.1*128.0).toFixed(4);
	}
	else if(index == 11)
	{
		materialAmbient[0] = parseFloat(0.24725).toFixed(4);
		materialAmbient[1] = parseFloat(0.1995).toFixed(4);
		materialAmbient[2] = parseFloat(0.0745).toFixed(4);
		//materialAmbient[3] = parseFloat(1.0).toFixed(4);
	
		materialDifuse[0] = parseFloat(0.75164).toFixed(4);
		materialDifuse[1] = parseFloat(0.60648).toFixed(4);
		materialDifuse[2] = parseFloat(0.22648).toFixed(4);
		//materialDifuse[3] = parseFloat(1.0).toFixed(4);
	
		materialSpecular[0] = parseFloat(0.628281).toFixed(4);
		materialSpecular[1] = parseFloat(0.555802).toFixed(4);
		materialSpecular[2] = parseFloat(0.366065).toFixed(4);
		//materialSpecular[3] = parseFloat(1.0).toFixed(4);
	
		materialShininess = parseFloat(0.4*128.0).toFixed(4);
	}
	else if(index == 12)
	{
		materialAmbient[0] = parseFloat(0.19225).toFixed(4);
		materialAmbient[1] = parseFloat(0.19225).toFixed(4);
		materialAmbient[2] = parseFloat(0.19225).toFixed(4);
		//materialAmbient[3] = parseFloat(1.0).toFixed(4);

		materialDifuse[0] = parseFloat(0.50754).toFixed(4);
		materialDifuse[1] = parseFloat(0.50754).toFixed(4);
		materialDifuse[2] = parseFloat(0.50754).toFixed(4);
		//materialDifuse[3] = parseFloat(1.0).toFixed(4);

		materialSpecular[0] = parseFloat(0.508273).toFixed(4);
		materialSpecular[1] = parseFloat(0.508273).toFixed(4);
		materialSpecular[2] = parseFloat(0.508273).toFixed(4);
		//materialSpecular[3] = parseFloat(1.0).toFixed(4);

		materialShininess = parseFloat(0.4*128.0).toFixed(4);
	}
	else if(index == 13)
	{
		materialAmbient[0] = parseFloat(0.0).toFixed(4);
		materialAmbient[1] = parseFloat(0.0).toFixed(4);
		materialAmbient[2] = parseFloat(0.0).toFixed(4);
		//materialAmbient[3] = parseFloat(1.0).toFixed(4);
	
		materialDifuse[0] = parseFloat(0.01).toFixed(4);
		materialDifuse[1] = parseFloat(0.01).toFixed(4);
		materialDifuse[2] = parseFloat(0.01).toFixed(4);
		//materialDifuse[3] = parseFloat(1.0).toFixed(4);
	
		materialSpecular[0] = parseFloat(0.50).toFixed(4);
		materialSpecular[1] = parseFloat(0.50).toFixed(4);
		materialSpecular[2] = parseFloat(0.50).toFixed(4);
		//materialSpecular[3] = parseFloat(1.0).toFixed(4);
	
		materialShininess = parseFloat(0.25*128.0).toFixed(4);
	}
	else if(index == 14)
	{
		materialAmbient[0] = parseFloat(0.0).toFixed(4);
		materialAmbient[1] = parseFloat(0.1).toFixed(4);
		materialAmbient[2] = parseFloat(0.06).toFixed(4);
		//materialAmbient[3] = parseFloat(1.0).toFixed(4);
	
		materialDifuse[0] = parseFloat(0.0).toFixed(4);
		materialDifuse[1] = parseFloat(0.50980392).toFixed(4);
		materialDifuse[2] = parseFloat(0.50980392).toFixed(4);
		//materialDifuse[3] = parseFloat(1.0).toFixed(4);
	
		materialSpecular[0] = parseFloat(0.50196078).toFixed(4);
		materialSpecular[1] = parseFloat(0.50196078).toFixed(4);
		materialSpecular[2] = parseFloat(0.50196078).toFixed(4);
		//materialSpecular[3] = parseFloat(1.0).toFixed(4);
	
		materialShininess = parseFloat(0.25*128.0).toFixed(4);
	}
	else if(index == 15)
	{
		materialAmbient[0] = parseFloat(0.0).toFixed(4);
		materialAmbient[1] = parseFloat(0.0).toFixed(4);
		materialAmbient[2] = parseFloat(0.0).toFixed(4);
		//materialAmbient[3] = parseFloat(1.0).toFixed(4);
	
		materialDifuse[0] = parseFloat(0.1).toFixed(4);
		materialDifuse[1] = parseFloat(0.35).toFixed(4);
		materialDifuse[2] = parseFloat(0.1).toFixed(4);
		//materialDifuse[3] = parseFloat(1.0).toFixed(4);
	
		materialSpecular[0] = parseFloat(0.45).toFixed(4);
		materialSpecular[1] = parseFloat(0.55).toFixed(4);
		materialSpecular[2] = parseFloat(0.45).toFixed(4);
		//materialSpecular[3] = parseFloat(1.0).toFixed(4);
	
		materialShininess = parseFloat(0.25*128.0).toFixed(4);
	}
	else if(index == 16)
	{
		materialAmbient[0] = parseFloat(0.0).toFixed(4);
		materialAmbient[1] = parseFloat(0.0).toFixed(4);
		materialAmbient[2] = parseFloat(0.0).toFixed(4);
		//materialAmbient[3] = parseFloat(1.0).toFixed(4);
	
		materialDifuse[0] = parseFloat(0.5).toFixed(4);
		materialDifuse[1] = parseFloat(0.0).toFixed(4);
		materialDifuse[2] = parseFloat(0.0).toFixed(4);
		//materialDifuse[3] = parseFloat(1.0).toFixed(4);
	
		materialSpecular[0] = parseFloat(0.7).toFixed(4);
		materialSpecular[1] = parseFloat(0.6).toFixed(4);
		materialSpecular[2] = parseFloat(0.6).toFixed(4);
		//materialSpecular[3] = parseFloat(1.0).toFixed(4);
	
		materialShininess = parseFloat(0.25*128.0).toFixed(4);
	}
	else if(index == 17)
	{
		materialAmbient[0] = parseFloat(0.0).toFixed(4);
		materialAmbient[1] = parseFloat(0.0).toFixed(4);
		materialAmbient[2] = parseFloat(0.0).toFixed(4);
		//materialAmbient[3] = parseFloat(1.0).toFixed(4);
	
		materialDifuse[0] = parseFloat(0.55).toFixed(4);
		materialDifuse[1] = parseFloat(0.55).toFixed(4);
		materialDifuse[2] = parseFloat(0.55).toFixed(4);
		//materialDifuse[3] = parseFloat(1.0).toFixed(4);
	
		materialSpecular[0] = parseFloat(0.70).toFixed(4);
		materialSpecular[1] = parseFloat(0.70).toFixed(4);
		materialSpecular[2] = parseFloat(0.70).toFixed(4);
		//materialSpecular[3] = parseFloat(1.0).toFixed(4);
	
		materialShininess = parseFloat(0.25*128.0).toFixed(4);
	}
	else if(index == 18)
	{
		materialAmbient[0] = parseFloat(0.0).toFixed(4);
		materialAmbient[1] = parseFloat(0.0).toFixed(4);
		materialAmbient[2] = parseFloat(0.0).toFixed(4);
		//materialAmbient[3] = parseFloat(1.0).toFixed(4);
	
		materialDifuse[0] = parseFloat(0.5).toFixed(4);
		materialDifuse[1] = parseFloat(0.5).toFixed(4);
		materialDifuse[2] = parseFloat(0.0).toFixed(4);
		//materialDifuse[3] = parseFloat(1.0).toFixed(4);
	
		materialSpecular[0] = parseFloat(0.60).toFixed(4);
		materialSpecular[1] = parseFloat(0.60).toFixed(4);
		materialSpecular[2] = parseFloat(0.50).toFixed(4);
		//materialSpecular[3] = parseFloat(1.0).toFixed(4);
	
		materialShininess = parseFloat(0.25*128.0).toFixed(4);
	}
	else if(index == 19)
	{
		materialAmbient[0] = parseFloat(0.02).toFixed(4);
		materialAmbient[1] = parseFloat(0.02).toFixed(4);
		materialAmbient[2] = parseFloat(0.02).toFixed(4);
		//materialAmbient[3] = parseFloat(1.0).toFixed(4);
	
		materialDifuse[0] = parseFloat(0.01).toFixed(4);
		materialDifuse[1] = parseFloat(0.01).toFixed(4);
		materialDifuse[2] = parseFloat(0.01).toFixed(4);
		//materialDifuse[3] = parseFloat(1.0).toFixed(4);
	
		materialSpecular[0] = parseFloat(0.4).toFixed(4);
		materialSpecular[1] = parseFloat(0.4).toFixed(4);
		materialSpecular[2] = parseFloat(0.4).toFixed(4);
		//materialSpecular[3] = parseFloat(1.0).toFixed(4);
	
		materialShininess = parseFloat(0.078125*128.0).toFixed(4);
	}
	else if(index == 20)
	{
		materialAmbient[0] = parseFloat(0.0).toFixed(4);
		materialAmbient[1] = parseFloat(0.05).toFixed(4);
		materialAmbient[2] = parseFloat(0.05).toFixed(4);
		//materialAmbient[3] = parseFloat(1.0).toFixed(4);
	
		materialDifuse[0] = parseFloat(0.4).toFixed(4);
		materialDifuse[1] = parseFloat(0.5).toFixed(4);
		materialDifuse[2] = parseFloat(0.5).toFixed(4);
		//materialDifuse[3] = parseFloat(1.0).toFixed(4);
	
		materialSpecular[0] = parseFloat(0.04).toFixed(4);
		materialSpecular[1] = parseFloat(0.7).toFixed(4);
		materialSpecular[2] = parseFloat(0.7).toFixed(4);
		//materialSpecular[3] = parseFloat(1.0).toFixed(4);
	
		materialShininess = parseFloat(0.078125*128.0).toFixed(4);
	}
	else if(index == 21)
	{
		materialAmbient[0] = parseFloat(0.0).toFixed(4);
		materialAmbient[1] = parseFloat(0.05).toFixed(4);
		materialAmbient[2] = parseFloat(0.0).toFixed(4);
		//materialAmbient[3] = parseFloat(1.0).toFixed(4);
	
		materialDifuse[0] = parseFloat(0.4).toFixed(4);
		materialDifuse[1] = parseFloat(0.5).toFixed(4);
		materialDifuse[2] = parseFloat(0.4).toFixed(4);
		//materialDifuse[3] = parseFloat(1.0).toFixed(4);
	
		materialSpecular[0] = parseFloat(0.04).toFixed(4);
		materialSpecular[1] = parseFloat(0.7).toFixed(4);
		materialSpecular[2] = parseFloat(0.04).toFixed(4);
		//materialSpecular[3] = parseFloat(1.0).toFixed(4);
	
		materialShininess = parseFloat(0.078125*128.0).toFixed(4);
	}
	else if(index == 22)
	{
		materialAmbient[0] = parseFloat(0.05).toFixed(4);
		materialAmbient[1] = parseFloat(0.0).toFixed(4);
		materialAmbient[2] = parseFloat(0.0).toFixed(4);
		//materialAmbient[3] = parseFloat(1.0).toFixed(4);
	
		materialDifuse[0] = parseFloat(0.5).toFixed(4);
		materialDifuse[1] = parseFloat(0.4).toFixed(4);
		materialDifuse[2] = parseFloat(0.4).toFixed(4);
		//materialDifuse[3] = parseFloat(1.0).toFixed(4);
	
		materialSpecular[0] = parseFloat(0.7).toFixed(4);
		materialSpecular[1] = parseFloat(0.04).toFixed(4);
		materialSpecular[2] = parseFloat(0.04).toFixed(4);
		//materialSpecular[3] = parseFloat(1.0).toFixed(4);
	
		materialShininess = parseFloat(0.078125*128.0).toFixed(4);
	}
	else if(index == 23)
	{
		materialAmbient[0] = parseFloat(0.5).toFixed(4);
		materialAmbient[1] = parseFloat(0.5).toFixed(4);
		materialAmbient[2] = parseFloat(0.5).toFixed(4);
		//materialAmbient[3] = parseFloat(1.0).toFixed(4);

		materialDifuse[0] = parseFloat(0.5).toFixed(4);
		materialDifuse[1] = parseFloat(0.5).toFixed(4);
		materialDifuse[2] = parseFloat(0.5).toFixed(4);
		//materialDifuse[3] = parseFloat(1.0).toFixed(4);

		materialSpecular[0] = parseFloat(0.7).toFixed(4);
		materialSpecular[1] = parseFloat(0.7).toFixed(4);
		materialSpecular[2] = parseFloat(0.7).toFixed(4);
		//materialSpecular[3] = parseFloat(1.0).toFixed(4);

		materialShininess = parseFloat(0.078125*128.0).toFixed(4);
	}
	else if(index == 24)
	{
		materialAmbient[0] = parseFloat(0.05).toFixed(4);
		materialAmbient[1] = parseFloat(0.05).toFixed(4);
		materialAmbient[2] = parseFloat(0.0).toFixed(4);
		//materialAmbient[3] = parseFloat(1.0).toFixed(4);
	
		materialDifuse[0] = parseFloat(0.5).toFixed(4);
		materialDifuse[1] = parseFloat(0.5).toFixed(4);
		materialDifuse[2] = parseFloat(0.4).toFixed(4);
		//materialDifuse[3] = parseFloat(1.0).toFixed(4);
	
		materialSpecular[0] = parseFloat(0.7).toFixed(4);
		materialSpecular[1] = parseFloat(0.7).toFixed(4);
		materialSpecular[2] = parseFloat(0.7).toFixed(4);
		//materialSpecular[3] = parseFloat(1.0).toFixed(4);
	
		materialShininess = parseFloat(0.078125*128.0).toFixed(4);
	}

}

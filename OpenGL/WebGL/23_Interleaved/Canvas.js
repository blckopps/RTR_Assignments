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
var vbo_texture_cube;

var vao_pyramid;
var vbo_pyramid;
var vbo_texture_pyramid

var mpUniform;
var vpUniform;
var pUniform;

var perspectiveProjectionMatrix;

var uniform_texture0_sampler ;

var cubeTexture = 0;
var pyramidTexture = 0;
//
var angle = 0.0;

//
var isLKeyIsPressedUniforms;

var laUniform;
var ldUniform;
var lsUniform;

var kaUniform;
var kdUniform;
var ksUniform;

var shininessUniform;

var lightPositionUniform;
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
	
var cubeVCNT = new Float32Array
([
	//vert							//color				//norms				//texccords
	1.0,1.0,-1.0,      1.0, 0.0, 0.0,           0.0, 1.0, 0.0,    0.0, 1.0,
	-1.0,1.0,-1.0,     1.0, 0.0, 0.0,           0.0, 1.0, 0.0,    0.0, 0.0,
	-1.0,1.0,1.0,	  1.0, 0.0, 0.0,          0.0, 1.0, 0.0,	   1.0, 0.0,
	1.0,1.0,1.0,       1.0, 0.0, 0.0,          0.0, 1.0, 0.0,     1.0, 1.0,
	
	//bottom
	1.0,-1.0,-1.0,      0.0, 1.0, 0.0,        0.0, -1.0, 0.0,      1.0, 1.0,
	-1.0,-1.0,-1.0,     0.0, 1.0, 0.0,        0.0, -1.0, 0.0,      0.0, 1.0,
	-1.0,-1.0,1.0,      0.0, 1.0, 0.0,        0.0, -1.0, 0.0,      0.0, 0.0,
	1.0, -1.0, 1.0,     0.0, 1.0, 0.0,        0.0, -1.0, 0.0,      1.0, 0.0,
	
	//ront
	 1.0,1.0,1.0,	    0.0, 0.0, 1.0,       0.0, 0.0, 1.0,       0.0, 0.0,
	 -1.0,1.0,1.0,	    0.0, 0.0, 1.0,       0.0, 0.0, 1.0,       1.0, 0.0,
	 -1.0,-1.0,1.0,	    0.0, 0.0, 1.0,       0.0, 0.0, 1.0,       1.0, 1.0,
	 1.0,-1.0,1.0,       0.0, 0.0, 1.0,       0.0, 0.0, 1.0,       0.0, 1.0,
	
	 //back
	  1.0,1.0,-1.0,	    	1.0, 1.0, 0.0,       0.0, 0.0,-1.0,       1.0, 0.0,
	  -1.0,1.0,-1.0,	    1.0, 1.0, 0.0,       0.0, 0.0,-1.0,       1.0, 1.0,
	  -1.0,-1.0,-1.0,	    1.0, 1.0, 0.0,       0.0, 0.0,-1.0,       0.0, 1.0,
	  1.0,-1.0,-1.0,     	 1.0, 1.0, 0.0,       0.0, 0.0,-1.0,       0.0, 0.0,
	
	  //Right
	   1.0,1.0,-1.0,	     1.0, 0.0, 1.0,       1.0, 0.0, 0.0,      1.0, 0.0,
	   1.0,1.0,1.0,	     	1.0, 0.0, 1.0,       1.0, 0.0, 0.0,      1.0, 1.0,
	   1.0,-1.0,1.0,	     1.0, 0.0, 1.0,       1.0, 0.0, 0.0,      0.0, 1.0,
	   1.0,-1.0,-1.0,       1.0, 0.0, 1.0,       1.0, 0.0, 0.0,      0.0, 0.0,
	
	   //left
		-1.0,1.0,1.0,	     0.0, 1.0, 1.0,        -1.0, 0.0, 0.0,    0.0, 0.0,
		-1.0,1.0,-1.0,	     0.0, 1.0, 1.0,        -1.0, 0.0, 0.0,    1.0, 0.0,
		-1.0,-1.0,-1.0,	     0.0, 1.0, 1.0,        -1.0, 0.0, 0.0,    1.0, 1.0,
		-1.0,-1.0,1.0,       0.0, 1.0, 1.0,        -1.0, 0.0, 0.0,    0.0, 1.0
]);

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
		"in vec3 vNormal;" +
		"in vec2 vTexcoord;" +
		"uniform mat4 u_model_matrix;" +
		"uniform mat4 u_view_matrix;" +
		"uniform mat4 u_projection_matrix;" +
		"uniform float islkeypressed;" +
		"uniform vec4 u_light_position;" +
		"out vec3 tnorm;" +
		"out vec4 out_color;" +
		"out vec2 out_texcoord;" +
		"out vec3 light_direction;" +
		"out vec3 viewer_vector;" +
		"void main(void)" +
		"{" +
			"if(islkeypressed == 1.0)" +
			"{" +
				"vec4 eye_coordinates = u_view_matrix *  u_model_matrix  * vPosition;" +
				"tnorm = mat3(u_view_matrix * u_model_matrix) * vNormal;" +
				"light_direction = vec3(u_light_position - eye_coordinates);" +
				"viewer_vector = vec3(-eye_coordinates.xyz);" +
			"}" +
				"gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" +
				"out_color = vColor;" +
				"out_texcoord = vTexcoord;" +
		" } ";
	
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
		"out vec4 fragColor;" +
		"uniform sampler2D u_Sampler;" +
		"uniform float  islkeypressed;" +
		"uniform vec3 u_la;" +
		"uniform vec3 u_ld;" +
		"uniform vec3 u_ls;" +
		"uniform vec3 u_ka;" +
		"uniform vec3 u_kd;" +
		"uniform vec3 u_ks;" +
		"in vec3 tnorm;" +
		"in vec4 out_color;" +
		"in vec2 out_texcoord;" +
		"in vec3 light_direction;" +
		"in vec3 viewer_vector;" +
		"uniform float u_shininess;" +
		"void main(void)" +
		"{" +
			"if(islkeypressed == 1.0)" +
			"{" +
				"vec3 tnorm_normalized = normalize(tnorm);" +
				"vec3 light_direction_normalized = normalize(light_direction);" +
				"vec3 viewer_vector_normalized = normalize(viewer_vector);" +
				"float tn_dot_ldirection = max(dot(light_direction_normalized, tnorm_normalized), 0.0);" +
				"vec3 reflection_vector = reflect(-light_direction_normalized, tnorm_normalized);" +
				"vec3 ambient = u_la * u_ka;" +
				"vec3 difuse = u_ld * u_kd * tn_dot_ldirection;" +
				"vec3 specular = u_ls * u_ks * pow(max(dot(reflection_vector,viewer_vector_normalized),0.0),u_shininess);" +
				"vec3 phong_light_pf = ambient + difuse + specular;" +
				"fragColor = vec4( texture(u_Sampler, out_texcoord) *  out_color * vec4(phong_light_pf,1.0));" +
				
			"}" +
			"else" +
			"{" +
				"fragColor = vec4( texture(u_Sampler, out_texcoord) *(out_color) );" +
				//"fragColor = out_color;" 
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
	gl.bindAttribLocation(shaderProgramObject, WebGLMacros.AMC_ATTRIBUTE_COLOR, "vColor");
	gl.bindAttribLocation(shaderProgramObject, WebGLMacros.AMC_ATTRIBUTE_NORMAL, "vNormal");
	gl.bindAttribLocation(shaderProgramObject, WebGLMacros.AMC_ATTRIBUTE_TEXTURE0, "vTexture0_Coord");
	
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
	pUniform = gl.getUniformLocation(shaderProgramObject, "u_projection_matrix");

	uniform_texture0_sampler = gl.getUniformLocation(shaderProgramObject, "u_texture0_sampler");

	isLKeyIsPressedUniforms = gl.getUniformLocation(shaderProgramObject,"islkeypressed");

	laUniform = gl.getUniformLocation(shaderProgramObject,"u_la");
	ldUniform = gl.getUniformLocation(shaderProgramObject,"u_ld");
	lsUniform = gl.getUniformLocation(shaderProgramObject,"u_ls");
	 
	kaUniform = gl.getUniformLocation(shaderProgramObject,"u_ka");
	kdUniform = gl.getUniformLocation(shaderProgramObject,"u_ks");
	ksUniform = gl.getUniformLocation(shaderProgramObject,"u_ks");
	
	shininessUniform = gl.getUniformLocation(shaderProgramObject,"u_shininess");
	
	lightPositionUniform = gl.getUniformLocation(shaderProgramObject,"u_light_position");

	////////////******Vertices color texcoord, vao vbo*****///////////
	


	//cube
	//STRAT:											
	vao_cube = gl.createVertexArray();
	gl.bindVertexArray(vao_cube);

	//position
	vbo_cube = gl.createBuffer();

	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_cube);
	gl.bufferData(gl.ARRAY_BUFFER, cubeVCNT, gl.STATIC_DRAW);

	//vertex
	gl.vertexAttribPointer(WebGLMacros.AMC_ATTRIBUTE_VERTEX, 3, gl.FLOAT, false , 11 *4, 0);
	gl.enableVertexAttribArray(WebGLMacros.AMC_ATTRIBUTE_VERTEX);
	
	//color
	gl.vertexAttribPointer(WebGLMacros.AMC_ATTRIBUTE_COLOR, 3, gl.FLOAT, false , 11 * 4,3 * 4);
	gl.enableVertexAttribArray(WebGLMacros.AMC_ATTRIBUTE_COLOR);

	//normals
	gl.vertexAttribPointer(WebGLMacros.AMC_ATTRIBUTE_NORMAL, 3, gl.FLOAT, false ,11 * 4, 6 * 4);
	gl.enableVertexAttribArray(WebGLMacros.AMC_ATTRIBUTE_NORMAL);
	//texture
	
	gl.vertexAttribPointer(WebGLMacros.AMC_ATTRIBUTE_TEXTURE0, 2, gl.FLOAT, false , 11 * 4, 9 * 4);
	gl.enableVertexAttribArray(WebGLMacros.AMC_ATTRIBUTE_TEXTURE0);

	gl.bindBuffer(gl.ARRAY_BUFFER, null);

	gl.bindVertexArray(null);
	//END:
	

	//Gen Texture for cube
	//START:
	cubeTexture = gl.createTexture();
	cubeTexture.image = new Image();
	cubeTexture.image.src = "marble.png";
	cubeTexture.image.onload = function()
	{
		gl.bindTexture(gl.TEXTURE_2D, cubeTexture);
		//gl.PixelStorei(gl.UNPACK_ALIGNMENT, 4);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, cubeTexture.image);
		gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
		gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST); //if NEAREST use ..no need to use mipmap
		gl.bindTexture(gl.TEXTURE_2D, null);
	}
	//END
	
	//END

	
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

	var modelMatrix = mat4.create();
	var ViewMatrix = mat4.create();

	//var modelViewProjectionMatrix = mat4.create();
	
	
	mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0 , -5.0]);

	mat4.rotateX(modelMatrix, modelMatrix, degToRad(angle));
	mat4.rotateY(modelMatrix, modelMatrix, degToRad(angle));
	mat4.rotateZ(modelMatrix, modelMatrix, degToRad(angle));
	
	//mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelMatrix);

	gl.uniformMatrix4fv(mUniform, false, modelMatrix);
	gl.uniformMatrix4fv(vUniform, false, ViewMatrix);
	gl.uniformMatrix4fv(pUniform, false, perspectiveProjectionMatrix);

	gl.uniform1i(isLKeyIsPressedUniforms, 1);

	gl.bindTexture(gl.TEXTURE_2D, cubeTexture);
	gl.uniform1i(uniform_texture0_sampler, 0);
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

	gl.bindTexture(gl.TEXTURE_2D, null);
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
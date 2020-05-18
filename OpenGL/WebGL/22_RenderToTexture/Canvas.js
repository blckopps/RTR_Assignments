//global variables
var canvas = null;
var gl = null;			// webgl context
var bFullscreen = false;
var canvas_original_width;
var canvas_original_height;

const WebGLMacros = //when whole 'WebGLMacros' is const all inside it are automaticaly const
{
	AMC_ATTRIBUTE_VERTEX:0,
	AMC_ATTRIBUTE_COLOR:1,
	AMC_ATTRIBUTE_NORMAL:2,
	AMC_ATTRIBUTE_TEXCOORD0:3,
}

var vertexShaderObject;
var fragmentShaderObject;
var shaderProgramObject;

var vao_cube;
var vbo_cube_vertices;
var vbo_cube_texcoord;

//framebuffer variables
var fbo;
var fbo_color_texture;
var fbo_depth_texture;

var gWidth;
var gHeight;

var samplerUniform;
var mvpUniform;

var kundaliTexture =0;

var perspectiveProjectionMatrix;

var cubeAngle = 0.0;

// To start animation : To have requestAnimationFrame() to be called "cross-browser" compatible
var requestAnimationFrame = 
window.requestAnimationFrame ||
window.webkitRequestAnimationFrame ||
window.mozRequestAnimationFrame ||
window.oRequestAnimationFrame ||
window.msRequestAnimationFrame ||
null;

// To stop animation : To have cancelAnimationFrame() to be called "cross-browser" compatible
var cancelAnimationFrame = 
window.cancelAnimationFrame ||
window.webkitCancelAnimationFrame || window.webkitCancelRequestAnimationFrame ||
window.mozCancelAnimationFrame || window.mozCancelRequestAnimationFrame ||
window.oCancelAmimationFrame || window.oCancelRequestAnimationFrame ||
window.msCancelAnimationFrame || window.msCancelRequestAnimationFrame ||
null;

// onload function
function myMain()
{
	// get <canvas> element
	canvas = document.getElementById("id_Canvas");
	if(!canvas)
		console.log("Obtaining Canvas Failed");
	else
		console.log("Obtaining Canvas Succeeded");

	canvas_original_width = canvas.width;
	canvas_original_height = canvas.height;
	
	// Register keyboard's keydown event handler
	window.addEventListener("keydown", keyDown, false);
	window.addEventListener("click", mouseDown, false);
	window.addEventListener("resize", resize, false);

	// initialize webgl
	init();

	//start drawing here as warming-up
	resize();
	draw();
}

function toggleFullScreen()
{
	//code
	var fullscreen_element = document.fullscreen_element ||
	document.webkitFullscreenElement ||
	document.mozFullScreenElement ||
	document.msFullscreenElement ||
	null;

	// if not fullscreen
	if(fullscreen_element == null)
	{
		if(canvas.requestFullscreen)
			canvas.requestFullscreen();
		else if(canvas.mozRequestFullScreen)
			canvas.mozRequestFullScreen();
		else if(canvas.webkitRequestFullscreen)
			canvas.webkitRequestFullscreen();
		else if(canvas.msRequestFullscreen)
			canvas.msRequestFullscreen();
		bFullscreen = true;
	}
	else 		// if already fullscreen
	{
		if(document.exitFullscreen)
			document.exitFullscreen();
		else if(document.mozCancelFullScreen)
			document.mozCancelFullScreen();
		else if(document.webkitExitFullscreen)
			document.webkitExitFullscreen()
		else if(document.webkitExitFullscreen)
			document.webkitExitFullscreen();
		bFullscreen = false;
	}
}

function init()
{
	//code
	// get webgl 2.0 context
	gl = canvas.getContext("webgl2");
	if(!gl)
	{
		console.log("Failed to get the rendering context for WebGL");
		return;
	}

	gl.viewportWidth = canvas.width;
	gl.viewportHeight = canvas.height;

	//vertex shader
	vertexShaderObject =gl.createShader(gl.VERTEX_SHADER);

	var vertexShaderSourceCode = 
	"#version 300 es " +
	"\n 	" +
	"in vec4 vPosition;	" +
	"in vec2 vTexcoord;	" +
	
	"out vec2 out_texcoord;	" +
	
	"uniform mat4 u_mvp_matrix;" +
	
	"void main(void)" +
	"{" +
	"	gl_Position = u_mvp_matrix * vPosition;	" +
	"	out_texcoord = vTexcoord;" +
	"}";

	gl.shaderSource(vertexShaderObject, vertexShaderSourceCode);
	gl.compileShader(vertexShaderObject);
	if(gl.getShaderParameter(vertexShaderObject, gl.COMPILE_STATUS) == false)
	{
		var error = gl.getShaderInfoLog(vertexShaderObject);
		if(error.length > 0)
		{
			alert(error);
			uninitialize();
		}
	}

	//fragment shader
	fragmentShaderObject = gl.createShader(gl.FRAGMENT_SHADER);

	var fragmentShaderSourceCode = 
	"#version 300 es" +
	"\n	" +
	"precision highp float;	" +
	"in vec2 out_texcoord;	" +
	
	"uniform highp sampler2D u_sampler_uniform;" +
	
	"out vec4 FragColor;" +
	
	"void main(void)" +
	"{	" +
	"FragColor = texture(u_sampler_uniform, out_texcoord);	" +
	"}" ;

	gl.shaderSource(fragmentShaderObject, fragmentShaderSourceCode);
	gl.compileShader(fragmentShaderObject);
	if(gl.getShaderParameter(fragmentShaderObject, gl.COMPILE_STATUS) == false)
	{
		var error = gl.getShaderInfoLog(fragmentShaderObject);
		if(error.length > 0)
		{
			alert(error);
			uninitialize();
		}
	}

	//shader program object
	shaderProgramObject = gl.createProgram();
	gl.attachShader(shaderProgramObject, vertexShaderObject);
	gl.attachShader(shaderProgramObject, fragmentShaderObject);

	//pre linking binding to shader program object with shader attributes
	gl.bindAttribLocation(shaderProgramObject, WebGLMacros.AMC_ATTRIBUTE_VERTEX, "vPosition");
	gl.bindAttribLocation(shaderProgramObject, WebGLMacros.AMC_ATTRIBUTE_TEXCOORD0, "vTexcoord");

	// linking
	gl.linkProgram(shaderProgramObject);
	if(!gl.getProgramParameter(shaderProgramObject, gl.LINK_STATUS))
	{
		var error = gl.getProgramInfoLog(shaderProgramObject);
		if(error.length > 0)
		{
			alert(error);
			uninitialize();
		}
	}

	// get mvp uniform location
	samplerUniform = gl.getUniformLocation(shaderProgramObject, "u_sampler_uniform");
	mvpUniform = gl.getUniformLocation(shaderProgramObject, "u_mvp_matrix");

	kundaliTexture = gl.createTexture();
	kundaliTexture.image = new Image();
	kundaliTexture.image.src = "kundali.png";
	kundaliTexture.image.onload = function()
	{
		gl.bindTexture(gl.TEXTURE_2D, kundaliTexture);
		gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, kundaliTexture.image);
		gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
		gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
		gl.bindTexture(gl.TEXTURE_2D, null);
	}


	// data for cube
	var cubeVertices = new Float32Array([
		1.0, 1.0, 1.0, -1.0, 1.0, 1.0, -1.0, -1.0, 1.0, 1.0, -1.0, 1.0,
        1.0, 1.0, -1.0, 1.0, 1.0, 1.0, 1.0, -1.0, 1.0, 1.0, -1.0, -1.0,
        1.0, 1.0, -1.0, -1.0, 1.0, -1.0, -1.0, -1.0, -1.0, 1.0, -1.0, -1.0,
        -1.0, 1.0, -1.0, -1.0, 1.0, 1.0, -1.0, -1.0, 1.0, -1.0, -1.0, -1.0,
        1.0, 1.0, -1.0, -1.0, 1.0, -1.0, -1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
        1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, 1.0, 1.0, -1.0, 1.0
		]);

	var cubeTexcoord = new Float32Array([
		1.0, 1.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0,
		1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0,
		1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0,
		0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 1.0, 1.0,
		1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0
		]);

	vao_cube = gl.createVertexArray();
	gl.bindVertexArray(vao_cube);

	vbo_cube_vertices = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_cube_vertices);
	gl.bufferData(gl.ARRAY_BUFFER, cubeVertices, gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.AMC_ATTRIBUTE_VERTEX,
		3,
		gl.FLOAT,
		false,
		0,
		0);
	gl.enableVertexAttribArray(WebGLMacros.AMC_ATTRIBUTE_VERTEX);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);

	vbo_cube_texcoord = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_cube_texcoord);
	gl.bufferData(gl.ARRAY_BUFFER, cubeTexcoord, gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.AMC_ATTRIBUTE_TEXCOORD0,
		2, 
		gl.FLOAT,
		false,
		0,
		0);
	gl.enableVertexAttribArray(WebGLMacros.AMC_ATTRIBUTE_TEXCOORD0);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);
	gl.bindVertexArray(null);

	//setup framebuffer
	fbo = gl.createFramebuffer();
	gl.bindFramebuffer(gl.FRAMEBUFFER, fbo);

	fbo_color_texture = gl.createTexture();
	gl.bindTexture(gl.TEXTURE_2D, fbo_color_texture);
	gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, 512, 512, 0, gl.RGBA, gl.UNSIGNED_BYTE, null);
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
    gl.bindTexture(gl.TEXTURE_2D, null);

	const attachmentPoint =  gl.COLOR_ATTACHMENT0;
	gl.framebufferTexture2D(gl.FRAMEBUFFER, attachmentPoint, gl.TEXTURE_2D, fbo_color_texture, 0);

	// set clear color
	gl.clearColor(0.0, 0.0, 0.0, 1.0); // blue colored screen
	//enable depth
	gl.clearDepth(1.0);
	gl.enable(gl.DEPTH_TEST);
	gl.depthFunc(gl.LEQUAL);

	// initialize projection matrix
	perspectiveProjectionMatrix = mat4.create();
}

function resize()
{
	//code
	if(bFullscreen == true)
	{
		canvas.width = window.innerWidth;
		canvas.height = window.innerHeight;
	}
	else
	{
		canvas.width = canvas_original_width;
		canvas.height = canvas_original_height;
	}

gWidth=canvas.width;
gHeight=canvas.height;
	//set the viewport to match
	gl.viewport(0, 0, canvas.width, canvas.height);

	//perspective projection:
    mat4.perspective(perspectiveProjectionMatrix, 45.0, parseFloat(canvas.width)/parseFloat(canvas.height), 0.1, 100.0);
}

function degToRad(degrees)
{
	///code
	return(degrees * Math.PI / 100);
}

function draw()
{

	//	gl.clear(gl.COLOR_BUFFER_BIT| gl.DEPTH_BUFFER_BIT);
	//code
	gl.bindFramebuffer(gl.FRAMEBUFFER, fbo);

	// render cube with our 3x2 texture
	gl.bindTexture(gl.TEXTURE_2D, fbo_color_texture);
	
	// Tell WebGL how to convert from clip space to pixels
	gl.viewport(0, 0, 512, 512);
	
	// Clear the attachment(s).
    gl.clearColor(0, 0, 1, 1);   // clear to blue
	gl.clear(gl.COLOR_BUFFER_BIT| gl.DEPTH_BUFFER_BIT);
	
	gl.useProgram(shaderProgramObject);

	var modelViewMatrix = mat4.create();
	var modelViewProjectionMatrix = mat4.create();
	var translationMatrix = mat4.create();
	var rotationMatrix = mat4.create();
	var scaleMatrix = mat4.create();

	mat4.rotateX(rotationMatrix, rotationMatrix, degToRad(cubeAngle));
	mat4.rotateY(rotationMatrix, rotationMatrix, degToRad(cubeAngle));
	mat4.rotateZ(rotationMatrix, rotationMatrix, degToRad(cubeAngle));
	mat4.translate(translationMatrix, translationMatrix, [0.0,0.0,-3.5]);
	mat4.scale(scaleMatrix, scaleMatrix, [0.80, 0.80, 0.80]);

	mat4.multiply(modelViewMatrix, modelViewMatrix, translationMatrix);
	mat4.multiply(modelViewMatrix, modelViewMatrix, scaleMatrix);
	mat4.multiply(modelViewMatrix, modelViewMatrix, rotationMatrix);
	mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);
	
	//gl.uniform1i(samplerUniform, 0);
	gl.uniformMatrix4fv(mvpUniform, false, modelViewProjectionMatrix);

	// draw cube
	gl.bindTexture(gl.TEXTURE_2D, kundaliTexture);
	gl.bindVertexArray(vao_cube);
	gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);
	gl.drawArrays(gl.TRIANGLE_FAN, 4, 4);
	gl.drawArrays(gl.TRIANGLE_FAN, 8, 4);
	gl.drawArrays(gl.TRIANGLE_FAN, 12, 4);
	gl.drawArrays(gl.TRIANGLE_FAN, 16, 4);
	gl.drawArrays(gl.TRIANGLE_FAN, 20, 4);

	gl.bindVertexArray(null);

	gl.useProgram(null);

	gl.bindTexture(gl.TEXTURE_2D, null);
	gl.bindFramebuffer(gl.FRAMEBUFFER, null);

////////////////////-----------------//////////////////////////
	
	gl.useProgram(shaderProgramObject);

	var modelViewMatrix = mat4.create();
	var modelViewProjectionMatrix = mat4.create();
	var translationMatrix = mat4.create();
	var rotationMatrix = mat4.create();
	var scaleMatrix = mat4.create();

	mat4.rotateX(rotationMatrix, rotationMatrix, degToRad(cubeAngle));
	mat4.rotateY(rotationMatrix, rotationMatrix, degToRad(cubeAngle));
	mat4.rotateZ(rotationMatrix, rotationMatrix, degToRad(cubeAngle));
	mat4.translate(translationMatrix, translationMatrix, [0.0,0.0,-3.5]);
	mat4.scale(scaleMatrix, scaleMatrix, [0.80, 0.80, 0.80]);

	mat4.multiply(modelViewMatrix, modelViewMatrix, translationMatrix);
	mat4.multiply(modelViewMatrix, modelViewMatrix, scaleMatrix);
	mat4.multiply(modelViewMatrix, modelViewMatrix, rotationMatrix);
	mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);
	
	//gl.uniform1i(samplerUniform, 0);
	gl.uniformMatrix4fv(mvpUniform, false, modelViewProjectionMatrix);
 
    gl.viewport(0,0,gWidth,gHeight);
	// draw cube
	gl.bindTexture(gl.TEXTURE_2D, fbo_color_texture);
	gl.bindVertexArray(vao_cube);
	gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);
	gl.drawArrays(gl.TRIANGLE_FAN, 4, 4);
	gl.drawArrays(gl.TRIANGLE_FAN, 8, 4);
	gl.drawArrays(gl.TRIANGLE_FAN, 12, 4);
	gl.drawArrays(gl.TRIANGLE_FAN, 16, 4);
	gl.drawArrays(gl.TRIANGLE_FAN, 20, 4);

	gl.bindVertexArray(null);

	gl.useProgram(null);
	gl.bindTexture(gl.TEXTURE_2D, null);

	if(cubeAngle == 360.0)
	cubeAngle = 0.0;
	cubeAngle = cubeAngle + 0.3;
	//animation loop
	requestAnimationFrame(draw, canvas);
}

function uninitialize()
{
	//code
	if(vao_cube)
	{
		gl.deleteVertexArray(vao_cube);
		vao_cube = null;
	}

	if(vbo_cube_vertices)
	{
		gl.deleteBuffer(vbo_cube_vertices);
		vbo_cube_vertices = null;
	}

	if(vbo_cube_texcoord)
	{
		gl.deleteBuffer(vbo_cube_texcoord);
		vbo_cube_texcoord = null;
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

function keyDown(event)
{
	//code
	switch(event.keyCode)
	{
		case 70: 	// for f or F
			toggleFullScreen();

			break;
		case 27:
		uninitialize();
		window.close();
		break;
	}
}

function mouseDown()
{
	//code
}

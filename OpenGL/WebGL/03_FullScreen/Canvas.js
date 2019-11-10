//global
var canvas = null;
var context = null;
function myMain()
{
	//get <canvas> element..
	 canvas = document.getElementById("id_Canvas");
	if(!canvas)
		console.log("Obtaining canvas faild!!!\n");
	else
		console.log("Obtaining canvas Successfull\n");
	
	//print canvas width and height
	
	//get 2D context..
	context = canvas.getContext("2d");
	if(!context)
		console.log("Obtaining context faild!!!\n");
	else
		console.log("Obtaining context Successfull\n");
	
	//fill canvas with black color..
	context.fillStyle = "black";
	context.fillRect(0,0,canvas.width, canvas.height);
	
	drawText("Hello World!!!");
	
	//register keyboards keydown and event handler..
	window.addEventListener("keydown",keydown,false);
	window.addEventListener("click",mouseDown,false);
}
function drawText(text)
{
	context.textAlign = "center";
	context.textBaseline = "middle";
	
	context.font = "48px sans-serif";
	
	//text color..
	context.fillStyle = "white";
	
	//display the text
	context.fillText(text,canvas.width/2, canvas.height/2);
	
}
function keydown(event)
{
	switch(event.keyCode)
	{
		case 70:
		//alert("F key is pressed");
		toggleFullScreen();
		
		drawText("Hello World!!!");
		break;
		
	}
}

function mouseDown()
{
	alert("Mouse is clicked");
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
			
	}
}
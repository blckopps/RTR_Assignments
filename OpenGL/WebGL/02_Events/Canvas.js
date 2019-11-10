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
	
	//center the text..
	context.textAlign = "center";
	context.textBaseline = "middle";
	
	//text
	var str = "Hello  World!!!";
	
	//text Font
	context.font = "48px sans-serif";
	
	//text color..
	context.fillStyle = "white";
	
	//display the text
	context.fillText(str,canvas.width/2, canvas.height/2);
	
	
	//register keyboards keydown and event handler..
	window.addEventListener("keydown",keydown,false);
	window.addEventListener("click",mouseDown,false);
}

function keydown(event)
{
	alert("A Key is pressed");
}

function mouseDown()
{
	alert("Mouse is clicked");
}
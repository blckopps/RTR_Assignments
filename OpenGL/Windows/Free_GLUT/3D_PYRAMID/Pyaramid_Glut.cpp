#include<GL/freeglut.h>

bool IsFullScreen=false;
float angle=0.0f;
int main(int argc,char *argv[])
{
	//function declarations
	void initialize(void);
	void uninitialize(void);
	void reshape(int ,int);
	void display(void);
	void keyboard(unsigned char,int,int);
	void mouse(int, int, int, int);
	void update(void);
	//code
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE |GLUT_DEPTH| GLUT_RGBA);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("GLUT_3D_PYRAMID PROGRAM-SHUBHAM");

	initialize();

	//callbacks
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutIdleFunc(update);
	glutCloseFunc(uninitialize);
	glutMainLoop();
	return(0);


}

void initialize(void)
{
	glShadeModel(GL_SMOOTH);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);
	
}

void uninitialize(void)
{

}

void reshape(int width, int height)
{
	/*glMatrixMode(GL_PROJECTION);
	glLoadIdentity();*/
	if(height == 0)
	{
		height = 1;
	}
	glViewport(0, 0, (GLsizei)width,(GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f,
		(GLfloat)width/(GLfloat)height,	
					0.1f,
					100.0f);
}


void display(void)
{
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f,0.0f,-4.0f);
	glRotatef(angle,0.0f,1.0f,0.0f);
	
	glBegin(GL_TRIANGLES);
		//1St
		glColor3f(1.0f,0.0f,0.0f);					//R 
		glVertex3f(0.0f, 1.0f, 0.0f);				//APEX

		glColor3f(0.0f,1.0f,0.0f);					//G 
		glVertex3f(-1.0f, -1.0f, 1.0f);				//left bottom

		glColor3f(0.0f,0.0f,1.0f);					//B
		glVertex3f(1.0f, -1.0f, 1.0f);				//Rigth BOttom

		//2nd
		glColor3f(1.0f,0.0f,0.0f);					//R 
		glVertex3f(0.0f, 1.0f, 0.0f);				//APEX

		glColor3f(0.0f,0.0f,1.0f);					//B 
		glVertex3f(1.0f, -1.0f, 1.0f);				//left bottom

		glColor3f(0.0f,1.0f,0.0f);					//G
		glVertex3f(1.0f, -1.0f, -1.0f);				//Right BOttom
		//3rd
		glColor3f(1.0f,0.0f,0.0f);					//R 
		glVertex3f(0.0f, 1.0f, 0.0f);				//APEX

		glColor3f(0.0f,1.0f,0.0f);					//G 
		glVertex3f(1.0f, -1.0f, -1.0f);				//left bottom

		glColor3f(0.0f,0.0f,1.0f);					//B
		glVertex3f(-1.0f, -1.0f, -1.0f);				//Rigth BOttom
		//4th
		glColor3f(1.0f,0.0f,0.0f);					//R 
		glVertex3f(0.0f, 1.0f, 0.0f);				//APEX

		glColor3f(0.0f,0.0f,1.0f);					//b
		glVertex3f(-1.0f, -1.0f, -1.0f);				//left bottom

		glColor3f(0.0f,1.0f,0.0f);					//g
		glVertex3f(-1.0f, -1.0f, 1.0f);				//Rigth BOttom
	glEnd();
	glutSwapBuffers();

}

void keyboard(unsigned char key,int x, int y)
{
	switch(key)
	{
	case 27:
		glutLeaveMainLoop();
		break;

	case 'F':
	case 'f':
		if(IsFullScreen==false)
		{
			glutFullScreen();
			IsFullScreen=true;
		}
		else
		{
			glutLeaveFullScreen();
			IsFullScreen=false;
		}
		break;
	}
}

void mouse(int button, int state, int x, int y)
{
	switch(button)
	{
	case GLUT_LEFT_BUTTON:
		break;

	case GLUT_RIGHT_BUTTON:
		glutLeaveMainLoop();
		break;
	}
}

void update(void)
{
	if(angle==360)
	{
		angle=0.0f;
	}
	angle=angle+0.02f;
	glutPostRedisplay();
}
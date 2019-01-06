#include<GL/freeglut.h>

bool IsFullScreen=false;

int main(int argc,char *argv[])
{
	//function declarations
	void initialize(void);
	void uninitialize(void);
	void reshape(int ,int);
	void display(void);
	void keyboard(unsigned char,int,int);
	void mouse(int, int, int, int);

	//code
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("MY FIRST OPENGL PROGRAM-SHUBHAM BENDRE");

	initialize();

	//callbacks
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutCloseFunc(uninitialize);
	glutMainLoop();
	return(0);


}

void initialize(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	
}

void uninitialize(void)
{

}

void reshape(int width, int height)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
}


void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glBegin(GL_TRIANGLES);

	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex2f(0.0f, 1.0f);

	
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(-1.0f, -1.0f);

	
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex2f(1.0f, -1.0f);

	glEnd();
	glFlush();

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


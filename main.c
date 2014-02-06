/*
 * @Brief:  
 *    
 * @v1: Jian Chen (jichen@umbc.edu) 
 * 
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#ifdef __APPLE__
  #include <GLUT/glut.h>
#else
  #include <GL/glut.h>
#endif

#include "terrain.h"

/* Some <math.h> files do not define M_PI... */
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#define DEFAULT_CAM_X  0
#define DEFAULT_CAM_Y 3.4
#define DEFAULT_CAM_Z  0
#define DEFAULT_FOC 26
//GLfloat camX=0, camY=25, camZ=78;
GLfloat camX=DEFAULT_CAM_X, camY=DEFAULT_CAM_Y, camZ=DEFAULT_CAM_Z;
GLfloat initX=0.0, initY=0.0, deltaX=0.0, deltaY=0.0;
GLfloat focX = 0.0, focY = 3, focZ = 26, angleX = 0.0, angleY = 0.0, angleZ = 0.0;
int mouseButton = -1;
float setRadian(float angle){
 float rAngle = angle * M_PI / 180;
 return rAngle;
}
//setting a camera position and what does camera look at will change depends on it's position0
void SetCamera(void)
{
    gluLookAt(camX, camY, camZ,  camX + focX, camY + focY, camZ + focZ + 0.5, 0.0,1.0,0.0);
}

//robot's position will change depend on camera's position
void Display(void)
{
  glLoadIdentity();
    SetCamera();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    drawScene();
    glPushMatrix();
    glTranslatef(camX, camY-0.5, camZ-0.8);
    glRotatef(angleX, 0,1,0);
    glTranslatef(-camX, -(camY + 0.5), -(camZ -0.7));
    glTranslatef(camX, camY-0.5, camZ-0.8);
    glRotatef(angleX, 0,1,0);
    drawRobot();
    glPopMatrix();
    glutSwapBuffers();

}

GLfloat lightpos[4] = { 0.0, 3.0, 2.0, 0.0 };
GLfloat lightamb[4] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat lightdif[4] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat lmodel_ambient[]={0.4, 0.4, 0.4, 1.0};
GLfloat local_view[]={0.0};

void myinit(void)
{
    glShadeModel (GL_SMOOTH);
    glFrontFace(GL_CCW);
    glEnable(GL_DEPTH_TEST);

    glLoadIdentity();
    glClearColor(0.7, 0.7, 0.9, 1.0); // clear to sky color
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightamb);
    glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightdif);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, local_view);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // objects' display lists
    initSceneLists();
}
//move camera and robot's position 
/* ARGSUSED1 */
void Keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
	case 27: exit(0);
	case 'w': 
	    camX +=0.5 * sin(setRadian(angleX));
	    camZ +=0.5* cos(setRadian(angleX)); 
		break;
	case 's': 
	    camX -=0.5 * sin(setRadian(angleX));
	    camZ -=0.5* cos(setRadian(angleX)); 
		break;
	case 'd': 
	    camX -=0.5 * cos(setRadian(angleX));
	    camZ +=0.5 * sin(setRadian(angleZ)); 
		break;
	case 'a': 
	    camX +=0.5 * cos(setRadian(angleX));
	    camZ -=0.5 * sin(setRadian(angleZ)); 
		break;
	/*case 'z': 
	    camZ +=0.5; break;
	case 'Z': 
	    camZ -=0.5; break;
*/
    default: 
	    return;
    }
	//printf("camX=%f %f %f\n", camX, camY, camZ);
    glutPostRedisplay();
}

void Motion(int x, int y){
	glutPostRedisplay();
	deltaX = x - initX;
	deltaY = y - initY;
	angleX += deltaX/200;
	angleY = deltaY/200;
/*	if(angleX > 360){ angleX -= 360;}
	if(angleX < 0){ angleX += 360;}
	if(angleY > 360) {angleY -= 360;}
	if(angleY < 0){ angleY +=360;}
*/	
	focX = camX + DEFAULT_FOC * sin(setRadian(angleX));
	focY += angleY;
	focZ = camZ + DEFAULT_FOC * cos(setRadian(angleX));
}
//get mouse's motion
void Mouse(int button, int state, int x, int y){
	mouseButton = button;
	initX= x;
	initY = y;	
	glutPostRedisplay();
}
//enter home key to reset to x,y,z
/* ARGSUSED1 */
void SpecialKey(int key, int x, int y)
{
    switch (key)
    {
	  case GLUT_KEY_HOME:	
	    camX=DEFAULT_CAM_X;
	    camY=DEFAULT_CAM_Y;
	    camZ=DEFAULT_CAM_Z;
	    focX = 0;
	    focY = 3;
	    focZ = 26;
	    angleX = 0, angleY = 0, angleZ = 0;
        break;
      default: return;
    }
    glutPostRedisplay();
}

void Reshape(int w, int h)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum (-0.1, 0.1, -0.1, 0.1, 0.3, 200.0); // perspective viewing
    glMatrixMode (GL_MODELVIEW);
    glViewport(0, 0, w, h);
    glLoadIdentity();
    SetCamera();
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitWindowPosition(200, 0);
	if(argc>1 && argc!=3)
	{
	  printf("usage: ./main <win width> <win height>\n");
	  exit(-1);
	}
	else if(argc==1)
      glutInitWindowSize(800, 600);
	else 
      glutInitWindowSize(atoi(argv[1]), atoi(argv[2]));

    glutInitDisplayMode(GLUT_DEPTH | GLUT_RGB | GLUT_DOUBLE | GLUT_MULTISAMPLE | GLUT_STENCIL);
    glutCreateWindow("EVE and WALL-E's Orange Land");

    myinit();

    glutKeyboardFunc(Keyboard);
    glutMouseFunc(Mouse);
    glutMotionFunc(Motion);
    glutSpecialFunc(SpecialKey);
    glutReshapeFunc(Reshape);
    glutDisplayFunc(Display);

    glutSwapBuffers();
    glutMainLoop();
    return 0;             
}

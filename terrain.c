/*
 * @brief: 
 *   will init and render the terrain
 * @ v1: Jian Chen (jichen@umbc.edu)  August 2013
 *
 */

#include <stdlib.h>
#include <stdio.h>

#ifdef __APPLE__
  #include <GLUT/glut.h>
#else
  #include <GL/glut.h>
#endif

#include <math.h>
#include <sys/time.h>

#include "terrain.h"

#define BLOCK_SIZE 9 
#define TSIZE 8       // terrain 1 block sub-block count

/* Some <math.h> files do not define M_PI... */
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

GLuint terrain1BlockList;

GLubyte terrainTexture[TXSX][TXSY][3];
GLuint terrainTextureList;
GLuint orangeQuadList;
GLuint treeList;
GLuint myRobotList;
float terrain[TSIZE+1][TSIZE+1][3];

void drawTheSun(int x, int y);

/*---------------------------------------------------------------------
 * initTexture
 *    init the terrainTexture display list
 * [VIEWING]
 */
void initTextures(void)
{
    extern void GenerateTextures(); // in texture.c

    // init terrain texture display list
    terrainTextureList = glGenLists(1);
    GenerateTextures(); // in texture.c
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glNewList(terrainTextureList, GL_COMPILE);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, TXSX, TXSY, 0, GL_RGB,
	             GL_UNSIGNED_BYTE, &terrainTexture[0][0][0]);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glEndList();
}

/*---------------------------------------------------------------------
 * initOrangeLand 
 *    init the display list for the orange ground
 * [VIEWING]
 */
void initOrangeLand()
{
  float y=-0.1;

  // generate the orange ground
  orangeQuadList=glGenLists(1);
  glNewList(orangeQuadList, GL_COMPILE);
    glDisable(GL_LIGHTING);
    glColor4f(1, 0.65, 0.0, 1.0);
    glBegin(GL_QUADS);
     glVertex3f(-BLOCK_SIZE, y, -BLOCK_SIZE);
     glVertex3f( BLOCK_SIZE, y, -BLOCK_SIZE);
     glVertex3f( BLOCK_SIZE, y, BLOCK_SIZE);
     glVertex3f(-BLOCK_SIZE, y, BLOCK_SIZE);
    glEnd();

	glColor4f(0.3, 0.3, 0.3, 1);
	glLineWidth(5.0);
    glBegin(GL_LINE_LOOP);
     glVertex3f(-BLOCK_SIZE, y, -BLOCK_SIZE);
     glVertex3f( BLOCK_SIZE, y, -BLOCK_SIZE);
     glVertex3f( BLOCK_SIZE, y, BLOCK_SIZE);
     glVertex3f(-BLOCK_SIZE, y, BLOCK_SIZE);
    glEnd();
	glLineWidth(1.0);
    glEnable(GL_LIGHTING);
  glEndList();

  // generate trees
  treeList = glGenLists(1);
  glNewList(treeList, GL_COMPILE);
  glDisable(GL_LIGHTING);
    glColor4f(0, 1, 0, 1);
	//  cone 1
	glPushMatrix();
	  glTranslatef(0, 0, -BLOCK_SIZE+1);
	  glRotatef(-90, 1, 0, 0);
 	  glutSolidCone(2, 6, 20, 20); 
	glPopMatrix();

	//  cone 2
	glPushMatrix();
	  glTranslatef(-3, 0, -BLOCK_SIZE+1);
	  glRotatef(-90, 1, 0, 0);
 	  glutSolidCone(2.5, 8, 20, 20); 
	glPopMatrix();

	//  cone 3
	glPushMatrix();
	  glTranslatef(2.5, 0, -BLOCK_SIZE+1);
	  glRotatef(-90, 1, 0, 0);
 	  glutSolidCone(2.4, 7.2, 20, 20); 
	glPopMatrix();

	//  cone 4
	glPushMatrix();
	  glTranslatef(5, 0, -BLOCK_SIZE+0.3);
	  glRotatef(-90, 1, 0, 0);
 	  glutSolidCone(2.5, 8, 20, 20); 
	glPopMatrix();

  glEnable(GL_LIGHTING);
  glEndList();
}

/*---------------------------------------------------------------------
 * Rendering 
 *   simply call the display list
 * [VIEWING]
 */
void drawOrangeLand(void)
{
    int i, j;

	for(i=-1; i<2; i++)
	{
      glPushMatrix();
      glTranslatef(BLOCK_SIZE*2*i, 0, 0);
	  for(j=0; j<4; j++)
	  {
	    glPushMatrix();
          glTranslatef(0, 0, BLOCK_SIZE*2*j);
	      glCallList(orangeQuadList);
        glPopMatrix();
	  }; // end j
      glPopMatrix();
	}; // end i;

	glCallList(treeList);

    int winWidth = glutGet(GLUT_WINDOW_WIDTH);
	int winHeight = glutGet(GLUT_WINDOW_HEIGHT);
    drawTheSun(winWidth-80, (int)(winHeight / 6.));
}

/*---------------------------------------------------------------------
 * drawTheSun
 *  this routine will draw the SUN on the screen coordinate so it does
 *  not translate, but rotate.
 */
void drawTheSun(int x, int y)
{
  // draw the sun
  GLint viewport[4];
  GLdouble modelview[16];
  GLdouble projection[16];

  GLfloat winX=0, winY, winZ;
  GLdouble posX, posY, posZ;

  // viewport
  glGetIntegerv(GL_VIEWPORT, viewport);
  // modelview determines how the vertices of OpenGL primitives are transformed to eye coords
  glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
  // projects transforms vertices in eye coord to clip coordinates
  glGetDoublev(GL_PROJECTION_MATRIX, projection); 
   
  // get z
  winX = (float)x;
  winY = (float)viewport[3] - (float)y;
  glReadPixels(x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
  gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);
  //printf("posx, y, z: %f, %f, %f\n", posX, posY, posZ);

  glDisable(GL_LIGHTING);
  glPushMatrix();
    glTranslatef(posX, posY, posZ);
    // the sun
    glColor4f(1, 1, 0, 1);
    glutSolidSphere(4, 20, 20);

    // draw the light ray
	glColor4f(1, 0.3, 0, 1);
    for(int i=0; i<12; i++)
	{
      glPushMatrix();
	    glRotatef(45*i, 0, 0, 1);
		glBegin(GL_TRIANGLES);
		   glVertex3f(-0.4, 4, 0);
		   glVertex3f(0.4,  4, 0);
		   glVertex3f(0, 7, 0);
		glEnd();
	  glPopMatrix();
	}
  glPopMatrix();

  glEnable(GL_LIGHTING);

}

/*---------------------------------------------------------------------
 * drawTerrain
 *   draw the terrain blocks
 * TODO: [VIEWING]: need to change this routine to draw multiple hilly
 *                  terrain blocks. Here we have two already.
 */
void drawTerrain(void)
{
  glCallList(terrain1BlockList);
  glPushMatrix();
    glTranslatef(BLOCK_SIZE, 0.0, 0);
    glCallList(terrain1BlockList);
  glPopMatrix();
}

/*---------------------------------------------------------------------
 * rnd: random number generator
 *  the height (Y) of each point on the terrain grid is randomly created. 
 */
double rnd(float max)
{
#if 0
  struct timeval tv;
  struct timezone tz;
  gettimeofday(&tv, &tz);

  double wideSeed = (unsigned long)tv.tv_sec * 1000000.0 + tv.tv_usec;
  unsigned short seed16[3];
  const double divisor = 1ul << 16;
  unsigned long topBits = (unsigned long)(wideSeed / divisor / divisor);
  seed16[2]=(unsigned short)topBits;
  wideSeed-=topBits * divisor * divisor;
  seed16[1]=(unsigned short)(wideSeed/divisor);
  wideSeed-=seed16[1]*divisor;
  seed16[0]=(unsigned short)wideSeed;

  seed48(seed16);
#endif
  return ((double)drand48()*max);
}


/*---------------------------------------------------------------------
 * initTerrain
 *    randomly generates the terrain height
 *    [VIEWING]: you code should measure the height based on the terrain
 *       height.
 */
void initTerrain(void)
{
  int gx,gz;

  // 8x8  grid
  for(gx=0; gx<=TSIZE; gx++)
  {
    for(gz=0; gz<=TSIZE; gz++)
	{
	  terrain[gx][gz][0]=gx;
	  terrain[gx][gz][1]=float(rnd(0.9));
	  terrain[gx][gz][2]=gz;
#if 0
	  printf("terrain: %d %d: %f %f %f\n", gx, gz, 
	      terrain[gx][gz][0],
	      terrain[gx][gz][1],
	      terrain[gx][gz][2]);
#endif
	}
  }

  terrain1BlockList = glGenLists(1);
  glNewList(terrain1BlockList, GL_COMPILE);

    glCallList(terrainTextureList);
	glEnable(GL_TEXTURE_2D);

    glBegin(GL_TRIANGLES);
    glNormal3f(0.0, 1.0, 0.0);
	for(gx=0; gx<TSIZE; gx++)
	  for(gz=0; gz<TSIZE; gz++)
	  {
	    glTexCoord2f(0.2*gx, 0.2*gz); 
		  glVertex3fv(terrain[gx][gz]);
	    glTexCoord2f(0.17+0.2*gx, 0.2*gz); 
		  glVertex3fv(terrain[gx][gz+1]);
	    glTexCoord2f(0.17+0.2*gx, 0.17+0.2*gz); 
		  glVertex3fv(terrain[gx+1][gz+1]);
	  } 
    glEnd();

    glBegin(GL_TRIANGLES);
	for(gx=0; gx<TSIZE; gx++)
	  for(gz=0; gz<TSIZE; gz++)
	  {
	    glTexCoord2f(0.2*gx, 0.17+0.2*gz); 
		  glVertex3fv(terrain[gx+1][gz]);
	    glTexCoord2f(0.17+0.2*gx, 0.2*gz); 
		  glVertex3fv(terrain[gx][gz]);
	    glTexCoord2f(0.17+0.2*gx, 0.17+0.2*gz); 
		  glVertex3fv(terrain[gx+1][gz+1]);
	  } 
    glEnd();

    // draw boundary
	glDisable(GL_TEXTURE_2D);
	glColor3f(0.2, 0.2, 0.8);

//z-1
	glBegin(GL_TRIANGLES);
	glNormal3f(0, 1, 0);
	for(gx=0; gx<TSIZE; gx++)
	{
	  glVertex3fv(terrain[gx][0]);
	  glVertex3fv(terrain[gx+1][0]);
	  glVertex3f(terrain[gx+1][0][0], 0, -0.5);
	}
	glEnd();

	glNormal3f(0, 1, 0);
	glBegin(GL_TRIANGLES);
	for(gx=0; gx<TSIZE; gx++)
	{
	  glVertex3fv(terrain[gx][0]);
	  glVertex3f(terrain[gx+1][0][0], 0, -0.5);
	  glVertex3f(terrain[gx][0][0], 0, -0.5);
	}
	glEnd();

// z+1
	glBegin(GL_TRIANGLES);
	for(gx=0; gx<TSIZE; gx++)
	{
	  glVertex3fv(terrain[gx][TSIZE]);
	  glVertex3f(terrain[gx][TSIZE][0], 0, 8.5);
	  glVertex3f(terrain[gx+1][TSIZE][0], 0, 8.5);
	}
	glEnd();

	glBegin(GL_TRIANGLES);
	for(gx=0; gx<TSIZE; gx++)
	{
	  glVertex3fv(terrain[gx][TSIZE]);
	  glVertex3f(terrain[gx+1][TSIZE][0], 0, 8.5);
	  glVertex3fv(terrain[gx+1][TSIZE]);
	}
	glEnd();

// x-1
	glBegin(GL_TRIANGLES);
	glNormal3f(0, 1, 0);
	for(gz=0; gz<TSIZE; gz++)
	{
	  glVertex3fv(terrain[0][gz+1]);
	  glVertex3fv(terrain[0][gz]);
	  glVertex3f(-0.5, 0, terrain[0][gz][2]);
	}
	glEnd();

	glNormal3f(0, 1, 0);
	glBegin(GL_TRIANGLES);
	for(gz=0; gz<TSIZE; gz++)
	{
	  glVertex3fv(terrain[0][gz+1]);
	  glVertex3f(-0.5, 0, terrain[0][gz][2]);
	  glVertex3f(-0.5, 0, terrain[0][gz+1][2]);
	}
	glEnd();

// x+1
	glBegin(GL_TRIANGLES);
	glNormal3f(0, 1, 0);
	for(gz=0; gz<TSIZE; gz++)
	{
	  glVertex3fv(terrain[TSIZE][gz]);
	  glVertex3fv(terrain[TSIZE][gz+1]);
	  glVertex3f(8.5, 0, terrain[TSIZE][gz+1][2]);
	}
	glEnd();

	glNormal3f(0, 1, 0);
	glBegin(GL_TRIANGLES);
	for(gz=0; gz<TSIZE; gz++)
	{
	  glVertex3fv(terrain[TSIZE][gz]);
	  glVertex3f(8.5, 0, terrain[TSIZE][gz+1][2]);
	  glVertex3f(8.5, 0, terrain[TSIZE][gz][2]);
	}
	glEnd();


  glEndList();
}


/*---------------------------------------------------------------------
 * init robot
 *    init a simple EVE 
 */
void initRobot()
{
#define BODY_SIZE 1
#define BODY_SX 1
#define BODY_SY 2 
#define BODY_SZ 1

#define HEAD_SX BODY_SX * 1.05
#define HEAD_SY BODY_SX * 1.05
#define HEAD_SZ BODY_SX * 1.05

#define ARM_SX 0.1
#define ARM_SY BODY_SY * 0.35
#define ARM_SZ 0.3

  myRobotList = glGenLists(1);
  glNewList(myRobotList, GL_COMPILE);

  glDisable(GL_LIGHTING);
    glColor3f(1, 0.9, 0.8);
    // body
    glPushMatrix();
	  glScalef(BODY_SX, BODY_SY, BODY_SZ); 
	  glutSolidSphere(1, 20, 20);
	glPopMatrix();

    // head
	glColor3f(1,1,1);
    glPushMatrix();
	  glTranslatef(0, HEAD_SY/2+HEAD_SX *0.4, 0);
	  glScalef(HEAD_SX, HEAD_SX, HEAD_SX); 
	  glutSolidSphere(1, 20, 20);
	glPopMatrix();

    // face
	glColor4f(0,0,0,0.5);
    glPushMatrix();
	  glTranslatef(0, BODY_SY/2+BODY_SX/2, 0.27);
	  glScalef(HEAD_SX*0.83, HEAD_SX*0.83, HEAD_SX*0.83); 
	  glutSolidSphere(1, 20, 20);
	glPopMatrix();

	// left arm
	glColor3f(1,1,1);
	glPushMatrix();
	  glTranslatef(-BODY_SX, 0, 0);
	  glRotatef(-10, 0, 0, 1);
	  glScalef(ARM_SX, ARM_SY, ARM_SZ); 
	  glutSolidSphere(1, 20, 20);
	glPopMatrix();

	// right arm
	glColor3f(1,1,1);
	glPushMatrix();
	  glTranslatef(BODY_SX, 0, 0);
	  glRotatef(10, 0, 0, 1);
	  glRotatef(20, 1, 0, 0);
	  glScalef(ARM_SX, ARM_SY, ARM_SZ); 
	  glutSolidSphere(1, 20, 20);
	glPopMatrix();

  glEnable(GL_LIGHTING);

  glEndList();
}

/*---------------------------------------------------------------------
 * drawRobot()
 * TODO: will need to move the robot in the scene
 */
void drawRobot()
{
  glPushMatrix();
  //glTranslatef(camX, camY, camZ);
  //glTranslatef(0, BODY_SY, 0);
  glCallList(myRobotList);
  glPopMatrix();
}

/*---------------------------------------------------------------------
 */
void initSceneLists(void)
{
    printf("init EVE and WALL-E's scene.\n");
    initTextures();
	initOrangeLand();
	initTerrain();
	initRobot();
}

/*---------------------------------------------------------------------
 * drawScene()
 *   this is where the real rendering happening.
 */
void drawScene(void)
{
    glEnable(GL_DEPTH_TEST);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    drawOrangeLand();
    drawTerrain();
//	drawRobot();

    glDisable(GL_DEPTH_TEST);
}

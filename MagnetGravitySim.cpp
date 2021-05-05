/* //////////////////////////////////////////////////////////////// */
/* CompSci459) Fundamentals of Computer Graphics                    */
/*			Programming Assignment 4		    */
/*                                                                  */
/* ------------                                                     */
/* amimotoAnna                                                      */
/* //////////////////////////////////////////////////////////////// */

#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <conio.h> 
#include <string.h>
#include <windows.h>

#include <ctime> 
#include "time.h"

//* ----------------------------------------- *//

#define PI 3.14159265
#define BUFSIZE 512

#define randf() ((float)rand()/(float)RAND_MAX)
#define abs(i) ((i < 0.0) ? -i : i)
#define min(x,y) ((x)<(y) ? (x) : (y))
#define max(x,y) ((x)>(y) ? (x) : (y))

//* ----------------------------------------- *//

/* Seed for RNG */
static GLuint seed = 1;

/* Menus */
int menuExit;
int menustatNumber;
int menustatMagnet;
int menuflyMagnet;
int val = 0;

/* Initial (Default) Magnet Numbers */
const int maxMagnets = 8;
const int maxFlying = 10;
int numMagnets = 4;
int numFlying = 1;

/* Physics Variables */
//gravitational acceleration
float g = 0.01;
// mass
float m = 9.0;
// polarity
float p = 0.05;
// velocity change
float ch = -0.8;

/* Window width and heght */
static GLint ww, hh;

/* Camera Location- Longitude & Latitdue */
static float alpha = 0.0;
static float beta = PI / 24.0;

/* Position of Camera */
static GLdouble cpos[3];

/* Position of Light Source */
static GLfloat lpos[] = { 3.0, 5.0, 1.0, 1.0 };

/* Infinitely Distant */
static GLfloat inflight[] = { 0.0, 1.0, 0.0, 0.0 };

/* Material Properties */
static GLfloat white[] = { 1.0, 1.0, 1.0, 1.0 };
static GLfloat black[] = { 0.0, 0.0, 0.0, 1.0 };
static GLfloat gold[] = { 0.9333333, 0.8666667, 0.66666667, 1.0 };
static GLfloat red[] = { 1.0, 0.0, 0.0, 1.0 };
static GLfloat purple[] = { 1.0, 0.5, 1.0, 1.0 };
static GLfloat blue[] = { 0.0, 0.0, 1.0, 1.0 };

static GLfloat white_translucent[] = { 1.0, 1.0, 1.0, 0.7 };

/* Particles */
GLint statmagnet[maxMagnets];
static GLdouble xx = 0.0, yy = 0.0, zz = 0.0;

static GLfloat magXcoord[maxMagnets], magZcoord[maxMagnets];
static GLint magcolor[maxMagnets];
static GLfloat px[maxFlying], py[maxFlying], pz[maxFlying];
static GLint flycolor[maxFlying];

/* Particle Physics */
// current velocity of flying magnet M
float vx[maxFlying], vy[maxFlying], vz[maxFlying];
// change in position and velocity over time increment
float ppx[maxFlying], ppy[maxFlying], ppz[maxFlying];
float vpx[maxFlying], vpy[maxFlying], vpz[maxFlying];


//* ----------------------------------------- *//

void writemessage()
{
	printf("\n\
Assignment 4 | Interactive Magnet Box  | By: Anna Fischer\n\
	NOTE: Blue	 ...	Negative\n\
		  Red	 ...	Positive\n\
          Purple ...	Neutral\n\
The following key bindings perform their respective actions as given.\n\
	arrow keys ---------------- moves the camera angling and viewpoint\n\
    right button -------------- linked to GLUT menu choices:\n\
                            --- changes number of magnets\n\
		                    --- randomizes stationary magnet locations\n\
						    --- changes polarities of stationary magnets\n\
							--- changes polarities of flying magnets\n\
							--- restarts program to default settings\n\
							--- exit\n\
	left button --------------- click and hold down to drag magnets\n\
							--- CTRL-click to change polarities cyclically\n\
	k ------------------------- increases number of flying magnets to 10 maximum\n\
	j ------------------------- decreases number of flying magnets to 1 minimum\n\
	> ------------------------- increases attraction/ repulsion power between magnets\n\
	< ------------------------- decreases attraction/ repulsion power between magnets\n\
	. ------------------------- increases gravitational acceleration constant\n\
	, ------------------------- decreases gravitational acceleration constant\n\
	m ------------------------- increases the mass of flying magnets\n\
	n ------------------------- decreases the mass of flying magnets\n\
	x, X, y, Y, z, Z ---------- changes light source position\n\
	\n");
}

//* ----------------------------------------- *//

void reshape(int w, int h)
{
	ww = w;
	hh = h;
	glViewport(0, 0, (GLsizei)ww, (GLsizei)hh);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(40.0, (GLfloat)ww / (GLfloat)hh, 1.0, 20.0);
}

void drawMagnets(GLenum mode)
{
	int i;
	for (i = 0; i < numMagnets; i++) {

		if (mode == GL_SELECT)
			glLoadName(i);

		if (magcolor[i] == 0)
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, red);
		else if (magcolor[i] == 1) {
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, blue);
		}
		else if (magcolor[i] == 2) {
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, purple);
		}

		glPushMatrix();
		glTranslatef(magXcoord[i], 0.09, magZcoord[i]);
		glutSolidCube(0.1);
		glPopMatrix();
	}
}

void drawFlyingMagnets(GLenum mode)
{
	int i;
	for (i = 0; i < numFlying; ++i) {

		if (mode == GL_SELECT)
			glLoadName(i);

		if (flycolor[i] == 0)
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, red);
		else if (flycolor[i] == 1) {
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, blue);
		}
		else if (flycolor[i] == 2) {
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, purple);
		}

		glPushMatrix();
		glTranslatef(px[i], py[i], pz[i]);
		glutSolidSphere(0.08, 8, 8);
		glPopMatrix();
	}
}

void display(void)
{
	//glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient); 

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	/* Update Camera Position */
	cpos[0] = 15.0 * cos(beta) * sin(alpha);
	cpos[1] = 15.0 * sin(beta);
	cpos[2] = 15.0 * cos(beta) * cos(alpha);
	gluLookAt(cpos[0], cpos[1], cpos[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	glLightfv(GL_LIGHT0, GL_POSITION, inflight);

	// FLYING MAGNETS //
	drawFlyingMagnets(GL_RENDER);

	// STATIONARY MAGNETS //
	drawMagnets(GL_RENDER);

	// BOX //
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, gold);

	// Opaque Floor 
	glNormal3f(0.0, 1.0, 0.0);
	glBegin(GL_POLYGON);
	glVertex3f(2.0, 0.0, 2.0);
	glVertex3f(2.0, 0.0, -2.0);
	glVertex3f(-2.0, 0.0, -2.0);
	glVertex3f(-2.0, 0.0, 2.0);
	glEnd();

	glPushMatrix();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white_translucent);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 64);

	// Looking Glass
		// back panel
	glPushMatrix();
	glNormal3f(0.0, 0.0, -1.0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, white_translucent);
	glBegin(GL_POLYGON);
	glVertex3f(2.0, 4.0, -2.0);
	glVertex3f(-2.0, 4.0, -2.0);
	glVertex3f(-2.0, 0.0, -2.0);
	glVertex3f(2.0, 0.0, -2.0);
	glEnd();
	glPopMatrix();

	// side panels
	glPushMatrix();
	glNormal3f(1.0, 0.0, 0.0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, white_translucent);
	glBegin(GL_POLYGON);
	glVertex3f(2.0, 4.0, -2.0);
	glVertex3f(2.0, 4.0, 2.0);
	glVertex3f(2.0, 0.0, 2.0);
	glVertex3f(2.0, 0.0, -2.0);
	glEnd();
	glPopMatrix();
	glPushMatrix();
	glNormal3f(-1.0, 0.0, 0.0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, white_translucent);
	glBegin(GL_POLYGON);
	glVertex3f(-2.0, 4.0, -2.0);
	glVertex3f(-2.0, 4.0, 2.0);
	glVertex3f(-2.0, 0.0, 2.0);
	glVertex3f(-2.0, 0.0, -2.0);
	glEnd();
	glPopMatrix();

	// top panel
	glPushMatrix();
	glNormal3f(0.0, 1.0, 0.0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, white_translucent);
	glBegin(GL_POLYGON);
	glVertex3f(2.0, 4.0, -2.0);
	glVertex3f(-2.0, 4.0, -2.0);
	glVertex3f(-2.0, 4.0, 2.0);
	glVertex3f(2.0, 4.0, 2.0);
	glEnd();
	glPopMatrix();

	// front panel
	glPushMatrix();
	glNormal3f(0.0, 0.0, 1.0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, white_translucent);
	glBegin(GL_POLYGON);
	glVertex3f(2.0, 4.0, 2.0);
	glVertex3f(-2.0, 4.0, 2.0);
	glVertex3f(-2.0, 0.0, 2.0);
	glVertex3f(2.0, 0.0, 2.0);
	glEnd();
	glPopMatrix();

	glDisable(GL_BLEND);
	glPopMatrix();



	glutSwapBuffers();
	glFlush();
}

//* ----------------------------------------- *//

void menu(int id)
{
	switch (id) {

	case 0:
		exit(0);
		break;
	case 1:
		numMagnets = 4;
		numFlying = 1;
		glutPostRedisplay();
		break;
	case 2:
		for (int i = 0; i < maxMagnets; i++) {
			magXcoord[i] = randf() * 2.0 - 1.0;
			magZcoord[i] = randf() * 2.0 - 1.0;
		};
		glutPostRedisplay();
		break;

	default:
		break;
	}
}

void menu_Num(int id)
{
	switch (id) {

		//number of stationary magnets
	case 0:
		numMagnets = 0;
		glutPostRedisplay();
		break;
	case 1:
		numMagnets = 1;
		glutPostRedisplay();
		break;
	case 4:
		numMagnets = 4;
		glutPostRedisplay();
		break;
	case 8:
		numMagnets = 8;
		glutPostRedisplay();
		break;

	default:
		break;
	}
}

void menu_Stat(int id)
{
	switch (id) {

		//stationary magnet polarity
	case 1:
		for (int i = 0; i < maxMagnets; i++) {
			magcolor[i] = 0;
		}
		glutPostRedisplay();
		break;
	case 2:
		for (int i = 0; i < maxMagnets; i++) {
			magcolor[i] = 2;
		}
		glutPostRedisplay();
		break;
	case 3:
		for (int i = 0; i < maxMagnets; i++) {
			magcolor[i] = 1;
		}
		glutPostRedisplay();
		break;
	case 4:
		for (int i = 0; i < maxMagnets; i++)
			magcolor[i] = rand() % 3;
		glutPostRedisplay();
		break;

	default:
		break;
	}
}

void menu_Fly(int id)
{
	switch (id) {

		//flying magnet polarity
	case 5:
		for (int i = 0; i < maxFlying; i++) {
			flycolor[i] = 0;
		}
		glutPostRedisplay();
		break;
	case 6:
		for (int i = 0; i < maxFlying; i++) {
			flycolor[i] = 2;
		}
		glutPostRedisplay();
		break;
	case 7:
		for (int i = 0; i < maxFlying; i++) {
			flycolor[i] = 1;
		}
		glutPostRedisplay();
		break;
	case 8:
		for (int i = 0; i < maxFlying; i++) {
			flycolor[i] = rand() % 3;
		}
		glutPostRedisplay();
		break;

	default:
		break;
	}
}

//* ----------------------------------------- *//

void processHits(GLuint hits, GLuint buffer[])
{
	unsigned int k, m;
	GLuint names, *ptr;
	printf("hits = %d\n", hits);
	ptr = (GLuint *)buffer;

	for (k = 0; k < hits; k++) {
		names = *ptr;
		printf(" number of names on stack for this hit = %d\n", names); ptr++;
		printf(" z1 is %f;", (float)*ptr / 0xffffffff); ptr++;
		printf(" z2 is %f\n", (float)*ptr / 0xffffffff); ptr++;
		printf(" names on stack are ");
		for (m = 0; m < names; m++) {
			printf("%d ", *ptr);

			if (magcolor[*ptr] == 0) {
				magcolor[*ptr] = 1;
			}
			else if (magcolor[*ptr] == 1) {
				magcolor[*ptr] = 2;
			}
			else if (magcolor[*ptr] == 2) {
				magcolor[*ptr] = 0;
			}
		}
		ptr++;
	}
	printf("\n");

}

void processHits_drag(GLuint hits, GLuint buffer[])
{
	printf("\n\nIN PROCESS HITS %f, %f", xx, zz);
	unsigned int k, m;
	GLuint names, *ptr;
	printf("hits = %d\n", hits);
	ptr = (GLuint *)buffer;

	for (k = 0; k < hits; k++) {
		names = *ptr;
		printf(" number of names on stack for this hit = %d\n", names); ptr++;
		printf(" z1 is %f;", (float)*ptr / 0xffffffff); ptr++;
		printf(" z2 is %f\n", (float)*ptr / 0xffffffff); ptr++;
		printf(" names on stack are ");
		for (m = 0; m < names; m++) {
			printf("%d ", *ptr);
			if (statmagnet[*ptr] == 0) {
				statmagnet[*ptr] = 1;
			}
			printf("\n\nMoving stuffsssss %f, %f", magXcoord[*ptr], magZcoord[*ptr]);
			ptr++;
		}
		printf("\n");
	}
}

void dragMagnets(int x, int y)
{
	GLint viewport[4];
	GLdouble mvmatrix[16], projmatrix[16];
	GLint realy;  			 //  OpenGL y coordinate position
	GLdouble wx1, wy1, wz1;  //  returned world x1, y1, z1 coords
	GLdouble wx2, wy2, wz2;  //  returned world x2, y2, z2 coords
	double t;
	
	for (int i = 0; i < numMagnets; ++i) {

		if (statmagnet[i] == 1)  
		{
			glGetIntegerv(GL_VIEWPORT, viewport);
			glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);
			glGetDoublev(GL_PROJECTION_MATRIX, projmatrix);
			realy = viewport[3] - (GLint)y - 1;
			printf("Coordinates at cursor are (%4d, %4d)\n", x, realy);
			gluUnProject((GLdouble)x, (GLdouble)realy, 0.0,
				mvmatrix, projmatrix, viewport, &wx1, &wy1, &wz1);
			printf("World coords at z=0.0 are (%f, %f, %f)\n",
				wx1, wy1, wz1);
			gluUnProject((GLdouble)x, (GLdouble)realy, 1.0,
				mvmatrix, projmatrix, viewport, &wx2, &wy2, &wz2);
			printf("World coords at z=1.0 are (%f, %f, %f)\n",
				wx2, wy2, wz2);

			t = (-wy1 / (wy2 - wy1));

			if (((magXcoord[i] > -1.95) && (magXcoord[i] < 1.95)) && ((magZcoord[i] > -1.95) && (magZcoord[i] < 1.95)))
			{
				magXcoord[i] = wx1 + t * (wx2 - wx1);
				yy = 0;
				magZcoord[i] = wz1 + t * (wz2 - wz1);
			}
			else if ((magXcoord[i] < -1.95)){
				magXcoord[i] = -1.9;
				yy = 0;
				magZcoord[i] = wz1 + t * (wz2 - wz1);
			}
			else if (magXcoord[i] > 1.95) {
				magXcoord[i] = 1.9;
				yy = 0;
				magZcoord[i] = wz1 + t * (wz2 - wz1);
			}
			else if ((magZcoord[i] < -1.95)) {
				magXcoord[i] = wx1 + t * (wx2 - wx1);
				yy = 0;
				magZcoord[i] = -1.9;
			}
			else if ((magZcoord[i] > 1.95)) {
				magXcoord[i] = wx1 + t * (wx2 - wx1);
				yy = 0;
				magZcoord[i] = 1.9;
			}

			printf("t= %f\n", t);
			printf("xx= %f, yy= %f, zz= %f\n", magXcoord[i], yy, magZcoord[i]);
		}
	}

	glutPostRedisplay();
}

//mouse functon handler
void pickMagnets(int button, int state, int x, int y)
{
	GLuint selectBuf[BUFSIZE];
	GLint hits;
	GLint viewport[4];

	GLdouble mvmatrix[16], projmatrix[16];
	GLint realy;  			 //  OpenGL y coordinate position
	GLdouble wx1, wy1, wz1;  //  returned world x1, y1, z1 coords
	GLdouble wx2, wy2, wz2;	 //  returned world x2, y2, z2 coords

	if ((button != GLUT_LEFT_BUTTON) && (state != GLUT_DOWN)) return;

	glGetIntegerv(GL_VIEWPORT, viewport);
	glSelectBuffer(BUFSIZE, selectBuf);
	glRenderMode(GL_SELECT);
	glInitNames();
	glPushName(0);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluPickMatrix((GLdouble)x, (GLdouble)(viewport[3] - y), 5.0, 5.0, viewport);

	gluPerspective(40.0, (GLfloat)ww / (GLfloat)hh, 1.0, 20.0);
	glMatrixMode(GL_MODELVIEW);

	drawMagnets(GL_SELECT);

	glMatrixMode(GL_PROJECTION);
	glFlush();
	glPopMatrix();

	hits = glRenderMode(GL_RENDER);
	processHits_drag(hits, selectBuf);
	
	if (state == GLUT_DOWN) {
		
		if (glutGetModifiers() == GLUT_ACTIVE_CTRL)
				processHits(hits, selectBuf);

			glGetIntegerv(GL_VIEWPORT, viewport);
			glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);
			glGetDoublev(GL_PROJECTION_MATRIX, projmatrix);
			realy = viewport[3] - (GLint)y - 1;
			printf("Coordinates at cursor are (%4d, %4d)\n", x, realy);
			gluUnProject((GLdouble)x, (GLdouble)realy, 0.0,
				mvmatrix, projmatrix, viewport, &wx1, &wy1, &wz1);
			printf("World coords at z=0.0 are (%f, %f, %f)\n",
				wx1, wy1, wz1);
			gluUnProject((GLdouble)x, (GLdouble)realy, 1.0,
				mvmatrix, projmatrix, viewport, &wx2, &wy2, &wz2);
			printf("World coords at z=1.0 are (%f, %f, %f)\n",
				wx2, wy2, wz2);

			printf("xx: %f yy: %f zz: %f\n", xx, yy, zz);

			/////////////////// WONKY EXTRA CREDIT ///////////////////////////////////////
			if ((hits == 0) && (numMagnets < maxMagnets)) {

				double t;
				t = (-wy1 / (wy2 - wy1));
				xx = wx1 + (t * (wx2 - wx1));
				yy = 0.09;
				zz = wz1 + (t * (wz2 - wz1));
				if (((xx > -1.95) && (xx < 1.95)) && ((zz > -1.95) && (zz < 1.95)))
				{
					numMagnets = numMagnets + 1;
					magXcoord[numMagnets] = xx;
					magZcoord[numMagnets] = zz;
				}
				glutPostRedisplay();
			}
			///////////////////////////////////////////////////////////////////////////////
		
	}
	else {
		for (int i = 0; i < numMagnets; ++i) {
			if (statmagnet[i] == 1) {
				statmagnet[i] = 0;
			}
		}
	}

	glutPostRedisplay();
}


//* ----------------------------------------- *//

void physics(int t)
{
	float dist;
	float distf;
	float forcex[maxFlying], forcey[maxFlying], forcez[maxFlying];

	int i;
	for (i = 0; i < numFlying; ++i) {

		int j;
		for (j = 0; j < numMagnets; ++j) {

			dist = sqrt((pow((px[i] - magXcoord[j]), 2)) + (pow((py[i]), 2)) + (pow((pz[i] - magZcoord[j]), 2)));

			if ((magcolor[j] == (flycolor[i])) && (magcolor[j] == 2) && (flycolor[i] == 2)) {
				forcex[j] = 0;
				forcey[j] = 0;
				forcez[j] = 0;
			}
			if ((magcolor[j] == (flycolor[i])) && (magcolor[j] != 2) && (flycolor[i] != 2)) {
				forcex[j] = (1.0 * (p / (pow(dist, 2)))) * ((px[i] - magXcoord[j]) / dist);
				forcey[j] = (1.0 * (p / (pow(dist, 2)))) * (py[i] / dist);
				forcez[j] = (1.0 * (p / (pow(dist, 2)))) * ((pz[i] - magZcoord[j]) / dist);
			}
			if ((magcolor[j] != (flycolor[i]))) {
				forcex[j] = (-1.0 * (p / (pow(dist, 2)))) * ((px[i] - magXcoord[j]) / dist);
				forcey[j] = (-1.0 * (p / (pow(dist, 2)))) * (py[i] / dist);
				forcez[j] = (-1.0 * (p / (pow(dist, 2)))) * ((pz[i] - magZcoord[j]) / dist);
			}

			forcex[i] = forcex[i] + forcex[j];
			forcey[i] = forcey[i] + forcey[j];
			forcez[i] = forcez[i] + forcez[j];

		}
		int k;
		for (k = 0; k < numFlying; ++k) {

			if (k != i) {

				distf = sqrt((pow((px[i] - px[k]), 2)) + (pow((py[i] - py[k]), 2)) + (pow((pz[i] - pz[k]), 2)));

				if ((flycolor[k] == (flycolor[i])) && (flycolor[i] == 2) && (flycolor[k] == 2)) {
					forcex[k] = 0;
					forcey[k] = 0;
					forcez[k] = 0;
				}
				if ((flycolor[i] == (flycolor[k])) && (flycolor[k] != 2) && (flycolor[i] != 2)) {
					forcex[k] = (1.0 * (p / (pow(distf, 2)))) * ((px[i] - px[k]) / distf);
					forcey[k] = (1.0 * (p / (pow(distf, 2)))) * ((py[i] - py[k]) / distf);
					forcez[k] = (1.0 * (p / (pow(distf, 2)))) * ((pz[i] - pz[k]) / distf);
				}
				if ((flycolor[i] != (flycolor[k]))) {
					forcex[k] = (-1.0 * (p / (pow(distf, 2)))) * ((px[i] - px[k]) / distf);
					forcey[k] = (-1.0 * (p / (pow(distf, 2)))) * ((py[i] - py[k]) / distf);
					forcez[k] = (-1.0 * (p / (pow(distf, 2)))) * ((pz[i] - pz[k]) / distf);
				}

				forcex[i] = forcex[i] + forcex[k];
				forcey[i] = forcey[i] + forcey[k];
				forcez[i] = forcez[i] + forcez[k];
			}
		}

		vpx[i] = vx[i] + ( ((g + forcex[i]) / m));
		vpy[i] = vy[i] + ( ((g + forcey[i]) / m));
		vpz[i] = vz[i] + ( ((g + forcez[i]) / m));
		
		ppx[i] = px[i] + ((vpx[i]));
		ppy[i] = py[i] + ((vpy[i]));
		ppz[i] = pz[i] + ((vpz[i]));

		if ((ppx[i] < -1.9)) {
			ppx[i] = -1.9;
			vpx[i] = (-0.8 * vpx[i]);
		}
		if ((ppx[i] > 1.9)) {
			ppx[i] = 1.9;
			vpx[i] = (-0.8 * vpx[i]);
		}
		
		if ((ppy[i] < 0.9)) {
			ppy[i] = 0.9;
			vpy[i] = (-0.8 * vpy[i]);
		}
		if ((ppy[i] > 3.9)) {
			ppy[i] = 3.9;
			vpy[i] = (-0.8 * vpy[i]);
		}

		if ((ppz[i] < -1.9)) {
			ppz[i] = -1.9;
			vpz[i] = (-0.8 * vpz[i]);
		}
		if ((ppz[i] > 1.9)) {
			ppz[i] = 1.9;
			vpz[i] = (-0.8 * vpz[i]);
		}
		
			px[i] = ppx[i];
			py[i] = ppy[i];
			pz[i] = ppz[i];

			vx[i] = vpx[i];
			vy[i] = vpy[i];
			vz[i] = vpz[i];

	}

	glutPostRedisplay();
	glutTimerFunc(50, physics, 0);

}

//* ----------------------------------------- *//

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 27:
		exit(0);
		break;

	case 'k':
		if (numFlying < 10) {
			numFlying = numFlying + 1;
			glutPostRedisplay();
		}
		break;
	case 'j':
		if (numFlying > 1) {
			numFlying = numFlying - 1;
			glutPostRedisplay();
		}
		break;

	case '>':
		g = g + 0.05;
		glutPostRedisplay();
		break;
	case '<':
		if (g > 0) {
			g = g - 0.05;
			if (g < 0) g = 0;
		}
		glutPostRedisplay();
		break;

	case '.':
		p = p + 0.02;
		glutPostRedisplay();
		break;
	case ',':
		if (p > 0) {
			p = p - 0.02;
			if (p < 0) p = 0;
		}
		glutPostRedisplay();
		break;

	case 'm':
		m = m + 0.5;
		glutPostRedisplay();
		break;
	case 'n':
		if (m > 0.5) {
			m = m - 0.5;
			if (m < 0.5) m = 0.5;
		}
		glutPostRedisplay();
		break;

	case 'x':
		lpos[0] = lpos[0] + 0.2;
		glutPostRedisplay();
		break;
	case 'X':
		lpos[0] = lpos[0] - 0.2;
		glutPostRedisplay();
		break;
	case 'y':
		lpos[1] = lpos[1] + 0.2;
		glutPostRedisplay();
		break;
	case 'Y':
		lpos[1] = lpos[1] - 0.2;
		glutPostRedisplay();
		break;
	case 'z':
		lpos[2] = lpos[2] + 0.2;
		glutPostRedisplay();
		break;
	case 'Z':
		lpos[2] = lpos[2] - 0.2;
		glutPostRedisplay();
		break;

	default:
		break;
	}
}

void specialkey(GLint key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_RIGHT:
		alpha = alpha + PI / 180;
		if (alpha > 2 * PI) alpha = alpha - 2 * PI;
		glutPostRedisplay();
		break;
	case GLUT_KEY_LEFT:
		alpha = alpha - PI / 180;
		if (alpha < 0) alpha = alpha + 2 * PI;
		glutPostRedisplay();
		break;
	case GLUT_KEY_UP:
		if (beta < 0.45*PI) beta = beta + PI / 180;
		glutPostRedisplay();
		break;
	case GLUT_KEY_DOWN:
		if (beta > 0.05*PI) beta = beta - PI / 180;
		glutPostRedisplay();
		break;

	default:
		break;
	}
}

//* ----------------------------------------- *//

int main(int argc, char** argv) {

	writemessage();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(50, 50);
	//glutCreateWindow(argv[0]);
	glutCreateWindow("Assignment 4- Interactive Magnet Box");

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutMotionFunc(dragMagnets);
	glutMouseFunc(pickMagnets);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialkey);
	

	//* ------------------------------------ *//
	menustatNumber = glutCreateMenu(menu_Num);

	glutAddMenuEntry("None", 0);
	glutAddMenuEntry("One", 1);
	glutAddMenuEntry("Four", 4);
	glutAddMenuEntry("Eight", 8);

	//* ------------------------------------ *//
	menustatMagnet = glutCreateMenu(menu_Stat);

	glutAddMenuEntry("Positive", 1);
	glutAddMenuEntry("Neutral", 2);
	glutAddMenuEntry("Negative", 3);
	glutAddMenuEntry("Randomize", 4);

	//* ------------------------------------ *//
	menuflyMagnet = glutCreateMenu(menu_Fly);

	glutAddMenuEntry("Positive", 5);
	glutAddMenuEntry("Neutral", 6);
	glutAddMenuEntry("Negative", 7);
	glutAddMenuEntry("Randomize", 8);

	//* ------------------------------------ *//
	menuExit = glutCreateMenu(menu);

	glutAddSubMenu("Stationary Magnets", menustatNumber);
	glutAddSubMenu("Stationay Magnet Polarity", menustatMagnet);
	glutAddSubMenu("Flying Magnet Polarity", menuflyMagnet);
	glutAddMenuEntry("Initialize", 1);
	glutAddMenuEntry("Randomize", 2);
	glutAddMenuEntry("Exit", 0);

	glutAttachMenu(GLUT_RIGHT_BUTTON);

	//* ------------------------------------ *//
	srand(time(0));
	for (int i = 0; i < maxMagnets; i++) {
		magcolor[i] = rand() % 3;
		magXcoord[i] = randf() * 2.0 - 1.0;
		magZcoord[i] = randf() * 2.0 - 1.0;
	};
	for (int i = 0; i < maxFlying; i++) {
		flycolor[i] = rand() % 3;
		px[i] = (rand() % 4 + 0.9) - 2.0;
		py[i] = rand() % 4 + 0.9;
		pz[i] = (rand() % 4 + 0.9) - 2.0;

	};

	glutTimerFunc(50, physics, 0);
	glutMainLoop();
	return 0;
}

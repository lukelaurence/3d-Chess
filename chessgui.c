// clang chessgui.cpp -framework OpenGL -framework GLUT -o chess
#include "chesslogic.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <OpenGL/gl.h>
#include <OpenGl/glu.h>
#include <GLUT/glut.h>

// the cube's angle of rotation
float rotationangle = 0.0f;
// angle of rotation for the camera direction
float cameraangle=0.0;
// actual vector representing the camera's direction
float lx=0.0f,ly=0.0f,lz=-1.0f;
// XZ position of the camera
float px=0.0f,py=1.0f,pz=5.0f;
// the key states. These variables will be zero when no key is being pressed
// camera angular acceleration
float ax = 0.0f;
// camera linear acceleration
float dx = 0.0f, dy = 0.0f, dz = 0.0f;
// mouse origins
int xOrigin = -1, yOrigin = -1;
// coordinates of cube rotational vector
float rx = 0.0f, ry = 0.0f, rz = 0.0f;
// angular velocity
float av = 0.0f;
// key held booleans
int leftheld = 0, rightheld = 0, upheld = 0,
	downheld = 0, minusheld = 0, plusheld = 0;

/* draws a cube of size 1 that is either black or white depending on input */
void drawcube(int input){
	int color = input % 2;
	glColor4f(color,color,color,0.3f);
	glutSolidCube(1.0f);
}

/* pans the camera with a given acceleration, taking which XYZ axis as input */
void computePos(int which) {
	switch(which){
		case 1 :
			px += dx * lx * 0.1f;
			break;
		case 2 :
			py += dy * ly * 0.1f;
			break;
		case 3 :
			pz += dz * lz * 0.1f;
			break;
	}
}

void computeDir(float ax) {
	cameraangle += ax;
	lx = sin(cameraangle);
	lz = -cos(cameraangle);
}

void display(void){
	// Clears color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Enable the OpenGL Blending functionality 
	glEnable(GL_BLEND);
	// Set the blend mode to blend our current RGBA with what is already in the buffer
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// Reset transformations
	glLoadIdentity();
	// Set the camera
	gluLookAt(	px, py, pz,
				px+lx, py+ly, pz+lz,
				0.0f, 1.0f, 0.0f);
	glRotatef(rotationangle,rx,ry,rz);
	// draw 512 cubes
	for(int a = -4; a < 4; a++){
		for(int b = -4; b < 4; b++){
			for(int c = -4; c < 4; c++){
				glPushMatrix();
				glTranslatef(a,b,c);
				drawcube(abs(a) + abs(b) + abs(c));
				glPopMatrix();
			}
		}
	}
	if(dx){
		computePos(1);
	}
	if(dy){
		computePos(2);
	}
	if(dz){
		computePos(3);
	}
	if (ax)
		computeDir(ax);
	rotationangle+=av;
	glutSwapBuffers();
    // Don't wait start processing buffered OpenGL routines
    //glFlush();
}

void changeSize(int w, int h) {
	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if(h == 0) h = 1;
	float ratio = 1.0* w / h;
	// Use the Projection Matrix
	glMatrixMode(GL_PROJECTION);
	// Reset Matrix
	glLoadIdentity();
	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);
	// Set the correct perspective.
	gluPerspective(45.0f,ratio,0.1f,100.0f);
	// Get Back to the Modelview
	glMatrixMode(GL_MODELVIEW);
}


/* takes which keys are being held and acts accordingly */
void executekeys(){
	/*
	if(escheld){
		exit(0);
	}
	if(leftheld || rightheld){
		ry = 1.0f;
		if(leftheld){
			av = -1.0f;
		}
		if(rightheld){
			av = 1.0f;
		}
	} else{
		ry = 0.0f;
	}
	if(upheld || downheld){
		rx = 1.0f;
		if(upheld){
			av = -1.0f;
		}
		if(downheld){
			av = 1.0f;
		}
	} else{
		rx = 0.0f;
	}
	if(!(leftheld || rightheld || upheld || downheld)){
		av = 0.0f;
	}
	if(minusheld != plusheld){
		if(minusheld){
			dz = 0.5f;
		} else{
			dz = -0.5f;
		}
	} else{
		dz = 0.0f;
	} */
	int xbools = rightheld + leftheld;
	int ybools = upheld + downheld;
	if(!xbools){
		ry = 0.0f;
	}
	if(!ybools){
		rx = 0.0f;
	}
	if(!(xbools + ybools)){
		av = 0.0f;
	}
	if(!(minusheld + plusheld)){
		dz = 0.0f;
	}
}

/* registers when keys are first pressed */
void pressKey(int key, int xx, int yy){
	switch (key) {
		case 27 : exit(0); break;
		case GLUT_KEY_LEFT : leftheld = 1; ry = 1.0f; av = -1.0f; break;
		case GLUT_KEY_RIGHT : rightheld = 1; ry = 1.0f; av = 1.0f; break;
		case GLUT_KEY_UP : upheld = 1; rx = 1.0f; av = -1.0f; break;
		case GLUT_KEY_DOWN : downheld = 1; rx = 1.0f; av = 1.0f; break;
		case 61 : minusheld = 1; dz = 0.5f; break;
		case 45 : plusheld = 1; dz = -0.5f; break;
	}
	executekeys();
}

/* registers when keys are released */
void releaseKey(int key, int xx, int yy){
	switch (key) {
		case GLUT_KEY_LEFT : leftheld = 0; break;
		case GLUT_KEY_RIGHT : rightheld = 0; break;
		case GLUT_KEY_UP : upheld = 0; break;
		case GLUT_KEY_DOWN : downheld = 0; break;
		case 61 : minusheld = 0; break;
		case 45 : plusheld = 0; break;
	}
	executekeys();
}

/*
void pressKey(int key, int xx, int yy){
	switch (key) {
		case 27 : exit(0); break;
		case GLUT_KEY_LEFT : ry = 1.0f; av = -1.0f; break;
		case GLUT_KEY_RIGHT : ry = 1.0f; av = 1.0f; break;
		case GLUT_KEY_UP : rx = 1.0f; av = -1.0f; break;
		case GLUT_KEY_DOWN : rx = 1.0f; av = 1.0f; break;
		case 61 : dz = 0.5f; break;
		case 45 : dz = -0.5f; break;
	}
}

void releaseKey(int key, int xx, int yy){
	switch (key) {
		case GLUT_KEY_LEFT : ry = 0.0f; av = 0.0f; break;
		case GLUT_KEY_RIGHT : ry = 0.0f; av = 0.0f; break;
		case GLUT_KEY_UP : rx = 0.0f; av = 0.0f; break;
		case GLUT_KEY_DOWN : rx = 0.0f; av = 0.0f; break;
		case 61 : dz = 0; break;
		case 45 : dz = 0; break;
	}
}
*/

void mouseButton(int button, int state, int xx, int yy){
	// only start motion if the left button is pressed
	if (button == GLUT_LEFT_BUTTON) {
		// when the button is released
		if (state == GLUT_UP) {
			//cameraangle += ax;
			ax = 0;
			xOrigin = -1;
		}
		else  {// state = GLUT_DOWN
			xOrigin = xx;
			yOrigin = yy;
		}
	}
}

void mouseMove(int xx, int yy){
	// this will only be true when the left button is down
	if (xOrigin >= 0) {
		// update ax
		ax = (xx - xOrigin) * 0.001f;
		// update camera's direction
		lx = sin(cameraangle - ax);
		lz = -cos(cameraangle - ax);
	}
}

int main(int argc, char **argv){
	//Initialise GLUT with command-line parameters. 
	glutInit(&argc, argv);
	//Set Display Mode
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	//Set the window position
	glutInitWindowPosition(-1,-1);
	//Set the window size
	glutInitWindowSize(750,500);
	//Create the window
	glutCreateWindow("Someday to be 3d Chess");
	//select clearing (background) color
	glClearColor(0.50, 0.50, 0.50, 0.0);
	//initialize viewing values 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
	// Register callbacks
	glutDisplayFunc(display);
	glutReshapeFunc(changeSize);
	glutIdleFunc(display);
	//glutKeyboardFunc(processNormalKeys);
	glutSpecialFunc(pressKey);
	glutIgnoreKeyRepeat(1);
	glutSpecialUpFunc(releaseKey);
	glutMouseFunc(mouseButton);
	glutMotionFunc(mouseMove);
	//Enter the GLUT event loop
	glutMainLoop();
	return 0;
}
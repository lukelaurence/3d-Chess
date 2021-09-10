// clang chessgui.cpp -framework OpenGL -framework GLUT -o chess
#include "chesslogic.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#include <OpenGl/glu.h>
#include <GLUT/glut.h>

// the cube's angle of rotation
float rotationangle = 270.0f;
// angle of rotation for the camera direction
float cameraangle=0.0;
// actual vector representing the camera's direction
float lz=-1.0f;
// XYZ position of the camera
float px=0.0f,py=1.0f,pz=15.0f;
// the key states. These variables will be zero when no key is being pressed
// mouse distance displacement from key origin
float dxx = 0.0f, dyy = 0.0f;
// camera linear acceleration
float dz = 0.0f;
// mouse origins
int xOrigin = -1, yOrigin = -1;
// angular velocity
float av = 0.0f;
// key held booleans
int leftheld = 0, rightheld = 0, upheld = 0,
	downheld = 0, minusheld = 0, plusheld = 0;
// mouse booleans
int leftmouse = 0, rightmouse = 0;
// this might get around glut being unable to use pointers
board_t* gameboard;
// variable to log input mode
// 0 if not, 1 if the first term, and 6 if the 6th term. loops over.
int inputmode = 0;
int fromX = -16, fromY = -16, fromZ = -16, toX = -16, toY = -16, toZ = -16;

/* draws a cube of size 1 that is either black or white depending on input */
void drawcube(int input){
	int color = input % 2;
	glColor4f(color,color,color,0.3f);
	glutSolidCube(1.0f);
}

/* draws an 8 x 8 x 8 chessboard */
void drawboard(){
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
}

/* highlights a square, given RGB values and coordinates*/
void highlightsquare(float r, float g, float b, float x, float y, float z){
	glPushMatrix();
	glTranslatef(x,y,z);
	glColor4f(r,g,b,0.3);
	glutSolidCube(1.0f);
	glPopMatrix();
}

void highlightfrom(){
	if(inputmode == 2){
		for(int a = -4; a < 4; a++){
			for(int b = -4; b < 4; b++){
				highlightsquare(1,1,0,fromX-4,a,b);
			}
		}
	} else if(inputmode == 3){
		for(int a = -4; a < 4; a++){
			highlightsquare(1,1,0,fromX-4,fromY-4,a);
		}
	} else if(inputmode > 3){
		highlightsquare(1,1,0,fromX-4,fromY-4,fromZ-4);
	}
}

void highlightto(){
	if(inputmode == 5){
		for(int a = -4; a < 4; a++){
			for(int b = -4; b < 4; b++){
				highlightsquare(0,1,0,toX-4,a,b);
			}
		}
	} else if(inputmode == 6){
		for(int a = -4; a < 4; a++){
			highlightsquare(0,1,0,toX-4,toY-4,a);
		}
	} else if(inputmode > 6){
		highlightsquare(0,1,0,toX-4,toY-4,toZ-4);
	}
}

/* draws a character that represents a king */
void drawking(int color){
	glColor4f(color,color,color,0.75f);
	glutSolidCone(0.75f,0.75,7,5);
	glColor4f(1,1,0,0.75f);
	glutSolidTorus(0.1,0.4,5,10);
}

/* draws a character that represents a queen */
void drawqueen(int color){
	glColor4f(color,color,color,0.75f);
	glutSolidCone(0.75f,0.75,8,5);
	glColor4f(1,0,1,0.75f);
	glutSolidTorus(0.1,0.4,5,10);
}

/* draws a character that represents a bishop */
void drawbishop(int color){
	glColor4f(color,color,color,0.75f);
	glutSolidCone(0.75f,0.75,6,5);
	glColor4f(0,0,1,0.75f);
	glutSolidTorus(0.1,0.4,5,10);
}

/* draws a character that represents a knight */
void drawknight(int color){
	glColor4f(color,color,color,0.75f);
	glutSolidCone(0.75f,0.75,5,5);
	glColor4f(0,1,0,0.75f);
	glutSolidTorus(0.1,0.4,5,10);
}

/* draws a character that represents a rook */
void drawrook(int color){
	glColor4f(color,color,color,0.75f);
	glutSolidCone(0.75f,0.75,4,5);
	glColor4f(1,0,0,0.75f);
	glutSolidTorus(0.1,0.4,5,10);
}

/* draws a character that represents a pawn */
void drawpawn(int color){
	glColor4f(color,color,color,0.75f);
	glutSolidCone(0.75f,0.75,3,5);
	glColor4f(0,1,1,0.75f);
	glutSolidTorus(0.1,0.4,5,10);
}

/* takes a board and draws its pieces. it would be faster to iterate over the
piece arrays, but that misses queen promotions */
void drawpieces(board_t *input){
	for(int a = 0; a < 8; a++){
		for(int b = 0; b < 8; b++){
			for(int c = 0; c < 8; c++){
				glPushMatrix();
				glTranslatef(a-4,b-4,c-4.5f);
				int color = (input->square[a][b][c].side + 1) % 2;
				switch(input->square[a][b][c].piecetype){
					case 1 :
						drawking(color); break;
					case 2 :
						drawqueen(color); break;
					case 3 :
						drawbishop(color); break;
					case 4 :
						drawknight(color); break;
					case 5 :
						drawrook(color); break;
					case 6 :
						drawpawn(color); break;
				}
				glPopMatrix();
			}
		}
	}
}

/* takes a 0-9 char, and returns the corresponding integer */
int chartoint(char input){
	return(input - '0');
}

/* takes which keys are being held and acts accordingly */
void executekeys(){
	// move only when either key is held
	if(!(rightheld || leftheld) && !(upheld || downheld)){av = 0.0f;}
	if(!minusheld && !plusheld){dz = 0.0f;}
	// do ands here, first, if the diagonals don't automatically work
	if(leftheld == 1){av = -1;}
	if(rightheld == 1){av = 1;}
	if(upheld == 1){av = -1;}
	if(downheld == 1){av = 1;}
	if(minusheld == 1){dz = 0.5f;}
	if(plusheld == 1){dz = -0.5f;}
	if(inputmode == 1){
		fromX = -16; fromY = -16; fromZ = -16; toX = -16; toY = -16; toZ = -16;
	}
}

/* displays fonts */
void renderBitmapString(float x, float y, float z, void *font, char *string){
	glColor4f(0,0,0,0.75f);
	glRasterPos3f(x,y,z);
	for(int a = 0; a < strlen(string); a++){
		glutBitmapCharacter(font, string[a]);
	}
}

void rendertext(){
	if(gameboard->turn % 2){
		renderBitmapString(-8,-1,0,GLUT_BITMAP_TIMES_ROMAN_24,"Black to Move");
	} else{
		renderBitmapString(-8,-1,0,GLUT_BITMAP_TIMES_ROMAN_24,"White to Move");
	} if(inputmode){
		renderBitmapString(0,0,0,GLUT_BITMAP_TIMES_ROMAN_24,"Enter Move");
		char str[] = {'(',fromX+'1',',',fromY+'1',',',fromZ+'1',')',
		' ','t','o',' ','(',toX+'1',',',toY+'1',',',toZ+'1',')','\0'};
		renderBitmapString(0,0.75,0,GLUT_BITMAP_TIMES_ROMAN_24,str);
		switch(inputmode){
			case 1 :
				renderBitmapString(0,1,0,GLUT_BITMAP_TIMES_ROMAN_24,"Enter from X-component");
				break;
			case 2 :
				renderBitmapString(0,1,0,GLUT_BITMAP_TIMES_ROMAN_24,"Enter from Y-component");
				break;
			case 3 :
				renderBitmapString(0,1,0,GLUT_BITMAP_TIMES_ROMAN_24,"Enter from Z-component");
				break;
			case 4 :
				renderBitmapString(0,1,0,GLUT_BITMAP_TIMES_ROMAN_24,"Enter to X-component");
				break;
			case 5 :
				renderBitmapString(0,1,0,GLUT_BITMAP_TIMES_ROMAN_24,"Enter to Y-component");
				break;
			case 6 :
				renderBitmapString(0,1,0,GLUT_BITMAP_TIMES_ROMAN_24,"Enter to Z-component");
				break;
			case 7 :
				renderBitmapString(0,1,0,GLUT_BITMAP_TIMES_ROMAN_24,"Press Enter to confirm move");
				break;
		}
	}
}

/* main display loop function. renders everything */
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
	gluLookAt(	0.0f, 1.0f, pz,
				0.0f, 1.0f, pz+lz,
				0.0f, 1.0f, 0.0f);
	//glRotatef(rotationangle,rx,ry,rz);
	glRotatef(rotationangle,0,0,0);
	drawboard();
	//drawpieces(newgame());
	if(dz){pz += dz * lz * 0.1f;}
	executekeys();
	rotationangle+=av;
	drawpieces(gameboard);
	rendertext();
	highlightfrom();
	highlightto();
	glutSwapBuffers();
    // Don't wait start processing buffered OpenGL routines
    glFlush();
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

/* registers when keys are first pressed */
void pressKey(int key, int xx, int yy){
	switch (key) {
		case 27 :
			exit(0);
		case GLUT_KEY_LEFT :
			leftheld = 1; if(rightheld == 1){rightheld = 2;} break;
		case GLUT_KEY_RIGHT :
			rightheld = 1; if(leftheld == 1){leftheld = 2;} break;
		case GLUT_KEY_UP :
			upheld = 1; if(downheld == 1){downheld = 2;}  break;
		case GLUT_KEY_DOWN :
			downheld = 1; if(upheld == 1){upheld = 2;} break;
		case 61 :
			minusheld = 1; if(plusheld == 1){plusheld = 2;} break;
		case 45 :
			plusheld = 1; if(minusheld == 1){minusheld = 2;} break;
		case 13 :
			if(inputmode == 0){inputmode = 1;} break;
	} if(inputmode){
		if(key == 127){inputmode--;}
		if(key == 13 && inputmode == 7){
			printf("Turn on start: %i\n",gameboard->turn);
			move(gameboard,makeaddress(fromX,fromY,fromZ),
				makeaddress(toX,toY,toZ));
			inputmode = 0;
			printf("Turn on end: %i\n",gameboard->turn);
		}
		// restrict to 1-8 input
		else if(key > 48 && key < 57){
			// converts one-based numbering to zero-based internally
			int input = key - 49;
			switch(inputmode){
				case 1 :
					inputmode++; fromX = input; break;
				case 2 :
					fromY = input; inputmode++; break;
				case 3 :
					fromZ = input;
					/* // short circuit if move is already illegal
					if(!gameboard->square[fromX][fromY][fromZ].piecetype){
						renderBitmapString(0,0,0,GLUT_BITMAP_TIMES_ROMAN_24,
						"Invalid selection: Empty Square");
						inputmode = 1;
					} else if(gameboard->square[fromX][fromY][fromZ].side != gameboard->turn){
						renderBitmapString(0,0,0,GLUT_BITMAP_TIMES_ROMAN_24,
						"Invalid selection: Not your piece");
						inputmode = 1;
					} else{inputmode++;} break;*/
					inputmode++; break;
				case 4 :
					toX = input; inputmode++; break;
				case 5 :
					toY = input; inputmode++; break;
				case 6 :
					toZ = input;
					/*move(gameboard,makeaddress(fromX-1,fromY-1,fromZ-1),
					makeaddress(toX-1,toY-1,toZ-1));*/
					printf("piece type %i, side %i\n",
					gameboard->square[fromX][fromY][fromZ].piecetype,
					gameboard->square[fromX][fromY][fromZ].side);
					//inputmode = 0;
					inputmode++;
					break;
			}
		}
	}
}

/* registers when keys are released */
void releaseKey(int key, int xx, int yy){
	switch (key) {
		case GLUT_KEY_LEFT : leftheld = 0; if(rightheld == 2){rightheld = 1;} break;
		case GLUT_KEY_RIGHT : rightheld = 0; if(leftheld == 2){leftheld = 1;} break;
		case GLUT_KEY_UP : upheld = 0; if(downheld == 2){downheld = 1;} break;
		case GLUT_KEY_DOWN : downheld = 0; if(upheld == 2){upheld = 1;} break;
		case 61 : minusheld = 0; if(plusheld == 2){plusheld = 1;} break;
		case 45 : plusheld = 0; if(minusheld == 2){minusheld = 1;} break;
	}
}

/* registers when the mouse is pressed, and stores the x and y coordinates
in xOrigin and yOrigin while held */
void mouseButton(int button, int state, int xx, int yy){
	if(state == GLUT_DOWN){ //key pressed
		xOrigin = xx;
		yOrigin = yy;
		if(button == GLUT_LEFT_BUTTON){
			leftmouse = 1;
		} else if(button == GLUT_RIGHT_BUTTON){
			rightmouse = 1;
		}
	} else{ // key released
		if(button == GLUT_LEFT_BUTTON){
			leftmouse = 0;
		} else if(button == GLUT_RIGHT_BUTTON){
			rightmouse = 0;
		}
		if(!leftmouse && !rightmouse){
			xOrigin = -1;
			yOrigin = -1;
			dyy = 0;
			dxx = 0;
		}
	}
}

/* when the mouse is held, registers movement from the origin of the click */
void mouseMove(int xx, int yy){
	if(xOrigin >= 0){ // this is only true when the button is down
		// update dyy
		dyy = (xx - xOrigin) * 0.01f;
		// update dxx
		dxx = (yy - yOrigin) * 0.01f;
		float hypotenuse = sqrt(dxx * dxx + dyy * dyy);;
		if(leftmouse){
			// update rotation
			//rx += dxx;
			//ry += dyy;
			av = hypotenuse;
		} if(rightmouse){
			dz = hypotenuse * sgn(dxx + dyy);
		}
	}
}

void mousePassive(int xx, int yy){
	if(0){
		//rx *= 0.5;
		//ry *= 0.5;
		av *= 0.5;
	}
}

/* contains the glut portion of the main function for modularity */
void glutmain(){
	//Set Display Mode
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	//Set the window position
	glutInitWindowPosition(-1,-1);
	//Set the window size
	glutInitWindowSize(750,500);
	//Create the window
	glutCreateWindow("3d Chess");
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
	glutSpecialFunc(pressKey);
	glutIgnoreKeyRepeat(1);
	glutSpecialUpFunc(releaseKey);
	glutMouseFunc(mouseButton);
	glutMotionFunc(mouseMove);
	glutPassiveMotionFunc(mousePassive);
	//Enter the GLUT event loop
	glutMainLoop();
}

int main(int argc, char **argv){
	gameboard = malloc(2000000);
	resetboard(gameboard);
	//Initialise GLUT with command-line parameters. 
	glutInit(&argc, argv);
	glutmain();
	return 0;
}
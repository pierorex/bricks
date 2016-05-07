#include <iostream>
#include <GL\freeglut.h>

using namespace std;

const int NUMBER_OF_BRICKS = 7*5;


void changeSize(int w, int h) { // callback to render nicely if the screen gets resized by the user
	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if (h == 0) h = 1;

	float ratio =  w * 1.0 / h;

	// Use the Projection Matrix
	glMatrixMode(GL_PROJECTION);

	// Reset Matrix
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set the correct perspective
	gluPerspective(45.0f, ratio, 0.1f, 100.0f);

	// Get Back to the Modelview
	glMatrixMode(GL_MODELVIEW);
}


class Ball {
	float x, y;
} ball;


class Brick {
	float x, y;
} bricks[NUMBER_OF_BRICKS], bonus[NUMBER_OF_BRICKS];
// bricks move from the 'brick' array to the 'bonus' array if they contained a bonus


class Pad {
	float x, y;
} pad;


void render(){ // Function to be called by openGL in every cycle of the main loop
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// clear transformations
	glLoadIdentity();

	// set camera
	gluLookAt(0.0f, 0.0f, 30.0f, // camera position
			  0.0f, 0.0f, 0.0f,  // look at this point
			  0.0f, 1.0f, 0.0f); // camera's "tilt vector"

	// configurations
	glLineWidth(3);

	// detect collisions with bricks
	for (int i=0; i<NUMBER_OF_BRICKS; i++)
		if (ball.collidesBrick(brick[i].x, brick[i].y))
			brick[i].destroy();

	// detect collisions with bonuses
	for (int i=0; i<count_falling_bonuses(); i++) {
		if (bonus[i].is_falling && ball.collidesBrick(bonus[i].x, bonus[i].y)) {
			apply_effect(bonus.effect);
			bonus.destroy();
		}
	}

	// detect collisions with pad
	if (ball.collidesPad(pad.x, pad.y)) {
		ball.reflectSpeedVector(pad.x, pad.y);
	}

	// draw bricks
	for (int i=0; i<NUMBER_OF_BRICKS; i++)
		bricks[i].draw();

	// draw pad

	// draw ball

	glutSwapBuffers();
}

void processKeys(unsigned char key, int x, int y) {
	float magnitude = 10.0f;  // pad traslation magnitude

	if (key == 27) exit(0);
	
}

void processSpecialKeys(int key, int x, int y) {
	switch (key) {
	case GLUT_CURSOR_LEFT_ARROW:
		pad.moveLeft();
		break;
	case GLUT_CURSOR_RIGHT_ARROW:
		pad.moveRight();
		break;
	default:
		break;
	}
}

int main (int argc, char** argv) {
	// initialize glut
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800,600);
	glutCreateWindow("Opengl");

	// register callbacks
	glutDisplayFunc(render);
	glutIdleFunc(render);
	glutKeyboardFunc(processKeys);
	glutSpecialFunc(processSpecialKeys);
	glutReshapeFunc(changeSize);
	
	// enter main loop
	glutMainLoop();
	return 1;
}
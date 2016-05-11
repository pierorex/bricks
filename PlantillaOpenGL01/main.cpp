#include <iostream>
#include <vector>
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
public:
	float x, y, y_speed, x_speed, speed_magnitude, speed_direction;

	Ball(float _x, float _y, float _y_speed, float _x_speed, float _speed_magnitude, float _speed_direction) {
		x = _x;
		y = _y;
		y_speed = _y_speed;
		x_speed = _x_speed;
		speed_magnitude = _speed_magnitude;
		speed_direction = _speed_direction;
	}
} ball;


class Brick {
public:
	float x, y;
	bool has_bonus, is_falling, is_special;
	string effect;

	Brick(float _x, float _y, bool _has_bonus, bool _is_special) {
		x = _x;
		y = _y;
		has_bonus = _has_bonus;
		is_special = _is_special;
		if (has_bonus) is_falling = false;
	}

	void draw(){ glRectf(x-3, y+1, x+3, y-1); }
};
// bricks move from the 'brick' array to the 'bonus' array if they contained a bonus


class Pad {
public:
	float x, y, length, movement_magnitude;

	Pad(float _x, float _y, float _length, float _movement_magnitude) {
		x = _x;
		y = _y;
		length = _length;
		movement_magnitude = _movement_magnitude;
	}

	void draw() { glRectf(x-5, y, x+5, y-2); }
	void moveLeft() { x -= movement_magnitude; }
	void moveRight() { x += movement_magnitude; }
} pad;


void apply_effect(string effect) {
	if (effect.compare(string("ball_speed_up")) == 0) {
		ball.speed_magnitude *= 1.4;
	}
	if (effect.compare(string("pad_shrink")) == 0) {
		pad.length *= 0.85;
	}
}


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

	vector<Brick> bricks;

	// detect collisions with walls
	for (int i=0; i<walls.size(); i++) {
		if (ball.collidesWall(wall)) ball.reflectSpeedVector();
	}

	// detect collisions with bricks
	for (int i=0; i<bricks.size(); i++) {
		if (ball.collidesBrick(bricks[i].x, bricks[i].y)) {
			ball.reflectSpeedVector();
			bricks[i].destroy();
		}
	}
	
	// detect collisions with bonuses
	for (int i=0; i<bricks.size(); i++) {
		if (bricks[i].is_falling) {
			bricks[i].moveDown();

			if (ball.collidesBrick(bricks[i].x, bricks[i].y)) {
				ball.reflectSpeedVector();
				apply_effect(bricks[i].effect);
				bricks[i].destroyBonus();
			}
		}
	}

	// detect collisions with pad
	if (ball.collidesPad(pad.x, pad.y)) {
		ball.reflectSpeedVector();
	}

	// draw bricks
	for (int i=0; i<bricks.size(); i++)
		bricks[i].draw();

	// draw pad

	// draw ball

	glutSwapBuffers();
}

void processKeys(unsigned char key, int x, int y) {
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
#include <iostream>
#include <vector>
#include <GL\freeglut.h>

using namespace std;

const int NUMBER_COLUMNS_BRICKS = 7;
const int NUMBER_ROWS_BRICKS = 5;
const int NUMBER_OF_BRICKS = NUMBER_COLUMNS_BRICKS * NUMBER_ROWS_BRICKS;

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

void drawCircle(GLfloat x, GLfloat y, GLfloat radius){
	int i;
	int lineAmount = 100;
	GLfloat twicePi = 2.0f * 3.1416;
	glBegin(GL_LINE_LOOP);
		for(i = 0; i <= lineAmount;i++) {
			glVertex2f(
			    x + (radius * cos(i *  twicePi / lineAmount)),
			    y + (radius* sin(i * twicePi / lineAmount))
			);
		}
	glEnd();
}

void collisionLine(float xBrick, float yBrick, float xball, float yball, float ratio){
	// Collision in Y+
	if (y+1 == yball-ratio && xBrick+3 <= xball && xBrick-3 >= xball) return true;
	// Collision in Y-
	if (y-1 == yball+ratio && xBrick+3 <= xball && xBrick-3 >= xball) return true;
	// Collision in X+
	if (x+3 == xball-ratio && yBrick+1 <= yball && yBrick-1 >= yball) return true;
	// Collision in X-
	if (x-3 == xball+ratio && yBrick+1 <= yball && yBrick-1 >= yball) return true;
	return false;
}

void collisionCircle(float xBrick, float yBrick, float xball, float yball, float ratio){
	float x1 = (xBrick+3-xball)*(xBrick+3-xball);
	float x2 = (xBrick-3-xball)*(xBrick-3-xball);
	float y1 = (ybrick+1-yball)*(ybrick+1-yball);
	float y2 = (ybrick-1-yball)*(ybrick-1-yball);
	float r = ratio * ratio;
	if(x1 + y1 == r) return true;
	if(x1 + y2 == r) return true;
	if(x2 + y1 == r) return true;
	if(x1 + y2 == r) return true;
	return false;
}

class Ball {
public:
	float x, y, y_speed, x_speed, speed_magnitude, speed_direction, ratio;

	Ball(float _x, float _y, float _y_speed, float _x_speed, float _speed_magnitude, float _speed_direction, float _ratio) {
		x = _x;
		y = _y;
		ratio = _ratio;
		y_speed = _y_speed;
		x_speed = _x_speed;
		speed_magnitude = _speed_magnitude;
		speed_direction = _speed_direction;
	}

	void draw(){
		glTranslated();
		drawCircle(x, y, ratio);
	}

	void collidesBrick(float xBrick, float yBrick){
		// Collision in rect
		if (collisionLine(xBrick, yBrick, x, y, ratio)){
			return true;
		}
		// Collision Vertex
		if (collisionCircle(xBrick, yBrick, x, y, ratio)) {
			return true;
		}
		return false;
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

	void draw(){
		glRectf(x-3, y+1, x+3, y-1);

	}
	void bonus(){
		glRectf(x-2, y+1, x+2, y-1);
	}

} bricks[NUMBER_OF_BRICKS], bonus[NUMBER_OF_BRICKS];
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


class Wall {
public:
	float x1, y1, x2, y2;

	Wall(float _x1, float _y1, float _x2, float _y2) {
		x1 = _x1;
		y1 = _y1;
		x2 = _x2;
		y2 = _y2;
	}

	void draw() { }
};


void apply_effect(string effect) {
	if (effect.compare(string("ball_speed_up")) == 0) {
		ball.speed_magnitude *= 1.4;
	}
	if (effect.compare(string("pad_shrink")) == 0) {
		pad.length *= 0.85;
	}
}


vector<Brick> bricks;
vector<Wall> walls;


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
	for (int i=0; i<bricks.size(); i++) bricks[i].draw();

	// draw walls
	for (int i=0; i<walls.size(); i++) walls[i].draw();

	// draw pad
	pad.draw();

	// draw ball
	ball.draw();

	glutSwapBuffers();
}

void init_board() {
	for (int i=0; i<NUMBER_ROWS_BRICKS; i++) {
		for (int j=0; j<NUMBER_COLUMNS_BRICKS; j++)
			bricks.push_back(Brick());
	}

	walls.push_back(Wall());
	walls.push_back(Wall());
	walls.push_back(Wall());
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
	
	// initialize game board
	init_board();

	// enter main loop
	glutMainLoop();
	return 1;
}
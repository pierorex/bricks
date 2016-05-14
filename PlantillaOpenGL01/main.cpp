#include <iostream>
#include <vector>
#include <stdlib.h>
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

int collisionLine(float xBrick, float yBrick, float xball, float yball, float ratio, float longy, float width){
	// Collision in Y+
	if (yBrick+longy == yball-ratio && xBrick+width >= xball && xBrick-width <= xball) return 1;
	// Collision in Y-
	if (yBrick-longy == yball+ratio && xBrick+width >= xball && xBrick-width <= xball) return 1;
	// Collision in X+
	if (xBrick+width == xball-ratio && yBrick+longy >= yball && yBrick-longy <= yball) return 2;
	// Collision in X-
	if (xBrick-width == xball+ratio && yBrick+longy >= yball && yBrick-longy <= yball) return 2;
	return 0;
}

int collisionCircle(float xBrick, float yBrick, float xball, float yball, float ratio, float longy, float width){
	float x1 = (xBrick+width-xball)*(xBrick+width-xball);
	float x2 = (xBrick-width-xball)*(xBrick-width-xball);
	float y1 = (yBrick+longy-yball)*(yBrick+longy-yball);
	float y2 = (yBrick-longy-yball)*(yBrick-longy-yball);
	float r = ratio * ratio;
	if((x1 + y1) == r || (x1 + y2) == r || (x2 + y1) == r || (x1 + y2) == r) return 3;
	return 0;
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
		drawCircle(x, y, ratio);
	}

	int collidesBrick(float xBrick, float yBrick){
		// Collision in rect
		int a = collisionLine(xBrick, yBrick, x, y, ratio, 1, 3);
		if (a != 0){
			return a;
		}
		// Collision Corner
		a = collisionCircle(xBrick, yBrick, x, y, ratio, 1, 3);
		if (a != 0) {
			return a;
		}
		return 0;
	}

	int collidesWall(float x1, float y1, float x2, float y2){
		// Collision in Y+
		if (y1 == (y-ratio) && x1 <= x && x2 >= x) return 1;
		// Collision in Y-
		if (y2 == y+ratio && x1 <= x && x2 >= x) return 1;
		// Collision in X+
		if (x1 == x+ratio && y1 >= y && y2 <= y) return 2;
		// Collision in X-
		if (x2 == x-ratio && y1 >= y && y2 <= y) return 2;
		return 0;
	}

	int collidesPad(float xPad, float yPad, float len){
		// Collision in rect
		int a = collisionLine(xPad, yPad, x, y, ratio, 1, len); 
		if (a != 0){
			return a;
		}
		// Collision Vertex
		a = collisionCircle(xPad, yPad, x, y, ratio, 1, len);
		if (a != 0) {
			return a;
		}
		return 0;
	}

	void reflectSpeedVector(int collisionPoint){
		// Line
		if (collisionPoint == 1) y_speed = y_speed*(-1);
		if (collisionPoint == 2) x_speed = x_speed*(-1);
		// Corner
		if (collisionPoint == 3) {
			float aux = y_speed;
			y_speed = x_speed*(-1);
			x_speed = aux*(-1);
		}
	}

} ball(0.0, 3.0, 3.0, 3.0, 5.0, 5.0, 1.5);


class Brick {
public:
	float x, y, times;
	bool has_bonus, is_falling, is_special;
	string effect;

	Brick(float _x, float _y, bool _has_bonus, bool _is_special, float _times){
		x = _x;
		y = _y;
		has_bonus = _has_bonus;
		is_special = _is_special;
		times = _times;
		if (has_bonus) is_falling = false;
	}

	void draw(){
		if (is_special && times == 1.0){
			glRectf(x-3, y+1, x+3, y-1);
			glLineWidth(1.5); 
			glColor3f(1.0, 0.0, 0.0);
			
			glBegin(GL_LINES);
				glVertex3f(x-3, y+0.5, 0.0);
				glVertex3f(x+3, y+0.6, 0.0);
			glEnd();
			glBegin(GL_LINES);
				glVertex3f(x-3, y-0.5, 0.0);
				glVertex3f(x+3, y-0.6, 0.0);
			glEnd();

			glBegin(GL_LINES);
				glVertex3f(x, y+1, 0.0);
				glVertex3f(x+0.5, y+0.3, 0.0);
			glEnd();
			glBegin(GL_LINES);
				glVertex3f(x+0.5, y+0.3, 0.0);
				glVertex3f(x, y-0.3, 0.0);
			glEnd();
			glBegin(GL_LINES);
				glVertex3f(x, y-0.3, 0.0);
				glVertex3f(x+0.7, y-1, 0.0);
			glEnd();
		} else {
			glColor3f(0.0, 0.0, 1.0);
			if (is_special) glColor3f(0.0, 1.0, 0.0);
			glRectf(x-3, y+1, x+3, y-1);
		}
	}

	void bonus(){
		glRectf(x-2, y+1, x+2, y-1);
	}

	void destroy(){
	
	}

	void moveDown(){
	
	}

	void destroyBonus(){
	
	}
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

	void draw() { 
		float x1 = x-(length/2); 
		float x2 = x+(length/2);
		glRectf(x1, y+1.0, x2, y-1.0); 
	
	}
	void moveLeft() { x -= movement_magnitude; }
	void moveRight() { x += movement_magnitude; }
} pad(0.0, 0.0, 6.0, 2.0);


class Wall {
public:
	float x1, y1, x2, y2;

	Wall(float _x1, float _y1, float _x2, float _y2) {
		x1 = _x1;
		y1 = _y1;
		x2 = _x2;
		y2 = _y2;
	}

	void draw() { glRectf(x1, y1, x2, y2); }
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

	int aux = 0;
	// detect collisions with walls
	for (int i=0; i<walls.size(); i++) {
		aux = ball.collidesWall(walls[i].x1, walls[i].y1, walls[i].x2, walls[i].y2);
		if (aux != 0) ball.reflectSpeedVector(aux);
	}

	// detect collisions with bricks
	for (int i=0; i<bricks.size(); i++) {
		aux = ball.collidesBrick(bricks[i].x, bricks[i].y);
		if (aux != 0) {
			ball.reflectSpeedVector(aux);
			if (bricks[i].times == 1.0) bricks[i].destroy();
			if (bricks[i].times == 2.0) bricks[i].times = 1.0; 
		}
	}

	// detect collisions with bonuses
	for (int i=0; i<bricks.size(); i++) {
		if (bricks[i].is_falling) {
			bricks[i].moveDown();
			aux = ball.collidesBrick(bricks[i].x, bricks[i].y);
			if (aux != 0){
				ball.reflectSpeedVector(aux);
				apply_effect(bricks[i].effect);
				bricks[i].destroyBonus();
			}
		}
	}

	// detect collisions with pad
	aux = ball.collidesPad(pad.x, pad.y, pad.length);
	if (aux) {
		ball.reflectSpeedVector(aux);
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
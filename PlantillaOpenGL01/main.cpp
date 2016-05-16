#include <iostream>
#include <vector>
#include <string>
#include <stdio.h>      /* NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <GL\freeglut.h>

/* requirements for fonts */
#include <math.h>
#ifdef WIN32
#include <windows.h>
#endif
typedef enum {
    MODE_BITMAP,
    MODE_STROKE
} mode_type;

static mode_type mode;
static int font_index;

void print_bitmap_string(void* font, char* s) {
    if (s && strlen(s)) {
       while (*s) {
          glutBitmapCharacter(font, *s);
          s++;
       }
    }
}

void print_stroke_string(void* font, char* s){
    if (s && strlen(s)) {
       while (*s) {
          glutStrokeCharacter(font, *s);
          s++;
       }
    }
}

void init_fonts() {
    mode = MODE_BITMAP;
    font_index = 0;
}

void draw_message(float x, float y, char *str) {
    unsigned int i, j;
    unsigned int count;
    void* bitmap_fonts[7] = {
       GLUT_BITMAP_9_BY_15,
       GLUT_BITMAP_8_BY_13,
       GLUT_BITMAP_TIMES_ROMAN_10,
       GLUT_BITMAP_TIMES_ROMAN_24,
       GLUT_BITMAP_HELVETICA_10,
       GLUT_BITMAP_HELVETICA_12,
       GLUT_BITMAP_HELVETICA_18     
    };
 
    char* bitmap_font_names[7] = {
       "GLUT_BITMAP_9_BY_15",
       "GLUT_BITMAP_8_BY_13",
       "GLUT_BITMAP_TIMES_ROMAN_10",
       "GLUT_BITMAP_TIMES_ROMAN_24",
       "GLUT_BITMAP_HELVETICA_10",
       "GLUT_BITMAP_HELVETICA_12",
       "GLUT_BITMAP_HELVETICA_18"     
    };
    
    GLfloat ystep, yild, stroke_scale;
    
    /* Draw the string, according to the current mode and font. */
    glColor4f(0.0, 1.0, 0.0, 0.0);
    ystep  = 10.0; // 100.0
    yild   = 2.0;
    glRasterPos2f(-150, y+1.25*yild);
    print_bitmap_string(bitmap_fonts[font_index], bitmap_font_names[font_index]);
	glRasterPos2f(x, y);
	print_bitmap_string(bitmap_fonts[font_index], str);
}
/* end of requirements for fonts*/


using namespace std;

const int NUMBER_COLUMNS_BRICKS = 7;
const int NUMBER_ROWS_BRICKS = 5;
const int NUMBER_OF_BRICKS = NUMBER_COLUMNS_BRICKS * NUMBER_ROWS_BRICKS;
const float eps = 0.1f;

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

int collisionLine(float xBrick, float yBrick, float xball, float yball, float radius, float height, float width){
	
	float x1, y1, x2, y2, x, y;
	x = xball; y = yball; 
	x1 = xBrick - width; 
	x2 = xBrick + width;
	y1 = yBrick + height; 
	y2 = yBrick - height;
	// Collision in Y+
	float aux = y1 - (y-radius);
	if ( aux >= 0 && aux <= eps && x1 <= x && x2 >= x) return 1;
	// Collision in Y-
	aux = y2 - (y+radius);
	if ( aux >= 0 && aux <= eps && x1 <= x && x2 >= x) return 1;
	// Collision in X+
	aux = x1 - (x+radius);
	if (aux >= 0 && aux <= eps && y1 >= y && y2 <= y) return 2;
	// Collision in X-
	aux = x2 - (x-radius);
	if (aux >= 0 && aux <= eps && y1 >= y && y2 <= y) return 2;
	return 0;

}

void explote(float x, float y, float i){
	glColor3f(1.0, 1.0, 1.0);
	drawCircle(x, y+0.001*i, 0.5);
	drawCircle(x+0.001*i, y+0.001*i, 0.5);
	drawCircle(x-0.001*i, y+0.001*i, 0.5);
	drawCircle(x+0.003*i, y+0.003*i, 0.5);
	drawCircle(x-0.003*i, y+0.003*i, 0.5);
	drawCircle(x+0.005*i, y, 0.5);
	drawCircle(x-0.005*i, y, 0.5);
	drawCircle(x, y-0.001*i, 0.5);
	drawCircle(x-0.002*i, y-0.002*i, 0.5);
	drawCircle(x+0.002*i, y-0.002*i, 0.5);
	drawCircle(x+0.004*i, y-0.003*i, 0.5);
}

int collisionCircle(float xBrick, float yBrick, float xball, float yball, float radius, float height, float width, float x_speed, float y_speed){

	float x1 = xBrick - width - xball;
	float x2 = xBrick + width - xball;
	float y1 = yBrick + height - yball;
	float y2 = yBrick - height - yball;
	float r = radius * radius;

	if (x1*x1 + y1*y1 - r <= eps && x1*x1 + y1*y1 - r >= 0 && x_speed < 0 && y_speed < 0) return 1;
	else if (x1*x1 + y2*y2 - r <= eps && x1*x1 + y2*y2 - r >= 0 && x_speed < 0 && y_speed > 0) return 1;
	else if (x2*x2 + y1*y1 - r <= eps && x2*x2 + y1*y1 - r >= 0 && x_speed > 0 && y_speed < 0) return 1;
	else if (x2*x2 + y2*y2 - r <= eps && x2*x2 + y2*y2 - r >= 0 && x_speed > 0 && y_speed > 0) return 1;
	else if (x1*x1 + y1*y1 - r <= eps && x1*x1 + y1*y1 - r >= 0 && x_speed > 0 && y_speed > 0) return 2;
	else if (x1*x1 + y2*y2 - r <= eps && x1*x1 + y2*y2 - r >= 0 && x_speed > 0 && y_speed < 0) return 2;
	else if (x2*x2 + y1*y1 - r <= eps && x2*x2 + y1*y1 - r >= 0 && x_speed < 0 && y_speed > 0) return 2;
	else if (x2*x2 + y2*y2 - r <= eps && x2*x2 + y2*y2 - r >= 0 && x_speed < 0 && y_speed < 0) return 2;
	else if ((x1*x1 + y2*y2 - r <= eps && x1*x1 + y2*y2 - r >= 0)
	  ||(x2*x2 + y1*y1 - r <= eps && x2*x2 + y1*y1 - r >= 0)) return 3;
	else if (x1*x1 + y1*y1 - r <= eps && x1*x1 + y1*y1 - r >= 0) return 2;
	else if (x2*x2 + y2*y2 - r <= eps && x2*x2 + y2*y2 - r >= 0) return 2;
	else return 0;
}

class Ball {
public:
	float x, y, y_speed, x_speed, speed_magnitude, radius;

	Ball(float _x, float _y, float _y_speed, float _x_speed, float _speed_magnitude, float _radius) {
		x = _x;
		y = _y;
		radius = _radius;
		y_speed = _y_speed;
		x_speed = _x_speed;
		speed_magnitude = _speed_magnitude;
	}

	void draw(){
		glColor3f(0.0f, 0.0f, 1.0f);
		drawCircle(x, y, radius);
	}

	int collidesBrick(float xBrick, float yBrick){
		// Collision in rect
		int a = collisionLine(xBrick, yBrick, x, y, radius, 1, 3);
		if (a != 0){
			return a;
		} else {	
			a = collisionCircle(xBrick, yBrick, x, y, radius, 1, 3, x_speed, y_speed);
			if (a != 0) {
				return a;
			}
			return 0;
		}
	}

	int collidesWall(float x1, float y1, float x2, float y2){
		// Collision in Y+
		float aux = y1 - (y-radius);
		if ( aux >= 0 && aux <= eps && x1 <= x && x2 >= x) return 1;
		// Collision in Y-
		aux = y2 - (y+radius);
		if ( aux >= 0 && aux <= eps && x1 <= x && x2 >= x) return 1;
		// Collision in X+
		aux = x1 - (x+radius);
		if (aux >= 0 && aux <= eps && y1 >= y && y2 <= y) return 2;
		// Collision in X-
		aux = x2 - (x-radius);
		if (aux >= 0 && aux <= eps && y1 >= y && y2 <= y) return 2;
		return 0;
	}

	int collidesPad(float xPad, float yPad, float len){
		// Collision in rect
		int a = collisionLine(xPad, yPad, x, y, radius, 1, len/2); 
		if (a != 0){
			return a;
		} else {
			// Collision Vertex
			return collisionCircle(xPad, yPad, x, y, radius, 1, len/2, x_speed, y_speed);
		}
	}

	void reflectSpeedVector(int collisionPoint){
		// Line
		if (collisionPoint == 1) y_speed *= -1;
		else if (collisionPoint == 2) x_speed *= -1;
		else if (collisionPoint == 3) {
			float aux = y_speed;
			y_speed = x_speed*(-1);
			x_speed = aux*(-1);
		}
	}

	void updatePosition() {
		x += x_speed * speed_magnitude;
		y += y_speed * speed_magnitude;
	}

} ball(0.1, 4.0, -0.001, 0.001, 10.0, 1.5);


class Brick {
public:
	static int live_bricks;
	float x, y, falling_magnitude, i;
	int times, effect;
	bool has_bonus, is_falling, is_special;

	Brick(float _x, float _y){
		x = _x;
		y = _y;
		has_bonus = false;
		is_special = false;
		times = 1;
		is_falling = false;
		falling_magnitude = 0.01f;
		i = 0;
	}

	void draw(){

		if (is_special && times == 0 && i < 1000){
			explote(x, y, i);
			i++;
		}

		if (times == 0 && !is_falling) return;
		if (is_falling) {
			// draw as falling bonus
			glColor3f(1.0f + effect*1.0f, effect*1.0f, effect*1.0f);
			glRectf(x-1, y+1, x+1, y-1);
		}
		else if (is_special && times == 1){
			// draw as special brick that is almost broken
			glColor3f(0.0, 0.0, 1.0);
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
			// draw as normal or intact special brick
			glColor3f(0.0, 0.0, 1.0);
			if (is_special) glColor3f(0.0, 1.0, 0.0);
			glRectf(x-3, y+1, x+3, y-1);
			glColor3f(0.0, 0.0, 1.0);
		}
	}

	void bonus(){ glRectf(x-2, y+1, x+2, y-1); }

	void moveDown(){ y -= falling_magnitude; }

	void destroyBonus() { is_falling = false; }
}; 


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
	void moveLeft() {
		if (x - length >= -41.0f)
			x -= movement_magnitude; 
	}
	void moveRight() {
		if (x + length <= 41.0f)
			x += movement_magnitude;
	}

	int collidesBonus(float xBonus, float yBonus) {
		return collisionLine(xBonus, yBonus, x, y, 1, 1, 1);
	}
} pad(0.0, 0.0, 10.0, 2.0);


class Wall {
public:
	float x1, y1, x2, y2;

	Wall(float _x1, float _y1, float _x2, float _y2) {
		x1 = _x1;
		y1 = _y1;
		x2 = _x2;
		y2 = _y2;
	}

	void draw() {
		glColor3f(0.0f, 0.0f, 1.0f);
		glRectf(x1, y1, x2, y2); 
	}
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
vector<string> effects;


void render(){ // Function to be called by openGL in every cycle of the main loop
	float msg_x = -30.0, msg_y = 20.0, msg_y_separation = 5.0;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// clear transformations
	glLoadIdentity();

	// set camera
	gluLookAt(0.0f, 25.0f, 80.0f, // camera position
			  0.0f, 25.0f, 0.0f,  // look at this point
			  0.0f, 1.0f, 0.0f); // camera's "tilt vector"

	// configurations
	glLineWidth(3);

	// update ball position
	ball.updatePosition();

	int aux = 0;
	// detect collisions with walls
	for (int i=0; i<walls.size(); i++) {
		aux = ball.collidesWall(walls[i].x1, walls[i].y1, walls[i].x2, walls[i].y2);
		//printf("%d ", aux);
		if (aux != 0) ball.reflectSpeedVector(aux);
	}

	// detect collisions with bricks
	for (int i=0; i<bricks.size(); i++) {
		if (bricks[i].times == 0) continue;
		aux = ball.collidesBrick(bricks[i].x, bricks[i].y);
		if (aux != 0) {
			ball.reflectSpeedVector(aux);
			bricks[i].times--;
			if (bricks[i].times == 0) {
				Brick::live_bricks--;
				if (bricks[i].has_bonus) bricks[i].is_falling = true;
			}
		}
	}

	// update bonuses positions and detect collisions between them and the pad
	for (int i=0; i<bricks.size(); i++) {
		if (bricks[i].is_falling) {
			bricks[i].moveDown();
			aux = pad.collidesBonus(bricks[i].x, bricks[i].y);
			if (aux != 0){
				apply_effect(effects[bricks[i].effect]);
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

	// show winning message if there are no more bricks to break
	if (Brick::live_bricks == 0) { 
		draw_message(msg_x, msg_y, "You won! Congratulations :D");
		draw_message(msg_x, msg_y - msg_y_separation, "Press [ESC] to leave.");
	}

	// show losing message if the ball falls down
	if (ball.y < pad.y) {
		draw_message(msg_x, msg_y, "You lost.. u.u Try again next time!");
		draw_message(msg_x, msg_y - msg_y_separation, "Press [ESC] to leave.");
	}

	// draw pad and ball
	pad.draw();
	ball.draw();

	glutSwapBuffers();
}

int rand_int(int min, int max) { return min + (rand() % (int)(max - min + 1)); }

// define number of bricks alive from start
int Brick::live_bricks = NUMBER_OF_BRICKS;

void init_board() {
	int x,y;
	y = 45.0;
	for (int i=0; i<NUMBER_ROWS_BRICKS; i++) {
		x = -27.0;
		for (int j=0; j<NUMBER_COLUMNS_BRICKS; j++){
			bricks.push_back(Brick(x, y));
			x += 9.0;
		}
		y -= 5.0;
	}

	// initialize random seed
	srand(time(NULL));

	// put bonuses inside some random bricks
	int bonuses_left = 6, rand_brick, rand_effect;

	while (bonuses_left--) {
		rand_brick = rand_int(0, bricks.size()-1);
		rand_effect = rand_int(0, 1);
		bricks[rand_brick].has_bonus = true;
		bricks[rand_brick].effect = rand_effect;
	}

	// evolve some bricks to make them 'special'
	int specials_left = 5;
	while (specials_left--) {
		rand_brick = rand_int(0, bricks.size()-1);
		bricks[rand_brick].is_special = true;
		bricks[rand_brick].times = 2;
		
	}

	walls.push_back(Wall(-41.0, 50.0, -38.0, -3.0));
	walls.push_back(Wall(38.0, 50.0, 41.0, -3.0));
	walls.push_back(Wall(-41.0, 53.0, 41.0, 50.0));
	
	// TODO: remove this wall, it is just for testing
	//walls.push_back(Wall(-100.0, 0.0, 100.0, 0.0));

	effects.push_back(string("ball_speed_up"));
	effects.push_back(string("pad_shrink"));
}

void processKeys(unsigned char key, int x, int y) {
	if (key == 27) exit(0);

	switch (key) {
	case 27: 
		exit(0);
	break;
	case 'a':
	case 'A':
		pad.moveLeft();
	break;
	case 'd':
	case 'D':
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
	glutReshapeFunc(changeSize);
	
	// initialize game board
	init_board();

	// initialize fonts
	init_fonts();

	// enter main loop
	glutMainLoop();
	return 1;
}
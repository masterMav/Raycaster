#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#define PI 3.1415926535
#define DR 0.0174533

float px, py, pdx, pdy, pa;

void drawPlayer() {
	
	glColor3f(1, 1, 0);
	glPointSize(8);
	glBegin(GL_POINTS);
	glVertex2i(px, py);
	glEnd();
	
	glLineWidth(3);
	glBegin(GL_LINES);
	glVertex2i(px			 , py			 );
	glVertex2i(px + (pdx * 5), py + (pdy * 5));
	glEnd();
}

int mapX = 8, mapY = 8, mapS = 64;
int map[] = {
	
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 0, 1, 0, 0, 0, 0, 1,
	1, 0, 1, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 1, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 1,
	1, 1, 1, 1, 1, 1, 1, 1
};

void drawMap2D() {
	
	int x, y, xo, yo;
	for(y = 0; y < mapY; y++) {
		for(x = 0; x < mapX; x++) {
			
			if(map[(y * mapY) + x]) glColor3f(1, 1, 1); else glColor3f(0, 0, 0);
			xo = x * mapS; yo = y * mapS;
			glBegin(GL_QUADS);
			glVertex2i(xo + 1		, yo + 1		);
			glVertex2i(xo + 1		, yo + mapS - 1	);
			glVertex2i(xo + mapS - 1, yo + mapS - 1	);
			glVertex2i(xo + mapS - 1, yo + 1		);
			glEnd();
		}
	}
}

float dist(float ax, float ay, float bx, float by, float ang) {
	
	return sqrt( ((ax - bx) * (ax - bx)) + ((ay - by) * (ay - by)) );
}

void drawRays3D() {
	
	int r, dof, mx, my, mp;
	float rx, ry, ra, atan, xo, yo, disT;
	
	ra = pa - (DR * 30); if(ra < 0) { ra += (2 * PI); } if(ra > (2 * PI)) { ra -= (2 * PI); } 
	
	for(r = 0; r < 60; r++) {
		
		// HORIZONTAL LINE DETECTION
		dof = 0;
		atan = -1 / tan(ra);
		float disH = 1000000, hx = px, hy = py;
		
		// looking down
		if(ra > PI) { ry = (((int)py>>6)<<6) - 0.0001; rx = ((py - ry) * atan) + px;	yo = -64; xo = -yo * atan; }
		// looking up
		if(ra < PI)	{ ry = (((int)py>>6)<<6) + 64;	   rx = ((py - ry) * atan) + px;	yo = 64;  xo = -yo * atan; }
		// looking straight right or left
		if((ra == 0) || (ra == PI)) { rx = px; ry = py;	dof = 8; }
		
		while(dof < 8) {
			
			mx = (int)(rx)>>6; my = (int)(ry)>>6; mp = (my * mapX) + mx;
			if( (mp > 0) && (mp < (mapX * mapY)) && map[mp] ) { dof = 8; hx = rx; hy = ry; disH = dist(px, py, hx, hy, ra); }
			else { ry += yo; rx += xo; dof++; }
		}
		
		// VERTICAL LINE DETECTION
		dof = 0;
		atan = -tan(ra);
		float disV = 1000000, vx = px, vy = py;
		
		// looking left
		if( ((PI / 2) < ra) && (ra < ((3 * PI) / 2)) ) { 
		
			rx = (((int)px>>6)<<6) - 0.0001; ry = ((px - rx) * atan) + py; xo = -64; yo = -xo * atan;
		}
		// looking right
		if( ((PI / 2) > ra) || (ra > ((3 * PI) / 2)) ) {
			
			rx = (((int)px>>6)<<6) + 64; ry = ((px - rx) * atan) + py; xo = 64; yo = -xo * atan;			
		}
		// looking straight up or down
		if( (ra == (PI / 2)) || (ra == ((3 * PI) / 2)) ) {
			
			rx = px; ry = py; dof = 8;
		}
		
		while(dof < 8) {
			
			mx = (int)(rx)>>6;	my = (int)(ry)>>6;	mp = (my * mapX) + mx;
			if((mp > 0) && (mp < (mapX * mapY)) && map[mp] ) { dof = 8; vx = rx; vy = ry; disV = dist(px, py, vx, vy, ra); }
			else { ry += yo; rx += xo; dof++; }
		}
		
		if(disH < disV) { rx = hx;	ry = hy; disT = disH; glColor3f(0.7, 0, 0); }
		if(disV < disH) { rx = vx;	ry = vy; disT = disV; glColor3f(0.9, 0, 0); }	
		glLineWidth(3); glBegin(GL_LINES); glVertex2i(px, py); glVertex2i(rx, ry); glEnd();	
		
		// Draw 3D Walls
		float ca = pa - ra; if(ca < 0) { ca += (2 * PI); } if(ca > (2 * PI)) { ca -= (2 * PI); } disT = disT * cos(ca);
		float lineH = (mapS * 320) / disT;	if(lineH > 320) { lineH = 320; };	// line height
		float lineO = 160 - (lineH / 2); 	// line offset
		
		glLineWidth(8); glBegin(GL_LINES); glVertex2i(r*8+530, lineO); glVertex2i(r*8+530, lineO + lineH); glEnd();	
		
		ra += DR; if(ra < 0) { ra += (2 * PI); } if(ra > (2 * PI)) { ra -= (2 * PI); }
	}
}

void display()
{
 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 drawMap2D();
 drawRays3D();
 drawPlayer();
 glutSwapBuffers();
}

void buttons(unsigned char key, int x, int y) {
	
	if(key == 'a') { pa -= 0.1; if(pa < 		0) pa += (2 * PI); pdx = cos(pa) * 5; pdy = sin(pa) * 5; };
	if(key == 'd') { pa += 0.1; if(pa > (2 * PI) ) pa -= (2 * PI); pdx = cos(pa) * 5; pdy = sin(pa) * 5; };
	if(key == 'w') { px += pdx;	py += pdy; };
	if(key == 's') { px -= pdx; py -= pdy; };
	glutPostRedisplay();
}

void init () 
{
	glClearColor(0.3, 0.3, 0.3, 0);
	gluOrtho2D(0, 1024, 512, 0);
	px = 300;	py = 300;	pdx = cos(pa) * 5; pdy = sin(pa) * 5;
}

int main(int argc, char** argv)
{ 
 glutInit(&argc, argv);
 glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
 glutInitWindowSize(1024, 512);
 glutCreateWindow("raycaster");
 init();
 glutDisplayFunc(display);
 glutKeyboardFunc(buttons);
 glutMainLoop();
 return 0;
}

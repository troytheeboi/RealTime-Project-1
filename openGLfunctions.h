#ifndef openGL_H
#define openGL_H

extern int windowWidth;
extern int windowHeight;
extern int triangleWidth;
extern int triangleHeight;
extern int yOffset;
extern int circleRadius;


void drawItems(void);
void drawWhiteRectangle(void);
void drawTriangle(int x);
void drawCircle(int x,int y, int radius, int segments);
void display(void);
void reshape(int w, int h);

#endif
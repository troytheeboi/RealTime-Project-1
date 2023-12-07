// Global variables to store window size and triangle size
#include <GL/freeglut.h>
#include<math.h>
#include "fileReaders.h"
#include<stdio.h>


int windowWidth = 800;
int windowHeight = 600;
int triangleWidth = 100;
int triangleHeight = 100;
int yOffset = 90; // Shift triangles down by 90 units
int circleRadius = 50;


void drawItems() {
    glColor3f(0.0, 0.0, 0.0); // Set color to black for text
    glRasterPos2f(10, windowHeight - 10); // Initial position for the first item

    // List of items and their prices

    // Draw items and their prices in the same row with a bold-like effect
    for (int i = 0; i < itemCount; ++i) {
        char itemString[50]; // Assuming a reasonable length for the item string

        // Format the item and price string
        sprintf(itemString, "%s: %d", Item_arr[i].itemName, Item_arr[i].itemPrice);

        // Scale the text to achieve a larger font size
        glPushMatrix();
        glScalef(1.5, 1.5, 1.0); // Adjust the scaling factor as needed

        // Draw the text with scaled size
        glutBitmapString(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)itemString);

        glPopMatrix();

        // Move to the next position
        glRasterPos2f(10 + (i + 1) * 80, windowHeight - 10);
    }
}

void drawWhiteRectangle() {
    glBegin(GL_QUADS);
    glColor3f(1.0, 1.0, 1.0); // Set color to white
    glVertex2f(0.0, windowHeight);
    glVertex2f(0.0, windowHeight - 80.0);
    glVertex2f(windowWidth, windowHeight - 80.0);
    glVertex2f(windowWidth, windowHeight);
    glEnd();
}

void drawTriangle(int x) {
    glBegin(GL_TRIANGLES);
    glColor3f(1.0, 0.0, 0.0); // Set color to red
    glVertex2f(x, windowHeight - triangleHeight - yOffset);
    glVertex2f(x + triangleWidth / 2.0, windowHeight - yOffset);
    glVertex2f(x + triangleWidth, windowHeight - triangleHeight - yOffset);
    glEnd();
}

void drawCircle(int x, int y, int radius, int segments) {
    glBegin(GL_POLYGON);
    glColor3f(1.0, 0.0, 0.0); // Set color to red

    for (int i = 0; i < segments; ++i) {
 
        float theta = 2.0f * 3.1415926f * (float) i/ (float)segments;

        float xPos = x + radius * cosf(theta);
        float yPos = y + radius * sinf(theta);

        glVertex2f(xPos, yPos);
    }

    glEnd();
}


void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw the white rectangle above the shifted row of triangles
    drawWhiteRectangle();

    // Calculate the spacing between triangles
    int totalSpacing = windowWidth - NUM_CASHIERS * circleRadius; // 10 triangles in a row
    int spacing = totalSpacing / NUM_CASHIERS+1; // 11 spaces between 10 triangles

    // Draw 10 triangles with suitable spaces between them at the top of the window
    int currentPosition = spacing; // Start from the first spacing
    for (int i = 0; i < NUM_CASHIERS; ++i) {
        drawCircle(currentPosition, windowHeight-150, circleRadius, 100);
        currentPosition += (circleRadius + spacing); // Move to the next position
    }

    // Draw items and their prices
    drawItems();

    glutSwapBuffers();
}

void reshape(int w, int h) {
    windowWidth = w;
    windowHeight = h;

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, (GLdouble)w, 0.0, (GLdouble)h);
    glMatrixMode(GL_MODELVIEW);
}
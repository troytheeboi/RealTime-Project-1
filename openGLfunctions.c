// Global variables to store window size and triangle size
#include <GL/freeglut.h>
#include <math.h>
#include "fileReaders.h"
#include <stdio.h>
#include "openGLfunctions.h"
#include "cashierSm_Sem.h"

int windowWidth = 1800;
int windowHeight = 800;
int triangleWidth = 100;
int triangleHeight = 100;
int yOffset = 90; // Shift triangles down by 90 units
int circleRadius = 50;

void drawItems()
{
    glColor3f(0.0, 0.0, 0.0);             // Set color to black for text
    glRasterPos2f(10, windowHeight + 50); // Initial position for the first item

    // List of items and their prices
    sem_wait(itemsSemaphore);
    // Draw items and their prices in the same row with a bold-like effect
    for (int i = 1; i <= itemCount; ++i)
    {
        char itemString[60]; // Assuming a reasonable length for the item string

        // Format the item and price string
        sprintf(itemString, "%s : %d", Item_arr[i].itemName, Item_arr[i].quantity);

        // Scale the text to achieve a larger font size
        glPushMatrix();
        // glScalef(1.5, 1.5, 1.0); // Adjust the scaling factor as needed

        // Draw the text with scaled size
        glutBitmapString(GLUT_BITMAP_HELVETICA_12, (const unsigned char *)itemString);

        glPopMatrix();

        // Move to the next position
        glRasterPos2f((i) * 95, windowHeight + 50);
    }
    sem_signal(itemsSemaphore);
}

void drawWhiteRectangle()
{
    glBegin(GL_QUADS);
    glColor3f(1.0, 1.0, 1.0); // Set color to white
    glVertex2f(0.0, windowHeight + 100);
    glVertex2f(0.0, windowHeight);
    glVertex2f(windowWidth + 100, windowHeight);
    glVertex2f(windowWidth + 100, windowHeight + 100);
    glEnd();
}

// void drawTriangle(int x)
// {
//     glBegin(GL_TRIANGLES);
//     glColor3f(1.0, 0.0, 0.0); // Set color to red
//     glVertex2f(x, windowHeight - triangleHeight - yOffset);
//     glVertex2f(x + triangleWidth / 2.0, windowHeight - yOffset);
//     glVertex2f(x + triangleWidth, windowHeight - triangleHeight - yOffset);
//     glEnd();
// }

// void drawCircle(int x, int y, int radius, int segments)
// {
//     glBegin(GL_POLYGON);
//     glColor3f(1.0, 0.0, 0.0); // Set color to red

//     for (int i = 0; i < segments; ++i)
//     {

//         float theta = 2.0f * 3.1415926f * (float)i / (float)segments;

//         float xPos = x + radius * cosf(theta);
//         float yPos = y + radius * sinf(theta);

//         glVertex2f(xPos, yPos);
//     }

//     glEnd();
// }

// void display()
// {
//     glClear(GL_COLOR_BUFFER_BIT);

//     // Draw the white rectangle above the shifted row of triangles
//     drawWhiteRectangle();

//     // Calculate the spacing between triangles
//     int totalSpacing = windowWidth - NUM_CASHIERS * circleRadius; // 10 triangles in a row
//     int spacing = totalSpacing / NUM_CASHIERS + 1;                // 11 spaces between 10 triangles

//     // Draw 10 triangles with suitable spaces between them at the top of the window
//     int currentPosition = spacing; // Start from the first spacing
//     for (int i = 0; i < NUM_CASHIERS; ++i)
//     {
//         drawCircle(currentPosition, windowHeight - 150, circleRadius, 100);
//         currentPosition += (circleRadius + spacing); // Move to the next position
//     }

//     // Draw items and their prices
//     drawItems();

//     glutSwapBuffers();
// }

// void reshape(int w, int h)
// {
//     windowWidth = w;
//     windowHeight = h;

//     glViewport(0, 0, w, h);
//     glMatrixMode(GL_PROJECTION);
//     glLoadIdentity();
//     gluOrtho2D(0.0, (GLdouble)w, 0.0, (GLdouble)h);
//     glMatrixMode(GL_MODELVIEW);
// }

void drawString(const char *str)
{
    for (int i = 0; str[i] != '\0'; ++i)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, str[i]);
    }
}

void drawCashier(int cashier, int index, int queueSize, int itemsCount,int behavior, int tpi)
{

    const float margin = 70.0;
    const float squareWidth = 150;  // Margin value
    const float lineLength = 700.0; // Length of the vertical lines

    const float triangleSpacing = 90.0;

    // Draw the square (cashier queue information)
    glBegin(GL_QUADS);
    glColor3f(0.5, 0.5, 0.5);                               // Set color to white
    glVertex2f(margin, windowHeight - 100.0);               // Bottom-right vertex of the square
    glVertex2f(margin + squareWidth, windowHeight - 100.0); // Bottom-right vertex of the square
    glVertex2f(margin + squareWidth, windowHeight - 20);    // Top-right vertex of the square
    glVertex2f(margin, windowHeight - 20);                  // Top-right vertex of the square
    glEnd();

    // add the cashier information.
    glColor3f(0.0, 0.0, 0.0); // Set color to black for text

    char itemString[50];
    // Format the item and price string
    sprintf(itemString, "Cashier id: %d", cashier);
    glRasterPos2f(80, windowHeight - 40);
    drawString(itemString);
    sprintf(itemString, "Queue Size: %d", queueSize);
    glRasterPos2f(80, windowHeight - 60);
    drawString(itemString);
    sprintf(itemString, "Total Items Count: %d", itemsCount);
    glRasterPos2f(80, windowHeight - 80);

    float score;

    if (queueSize == 0 || itemsCount == 0)
    {
        score = (float) behavior / tpi;
    }
    else
    {
        score = (float)behavior / (tpi* queueSize* itemsCount);
    }
    sprintf(itemString, "Score: %f", score);
    glRasterPos2f(80, windowHeight - 80);

    // Scale the text to achieve a larger font size
    glPushMatrix();
    // glScalef(3.0, 1.0, 1.0); // Adjust the scaling factor as needed

    // Draw the text with scaled size
    glutBitmapString(GLUT_BITMAP_HELVETICA_12, (const unsigned char *)itemString);

    glPopMatrix();

    // Move to the next position
    // glRasterPos2f(margin +(index + 1) * 80, windowHeight - 90);

    // Draw the vertical lines
    glBegin(GL_LINES);
    glColor3f(1.0, 1.0, 1.0); // Set color to white
    // First vertical line
    glVertex2f(margin - 20, windowHeight - 70);
    glVertex2f(margin - 20, windowHeight - 200.0 - lineLength);

    // Second vertical line
    glVertex2f(margin + squareWidth + 20, windowHeight - 70);
    glVertex2f(margin + squareWidth + 20, windowHeight - 200.0 - lineLength);
    glEnd();

    // Draw the triangles using a for loop
    glBegin(GL_TRIANGLES);
    glColor4f(0.0, 1.0, 1.0, 1.0); // Set color to white
    for (int i = 0; i < queueSize; i++)
    {
        float yBase = windowHeight - 200.0 - i * triangleSpacing;

        glVertex2f(margin + squareWidth / 10, yBase);       // Bottom-left vertex of the triangle
        glVertex2f(margin + squareWidth / 10 + 120, yBase); // Bottom-right vertex of the triangle
        glVertex2f(margin + squareWidth / 2, yBase + 80);   // Top vertex of the triangle
    }
    glEnd();
}

void display()
{

    const float padding = 20.0; // Adjust the padding as needed

    glClear(GL_COLOR_BUFFER_BIT);
    drawWhiteRectangle();
    drawItems();

    float initialX = 20.0;                // Adjust the initial X coordinate as needed
    glColor3f(0.0, 0.0, 0.0);             // Set color to black for text
    glRasterPos2f(10, windowHeight - 20); // Initial position for the first item

    sem_wait(cashiersSemaphore);

    for (int i = 0; i < NUM_CASHIERS; i++)
    {

        if (Cashier_arr[i].cashierAvailable == 1)
        {

            glPushMatrix();                                                                                              // Save the current matrix
            glTranslatef(initialX, 0.0, 0.0);                                                                            // Translate to the current position
            drawCashier(Cashier_arr[i].cashierId, i, Cashier_arr[i].cashierQueueSize, Cashier_arr[i].totalItemsInQueue,Cashier_arr[i].behavior,Cashier_arr[i].timePerItem); // Assuming you want to pass the cashier ID (i + 1)
            glPopMatrix();                                                                                               // Restore the original matrix
            initialX += padding + 150.0;                                                                                 // Adjust the padding and cashier width as needed
        }
    }
    sem_signal(cashiersSemaphore);

    glutSwapBuffers();
    glutPostRedisplay();
}

void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, (GLdouble)w, 0.0, (GLdouble)h);
    glMatrixMode(GL_MODELVIEW);
}
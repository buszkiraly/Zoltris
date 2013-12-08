#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <set>

// Linux specific headers
#include <pthread.h>
#include <unistd.h>

// OpenGL headers
#include <GL/freeglut.h>
#include <GL/gl.h>

// Game headers
#include "cell.hpp"
#include "element.hpp"
#include "table.hpp"

// Window
#define W_HEIGHT                (700)
#define W_WIDTH                 (400)

// Board
#define HEIGHT                   (30) 
#define WIDTH                    (15)
#define TOP_X                     (0)
#define TOP_Y           (W_HEIGHT-70)
#define BOTTOM_X         (W_WIDTH-70)
#define BOTTOM_Y                  (0)  

using namespace std;

void display(void);
void GL_init(int argc, char** argv);
void *logic_thread_run(void*);
void timer(int v);
void clear_full_rows();
void keyboard_callback(unsigned char key,int x, int y);
void special_callback(int key,int x, int y);
void attachElement();
void reshape(GLsizei width, GLsizei height);

int FPS = 60;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


Element* element = NULL;
Table *table;
long score = 0;
float eyez = 0;

int main(int argc, char** argv){

    // Table init
    table = new Table(WIDTH,HEIGHT);

    // Element init
    element = new Element(table,rand()%7);

    // OpenGL Init
    GL_init(argc,argv);

    // Logic Thread init
    pthread_t logic_thr;
    pthread_create(&logic_thr,NULL,logic_thread_run,NULL);

    // Starting event loop
    glutMainLoop();

    return 0;
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers
    //glMatrixMode(GL_MODELVIEW);     // To operate on model-view matrix
    
     pthread_mutex_lock(&mutex);

    double cell_width = 0.1;
	
    vector<Cell*> cells = table->getCells();
    vector<Cell*> elem_cells;
    
    if (element != NULL) 
    {
        elem_cells = element->getCells();
        cells.reserve(cells.size() + elem_cells.size());
        cells.insert(cells.end(), elem_cells.begin(), elem_cells.end());
    }

    for (vector<Cell*>::iterator it = cells.begin(); it != cells.end(); ++it)
    {
    	Cell* cell = *it;
    	int i = cell->getY();
    	int j = cell->getX(); 
	
	if (i >= HEIGHT) continue;   

	glColor3f(cell->getR(), cell->getG(), cell->getB());
	glLoadIdentity(); 
	gluLookAt(0,0,eyez,0.0,0.0,-4,0,1,0);             
	glTranslatef(-cell_width*WIDTH/2+cell_width*j,-cell_width*HEIGHT/2+cell_width*i,-4.0);
	glutSolidCube(cell_width-0.01);
	
    } 
   
    glClear(GL_DEPTH_BUFFER_BIT);
  
    glFlush ();
  
    pthread_mutex_unlock(&mutex);
 
    //glFlush();
}
 
 
void GL_init(int argc, char** argv)
{
    glutInit(&argc,argv);
    glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(W_WIDTH,W_HEIGHT);
    glutInitWindowPosition(0,0);
    glutCreateWindow("Tetris");
    glutDisplayFunc(display);
    glutTimerFunc(100,timer,FPS);
    glutKeyboardFunc(keyboard_callback);
    glutSpecialFunc(special_callback);
    glutReshapeFunc(reshape);	
 
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black and opaque
    glClearDepth(1.0f);                   // Set background depth to farthest
    
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess[] = { 100};
    GLfloat light_position[] = { 5.0, -5.0, 5, 0.0 };
    glClearColor (1.0, 1.0, 1.0, 0.0);
    glShadeModel (GL_SMOOTH); 

    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glEnable(GL_DEPTH_TEST);   // Enable depth testing for z-culling
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glDepthFunc(GL_LEQUAL);    // Set the type of depth-test
    glShadeModel(GL_SMOOTH);   // Enable smooth shading
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // Nice perspective corrections

}

void *logic_thread_run(void*)
{
    while(1)
    {
        pthread_mutex_lock(&mutex);
        int rows = table->clear_full_rows();
	
	cout << "score: " << score << endl;
	
	if (rows) eyez = rows;
	
	switch (rows)
	{
	    case 1: score += 40; break;
	    case 2: score += 100; break;
	    case 3: score += 300; break;
	    case 4: score += 1200; break;
	}
	if (element)
	{
	    if (element->reachedBottom())
	    {
	        attachElement();   
	    }
	    element->step(DOWN);
	}
	pthread_mutex_unlock(&mutex);
	usleep(200000); 
    }
    
    return NULL;
}

void timer(int v)
{
    if (eyez > 0) eyez -= 0.08;
    else eyez = 0;
    glutPostRedisplay();
    glutTimerFunc(1000/FPS,timer,v);
}


void keyboard_callback(unsigned char key,int x, int y)
{
    if (key == 0x20) element->rotate();
}

void special_callback(int key,int x, int y)
{
    if (element == NULL) return; 
    
    int dir = -1;

    switch(key)
    {
    case GLUT_KEY_UP:
        element->rotate();
        break;
    case GLUT_KEY_DOWN:
        dir = DOWN;
        break;
    case GLUT_KEY_LEFT:
        dir = LEFT;
        break;
    case GLUT_KEY_RIGHT:
        dir = RIGHT;
        break;
    default:
        break;
    }
    
    element->step(dir);
   
}


void attachElement()
{
    table->attachCells(element->getCells());
    delete element;
    element = new Element(table,rand()%7);
    if (element->reachedBottom()) 
    {
        cout << "end" << endl;
	delete table;
	delete element;
	table = new Table(WIDTH, HEIGHT);
	element = new Element(table,rand()%7);
	score = 0;
    }
    
}

void reshape(GLsizei width, GLsizei height) {  // GLsizei for non-negative integer
   // Compute aspect ratio of the new window
   if (height == 0) height = 1;                // To prevent divide by 0
   GLfloat aspect = (GLfloat)width / (GLfloat)height;
 
   // Set the viewport to cover the new window
   glViewport(0, 0, width, height);
 
   // Set the aspect ratio of the clipping volume to match the viewport
   glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
   glLoadIdentity();             // Reset
   // Enable perspective projection with fovy, aspect, zNear and zFar
   gluPerspective(45.0f, aspect, 0.1f, 100.0f);
   glMatrixMode (GL_MODELVIEW);
}























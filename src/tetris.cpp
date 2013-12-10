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
#define HEIGHT                   (20) 
#define WIDTH                    (12)
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

// Texture mapping
/* storage for one texture  */
GLuint texture[1];

/* Image type - contains height, width, and data */
struct Image {
    unsigned long sizeX;
    unsigned long sizeY;
    char *data;
};
typedef struct Image Image;
int ImageLoad(char *filename, Image *image);
void LoadGLTextures();

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

    double cell_width = 0.15;
    
    vector<Cell*> cells = table->getCells();
    vector<Cell*> elem_cells;
    
   
    if (element != NULL) 
    {
        elem_cells = element->getCells();
        cells.reserve(cells.size() + elem_cells.size());
        cells.insert(cells.end(), elem_cells.begin(), elem_cells.end());
    }
    
    GLfloat color[4];
    GLfloat shininess[] = {50};

    for (vector<Cell*>::iterator it = cells.begin(); it != cells.end(); ++it)
    {
    	Cell* cell = *it;
    	int i = cell->getY();
    	int j = cell->getX(); 
	
	if (i >= HEIGHT) continue;   

        color[0] = 1;
        color[1] = 1;
        color[2] = 1;
	color[3] = 1.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);

	glLoadIdentity(); 
	gluLookAt(0,0,eyez,0.0,0.0,-4,0,1,0);             
	glTranslatef(-cell_width*(double)WIDTH/2+cell_width*j + cell_width/2,-cell_width*(double)HEIGHT/2+cell_width*i + cell_width/2,-4.0);
	//glutSolidCube(cell_width-0.01);
	
	glBegin(GL_QUADS);
         glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.9*cell_width/2, -0.9*cell_width/2, 0.9*cell_width/2);
         glTexCoord2f(1.0f, 0.0f); glVertex3f(0.9*cell_width/2, -0.9*cell_width/2, 0.9*cell_width/2);
         glTexCoord2f(1.0f, 1.0f); glVertex3f(0.9*cell_width/2, 0.9*cell_width/2, 0.9*cell_width/2);
         glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.9*cell_width/2, 0.9*cell_width/2, 0.9*cell_width/2);
        glEnd();
	glBegin(GL_QUADS);
         glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.9*cell_width/2, 0.9*cell_width/2, 0.9*cell_width/2);
         glTexCoord2f(1.0f, 0.0f); glVertex3f(0.9*cell_width/2, 0.9*cell_width/2, 0.9*cell_width/2);
         glTexCoord2f(1.0f, 1.0f); glVertex3f(0.9*cell_width/2, 0.9*cell_width/2, -0.9*cell_width/2);
         glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.9*cell_width/2, 0.9*cell_width/2, -0.9*cell_width/2);
        glEnd();
	glBegin(GL_QUADS);
         glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.9*cell_width/2, -0.9*cell_width/2, 0.9*cell_width/2);
         glTexCoord2f(1.0f, 0.0f); glVertex3f(0.9*cell_width/2, -0.9*cell_width/2, 0.9*cell_width/2);
         glTexCoord2f(1.0f, 1.0f); glVertex3f(0.9*cell_width/2, -0.9*cell_width/2, -0.9*cell_width/2);
         glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.9*cell_width/2, -0.9*cell_width/2, -0.9*cell_width/2);
        glEnd();
	glBegin(GL_QUADS);
         glTexCoord2f(0.0f, 0.0f); glVertex3f(0.9*cell_width/2, -0.9*cell_width/2, 0.9*cell_width/2);
         glTexCoord2f(1.0f, 0.0f); glVertex3f(0.9*cell_width/2, -0.9*cell_width/2, -0.9*cell_width/2);
         glTexCoord2f(1.0f, 1.0f); glVertex3f(0.9*cell_width/2, 0.9*cell_width/2, -0.9*cell_width/2);
         glTexCoord2f(0.0f, 1.0f); glVertex3f(0.9*cell_width/2, 0.9*cell_width/2, 0.9*cell_width/2);
        glEnd();
	glBegin(GL_QUADS);
         glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.9*cell_width/2, -0.9*cell_width/2, -0.9*cell_width/2);
         glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.9*cell_width/2, -0.9*cell_width/2, 0.9*cell_width/2);
         glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.9*cell_width/2, 0.9*cell_width/2, 0.9*cell_width/2);
         glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.9*cell_width/2, 0.9*cell_width/2, -0.9*cell_width/2);
        glEnd();
	
    } 
    

    color[0] = 0.1;
    color[1] = 0.1;
    color[2] = 0.1;
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
    glLoadIdentity(); 
    gluLookAt(0,0,eyez,0.0,0.0,-4,0,1,0);
    glBindTexture(GL_TEXTURE_2D, texture[0]);   // choose the texture to use.
   
   
    // Bounding box 
    glBegin(GL_QUADS);
      glTexCoord2f(0.0f, 0.0f); glVertex3f(-cell_width*(double)WIDTH/2 + 0.1*cell_width,-cell_width*(double)HEIGHT/2 + 0.1*cell_width,-4  - 0.9*cell_width/2);
      glTexCoord2f(1.0f, 0.0f); glVertex3f(cell_width*(double)WIDTH/2 - 0.1*cell_width,-cell_width*(double)HEIGHT/2 + 0.1*cell_width,-4  - 0.9*cell_width/2);
      glTexCoord2f(1.0f, 1.0f); glVertex3f(cell_width*(double)WIDTH/2 - 0.1*cell_width,cell_width*(double)HEIGHT/2 - 0.1*cell_width,-4  - 0.9*cell_width/2);
      glTexCoord2f(0.0f, 1.0f); glVertex3f(-cell_width*(double)WIDTH/2 + 0.1*cell_width, cell_width*(double)HEIGHT/2 - 0.1*cell_width,-4 - 0.9*cell_width/2);
    glEnd();
    color[0] = 0;
    color[1] = 1;
    color[2] = 0;
    //glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
    glBegin(GL_QUADS);
      glVertex3f(-cell_width*(double)WIDTH/2 + 0.1*cell_width,-cell_width*(double)HEIGHT/2 + 0.1*cell_width,-4  - 0.9*cell_width/2);
      glVertex3f(-cell_width*(double)WIDTH/2 + 0.1*cell_width,-cell_width*(double)HEIGHT/2 + 0.1*cell_width,-4  + 0.9*cell_width/2);      
      glVertex3f(cell_width*(double)WIDTH/2 - 0.1*cell_width,-cell_width*(double)HEIGHT/2 + 0.1*cell_width,-4  + 0.9*cell_width/2);      
      glVertex3f(cell_width*(double)WIDTH/2 - 0.1*cell_width,-cell_width*(double)HEIGHT/2 + 0.1*cell_width,-4  - 0.9*cell_width/2);
    glEnd();
    glBegin(GL_QUADS);
      glVertex3f(cell_width*(double)WIDTH/2 - 0.1*cell_width,-cell_width*(double)HEIGHT/2 + 0.1*cell_width,-4  - 0.9*cell_width/2);          
      glVertex3f(cell_width*(double)WIDTH/2 - 0.1*cell_width,-cell_width*(double)HEIGHT/2 + 0.1*cell_width,-4  + 0.9*cell_width/2); 
      glVertex3f(cell_width*(double)WIDTH/2 - 0.1*cell_width, cell_width*(double)HEIGHT/2 - 0.1*cell_width,-4 + 0.9*cell_width/2);
      glVertex3f(cell_width*(double)WIDTH/2 - 0.1*cell_width,cell_width*(double)HEIGHT/2 - 0.1*cell_width,-4  - 0.9*cell_width/2);
    glEnd();
    glBegin(GL_QUADS); 
      glVertex3f(cell_width*(double)WIDTH/2 - 0.1*cell_width,cell_width*(double)HEIGHT/2 - 0.1*cell_width,-4  - 0.9*cell_width/2);
      glVertex3f(cell_width*(double)WIDTH/2 - 0.1*cell_width,cell_width*(double)HEIGHT/2 - 0.1*cell_width,-4  + 0.9*cell_width/2);
      glVertex3f(-cell_width*(double)WIDTH/2 + 0.1*cell_width, cell_width*(double)HEIGHT/2 - 0.1*cell_width,-4 + 0.9*cell_width/2);
      glVertex3f(-cell_width*(double)WIDTH/2 + 0.1*cell_width, cell_width*(double)HEIGHT/2 - 0.1*cell_width,-4 - 0.9*cell_width/2);
    glEnd();
    glBegin(GL_QUADS);
      glVertex3f(-cell_width*(double)WIDTH/2 + 0.1*cell_width, cell_width*(double)HEIGHT/2 - 0.1*cell_width,-4 + 0.9*cell_width/2);
      glVertex3f(-cell_width*(double)WIDTH/2 + 0.1*cell_width, cell_width*(double)HEIGHT/2 - 0.1*cell_width,-4 - 0.9*cell_width/2);
      glVertex3f(-cell_width*(double)WIDTH/2 + 0.1*cell_width,-cell_width*(double)HEIGHT/2 + 0.1*cell_width,-4  - 0.9*cell_width/2);
      glVertex3f(-cell_width*(double)WIDTH/2 + 0.1*cell_width,-cell_width*(double)HEIGHT/2 + 0.1*cell_width,-4  + 0.9*cell_width/2);  
    glEnd();
    
    /*
    // Bounding box skeleton
    glColor3f(1,0,0);
    glBegin(GL_LINE_LOOP);
      glVertex3f(-cell_width*(double)WIDTH/2 + 0.1*cell_width,-cell_width*(double)HEIGHT/2 + 0.1*cell_width,-4  - 0.9*cell_width/2);
      glVertex3f(cell_width*(double)WIDTH/2 - 0.1*cell_width,-cell_width*(double)HEIGHT/2 + 0.1*cell_width,-4  - 0.9*cell_width/2);
      glVertex3f(cell_width*(double)WIDTH/2 - 0.1*cell_width,cell_width*(double)HEIGHT/2 - 0.1*cell_width,-4  - 0.9*cell_width/2);
      glVertex3f(-cell_width*(double)WIDTH/2 + 0.1*cell_width,cell_width*(double)HEIGHT/2 - 0.1*cell_width,-4  - 0.9*cell_width/2);
    glEnd();
    glBegin(GL_LINE_LOOP);
      glVertex3f(-cell_width*(double)WIDTH/2 + 0.1*cell_width,-cell_width*(double)HEIGHT/2 + 0.1*cell_width,-4  + 0.9*cell_width/2);
      glVertex3f(cell_width*(double)WIDTH/2 - 0.1*cell_width,-cell_width*(double)HEIGHT/2 + 0.1*cell_width,-4 + 0.9*cell_width/2);
      glVertex3f(cell_width*(double)WIDTH/2 - 0.1*cell_width, cell_width*(double)HEIGHT/2 - 0.1*cell_width,-4 + 0.9*cell_width/2);
      glVertex3f(-cell_width*(double)WIDTH/2 + 0.1*cell_width,cell_width*(double)HEIGHT/2 - 0.1*cell_width,-4 + 0.9*cell_width/2);
    glEnd();
    glBegin(GL_LINE_LOOP);
      glVertex3f(-cell_width*(double)WIDTH/2 + 0.1*cell_width,-cell_width*(double)HEIGHT/2 + 0.1*cell_width,-4  - 0.9*cell_width/2);
      glVertex3f(-cell_width*(double)WIDTH/2 + 0.1*cell_width,-cell_width*(double)HEIGHT/2 + 0.1*cell_width,-4  + 0.9*cell_width/2);
    glEnd();
    glBegin(GL_LINE_LOOP);
      glVertex3f(cell_width*(double)WIDTH/2 - 0.1*cell_width,-cell_width*(double)HEIGHT/2 + 0.1*cell_width,-4  - 0.9*cell_width/2);
      glVertex3f(cell_width*(double)WIDTH/2 - 0.1*cell_width,-cell_width*(double)HEIGHT/2 + 0.1*cell_width,-4  + 0.9*cell_width/2);
    glEnd();
    glBegin(GL_LINE_LOOP); 
      glVertex3f(cell_width*(double)WIDTH/2 - 0.1*cell_width,cell_width*(double)HEIGHT/2 - 0.1*cell_width,-4  - 0.9*cell_width/2);
      glVertex3f(cell_width*(double)WIDTH/2 - 0.1*cell_width,cell_width*(double)HEIGHT/2 - 0.1*cell_width,-4  + 0.9*cell_width/2);
    glEnd();
    glBegin(GL_LINE_LOOP);
      glVertex3f(-cell_width*(double)WIDTH/2 + 0.1*cell_width,cell_width*(double)HEIGHT/2 - 0.1*cell_width,-4  - 0.9*cell_width/2);
      glVertex3f(-cell_width*(double)WIDTH/2 + 0.1*cell_width,cell_width*(double)HEIGHT/2 - 0.1*cell_width,-4  + 0.9*cell_width/2);
    glEnd();
    */
    
   
    glClear(GL_DEPTH_BUFFER_BIT);
  
    glFlush ();
  
    pthread_mutex_unlock(&mutex);

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
    glClearDepth(5.0f);                   // Set background depth to farthest
    
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 0.0 };
    GLfloat mat_shininess[] = { 50};
    GLfloat light_position[] = { 0.0, 0, -2, 1.0 };

    LoadGLTextures();				// Load The Texture(s) 
    glEnable(GL_TEXTURE_2D);
    //glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    //glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glEnable(GL_DEPTH_TEST);   // Enable depth testing for z-culling
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    //glEnable(GL_COLOR_MATERIAL);
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

// quick and dirty bitmap loader...for 24 bit bitmaps with 1 plane only.  
// See http://www.dcs.ed.ac.uk/~mxr/gfx/2d/BMP.txt for more info.
int ImageLoad(char *filename, Image *image) {
    FILE *file;
    unsigned long size;                 // size of the image in bytes.
    unsigned long i;                    // standard counter.
    unsigned short int planes;          // number of planes in image (must be 1) 
    unsigned short int bpp;             // number of bits per pixel (must be 24)
    char temp;                          // temporary color storage for bgr-rgb conversion.

    // make sure the file is there.
    if ((file = fopen(filename, "rb"))==NULL)
    {
	printf("File Not Found : %s\n",filename);
	return 0;
    }
    
    // seek through the bmp header, up to the width/height:
    fseek(file, 18, SEEK_CUR);

    // read the width
    if ((i = fread(&image->sizeX, 4, 1, file)) != 1) {
	printf("Error reading width from %s.\n", filename);
	return 0;
    }
    printf("Width of %s: %lu\n", filename, image->sizeX);
    
    // read the height 
    if ((i = fread(&image->sizeY, 4, 1, file)) != 1) {
	printf("Error reading height from %s.\n", filename);
	return 0;
    }
    printf("Height of %s: %lu\n", filename, image->sizeY);
    
    // calculate the size (assuming 24 bits or 3 bytes per pixel).
    size = image->sizeX * image->sizeY * 3;

    // read the planes
    if ((fread(&planes, 2, 1, file)) != 1) {
	printf("Error reading planes from %s.\n", filename);
	return 0;
    }
    if (planes != 1) {
	printf("Planes from %s is not 1: %u\n", filename, planes);
	return 0;
    }

    // read the bpp
    if ((i = fread(&bpp, 2, 1, file)) != 1) {
	printf("Error reading bpp from %s.\n", filename);
	return 0;
    }
    if (bpp != 24) {
	printf("Bpp from %s is not 24: %u\n", filename, bpp);
	return 0;
    }
	
    // seek past the rest of the bitmap header.
    fseek(file, 24, SEEK_CUR);

    // read the data. 
    image->data = (char *) malloc(size);
    if (image->data == NULL) {
	printf("Error allocating memory for color-corrected image data");
	return 0;	
    }

    if ((i = fread(image->data, size, 1, file)) != 1) {
	printf("Error reading image data from %s.\n", filename);
	return 0;
    }

    for (i=0;i<size;i+=3) { // reverse all of the colors. (bgr -> rgb)
	temp = image->data[i];
	image->data[i] = image->data[i+2];
	image->data[i+2] = temp;
    }
    
    // we're done.
    return 1;
}

// Load Bitmaps And Convert To Textures
void LoadGLTextures() {	
    // Load Texture
    Image *image1;
    
    // allocate space for texture
    image1 = (Image *) malloc(sizeof(Image));
    if (image1 == NULL) {
	printf("Error allocating space for image");
	exit(0);
    }

    if (!ImageLoad("box.bmp", image1)) {
	exit(1);
    }        

    // Create Texture	
    glGenTextures(1, &texture[0]);
    glBindTexture(GL_TEXTURE_2D, texture[0]);   // 2d texture (x and y size)

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // scale linearly when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // scale linearly when image smalled than texture

    // 2d texture, level of detail 0 (normal), 3 components (red, green, blue), x size from image, y size from image, 
    // border 0 (normal), rgb color data, unsigned byte data, and finally the data itself.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image1->sizeX, image1->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image1->data);
};






















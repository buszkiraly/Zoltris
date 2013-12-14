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
#define W_WIDTH                 (800)

// Board
#define HEIGHT                   (20) 
#define WIDTH                    (12)
#define TOP_X                     (0)
#define TOP_Y           (W_HEIGHT-70)
#define BOTTOM_X         (W_WIDTH-70)
#define BOTTOM_Y                  (0)
#define BACKGROUND_TEXTURE_NUM    (7)
#define TABLE_TEXTURE_NUM         (BACKGROUND_TEXTURE_NUM + 1)
#define NUMBER_OF_TEXTURES        (BACKGROUND_TEXTURE_NUM + 2)

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


Element* elements[2];
Table *table;
long score = 0;
float eyez = 0;

// Texture mapping
/* storage for one texture  */
GLuint texture[7];

/* Image type - contains height, width, and data */
struct Image {
    unsigned short sizeX;
    unsigned short sizeY;
    unsigned char *data;
};
typedef struct Image Image;
int ImageLoad(char *filename, Image *image);
void LoadGLTextures();

int main(int argc, char** argv){
/*
    Image image;
    ImageLoad("32bit.bmp", &image);
    exit(1);
*/
    // Table init
    table = new Table(WIDTH,HEIGHT);

    // elements[0] init
    elements[0] = new Element(table,rand()%7);
    elements[0]->spawn();
    elements[1] = new Element(table,rand()%7);

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
    
    pthread_mutex_lock(&mutex);

    double cell_width = 0.15;

    glLoadIdentity(); 
    gluLookAt(0,0,0,0.0,0.0,-4,0,1,0);
   
    glBindTexture(GL_TEXTURE_2D, texture[BACKGROUND_TEXTURE_NUM]); 
   
    // Background
    glBegin(GL_QUADS);
      glTexCoord2f(0.0f, 0.0f); glVertex3f(-10,-10,-20);
      glTexCoord2f(1.0f, 0.0f); glVertex3f(10,-10,-20);
      glTexCoord2f(1.0f, 1.0f); glVertex3f(10,10,-20);
      glTexCoord2f(0.0f, 1.0f); glVertex3f(-10,10,-20);
    glEnd();
    
    // Bounding box 
    glColor4f(1.0,1.0,1.0,0.1);
    glDisable(GL_TEXTURE_2D);
    glLoadIdentity(); 
    gluLookAt(0,0,eyez,0.0,0.0,-4,0,1,0);
    // Bounding box 
    glBegin(GL_QUADS);
      glVertex3f(-cell_width*(double)WIDTH/2 + 0.1*cell_width,-cell_width*(double)HEIGHT/2 + 0.1*cell_width,-4  - 0.9*cell_width/2);
      glVertex3f(cell_width*(double)WIDTH/2 - 0.1*cell_width,-cell_width*(double)HEIGHT/2 + 0.1*cell_width,-4  - 0.9*cell_width/2);
      glVertex3f(cell_width*(double)WIDTH/2 - 0.1*cell_width,cell_width*(double)HEIGHT/2 - 0.1*cell_width,-4  - 0.9*cell_width/2);
      glVertex3f(-cell_width*(double)WIDTH/2 + 0.1*cell_width, cell_width*(double)HEIGHT/2 - 0.1*cell_width,-4 - 0.9*cell_width/2);
    glEnd();
    
    vector<Cell*> cells = table->getCells();
    vector<Cell*> elem_cells;
   
    if (elements[0] && elements[1]) 
    {
        elem_cells = elements[0]->getCells();
        cells.reserve(cells.size() + elem_cells.size());
        cells.insert(cells.end(), elem_cells.begin(), elem_cells.end());
	
	elements[1]->setCenter(WIDTH + 3, HEIGHT - 3);
	elem_cells = elements[1]->getCells();
        cells.reserve(cells.size() + elem_cells.size());
        cells.insert(cells.end(), elem_cells.begin(), elem_cells.end());
    }
    

    glColor4f(1,1,1,1);
    glEnable(GL_TEXTURE_2D);

    for (vector<Cell*>::iterator it = cells.begin(); it != cells.end(); ++it)
    {
    	Cell* cell = *it;
    	int i = cell->getY();
    	int j = cell->getX(); 
	
	if (i >= HEIGHT) continue;   


	glLoadIdentity(); 
        gluLookAt(0,0,eyez,0.0,0.0,-4,0,1,0);     
	glTranslatef(-cell_width*(double)WIDTH/2+cell_width*j + cell_width/2,-cell_width*(double)HEIGHT/2+cell_width*i + cell_width/2,-4.0);
	
	glBindTexture(GL_TEXTURE_2D, texture[cell->getType()]);   // choose the texture to use.
	
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
	
	glClear(GL_DEPTH_BUFFER_BIT);
	
    } 
    

       

   
   
/*
    color[0] = 0;
    color[1] = 0.3;
    color[2] = 0;
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
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
    */
  
    glClear(GL_DEPTH_BUFFER_BIT);
  
    glFlush();
    glutSwapBuffers();
  
    pthread_mutex_unlock(&mutex);

}
 
 
void GL_init(int argc, char** argv)
{
    glutInit(&argc,argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
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
    //glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glEnable(GL_DEPTH_TEST);   // Enable depth testing for z-culling
    //glEnable(GL_LIGHTING);
    //glEnable(GL_LIGHT0);
    //glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_BLEND); 
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glShadeModel (GL_FLAT);
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
	if (elements[0])
	{
	    if (elements[0]->reachedBottom())
	    {
	        attachElement();   
	    }
	    elements[0]->step(DOWN);
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
    if (key == 0x20) elements[0]->rotate();
}

void special_callback(int key,int x, int y)
{
    if (elements[0] == NULL) return; 
    
    int dir = -1;

    switch(key)
    {
    case GLUT_KEY_UP:
        elements[0]->rotate();
        break;
    case GLUT_KEY_DOWN:
        while (!elements[0]->reachedBottom()) elements[0]->step(DOWN);
	return;
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
    
    elements[0]->step(dir);
   
}


void attachElement()
{


    try
    {
        table->attachCells(elements[0]->stealCells());
    }
    catch(const char* e)
    {
        cerr << "Exception: " << e << endl;
	exit(1);
    }
    
    delete elements[0];
    elements[0] = elements[1];
    elements[0]->spawn();
    elements[1] = new Element(table,rand()%7);
    if (elements[0]->reachedBottom()) 
    {
        cout << "end" << endl;
	delete table;
	delete elements[0];
	delete elements[1];
	table = new Table(WIDTH, HEIGHT);
	elements[0] = new Element(table,rand()%7);
	elements[0]->spawn();
	elements[1] = new Element(table,rand()%7);
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
    unsigned long size = 0;                 // size of the image in bytes.
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
    if ((i = fread(&(image->sizeX), 4, 1, file)) != 1) {
	printf("Error reading width from %s.\n", filename);
	return 0;
    }
    
 
    printf("Width of %s: %lu\n", filename, image->sizeX);
      
    
    // read the height 
    if ((i = fread(&(image->sizeY), 4, 1, file)) != 1) {
	printf("Error reading height from %s.\n", filename);
	return 0;
    }
    
    printf("Height of %s: %lu\n", filename, image->sizeY);
    
    // calculate the size (assuming 24 bits or 3 bytes per pixel).
    size = image->sizeX * image->sizeY * 4;

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
	
 
    image->data = (unsigned char *) malloc(size);
    if (image->data == NULL) {
	printf("Error allocating memory for color-corrected image data");
	return 0;	
    }

    if (bpp == 32)
    {
    
        // seek past the rest of the bitmap header.
        fseek(file, 104, SEEK_CUR);
       
        if ((i = fread(image->data, size, 1, file)) != 1) {
	    printf("Error reading image data from %s.\n", filename);
	    return 0;
        }  
	
	for (i=0;i<size;i+=4) 
	{ // reverse all of the colors. (abgr -> rgba)
		temp = image->data[i];
		image->data[i] = image->data[i+3];  
		image->data[i+3] = temp;
		temp = image->data[i+1];
		image->data[i+1] = image->data[i+2]; 
		image->data[i+2] = temp;
        } 
    }
    else
    {
        // seek past the rest of the bitmap header.
        fseek(file, 24, SEEK_CUR);
        for (i = 0; i < image->sizeX*image->sizeX; i++)
	{
	    if (!fread(image->data + i*4 , 3, 1, file))
	    {
	        printf("Error reading image data from %s.\n", filename);
	        return 0;
            }


	    temp = image->data[i*4+1];
	    image->data[i*4+1] = image->data[i*4];
	    image->data[i*4] = temp;
	    
	    image->data[i*4+3] = 255;
	    	    
	       
	}
    }

    fclose(file);
    
    // we're done.
    return 1;
}

// Load Bitmaps And Convert To Textures
void LoadGLTextures() {	

printf("sizeof(short): %d\nsizeof(long): %d\nsizeof(char*): %d\nsizeof(Image): %d\nsizeof(Image*): %d",sizeof(short),sizeof(long),sizeof(char*),sizeof(Image),sizeof(Image*));

    // Load Texture
    Image *images[NUMBER_OF_TEXTURES];
    char filenames[NUMBER_OF_TEXTURES][20] = {
                         "0-32.bmp",
			 "1-32.bmp",
			 "2-32.bmp",
			 "3-32.bmp",
			 "4-32.bmp",
			 "5-32.bmp",
			 "6-32.bmp",
			 "background32.bmp",
			 "0-32.bmp"  
                       };

    for (int i = 0; i < NUMBER_OF_TEXTURES; i++)
    {
	    // allocate space for texture
	    images[i] = (Image *) malloc(sizeof(Image));
	    if (images[i] == NULL) {
		printf("Error allocating space for image");
		exit(0);
	    }

	    if (!ImageLoad(filenames[i], images[i])) {
		exit(1);
	    }        

	    // Create Texture	
	    glGenTextures(i, &texture[i]);
	    glBindTexture(GL_TEXTURE_2D, texture[i]);   // 2d texture (x and y size)

	    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // scale linearly when image bigger than texture
	    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // scale linearly when image smalled than texture

	    // 2d texture, level of detail 0 (normal), RGBA, x size from image, y size from image, 
	    // border 0 (normal), rgb color data, unsigned byte data, and finally the data itself.
	    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, images[i]->sizeX, images[i]->sizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE, images[i]->data);
    }
  

};























#include "graphics.hpp"
#include "tetris.hpp"
#include "music.hpp"

#include <string.h>
#include <sstream>

#define TICK 30 // ms

// Texture mapping
GLuint texture[NUMBER_OF_TEXTURES];
int FPS = 60;
float eyez = 0;
double cell_width = 0.15;

extern Element **elements;
extern Table *table;
extern bool _pause;
extern long score;
extern t_gamestate gamestate;
extern string player_name;
extern vector<player_score> scores;
extern char main_menu[5][20];
extern char options_menu[5][20];
extern unsigned char menu_index; 
extern Music *music;
extern unsigned short speed;
extern unsigned short preview_nr;
extern bool shadow;
extern Element *shadow_element; 

void timer(int v)
{
    logic();
    glutPostRedisplay();
    glutTimerFunc(TICK,timer,v);
}

void reshape(GLsizei width, GLsizei height) { 

   // GLsizei for non-negative integer
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
    unsigned long long size = 0;                 // size of the image in bytes.
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

    // Load Texture
    Image *images[NUMBER_OF_TEXTURES];
    char filenames[NUMBER_OF_TEXTURES][20] = {
                         "0-png.bmp",
			 "1-png.bmp",
			 "2-png.bmp",
			 "3-png.bmp",
			 "4-png.bmp",
			 "5-png.bmp",
			 "6-png.bmp",
			 "background32.bmp",
			 "pause32.bmp",
			 "font32.bmp" 
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
	    glGenTextures(1, &texture[i]);
	    glBindTexture(GL_TEXTURE_2D, texture[i]);   // 2d texture (x and y size)

	    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // scale linearly when image bigger than texture
	    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // scale linearly when image smalled than texture

	    // 2d texture, level of detail 0 (normal), RGBA, x size from image, y size from image, 
	    // border 0 (normal), rgb color data, unsigned byte data, and finally the data itself.
	    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, images[i]->sizeX, images[i]->sizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE, images[i]->data);
    }
  

};


void graphics_init(int argc, char** argv)
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
    
    LoadGLTextures();				// Load The Texture(s) 
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);   // Enable depth testing for z-culling

    glEnable(GL_BLEND); 
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glShadeModel (GL_FLAT);
    glDepthFunc(GL_LEQUAL);    // Set the type of depth-test
    glShadeModel(GL_SMOOTH);   // Enable smooth shading
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // Nice perspective corrections
    
    // Starting event loop
    glutMainLoop();
}


void display()
{

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers

    glLoadIdentity(); 
    gluLookAt(0,0,0,0.0,0.0,-4,0,1,0);
    glColor4f(1,1,1,1);
   
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
   
    if (elements[0]) 
    {
        elem_cells = elements[0]->getCells();
        cells.reserve(cells.size() + elem_cells.size());
        cells.insert(cells.end(), elem_cells.begin(), elem_cells.end());
	
	    for (int i = 0; i < preview_nr; i++)
		{
			elements[i+1]->setCenter(WIDTH + 3, HEIGHT - 3 - i * 3);
			elem_cells = elements[i+1]->getCells();
			cells.reserve(cells.size() + elem_cells.size());
			cells.insert(cells.end(), elem_cells.begin(), elem_cells.end());
		}
		
    }
    
    glEnable(GL_TEXTURE_2D);

    for (vector<Cell*>::iterator it = cells.begin(); it != cells.end(); ++it)
    {
    	Cell* cell = *it;
    	int i = cell->getY();
    	int j = cell->getX(); 
	
	if (i >= HEIGHT) continue;   


	glLoadIdentity(); 
	glColor4f(1,1,1,1);
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
	
    } 
	
	cells.clear();
	if (shadow_element) cells = shadow_element->getCells();
	
	for (vector<Cell*>::iterator it = cells.begin(); it != cells.end(); ++it)
    {
    	Cell* cell = *it;
    	int i = cell->getY();
    	int j = cell->getX(); 
	
	if (i >= HEIGHT) continue;   


	glLoadIdentity(); 
	glColor4f(0.5,0.5,0.5,0.5);
	gluLookAt(0,0,eyez,0.0,0.0,-4,0,1,0);     
	glTranslatef(-cell_width*(double)WIDTH/2+cell_width*j + cell_width/2,-cell_width*(double)HEIGHT/2+cell_width*i + cell_width/2,-4.0);
		
	glBegin(GL_QUADS);
         glVertex3f(-0.9*cell_width/2, -0.9*cell_width/2, 0.9*cell_width/2);
         glVertex3f(0.9*cell_width/2, -0.9*cell_width/2, 0.9*cell_width/2);
         glVertex3f(0.9*cell_width/2, 0.9*cell_width/2, 0.9*cell_width/2);
         glVertex3f(-0.9*cell_width/2, 0.9*cell_width/2, 0.9*cell_width/2);
	glEnd();
	glBegin(GL_QUADS);
         glVertex3f(-0.9*cell_width/2, 0.9*cell_width/2, 0.9*cell_width/2);
         glVertex3f(0.9*cell_width/2, 0.9*cell_width/2, 0.9*cell_width/2);
         glVertex3f(0.9*cell_width/2, 0.9*cell_width/2, -0.9*cell_width/2);
         glVertex3f(-0.9*cell_width/2, 0.9*cell_width/2, -0.9*cell_width/2);
	glEnd();
	glBegin(GL_QUADS);
         glVertex3f(-0.9*cell_width/2, -0.9*cell_width/2, 0.9*cell_width/2);
         glVertex3f(0.9*cell_width/2, -0.9*cell_width/2, 0.9*cell_width/2);
         glVertex3f(0.9*cell_width/2, -0.9*cell_width/2, -0.9*cell_width/2);
         glVertex3f(-0.9*cell_width/2, -0.9*cell_width/2, -0.9*cell_width/2);
	glEnd();
	glBegin(GL_QUADS);
         glVertex3f(0.9*cell_width/2, -0.9*cell_width/2, 0.9*cell_width/2);
         glVertex3f(0.9*cell_width/2, -0.9*cell_width/2, -0.9*cell_width/2);
         glVertex3f(0.9*cell_width/2, 0.9*cell_width/2, -0.9*cell_width/2);
         glVertex3f(0.9*cell_width/2, 0.9*cell_width/2, 0.9*cell_width/2);
	glEnd();
	glBegin(GL_QUADS);
         glVertex3f(-0.9*cell_width/2, -0.9*cell_width/2, -0.9*cell_width/2);
         glVertex3f(-0.9*cell_width/2, -0.9*cell_width/2, 0.9*cell_width/2);
         glVertex3f(-0.9*cell_width/2, 0.9*cell_width/2, 0.9*cell_width/2);
         glVertex3f(-0.9*cell_width/2, 0.9*cell_width/2, -0.9*cell_width/2);
	glEnd();
	
    } 
    
	// printing actual score
	glColor4f(0,1,0,1);
    printGL("Score",
            -cell_width*(double)WIDTH/2 + 0.1*cell_width - 0.2,
	    cell_width*(double)HEIGHT/2 - 0.1*cell_width - 0.2,
		-4.0,
	    0.1,
	    0.15,
	    1);
	    
	    
    string num;
    ostringstream convert;
    convert << score;
    num = convert.str();	    
	    
    printGL(num.c_str(),
            -cell_width*(double)WIDTH/2 + 0.1*cell_width - 0.2,
	    cell_width*(double)HEIGHT/2 - 0.1*cell_width - 0.4,
		-4.0,
	    0.1,
	    0.15,
	    1); 
		
		
	// printing high scores
	float y_index = 0;
	float x_index = -1.3;
	
	glColor4f(0,1,0,1);
	
	printGL("High Scores",
	        x_index + 0.3,
			y_index + 0.3,
			-4.0,
			0.075,
			0.15,
			1);
	for (vector<player_score>::iterator it = scores.begin(); it != scores.end(); ++it)
	{
	    player_score pl_score = *it;
		
	    printGL(pl_score.name.append(" - ").c_str(),
            x_index,
			y_index,
			-4.0,
			0.05,
			0.1,
			1);
	    
	    string num;
		ostringstream convert;
		convert << pl_score.score;
		num = convert.str();	    
	    
		printGL(num.c_str(),
            x_index,
			y_index,
			-4.0,
			0.05,
			0.1,
			0); 
			
		y_index -= 0.125;	
	}	   
    
    if (gamestate == end)
	{
	    glLoadIdentity();
		gluLookAt(0,0,eyez,0.0,0.0,-4,0,1,0); 
		glDisable(GL_TEXTURE_2D);
		glColor4f(0.08,0.08,0.08,0.5);
		glBegin(GL_QUADS);
		  glVertex3f(-1,-1,-2);
		  glVertex3f(1,-1,-2);
		  glVertex3f(1,1,-2);
		  glVertex3f(-1,1,-2);
		glEnd();
	    glEnable(GL_TEXTURE_2D);
		
		glDisable(GL_DEPTH_TEST);
        glColor4f(0,1,0,1);
		printGL("Name: ",
            0.0,
			0,
			-3.0,
			0.1,
			0.15,
			1);
			
		printGL(player_name.c_str(),
            0.0,
			0,
			-3.0,
			0.1,
			0.15,
			0);
		glEnable(GL_DEPTH_TEST);	
	}
	else if (gamestate == menu)
	{
		glLoadIdentity();
		gluLookAt(0,0,eyez,0.0,0.0,-4,0,1,0); 
		glDisable(GL_TEXTURE_2D);
		glColor4f(0.08,0.08,0.08,0.5);
		glBegin(GL_QUADS);
		  glVertex3f(-1,-1,-2);
		  glVertex3f(1,-1,-2);
		  glVertex3f(1,1,-2);
		  glVertex3f(-1,1,-2);
		glEnd();
	    glEnable(GL_TEXTURE_2D);
		
		
		glDisable(GL_DEPTH_TEST);
		
		for (int i = 0; i < 5; i++)
		{
			if (!elements[0] && (i == 1) ) 
			{
				glColor4f(0.5,0.5,0.5,1);
			}
			else 
			{
			    if (i == menu_index) glColor4f(1,0,0,1);
				else glColor4f(0,1,0,1);
			}	
			printGL(main_menu[i],
				0.0,
				0.5 - i * 0.2,
				-3.0,
				0.1,
				0.15,
				2);
		}
		
		glEnable(GL_DEPTH_TEST);
	}
	else if (gamestate == options)
	{
		glLoadIdentity();
		gluLookAt(0,0,eyez,0.0,0.0,-4,0,1,0); 
		glDisable(GL_TEXTURE_2D);
		glColor4f(0.08,0.08,0.08,0.5);
		glBegin(GL_QUADS);
		  glVertex3f(-1,-1,-2);
		  glVertex3f(1,-1,-2);
		  glVertex3f(1,1,-2);
		  glVertex3f(-1,1,-2);
		glEnd();
	    glEnable(GL_TEXTURE_2D);
		
		
		glDisable(GL_DEPTH_TEST);
		
		for (int i = 0; i < 5; i++)
		{
			if (i == menu_index) glColor4f(1,0,0,1);
			else glColor4f(0,1,0,1);
			printGL(options_menu[i],
				0.0,
				0.5 - i * 0.2,
				-3.0,
				0.1,
				0.15,
				1);
				
			string num;
			ostringstream convert;		
				
			switch (i)
			{
			    case 0 : convert << "  " << music->get_background_volume();
						 num = convert.str();
				
						 printGL(num.c_str(),
						 0.0,
						 0.5 - i * 0.2,
						 -3.0,
						 0.1,
						 0.15,
						 0);				
				
				         break;
				case 1 : convert << "  " << music->get_chunk_volume();
						 num = convert.str();
				
						 printGL(num.c_str(),
						 0.0,
						 0.5 - i * 0.2,
						 -3.0,
						 0.1,
						 0.15,
						 0);				
				
				         break;
				case 2 : convert << "  " << preview_nr;
						 num = convert.str();
				
						 printGL(num.c_str(),
						 0.0,
						 0.5 - i * 0.2,
						 -3.0,
						 0.1,
						 0.15,
						 0);				
				
				         break;
				case 3 : convert << "  " << speed;
						 num = convert.str();
				
						 printGL(num.c_str(),
						 0.0,
						 0.5 - i * 0.2,
						 -3.0,
						 0.1,
						 0.15,
						 0);				
				
				         break;
				case 4 :	if (shadow)
							{
					    		 printGL("  Yes",
								 0.0,
								 0.5 - i * 0.2,
								 -3.0,
								 0.1,
								 0.15,
								 0);	
							}
							else
							{
								 printGL("  No",
								 0.0,
								 0.5 - i * 0.2,
								 -3.0,
								 0.1,
								 0.15,
								 0);	
							}
				
				         
						 
						 break;
				default : break;
			}	
		}
		
		glEnable(GL_DEPTH_TEST);
	}

    //glClear(GL_DEPTH_BUFFER_BIT);
  
    glFlush();
    glutSwapBuffers();
}

void printGL(const char* text, float font_x, float font_y, float font_z, float font_size_x, float font_size_y, unsigned short alignment)
{

    if (alignment == 1)
    {
        font_x -= strlen(text) * font_size_x;
    }
	else if (alignment == 2)
	{
	    font_x -= strlen(text) * font_size_x/2;
	}

    glBindTexture(GL_TEXTURE_2D, texture[FONT_TEXTURE_NUM]);   // choose the texture to use.

    while (*text != '\0')
    {
        unsigned char y =  (*text) / 16;
	    unsigned char x =  (*text) % 16;
    
        glLoadIdentity(); 
        gluLookAt(0,0,eyez,0.0,0.0,-4,0,1,0);     
        glTranslatef(font_x,font_y,font_z);
	 
        glBegin(GL_QUADS);
          glTexCoord2f((float)x/16.0f, (float)(16-y)/16.0f - 1.0/16.0f); glVertex3f(-font_size_x/2, -font_size_y/2, 0);
          glTexCoord2f((float)x/16.0f + 1.0/16.0f, (float)(16-y)/16.0f - 1.0/16.0f); glVertex3f(font_size_x/2, -font_size_y/2, 0);
	      glTexCoord2f((float)x/16.0f + 1.0/16.0f, (float)(16-y)/16.0f); glVertex3f(font_size_x/2, font_size_y/2, 0);
          glTexCoord2f((float)x/16.0f, (float)(16-y)/16.0f); glVertex3f(-font_size_x/2, font_size_y/2, 0);
        glEnd();
     
        text++;
	    font_x += font_size_x;
    }
}




#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <set>

// OpenGL headers
#include <GL/freeglut.h>
#include <GL/gl.h>

// Game headers
#include "cell.hpp"
#include "element.hpp"
#include "table.hpp"

#define HEIGHT                   (20) 
#define WIDTH                    (12)

// Window
#define W_HEIGHT                (700)
#define W_WIDTH                 (800)
#define NUMBER_OF_TEXTURES       (10)
#define BACKGROUND_TEXTURE_NUM    (7)
#define PAUSE_TEXTURE_NUM         (8)
#define FONT_TEXTURE_NUM          (9)


/* Image type - contains height, width, and data */
struct Image {
    unsigned short sizeX;
    unsigned short sizeY;
    unsigned char *data;
};
typedef struct Image Image;

    
void graphics_init(int argc, char** argv);
void display();
int ImageLoad(char *filename, Image *image);
void LoadGLTextures();
void timer(int v);
void printGL(const char* text, float font_x, float font_y, float font_size_x, float font_size_y, unsigned char alignment, float r, float g, float b, float a);

#endif
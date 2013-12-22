#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <set>
#include <fstream>
#include <algorithm> 

// Linux specific headers
#include <unistd.h>

// OpenGL headers
#include <GL/freeglut.h>
#include <GL/gl.h>

// Game headers
#include "graphics.hpp"
#include "cell.hpp"
#include "element.hpp"
#include "table.hpp"
#include "tetris.hpp"
#include "music.hpp"

// Board
#define HEIGHT                   (20) 
#define WIDTH                    (12)
#define MAX_SCORES               (5)
#define NEXT_CTR_MAX             (50)
#define NEXT_CTR_MIN             (10)

using namespace std;

void attachElement();
void restart();
void load_scores();
void save_scores();
bool compare_scores(const player_score &a, const player_score &b);
void inc_speed();
void dec_speed();
void inc_preview_nr();
void dec_preview_nr();

long score = 0;
bool _pause = false;
bool to_rotate = false;
unsigned short dir_to_step = -1;
t_gamestate gamestate = game;
string player_name = "";
vector<player_score> scores;
player_score pl_score;
char main_menu[5][20] = {"New game", "Resume game", "Options", "Exit"};
char options_menu[5][20] = {"Music","Sound","Next elements","Speed","Show landing"};
unsigned char menu_index = 0;
unsigned short speed = 50;
unsigned short preview_nr = 0;
bool shadow = false;

Element **elements;
Element *shadow_element = NULL;
Table *table;
Music *music;

int main(int argc, char** argv)
{

    // Scores init
	load_scores();

    // Music init
    music = new Music(100,
                      25,
		      "/home/zoltan/prog/tetris/background.mp3",
		      "/home/zoltan/prog/tetris/row.wav");  
    music->play_background();	      

    // Table init
    table = new Table(WIDTH,HEIGHT);

    // elements init
	elements = (Element**)malloc((preview_nr+1)*sizeof(Element*));
    elements[0] = new Element(table,rand()%7);
    elements[0]->spawn();
    for (int i = 0; i < preview_nr; i++)
		elements[i+1] = new Element(table,rand()%7);

    // OpenGL Init
    graphics_init(argc,argv);
    return 0;
}

void logic()
{
    
    static unsigned int ctr = 0;
    ctr++;
	static unsigned char next_ctr = (NEXT_CTR_MAX - NEXT_CTR_MIN)*(100-speed)/100 + NEXT_CTR_MIN;

    if (_pause) music->pause_background();
    
    if (gamestate == game)
    {
        music->resume_background();
	
		if (to_rotate) elements[0]->rotate();
		to_rotate = false;
    
		if (elements[0])
		{
			if (dir_to_step == DOWN)
			{
				while (!elements[0]->reachedBottom()) elements[0]->step(DOWN);
			    next_ctr = ( (NEXT_CTR_MAX - NEXT_CTR_MIN)*(100-speed)/100 + NEXT_CTR_MIN) / 2;
			}
			else 
			{
			    elements[0]->step(dir_to_step); 
	            next_ctr--;
		    }
	 
			dir_to_step = -1;
		}

	
		if (elements[0] && (next_ctr == 0))
		{	    
	
			elements[0]->step(DOWN);

			if (elements[0]->reachedBottom())
			{
				attachElement();
			    next_ctr = ( (NEXT_CTR_MAX - NEXT_CTR_MIN)*(100-speed)/100 + NEXT_CTR_MIN) / 2;
			}
			else
			{
			    next_ctr = (NEXT_CTR_MAX - NEXT_CTR_MIN)*(100-speed)/100 + NEXT_CTR_MIN;
			}
						
			unsigned char rows = table->clear_full_rows();

			if (rows) music->play_chunk();

			switch (rows)
			{
				case 1: score += 40; break;
				case 2: score += 100; break;
				case 3: score += 300; break;
				case 4: score += 1200; break;
				default: break;
			}
		}
		
		if (shadow)
		{
			if (shadow_element) delete shadow_element;
			shadow_element = new Element(*elements[0]);
			while (!shadow_element->reachedBottom()) shadow_element->step(DOWN);
		}
    


    }
	else if (gamestate == end)
	{

	}
	
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
    
    delete elements[0];/*
    elements[0] = elements[1];
    elements[0]->spawn();
    elements[1] = new Element(table,rand()%7);
	*/
	
	int i = 0;
	for (i = 0; i < preview_nr; i++)
	{
		elements[i] = elements[i+1]; // there is no out of bounds error, 
		                             // because #preview_nr element is with the preview_nr index,
									 // because the array starts with the moving element
	}

    elements[i] = new Element(table,rand()%7);		
	elements[0]->spawn();
	
	
    if (elements[0]->reachedBottom()) 
    {
	    pl_score.name = player_name;
		pl_score.score = score;

		if (scores.size() == MAX_SCORES)
		{
			if ( !compare_scores(pl_score,scores[MAX_SCORES-1]) )
			{
				restart();
				gamestate = game;
			}
			else gamestate = end;
        }
		else gamestate = end;
    }

}

void keyboard_callback(unsigned char key,int x, int y)
{
    switch (gamestate)
	{
	    case game:
			if (key == 'p') gamestate = paused;
			if (key == 'r') restart();
			if (key == 27) 
			{
			    menu_index = 0;
				gamestate = menu;
			}
			
			break;
		
		case paused:
			if (key == 'p') gamestate = game;
			if (key == 'r') 
			{
			    restart();
			    gamestate = game;
			}
			if (key == 27) exit(0);
			
			break;
			
		case end: 
		    
		    switch (key)
			{
			    case 13: pl_score.name = player_name;
						 pl_score.score = score;
				         if (scores.size() < MAX_SCORES)
						 {
						     scores.push_back(pl_score);
							 sort(scores.begin(), scores.end(), compare_scores);
							 save_scores();
						 }
						 else if ( compare_scores(pl_score,scores[MAX_SCORES-1]) )
						 {
							 scores.push_back(pl_score);
							 sort(scores.begin(), scores.end(), compare_scores);
							 scores.erase(scores.begin()+MAX_SCORES,scores.end());
							 save_scores();
						 }
				         restart();
				         gamestate = game;
						 break;
						
				case 8: if (player_name.length()) player_name.erase(player_name.end()-1);	
				        break;
						
				case 27: restart();
				         gamestate = game; 
				
				         break;			 
						 
				default: if (key>=48 && key<=122) player_name += key;		 
			}
		
		    break;
			
		case menu: if (key == 27) gamestate = game;
		           if (key == 13)
				   {
				       switch (menu_index)
					   {
					       case 0: if (scores.size())
								   {
									   if (score > scores[scores.size()-1].score) gamestate = end;
									   else 
									   {
									       gamestate = game;
									       restart();
									   } 
								   }
								   else gamestate = end;
								   break;
					       case 1: gamestate = game;
								   break;
						   case 2: gamestate = options;
						           menu_index = 0;
						           
								   break;
						   case 3: exit(0);
						   default: break;
					   
					   }
				   }
		           break;
		
		case options: if (key == 27) 
		              {
					      menu_index = 0;
						  gamestate = menu;
					  }
				   	
		default: break;
	}

}

void special_callback(int key,int x, int y)
{

    if (_pause) return;

    if (elements[0] == NULL) return; 
    
	switch (gamestate)
	{
	    case game:
		    switch(key)
			{
			case GLUT_KEY_UP:
				to_rotate = true;
				break;
			case GLUT_KEY_DOWN:
				dir_to_step = DOWN;
				break;
			case GLUT_KEY_LEFT:
				dir_to_step = LEFT;
				break;
			case GLUT_KEY_RIGHT:
				dir_to_step = RIGHT;
				break;
			default:
				break;
			}
			
			break;
		
		case end:

			break;
			
		case menu:
			switch(key)
			{
			case GLUT_KEY_UP:
				if (menu_index) menu_index--;
				else menu_index = 3;
				break;
			case GLUT_KEY_DOWN:
				if (menu_index<3) menu_index++;
				else menu_index = 0;
				break;
			default:
				break;
			}
			break;
			
		case options:
			switch(key)
			{
			case GLUT_KEY_UP:
				if (menu_index) menu_index--;
				else menu_index = 4;
				break;
			case GLUT_KEY_DOWN:
				if (menu_index<4) menu_index++;
				else menu_index = 0;
				break;
			case GLUT_KEY_LEFT:
				switch (menu_index)
				{
				    case 0: music->set_background_volume(music->get_background_volume() - 1); 
					        break;
					case 1: music->set_chunk_volume(music->get_chunk_volume() - 1);
						    break;
					case 2: dec_preview_nr();	
							break;	
					case 3: dec_speed();
					        break;
					case 4: shadow = !shadow;
					        if (!shadow)
							{
							    if (shadow_element) delete shadow_element;
								shadow_element = NULL;
							}
					        break;		
					default: break;
				}
				break;
			case GLUT_KEY_RIGHT:
				switch (menu_index)
				{
				    case 0: music->set_background_volume(music->get_background_volume() + 1); 
					        break;
					case 1: music->set_chunk_volume(music->get_chunk_volume() + 1);
						    break;
					case 2: inc_preview_nr();	
							break;		
					case 3: inc_speed();	
							break;
					case 4: shadow = !shadow;
					        if (!shadow)
							{
							    if (shadow_element) delete shadow_element;
								shadow_element = NULL;
							}
					        break;
					default: break;
				}
				break;
			default:
				break;
			}
			break;	
			
		
		default: break;
	
	
	}


}

void restart()
{
    delete table;
	
	for (int i = 0; i <= preview_nr; i++)
        delete elements[i];
    
    table = new Table(WIDTH, HEIGHT);
    elements[0] = new Element(table,rand()%7);
    elements[0]->spawn();
	
	for (int i = 0; i < preview_nr; i++)
		elements[i+1] = new Element(table,rand()%7);
    
    score = 0;
	player_name = "";
    _pause = false;
}

void load_scores()
{
    ifstream input;
	input.open("scores",ios::in);
	
	if (!input.is_open()) return;
	
	string name;
	unsigned short score;
	player_score pl_score;
	
	while ( (input >> name >> score) && scores.size() < MAX_SCORES)
	{
	    pl_score.name = name;
		pl_score.score = score;
	    scores.push_back(pl_score);
	}
	
	input.close();
	
	sort(scores.begin(), scores.end(), compare_scores);
}

void save_scores()
{
    ofstream output;
	output.open("scores", ios::out);
	
	if (!output.is_open()) return;
	
	for (vector<player_score>::iterator it = scores.begin(); it != scores.end(); ++it)
	{
	    player_score pl_score = *it;
		
	    output << pl_score.name << " " << pl_score.score << endl;	
	}
	
	output.close();
}

bool compare_scores(const player_score &a, const player_score &b)
{ 
    return a.score > b.score;
}

void inc_speed()
{
    if (speed < 100) speed++;
}

void dec_speed()
{
    if (speed) speed--;
}

void inc_preview_nr()
{
    if (preview_nr < 4) 
	{
		preview_nr++;
		elements = (Element**)realloc(elements,(preview_nr+1)*sizeof(Element*));
		elements[preview_nr] = new Element(table,rand()%7);
	}
}

void dec_preview_nr()
{
    if (preview_nr) 
	{
	    preview_nr--;
		delete elements[preview_nr+1];
		elements = (Element**)realloc(elements,(preview_nr+1)*sizeof(Element*));
	}
}
























#include <stdio.h>
#include <stdbool.h>

typedef struct world{
	char ch;
	bool attributes[3];
	int color;
}world;


int file_length(FILE * fp);
world * load_world(char * filename,int world_height,int world_width);
void save_world(char * filename,world * param_world,int world_height, int world_width);

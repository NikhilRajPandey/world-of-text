/* This file is part of world of text.c */
#include "load_save_world.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ncurses.h>

int file_length(FILE * fp){
	fseek(fp, 0, SEEK_END);
	int fsize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	return fsize;
}

world * load_world(char * filename,int world_height,int world_width){
	FILE * fileptr = fopen(filename,"rb");
	if (fileptr == NULL) return NULL;
	
	int fileleng = file_length(fileptr);
	if (fileleng != world_height * world_width * sizeof(world)){
		printf("FILE DON'T SUPPORT FOR THIS TERMINAL DUE TO CHANGE IN RESOLUTION/FONT\n");
		exit(1);
	}
	world * file_world = (world*)malloc(fileleng * sizeof(world));
	fread(file_world, sizeof(world) * fileleng, 1, fileptr);

	fclose(fileptr);
	return file_world;
	
}
void save_world(char * filename,world * param_world,int world_height, int world_width){
	
	FILE * fileptr = fopen(filename,"wb");

	if (fileptr == NULL){
		printf("%s\n",filename);
	}
	fwrite(param_world, sizeof(world) * world_height * world_width, 1, fileptr);

	fclose(fileptr);
}
/* int main(){ */
/* 	world a[10]; */
/* 	a[0].ch = 'b'; */
/* 	a[0].attributes[0] = true; */
/* 	a[0].attributes[1] = false; */
/* 	a[0].attributes[2] = true; */
/* 	a[0].color = 3; */
/* 	save_world("file2",a,5,2); */
/* 	world * my_world = load_world("file",5,2); */
	/* 	printf("%c \n",my_world[0].ch); */
	/* } */
/* } */

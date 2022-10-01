#include <ncurses.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "load_save_world.h"

#define DEBUG false
#define margin_y 3
#define margin_x 10
enum attributes {BOLD=0, BLINK, UNDERLINE};

int limit(int num,int min,int max){
	if (num < min ) return min;
	else if ( num > max ) return max;
	return num;
}

void print_center();
void print_with_attrib(int y,int x,char ch,bool * attributes,int color);
void game(char *world_name,world * our_world, int world_height,int world_width);
void init_world(world * our_world, int world_height,int world_width);
void show_world(world * our_world, int world_height,int world_width);

int main(int argc, char *argv[]){
	initscr();
	raw();
	noecho();
	keypad(stdscr,true);
	set_escdelay(0);
	start_color();

	for(int i = 1; i <= COLOR_WHITE; i++){ /* It will initialize all color from 1(RED) to 7(WHITE) */
		init_pair(i,i,COLOR_BLACK);
	}

	if (argc < 2){
		endwin();
		printf("Usage: world_of_text <file>");
		exit(1);
	}

	char * world_name = argv[1];
	int world_height = (LINES - 2 * margin_y - 2);
	int world_width = (COLS - 2 * margin_x - 2);
	world * our_world = load_world(world_name,world_height,world_width);
	
	if ( ! our_world){
		our_world = (world *) calloc( world_height * world_width, sizeof(world));
		init_world(our_world,world_height,world_width);
	}
	
	game(world_name,our_world, world_height, world_width);
	endwin();
	return 0;
}
void game(char *world_name,world * our_world, int world_height,int world_width){
	bool cur_attributes[3] = {false,false,false};  /* The attributes enum denote the attribute meaning */
	int cur_color = 7; /* Default WHITE */
	
	bool char_mode = false;
	int cur_cursor_y = margin_y + 1;
	int cur_cursor_x = margin_x + 1;
	int cur_block_y = 0;
	int cur_block_x = 0;
	int win_max_y = LINES - margin_y - 1;
	int win_max_x = COLS - margin_x - 1;
	WINDOW * local_win = newwin(world_height + 2,world_width + 2, margin_y, margin_x);
	refresh();
	box(local_win, 0, 0);
	wrefresh(local_win);

	print_center(1,"WORLD OF TEXT");

	show_world(our_world,world_height,world_width);
	move(margin_y + 1,margin_x + 1);
	refresh();
	int prev_ch = -1;
	int ch = -1;
	
	while (true){
		prev_ch = ch;
		ch = getch();
		
		world * cur_block = our_world + world_width * cur_block_y + cur_block_x; /* our_world is (world *) */

		if (prev_ch == 27){	/* Escape Key */
			switch(ch){
			case 27:
				endwin();
				save_world(world_name, our_world,world_height,world_width);
				return;
				break;
				
			case 'w':
				save_world(world_name, our_world,world_height,world_width);
				break;				
			case 'c':
				char_mode = ! char_mode;
				break;
				
			case '+':
				cur_attributes[BOLD] = ! cur_attributes[BOLD];
				break;
				
			case '*':
				cur_attributes[BLINK] = ! cur_attributes[BLINK];
				break;
			case '-':
				cur_attributes[UNDERLINE] = ! cur_attributes[UNDERLINE];
				break;
			}
			if(ch >= 49 && ch <= 55){ /* Number 0-7 for colors */
				cur_color = ch - 48;
			}
		}
		else{
			switch(ch){
			case KEY_UP:
			case 'p' - 'a' + 1:
				cur_cursor_y--;
				break;
		
			case KEY_DOWN:
			case '\n':
			case 'n' - 'a' + 1:
				cur_cursor_y++;
				break;
		
			case KEY_LEFT:
			case 'b' - 'a' + 1:
				cur_cursor_x--;
				break;

			case KEY_RIGHT:
			case 'f' - 'a' + 1:
				cur_cursor_x++;
				break;

			case 330: /* Delete Key */
				cur_block->ch = ' ';
				break;

			case KEY_BACKSPACE:
				if (cur_cursor_x != margin_x + 1){
					cur_cursor_x--;
					cur_block = cur_block - 1;
				}
				cur_block->ch = ' ';
				break;
			}
		}
		if ( ch > 31 && ch < 127 && prev_ch != 27){
			/* print_with_attrib(cur_cursor_y,cur_cursor_x,ch,&cur_attributes[0],cur_color); */
			cur_block->ch = ch;
			cur_block->color = cur_color;

			for (int i = 0; i < 3; i++){
				cur_block->attributes[i] = cur_attributes[i];
			}

			cur_cursor_x += ! char_mode;
		}

		show_world(our_world,world_height,world_width);
		cur_cursor_y = limit(cur_cursor_y, margin_y + 1, win_max_y - 1);
		cur_cursor_x = limit(cur_cursor_x, margin_x + 1, win_max_x - 1);
		cur_block_y = cur_cursor_y - margin_y - 1;
		cur_block_x = cur_cursor_x - margin_x - 1;
		
		if (DEBUG){
			mvprintw(0,0,"%d %d ",cur_cursor_y,cur_cursor_x);
			mvprintw(1,0,"%d %d ",LINES,COLS);
			mvprintw(2,0,"%d %d ",cur_block_y,cur_block_x);
			mvprintw(3,0,"%d ",ch);
			mvprintw(0,10,"%d %d",world_height,world_width);
		}
		
		if(char_mode)
			print_center(LINES - 2,"CHAR-MODE: ENABLED ");
		else
			print_center(LINES - 2,"CHAR-MODE: DISABLED");

		char status_bar[40];
		sprintf(status_bar,"BOLD: %d BLINK: %d UNDERLINE: %d COLOR: %d",cur_attributes[0],cur_attributes[1],cur_attributes[2],cur_color);
		print_center(LINES - 1,status_bar);
		move(cur_cursor_y,cur_cursor_x);
		refresh();
	}
}
void print_center(int y,char * param_str){
	mvprintw(y,(COLS - strlen(param_str)) / 2, param_str);
}

void print_with_attrib(int y,int x,char ch,bool attributes[3],int color){
	attron(COLOR_PAIR(color));
	
	if(attributes[BOLD]){
		attron(A_BOLD);
		
	}if(attributes[BLINK]){
		attron(A_BLINK);
		
	}if(attributes[UNDERLINE]){
		attron(A_UNDERLINE);
	}
	mvaddch(y,x,ch);
	attroff(A_BOLD);
	attroff(A_BLINK);
	attroff(A_UNDERLINE);
	attroff(COLOR_PAIR(color));
}
void init_world(world * our_world, int world_height,int world_width){ /* Just putting default values of each block */
	for (int y = 0; y < world_height; y++){
		for(int x = 0; x < world_width; x++){
			world * cur_block = our_world + world_width * y + x;
			cur_block->ch = ' ';
			cur_block->color = 7;
		}
	}
}
void show_world(world * our_world, int world_height,int world_width){
	for (int y = 0; y < world_height; y++){
		for(int x = 0; x < world_width; x++){
			world * cur_block = our_world + world_width * y + x;
			print_with_attrib(y + margin_y + 1,x + margin_x + 1, cur_block->ch, cur_block->attributes, cur_block->color);
		}
	}
	return;
}

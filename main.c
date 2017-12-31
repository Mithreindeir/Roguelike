#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include "map.h"
#include "fov.h"
#include "player.h"

#define DELAY 3500


void init()
{
	initscr();
	noecho();
	curs_set(FALSE);
	cbreak();
	keypad(stdscr, TRUE);
	start_color();
	init_color(COLOR_BLACK, 0,0,0);
	init_color(COLOR_WHITE, 500,500,500); //238-221-130 	
	init_color(COLOR_YELLOW, 960, 880, 520);
	init_pair(1, COLOR_WHITE, COLOR_BLACK);
	init_pair(2, COLOR_YELLOW, COLOR_BLACK);
	init_pair(3, COLOR_GREEN, COLOR_BLACK);
	init_pair(4, COLOR_WHITE, COLOR_YELLOW);
	init_pair(5, COLOR_RED, COLOR_BLACK);



	attron(COLOR_PAIR(1));
}

void destroy()
{
	endwin();
}

int kbhit(void)
{
        struct timeval        timeout;
        fd_set                readfds;
        int                how;

        /* look only at stdin (fd = 0) */
        FD_ZERO(&readfds);
        FD_SET(0, &readfds);

        /* poll: return immediately */
        timeout.tv_sec = 0L;
        timeout.tv_usec = 0L;

        how = select(1, &readfds, (fd_set *)NULL, (fd_set *)NULL, &timeout);
        /* Change "&timeout" above to "(struct timeval *)0"       ^^^^^^^^
         * if you want to wait until a key is hit
         */

        if ((how > 0) && FD_ISSET(0, &readfds))
                return 1;
        else
                return 0;
}

void game_loop()
{
	//
	int x = 10, y = 10;
	int max_x = 0, max_y = 0;

	getmaxyx(stdscr, max_y, max_x);

	struct game_map * map = new_map(max_x-15, max_y);
	map->offset = 15;
	generate_map(map);
	int pi = add_obj(map, map->w/2, map->h/2, '@', PLAYER, &player_update);
	struct player *pd = init_player();
	map->objects[pi]->data = pd;
	//int end = add_obj(map, map->w/2, map->h/2, '^', &player_update);

	//
	int ch = getch();
   	refresh();
   	float loops = 0;
   	int iter = 0;
   	int scanning = 0;
   	float last_scan = 0;

	while (1) {

		if (kbhit()) {
			//clear();
			ch = getch();
	    	update_objects(map, ch);
 			//if (kbhit()) {
				
				if (ch == 'q' || ch == 'Q') {
					break;
				} else if (ch == ' ') {
					scanning = 1;
				}
 			//}
 		}
		clear();    	
		getmaxyx(stdscr, max_y, max_x);
	    	
    	float sway = round(sin(round(clock()/(CLOCKS_PER_SEC/2))*2));
    	fov(map, map->objects[pi]->x, map->objects[pi]->y, 6);
    	attron(COLOR_PAIR(1));
    	//
		//
    	for(int i = 0; i < map->h; i++) {
    		for (int j = 0; j < map->w; j++) {
    			if (map->seen[j+i*map->w]) {
    				//if (map->map[j+i*map->w] == WALL)
    					//attron(COLOR_PAIR(1));
    				//else 
					if (map->glare[j+i*map->w])
    					attron(COLOR_PAIR(2));
    				else
    					attron(COLOR_PAIR(1));
    					//attron(COLOR_PAIR(2));
    				if (map->map[j+i*map->w] == WALL) {
						if (map->map[(j-1)+i*map->w] == '.' && map->seen[(j-1)+i*map->w]) {
							mvaddch(i, j+map->offset, '|');
						}
						else if (map->map[(j+1)+i*map->w] == '.' && map->seen[(j+1)+i*map->w]) {
							mvaddch(i, j+map->offset, '|');
						}
						else if (map->map[j+(i-1)*map->w] =='.' && map->seen[j+(i-1)*map->w]) {
							mvaddch(i, j+map->offset, '~');
						}
						else if (map->map[j+(i+1)*map->w] =='.' && map->seen[j+(i+1)*map->w]) {
							mvaddch(i, j+map->offset, '_');
						}
						else if (map->map[(j+1)+(i-1)*map->w] =='.' && map->seen[(j+1)+(i-1)*map->w]) {
							mvaddch(i, j+map->offset, '\\');
						}
						else if (map->map[(j-1)+(i-1)*map->w] =='.' && map->seen[(j-1)+(i-1)*map->w]) {
							mvaddch(i, j+map->offset, '/');
						}
						else if (map->map[(j+1)+(i+1)*map->w] =='.' && map->seen[(j+1)+(i+1)*map->w]) {
							mvaddch(i, j+map->offset, ',');
						}
						else if (map->map[(j-1)+(i+1)*map->w] =='.' && map->seen[(j-1)+(i+1)*map->w]) {
							mvaddch(i, j+map->offset, ',');
						}
	    			}
    				else mvaddch(i, j+map->offset, map->map[j+i*map->w]);
    			}
    		}
	   	}
    	if (scanning) {
    		scan(map, map->objects[pi]->x, map->objects[pi]->y, 6, iter);
    		last_scan += clock() / CLOCKS_PER_SEC;
    		while (last_scan > 1000) {
    			iter++;
    			last_scan -= 1000;
    		}

    		if (iter > 120) {
    			scanning = 0;
    			iter = 0;
    		}
    	}
	    //
 	   	attron(COLOR_PAIR(3));
    	for (int i = 0; i < map->num_objects; i++) {
	   		if (map->seen[map->objects[i]->y*map->w+map->objects[i]->x] && !map->objects[i]->passive)
    			mvaddch(map->objects[i]->y, map->objects[i]->x+map->offset, map->objects[i]->ch);
    	}
 	   	attron(COLOR_PAIR(1));
    	mvprintw(5, 1, "%s", "Menu:");
    	mvprintw(6, 1, "%s", "[B]ackpack");
	   	mvprintw(7, 1, "%s", "[E]quip");
    	mvprintw(8, 1, "%s", "[L]antern");
    	mvprintw(9, 1, "%s", "[F]ire Gun");
     	mvprintw(10, 1, "%s %d %s", "Health", pd->health, " / 100");

    	//int * ojs = detect(map, map->objects[pi]->x, map->objects[pi]->y, 6, &len);

    	int start = 3 - map->num_events;
    	while (start < 0) {
    		pop_event(map);
    		start++;
    	}
    	if (start != 3) {
    		while (start < 3 ) {
    			mvprintw(start, 1, map->events[2-start]);
    			start++;
    		}
    	}
    	
  		//
    	refresh();	    	
		//usleep(DELAY);
		loops += 0.0005;
	}
	free_map(map);
}

int main(int argc, char *argv[]) {

	init();

	game_loop();

	destroy();
	return 0;
}
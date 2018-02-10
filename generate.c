#include "generate.h"

int random_max(int max)
{
    int divisor = RAND_MAX/(max+1);
    int num;

    do { 
        num = rand() / divisor;
    } while (num > max);
    
    return num;
}

int fit_rect(struct game_map * map, int x, int y, int w, int h, int o)
{
        //If its a coorridor let it connect things
        if (w == 1) {
                y++;
                h--;
        }
        if (h == 1) {
                x++;
                w--;
        }
        for (int i = y-1; i <= (y+h); i++) {
                for (int j = x-1; j <= (x+w); j++) {
                        if (((j+w) >= map->w) || (i <= 0)) return 0; 
                        if (((i+h) >= map->h) || (j <= 0)) return 0;
                        if (!o && map->map[j+i*map->w] =='.') return 0;
                }
        }
        return 1;
}

void fill_rect(struct game_map * map, int x, int y, int w, int h)
{
        for (int i = y; i < (y+h); i++) {
                for (int j = x; j < (x+w); j++) {
                        map->map[j+i*map->w] = '.';
                }
        }
}

int fill_deadends(struct game_map * map)
{
        int deadends = 0;
        for (int i = 0; i < (map->h-1); i++) {
                for (int j = 0; j < (map->w-1); j++) {
                        if (map->map[j+i*map->w] =='.' && ((i>0) && (j>0))) {
                                int num_walls = 0;
                                if (map->map[(j-1)+i*map->w] == WALL) {
                                        num_walls++;
                                }
                                if (map->map[(j+1)+i*map->w] == WALL) {
                                        num_walls++;
                                }
                                if (map->map[j+(i-1)*map->w] ==WALL) {
                                        num_walls++;
                                }
                                if (map->map[j+(i+1)*map->w] ==WALL) {
                                        num_walls++;
                                }
                                if (num_walls > 2) {
                                        map->map[j+i*map->w] = WALL;
                                        deadends = 1;
                                }
                        } else if (map->map[j+i*map->w] =='.' ) {
                                map->map[j+i*map->w] = WALL;
                                deadends = 1;
                        }
                }
        }
        return deadends;
}


void get_wall(struct game_map * map, int * x, int * y, int * t)
{
        int num_walls = 0;
        for (int i = 1; i < (map->h-1); i++) {
                for (int j = 1; j < (map->w-1); j++) {
                        if (map->map[j+i*map->w] ==WALL) {
                                if (map->map[(j-1)+i*map->w] =='.') {
                                        num_walls++;
                                } else if (map->map[(j+1)+i*map->w] =='.') {
                                        num_walls++;
                                } else if (map->map[j+(i-1)*map->w] =='.') {
                                        num_walls++;
                                } else if (map->map[j+(i+1)*map->w] =='.') {
                                        num_walls++;
                                }
                        }
                }
        }
        *t = -1;
        //up=0 right=1 down = 2 left = 3
        int idx = random_max(num_walls);
        for (int i = 2; i < (map->h-2); i++) {
                for (int j = 2; j < (map->w-2); j++) {
                        if (map->map[j+i*map->w] ==WALL) {
                                if (map->map[j+(i-1)*map->w] =='.') {
                                        idx--;
                                        *t=0;
                                } else if (map->map[(j+1)+i*map->w] =='.') {
                                        idx--;
                                        *t=1;
                                } else if (map->map[j+(i+1)*map->w] =='.') {
                                        idx--;
                                        *t=2;
                                } else if (map->map[(j-1)+i*map->w] =='.') {
                                        idx--;
                                        *t=3;
                                }
                                if (idx==0) {
                                        *x = j;
                                        *y = i;
                                        return;
                                }
                        }
                }
        }
}


void generate_map(struct game_map * map)
{
        srand(time(NULL));
        for (int i = 0; i < map->h; i++) {
                for (int j = 0; j < map->w; j++) {
                        map->map[j+i*map->w] = WALL;
                }
        }
        int size = random_max(7) + 5;
        //fill_rect(map, 0, 0, map->w, map->h);
        fill_rect(map, map->w/2-size/2, map->h/2-size/4 , size, size/2);
        //add_obj(map, map->w/2 + 1, map->h/2 + 1, '^', END, NULL);
        int success = 10;
        int mc = 20;
        int os = success;
        int max_tries = 500;
        while (success > 0 && max_tries > 0) {
                max_tries--;
                int room = (random_max(10) > 6) ? 1 : 0;
                int wx=0, wy=0, t=0;
                get_wall(map, &wx, &wy, &t);
                int attempts = 3;
                if (mc < 0 && !room) {
                        continue;
                }
                int x,y,w,h;
                while (attempts >= 0) {
                        w = random_max(3) + 3;
                        h = random_max(2) + 3;
                        if (!room) {
                                if (t==0 || t == 2) {
                                        w = 1;
                                        h = random_max(6) + 1;
                                }
                                else {
                                        h = 1;
                                        w = random_max(6) + 1;
                                }
                        }

                        if (t==0) { //down
                                x = wx, y=wy+1;
                                if (fit_rect(map, x,y,w,h, 0)) {
                                        fill_rect(map, x,y,w,h);
                                        success -= room;
                                        map->map[wx+wy*map->w] = '.';
                                        break;
                                }
                        } else if (t==1) { //left
                                x = wx-w, y=wy-h/2;
                                if (fit_rect(map, x,y,w,h,  0)) {
                                        fill_rect(map, x,y,w,h);
                                        success -= room;
                                        map->map[wx+wy*map->w] = '.';
                                        break;
                                }
                        } else if (t==2) { // up
                                x = wx-w/2, y=wy-h;
                                if (fit_rect(map, x,y,w,h,  0)) {
                                        fill_rect(map, x,y,w,h);
                                        success -= room;
                                        map->map[wx+wy*map->w] = '.';
                                        break;
                                }
                        } else if (t==3) { //right
                                x = wx+1, y=wy-h/2;
                                if (fit_rect(map, x,y,w,h,  0)) {
                                        fill_rect(map, x,y,w,h);
                                        success -= room;
                                        map->map[wx+wy*map->w] = '.';
                                        break;
                                }
                        }
                        attempts--;
                }
                if (!room && os != success) mc--;
                if (os != success) {
                        if (success == 0) {
                        add_obj(map, x+w/2, y+h/2, '^', END, NULL, NULL);
                } else if (random_max(10) > 3) {
                        int ix = add_obj(map, x+w/2, y+h/2, 's', ENEMY, &enemy_update, (game_object_free)&destroy_enemy);
                        map->objects[ix]->data = init_enemy();
                        map->objects[ix]->speed = 2;
                }
                }
#ifdef DEBUG
                if (os > success) {

                os = success;
                    getch();
        }
                clear();
        attron(COLOR_PAIR(1));

        for(int i = 0; i < map->h; i++) {
                for (int j = 0; j < map->w; j++) {
                        mvaddch(i, j+map->offset, map->map[j+i*map->w]);
                }
        }
        refresh();
#endif
        os = success;

        }
        while (fill_deadends(map)) {
        #ifdef DEBUG

                clear();
        attron(COLOR_PAIR(1));
        if (os != success) {

                os = success;
        }
        for(int i = 0; i < map->h; i++) {
                for (int j = 0; j < map->w; j++) {
                        mvaddch(i, j+map->offset, map->map[j+i*map->w]);
                }
        }
        refresh();
        getch();
        #endif
        }
        for (int i = 0; i < map->h; i++) {
                for (int j = 0; j < map->w; j++) {
                        map->nodes[j+(i*map->w)] = node_init(j, i, map->map[j+i*map->w] == WALL);
                }
        }
        //if (max_tries <= 0)
        //      generate_map(map);
        //while (fill_deadends(map));
}


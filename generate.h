#ifndef GENERATE_H
#define GENERATE_H

#include "map.h"
#include "enemy.h"
#include "player.h"

/*Proc Gen*/


int random_max(int max);
int fit_rect(struct game_map * map, int x, int y, int w, int h, int o);
void fill_rect(struct game_map * map, int x, int y, int w, int h);
int fill_deadends(struct game_map * map);
void get_wall(struct game_map * map, int * x, int * y, int * t);
void generate_map(struct game_map * map);


#endif

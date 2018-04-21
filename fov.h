#ifndef FOV_H
#define FOV_H

#include <stdlib.h>
#include <math.h>
#include <ncurses.h>
#include "map.h"

float max(float a, float b);
int diag_dist(int x0, int y0, int x1, int y1);
float lerp(float start, float end, float t);
void fov(struct game_map *map, int x, int y, int dist);	//Glare and seen calculations
void scan(struct game_map *map, int x, int y, int dist, int i);	// Scanning visual effect
int *detect(struct game_map *map, int x, int y, int dist, int *len);	// Returns objects in fov

#endif

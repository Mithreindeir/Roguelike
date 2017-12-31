#include "fov.h"

const float DEG2RAD = 3.14159/180;

float max(float a, float b)
{
	return a > b ? a : b;
}

int diag_dist(int x0, int y0, int x1, int y1) 
{
	int dx = x1-x0, dy = y1-y0;
	return max(abs(dx), abs(dy));
}

float lerp(float start, float end, float t)
{
	return start + t * (end-start);
}

int * detect(struct game_map * map, int x, int y, int dist, int * len)
{
	int length = 0;
	int * objects = NULL;
	int * obj_map = malloc(sizeof(int) * map->w * map->h);
	for (int i = 0; i < (map->w * map->h); i++) {
		obj_map[i] = -1;
	}
	for (int i = 0; i < map->num_objects; i++) {
		struct game_object * obj = map->objects[i];
		obj_map[obj->x + obj->y * map->w] = i;
		//fprintf(stderr, "%d and %d\n", obj->x, obj->y);
	}
	for (int i = 0; i < 360; i++) {
		float degr = i*DEG2RAD;
		int nx = round(cos(degr)*dist) + x;
		int ny = round(sin(degr)*dist/2) + y;
		int n = diag_dist(x,y,nx,ny);
		int lx=nx, ly =ny;
		for(int j = 0; j <= n; j++) {
			float t = n == 0 ? 0.0 : j/((float)n);
			int tx = round(lerp(x, nx, t));
			int ty = round(lerp(y, ny, t));
			if (tx < 0 || tx >= map->w) continue;
			if (ty < 0 || ty >= map->h) continue;
			if (map->map[tx+ty*map->w] == WALL) {
				break;
			}
			if (tx == x && ty == y)
				continue;
			if (obj_map[tx+ty*map->w] != -1) {
				int in_arr = 0;
				for(int k = 0; k < length; k++) {
					if (obj_map[tx+ty*map->w] == objects[k])
						in_arr = 1;
				}
				if (in_arr)
					continue;
				length++;
				if (length == 1) {
					objects = malloc(sizeof(int));
				} else {
					objects = realloc(objects, sizeof(int) * length);
				}
				objects[length-1] = obj_map[tx+ty*map->w];
			}
			lx = tx;
			ly = ty;
		}
	}
	free(obj_map);
	*len = length;
	return objects;
}

void fov(struct game_map * map, int x, int y, int dist)
{
	for(int i = 0; i < (map->w*map->h); i++) {
		map->glare[i] = 0;
	}
	for (int i = 0; i < 360; i++) {
		float degr = i*DEG2RAD;
		int nx = round(cos(degr)*dist) + x;
		int ny = round(sin(degr)*dist/2) + y;
		int n = diag_dist(x,y,nx,ny);
		int lx=nx, ly =ny;
		for(int j = 0; j <= n; j++) {
			float t = n == 0 ? 0.0 : j/((float)n);
			int tx = round(lerp(x, nx, t));
			int ty = round(lerp(y, ny, t));
			if (tx < 0 || tx >= map->w) continue;
			if (ty < 0 || ty >= map->h) continue;
			int dx = abs(lx-tx);
			int dy = abs(lx-ty);
			//Prevent seeing diagonally
			//if ((dx+dy)<2) break;
			if (map->map[tx+ty*map->w] == WALL) {
				//mvaddch(ty, tx, '#');
				map->seen[tx+ty*map->w] = 1;
				break;
			}
			if (tx == x && ty == y)
				continue;
			//if (t )
			map->glare[tx+ty*map->w] = 1;
			map->seen[tx+ty*map->w] = 1;
			//mvaddch(ty, tx, '.');
			lx = tx;
			ly = ty;
		}
	}
}

void scan(struct game_map * map, int x, int y, int dist, int i)
{
	float degr = i*DEG2RAD*3;
	int nx = round(cos(degr)*dist) + x;
	int ny = round(sin(degr)*dist/2) + y;
	int n = diag_dist(x,y,nx,ny);
	int lx=nx, ly =ny;
	for(int j = 0; j <= n; j++) {
		float t = n == 0 ? 0.0 : j/((float)n);
		int tx = round(lerp(x, nx, t));
		int ty = round(lerp(y, ny, t));
		if (tx < 0 || tx >= map->w) continue;
		if (ty < 0 || ty >= map->h) continue;
		int dx = abs(lx-tx);
		int dy = abs(lx-ty);
		//Prevent seeing diagonally
		if ((dx+dy)<2) break;
		if (map->map[tx+ty*map->w] == WALL) {
			break;
		}
		if (tx == x && ty == y)
			continue;
		//if (t )
 	   	attron(COLOR_PAIR(4));
		mvaddch(ty, tx+map->offset, '.');
		lx = tx;
		ly = ty;
	}
}

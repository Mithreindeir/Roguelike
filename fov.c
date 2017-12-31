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
/*
Today I will be showing you how to make a simple field of view calculator for roguelikes.
This is what the final algorithm will look like.
If you don't already know, a field of view algorithm simulated what a character would have seen or not seen.
It is often used in roguelikes to show what part of the map the player can see or not seen.

Many different algorithms can be used for this, but I will be showing a very simple one.
It works by making a line in 360 degrees from the player and iterating through the tiles the line is in.
If there is a wall it exits it.
Everything before the wall is set as seen and has a glare property (used in the internals of my roguelike to show if it is currently seen)

Thank you for watching


void fov(struct game_map * map, int x, int y, int dist)
{
	//First I set all the tiles glare to 0
	for (int i = 0; i < (map->w * map->h); i++) {
		map->glare[i] = 0;
	}
	//Obviously everything will be different based on how you have set up your game, however the principle is the same.

	//Now i loop through 360 degrees
	for (int i = 0; i < 360; i++) {
		//And make a line with the current degree using the distance passed into this field of view function
		float deg = i*DEG2RAD;
		//I will round the results because it is a grid map, so integers must be used
		int nx = round(cos(deg) * dist) + x; // The x and y passed in are the location of the player
		int ny = round(sin(deg) * dist/2) + y;
		//This works but because a terminals character height is twice the size of their width, the fov is an ellipse instead of a circle.
		//This can be corrected by dividing the distance by two for height


		//Now that we have the endpoint of the line, we must find out how many tiles the line goes through
		//This can be done with by calculating diagonal distance. The formula is shown above in the function diag_dist
		int d = diag_dist(x, y, nx, ny);
		//Now we loop through all the tiles that the line goes through. This can by done using linear interpolation.
		//You interpolate by the decreasing distance from the player to the endpoints moving by the distance of a tile.
		for (int j = 0; j < d; j++) {
			//The current ratio is the current distance / total distance (j/d)
			int tx = lerp(x, nx, j/((float)d)); // Cast to float because diving two ints will round
			int ty = lerp(y, ny, j/((float)d));

			//Now we make sure the current tile is in bounds 
			if (tx < 0 || tx > map->w) continue;
			if (ty < 0 || ty > map->h) continue;

			//If the tile is a wall, mark it as seen but stop the line early

			if (map->map[tx + ty * map->w] == WALL) {
				//Note x + y * w is a formula for finding a 1d index from a 2d coordinates
				map->seen[tx + ty * map->w] = 1;
				break;
			}
			//Otherwise set it as in glare and seen
			map->seen[tx + ty * map->w] = 1;
			map->glare[tx + ty * map->w] = 1;
		}
	}
}
*/

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
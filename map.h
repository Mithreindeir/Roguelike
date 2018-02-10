#ifndef MAP_H
#define MAP_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ncurses.h>
#include <math.h>
#include <time.h>
#include "astar.h"

//#define DEBUG
#define WALL '#'
#define IDXCOORD(x, y, w) ((y)*(w)+(x))

enum OBJECT_ID
{
	DOOR,
	PLAYER,
	ENEMY,
	END
};

static const char * object_names[] = {
	"A door",
	"The player",
	"An enemy",
	"the end"
};

struct game_object
{
	int y, x;
	int speed;
	int c_spd;
	int id;
	int passive;
	int ch;
	void* update;
	void* destroy;
	void* data;
};

struct game_map {
	int offset;
	struct game_object ** objects;
	int num_objects;
	int completed;

	int w, h;
	char * map;
	int * seen;
	int * glare;
	char ** events;
	int num_events;
	//This if for A* algorithm
	struct node ** nodes;
	int num_nodes;
};

typedef void (*game_object_update)(struct game_map * map, struct game_object*, int ch);
typedef void (*game_object_free)(void * data);

struct game_map * new_map(int w, int h);
int add_obj(struct game_map * map, int x, int y, int ch, int id, game_object_update update, game_object_free destroy);
void free_map(struct game_map * map);
int move_obj(struct game_map * map, struct game_object * obj, int y, int x);
void update_objects(struct game_map * map, int ch);
void interact(struct game_map * map, struct game_object * sobj, struct game_object * dobj);
void add_event(struct game_map * map, char * event);
void pop_event(struct game_map * map);
struct node_stack* get_neighbors(struct game_map * m, struct node * current);
void cell_auto(struct game_map * map, int iters);


#endif

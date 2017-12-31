#ifndef ENEMY_H
#define ENEMY_H

#include "map.h"
#include "fov.h"
#include "astar.h"

struct enemy
{
	//Stand still and if player is seen pursue
	int px, py;
	int active;
	struct node_stack * ns;
	int path;
	int health;
};

struct enemy * init_enemy();
void enemy_update(struct game_map * map, struct game_object * obj, int ch);

#endif
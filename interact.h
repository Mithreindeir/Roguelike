#ifndef INTERACT_H
#define INTERACT_H

#include "map.h"
#include "player.h"
#include "enemy.h"

void interact(struct game_map *map, struct game_object *sobj,
	      struct game_object *dobj);

#endif

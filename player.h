#ifndef PLAYER_H
#define PLAYER_H

#include "map.h"

struct player
{
	int health;
};

struct player * init_player();
void player_update(struct game_map * map, struct game_object * obj, int ch);
void destroy_player(struct player * p);


#endif

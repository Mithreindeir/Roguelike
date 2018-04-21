#include "player.h"

struct player *init_player()
{
	struct player *pd = malloc(sizeof(struct player));
	pd->health = 100;
	return pd;
}

void player_update(struct game_map *map, struct game_object *obj, int ch)
{

	//if (kbhit()) {
	//int ch = getch();
	if (ch == KEY_UP) {
		move_obj(map, obj, -1, 0);
	} else if (ch == KEY_RIGHT) {
		move_obj(map, obj, 0, 1);
	} else if (ch == KEY_LEFT) {
		move_obj(map, obj, 0, -1);
	} else if (ch == KEY_DOWN) {
		move_obj(map, obj, 1, 0);
	} else {
		//ungetch(ch);
	}
	//}
}

void destroy_player(struct player *p)
{
	if (!p)
		return;

	free(p);
}

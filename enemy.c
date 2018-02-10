#include "enemy.h"

struct enemy * init_enemy()
{
	struct enemy * e = malloc(sizeof(struct enemy));
	e->px = 0, e->py = 0;
	e->active = 0;
	e->ns = NULL;
	e->path = 0;
	e->health = 10;
	return e;
}

void destroy_enemy(struct enemy * e)
{
	if (!e) return;
	if (e->ns) {	
		for (int i = 0; i < e->ns->num_nodes; i++) {
			free(e->ns->nodes[i]);
		}

		ns_destroy(e->ns);
	}
	free(e);
}

void enemy_update(struct game_map * map, struct game_object * obj, int ch)
{
	int newa = 0;
	int pidx = -1;
	struct enemy * e = obj->data;
	int len = 0;
	int * objs = detect(map, obj->x, obj->y, 7, &len);
	if (len > 0) {
		for (int i = 0; i < len; i++) {
			struct game_object * cobj = map->objects[objs[i]];
			if (cobj->id == PLAYER && ((e->px != cobj->x) || (e->py != cobj->y))) {
				if (e->active) {
					ns_destroy(e->ns);
				}
				e->px = cobj->x;
				e->py = cobj->y;
				e->active = 1;
				newa = 1;
				pidx = objs[i];
			}
		}

		free(objs);
	}
	if (newa) {
		int idx0 = map->objects[pidx]->x + map->objects[pidx]->y*map->w, idx1 = obj->x + obj->y*map->w;

   		a_star(map->nodes[idx0], map->nodes[idx1], map, (struct node_stack * (*)(void *, struct node *))get_neighbors);

 		struct node_stack * ns = ns_init();
		struct node * next = map->nodes[idx1];
		while (next->parent) {
			ns_add(ns, next);
			if (next == map->nodes[idx0])
				break;
			next = next->parent;
		}
		e->ns = ns;
	}
	if (e->active) {
		struct node * n;
		for (int i = 0; i < e->ns->num_nodes-1; i++) {
			n = e->ns->nodes[i];
			if (n->x == obj->x && n->y == obj->y) {
				n = e->ns->nodes[i+1];
				break;
			}
		}
		int dx = n->x-obj->x, dy = n->y-obj->y;
		move_obj(map, obj, dy, dx);
		struct node * end = e->ns->nodes[e->ns->num_nodes-1];
		 if (obj->x == end->x && obj->y == end->y) {
			e->path = 0;
			ns_destroy(e->ns);
			e->ns = NULL;
			e->active = 0;
		}
	}
}

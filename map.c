#include "map.h"

struct game_map *new_map(int w, int h)
{
	struct game_map *map = malloc(sizeof(struct game_map));

	map->objects = NULL;
	map->num_objects = 0;
	map->w = w;
	map->h = h;
	map->map = malloc(w * h);
	map->seen = malloc(sizeof(int) * w * h);
	for (int i = 0; i < (w * h); i++) {
		map->seen[i] = 0;
	}
	map->glare = malloc(sizeof(int) * w * h);
	for (int i = 0; i < (w * h); i++) {
		map->glare[i] = 0;
	}
	map->completed = 0;
	map->events = NULL;
	map->num_events = 0;
	map->nodes = malloc(sizeof(struct node *) * (map->w * map->h));
	map->num_nodes = map->w * map->h;

	return map;
}

void add_event(struct game_map *map, char *event)
{
	//fprintf(stderr, "%s\n", event);
	char *str = strdup(event);
	map->num_events++;
	if (map->num_events == 1) {
		map->events = malloc(sizeof(char *));
	} else {
		map->events =
		    realloc(map->events, sizeof(char *) * map->num_events);
	}

	map->events[map->num_events - 1] = str;
	mvprintw(3, 1, "%s", map->events[map->num_events - 1]);
}

void pop_event(struct game_map *map)
{
	if (map->num_events <= 0)
		return;
	map->num_events--;
	char **events = malloc(sizeof(char *) * (map->num_events));

	free(map->events[0]);
	for (int i = 0; i < map->num_events; i++) {
		events[i] = map->events[i + 1];
	}

	free(map->events);
	map->events = events;
}

int add_obj(struct game_map *map, int x, int y, int ch, int id,
	    game_object_update update, game_object_free destroy)
{
	map->num_objects++;
	if (map->num_objects == 1) {
		map->objects = malloc(sizeof(struct game_object *));
	} else {
		map->objects =
		    realloc(map->objects,
			    sizeof(map->objects) * map->num_objects);
	}
	struct game_object *o = malloc(sizeof(struct game_object));
	o->x = x, o->y = y, o->ch = ch;
	o->update = update;
	o->id = id;
	o->passive = 0;
	o->speed = 1;
	o->c_spd = o->speed;
	o->destroy = destroy;
	map->objects[map->num_objects - 1] = o;
	return map->num_objects - 1;
}

void free_map(struct game_map *map)
{
	for (int i = 0; i < map->num_events; i++) {
		free(map->events[i]);
	}
	free(map->events);

	for (int i = 0; i < map->num_objects; i++) {
		if (map->objects[i]->data && map->objects[i]->destroy) {
			game_object_free destroy = map->objects[i]->destroy;
			destroy(map->objects[i]->data);
		}
		free(map->objects[i]);
	}

	if (map->objects)
		free(map->objects);
	free(map->glare);
	free(map->seen);
	for (int i = 0; i < map->num_nodes; i++) {
		free(map->nodes[i]);
	}
	free(map->nodes);
	free(map->map);
	free(map);
	map = NULL;
}

int move_obj(struct game_map *map, struct game_object *obj, int y, int x)
{
	//if ((abs(y) + abs(x)) > 2) return 0;
	int nx = obj->x + x, ny = obj->y + y;
	if (nx < 0)
		nx = 0;
	else if (nx >= map->w)
		nx = map->w - 1;
	if (ny < 0)
		ny = 0;
	else if (ny >= map->h)
		ny = map->h - 1;
	int idx = nx + ny * map->w;
	if (map->map[idx] != '.')
		return 0;
	for (int i = 0; i < map->num_objects; i++) {
		if (map->objects[i]->passive)
			continue;
		if (map->objects[i] == obj)
			continue;
		if ((map->objects[i]->x == nx) && (map->objects[i]->y == ny)) {
			interact(map, obj, map->objects[i]);
			return 0;
		}
	}
	obj->x = nx, obj->y = ny;
	return 1;
}

void update_objects(struct game_map *map, int ch)
{
	for (int i = 0; i < map->num_objects; i++) {
		struct game_object *obj = map->objects[i];
		game_object_update func = obj->update;
		obj->c_spd--;
		if (func && obj->c_spd == 0) {
			func(map, obj, ch);
			obj->c_spd = obj->speed;
		}
	}
}

void end_update(struct game_map *map, struct game_object *obj)
{
	for (int i = 0; i < map->num_objects; i++) {
		if (map->objects[i]->id == PLAYER) {
			if ((map->objects[i]->x == obj->x)
			    && (map->objects[i]->y == obj->y)) {
				map->completed = 1;
			}
		}
	}
}

struct node_stack *get_neighbors(struct game_map *m, struct node *current)
{
	struct node_stack *n = ns_init();

	int x = 0, y = 0;
	for (int i = 0; i < m->h; i++) {
		for (int j = 0; j < m->w; j++) {
			if (current == m->nodes[IDXCOORD(j, i, m->w)]) {
				x = j;
				y = i;
			}
		}
	}
	if ((x - 1) >= 0) {
		ns_add(n, m->nodes[IDXCOORD(x - 1, y, m->w)]);
	}
	if ((y + 1) < m->h) {
		ns_add(n, m->nodes[IDXCOORD(x, y + 1, m->w)]);
	}
	if (y - 1 >= 0) {
		ns_add(n, m->nodes[IDXCOORD(x, y - 1, m->w)]);
	}
	if ((x + 1) < m->w) {
		ns_add(n, m->nodes[IDXCOORD(x + 1, y, m->w)]);
	}

	return n;
}

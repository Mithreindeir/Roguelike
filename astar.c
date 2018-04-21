#include "astar.h"

struct node *node_init(int x, int y, int wall)
{
	struct node *n = malloc(sizeof(struct node));
	n->x = x;
	n->y = y;
	n->wall = wall;

	n->g = 0;
	n->f = 0;
	n->h = 0;
	n->parent = NULL;

	return n;
}

struct node_stack *ns_init()
{
	struct node_stack *ns = malloc(sizeof(struct node_stack));

	ns->nodes = NULL;
	ns->num_nodes = 0;

	return ns;
}

void ns_add(struct node_stack *ns, struct node *n)
{
	ns->num_nodes++;
	if (ns->num_nodes == 1) {
		ns->nodes = malloc(sizeof(struct node *));
	} else {
		ns->nodes =
		    realloc(ns->nodes, sizeof(struct node *) * ns->num_nodes);
	}
	ns->nodes[ns->num_nodes - 1] = n;
}

struct node *ns_top(struct node_stack *ns)
{
	int idx = 0;
	int cost = 100000;
	for (int i = 0; i < ns->num_nodes; i++) {
		if (ns->nodes[i]->f < cost) {
			cost = ns->nodes[i]->f;
			idx = i;
		}
	}
	return ns->nodes[idx];
}

int ns_find(struct node_stack *ns, struct node *n)
{
	int idx = -1;
	for (int i = 0; i < ns->num_nodes; i++) {
		if (ns->nodes[i] == n && idx == -1) {
			idx = i;
		}
	}
	return idx == -1 ? 0 : 1;
}

void ns_remove(struct node_stack *ns, struct node *n)
{
	int idx = -1;
	for (int i = 0; i < ns->num_nodes; i++) {
		if ((ns->nodes[i] == n) && (idx == -1)) {
			idx = i;
		} else if ((ns->nodes[i] == n)) {
			printf("ERROR:Node in array twice\n");
			return;
		}
	}

	if (idx == -1)
		return;
	struct node **nodes = NULL;
	if ((ns->num_nodes - 1) > 0)
		nodes = malloc(sizeof(struct node *) * (ns->num_nodes - 1));

	int c = 0;
	for (int i = 0; i < ns->num_nodes; i++) {
		if (i != idx) {
			nodes[c] = ns->nodes[i];
			c++;
		}
	}
	free(ns->nodes);
	ns->num_nodes--;
	ns->nodes = nodes;
}

void ns_destroy(struct node_stack *ns)
{
	free(ns->nodes);
	free(ns);
	ns = NULL;
}

int heuristic(struct node *a, struct node *b)
{
	return sqrt(pow(a->x - b->x, 2) + pow(a->y - b->y, 2));
}

void a_star(struct node *start, struct node *end, void *map,
	    neighbor_func get_neighbors)
{
	start->g = 0;
	struct node_stack *open = ns_init();
	struct node_stack *closed = ns_init();
	ns_add(open, start);

	while (open->num_nodes > 0) {
		struct node *current = ns_top(open);

		if (current == end)
			break;

		ns_remove(open, current);
		ns_add(closed, current);

		struct node_stack *neighbors = get_neighbors(map, current);
		for (int i = 0; i < neighbors->num_nodes; i++) {
			struct node *neighbor = neighbors->nodes[i];
			if (ns_find(closed, neighbor) || neighbor->wall)
				continue;

			int cost = current->g + 1;
			int bestg = 0;

			if (!ns_find(open, neighbor)) {
				bestg = 1;
				neighbor->h = heuristic(neighbor, end);
				ns_add(open, neighbor);
			} else if (cost < neighbor->g) {
				bestg = 1;
			}

			if (bestg) {

				neighbor->parent = current;
				neighbor->g = cost;
				neighbor->f = neighbor->g + neighbor->h;
			}

		}
		ns_destroy(neighbors);
	}

	ns_destroy(open);
	ns_destroy(closed);
}

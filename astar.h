#ifndef ASTAR_H
#define ASTAR_H

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <termios.h>


struct node {	
	int x, y;
	int g;
	int f;
	int h;
	int wall;
	struct node * parent;
};

struct node_stack {
	struct node ** nodes;
	int num_nodes;
};

typedef struct node_stack*(neighbor_func)(void * map, struct node * current);

struct node * node_init(int x, int y, int wall);

struct node_stack * ns_init();
void ns_add(struct node_stack * ns, struct node * n);
struct node * ns_top(struct node_stack * ns);
int ns_find(struct node_stack *ns, struct node * n);
void ns_remove(struct node_stack *ns, struct node * n);
void ns_destroy(struct node_stack * ns);

int heuristic(struct node * a, struct node * b);
void a_star(struct node * start, struct node * end, void * map, neighbor_func get_neighbors);

#endif
#include "interact.h"


void interact(struct game_map * map, struct game_object * sobj, struct game_object * dobj)
{
        if (sobj->passive || dobj->passive)
                return;
        char first[50];
        sprintf(first, "You Have Found %s", object_names[dobj->id]);
        add_event(map,  first);
        //free(first);
        if (sobj->id == ENEMY && dobj->id == PLAYER) {
                struct player * pd = dobj->data;
                //struct enemy * e = sobj->data;
                pd->health--;
        }
        else if (dobj->id == ENEMY && sobj->id == PLAYER) {
                //struct player * pd = sobj->data;
                struct enemy * e = dobj->data;
                e->health--;
                if (e->health < 0)
                        dobj->passive = 1;
        }

        if (dobj->id == DOOR) {
                dobj->passive = 1;
        }		
}

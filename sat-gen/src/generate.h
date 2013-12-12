#ifndef __GENERATE_H
#define __GENERATE_H

/* Generates all graphs with three matchings, no pair needs to form a cycle */
void generate_graphs_no_cycle(int nvertices);
/* Generates all graphs with three matchings, one pair form a cycle */
void generate_graphs_one_cycle(int nvertices);
/* Generates all graphs with three matchings, two pairs form cycles */
void generate_graphs_two_cycles(int nvertices); 
/* Generates one random graph with the given number of vertices */
void generate_graphs_random(int nvertices);

#endif

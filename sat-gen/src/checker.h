#ifndef __CHECKER_H
#define __CHECKER_H

#include "consts.h"
#include "clauses.h"

/* Forks off process and checks graph with minisat.
   If counterexample, copy graph to OUTPUT_FILE. */
void check_graph(const Clauses& cl, int reverse = 0);

/* Checks whether a graph is bipartite */
bool check_bipartite(istream& in);



#endif

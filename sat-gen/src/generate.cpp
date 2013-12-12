#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <time.h>
#include <assert.h>

#include "generate.h"
#include "clauses.h"
#include "consts.h"
#include "checker.h"

/* Checks the missing transitive constraints */
void check_transitive(int nvertices, Clauses& cl) {
	for (int i = 0; i < nvertices; ++i) {
		for (int j = 0; j < nvertices; ++j) {
			for (int k = 0; k < nvertices; ++k) {
				if ((i % 2) != (j % 2) || (i % 2) != (k % 2) || (j % 2) != (k % 2)) {
					cl.addClause(cl.var(i, j));
					cl.addClause(cl.var(j, k));
					cl.addClause(cl.var(k, i));
					//cl.writeSAT(stdout);
					check_graph(cl, 1);
					cl.removeClause();
					cl.removeClause();
					cl.removeClause();
				}
			}
		}
	}
}

/* Adds first two matchings as a cycle */
void add_cycle(int nvertices, Clauses& cl) {
    /* First matching */
    for (int i = 0; i < nvertices; i += 2) {
        cl.addEdge(0, i, (i + 1) % nvertices);
    }
    
    /* Second matching */
    for (int i = 1; i < nvertices; i += 2) {
        cl.addEdge(1, i, (i + 1) % nvertices);
    }
}

/* Fill out third matching arbitrarily */
void backtrack_one_cycle(int nvertices, Clauses& cl, vector<bool>& used, int pos, int max_diff) {
    if (pos >= nvertices) {
        #if DEBUG_GEN
        cl.writeGraph(stdout);
        #else
        check_graph(cl);
        #endif
    } else {
        /* Current vertex already matched */
        if (used[pos]) {
            backtrack_one_cycle(nvertices, cl, used, pos + 1, max_diff);
        } else {
            /* Add possible edges. Do not add edge (0, nvertices - 1) */
            int max_vert = (pos == 0) ? (nvertices - 1) : nvertices;
            for (int i = pos + 3; i < max_vert; i += 2) {
                /* Optimisation: rotation-invariant amount min(j - i, nvertices - (j - i))  
                   is maximal for the first edge */  
                if (! used[i] && min(i - pos, nvertices - (i - pos)) <= max_diff) {
                    used[i] = true;
                    cl.addEdge(2, pos, i);
                                    
                    if (pos == 0) {
                        backtrack_one_cycle(nvertices, cl, used, pos + 1, min(i, nvertices - i));
                    } else {
                        backtrack_one_cycle(nvertices, cl, used, pos + 1, max_diff);
                    }
                
                    cl.removeLastEdge();
                    used[i] = false;
                }
                
                /* Rudimentary progress */
                if (pos == 0) {
                    printf("Progress: %.0f%%\n", 100 * (double) (i - 1) / (nvertices - 4));
                }
            }
        }
    }
}

/* Fill out third matching such that it forms a cycle with matching 0 */
void backtrack_two_cycles(int nvertices, Clauses& cl, vector<bool>& used, int pos, int nr, int max_diff) {
    if (nr >= nvertices - 2) {
        cl.addEdge(2, pos, 1);
        #if DEBUG_GEN
        cl.writeGraph(stdout);
        #else
        check_graph(cl);
        #endif
        cl.removeLastEdge();
    } else {
        assert(pos % 2 == 0);
        
        /* Try to continue cycle */
        int max_vert = (pos == 0) ? (nvertices - 1) : nvertices;
        for (int i = 3; i < max_vert; i += 2) {
            if (! used[i] && abs(i - pos) > 1 && min(abs(i - pos), nvertices - abs(i - pos)) <= max_diff) {
                used[i] = true;
                cl.addEdge(2, pos, i);
                if (pos == 0) {
                    backtrack_two_cycles(nvertices, cl, used, i - 1, nr + 2, min(i, nvertices - i));
                } else {
                    backtrack_two_cycles(nvertices, cl, used, i - 1, nr + 2, max_diff);
                }
                cl.removeLastEdge();
                used[i] = false;
            }
            
            /* Rudimentary progress */
            if (pos == 0) {
                printf("Progress: %.0f%%\n", 100 * (double) (i - 1) / (nvertices - 4));
            }
        }
    }
}

/* Generate graphs, two matchings form cycle */
void generate_graphs_one_cycle(int nvertices) {
    Clauses cl(3, nvertices, true);
    vector<bool> used(nvertices, false); /* Vertex used in third matching? */
    
    /* Backtrack third matching */
    add_cycle(nvertices, cl);
    #if DEBUG_CLAUSES
    cl.writeSAT(stdout);
    #else
    backtrack_one_cycle(nvertices, cl, used, 0, INT_MAX / 2);
    #endif
}

/* Generate graphs, two pairs of matchings form cycles */
void generate_graphs_two_cycles(int nvertices) {
    Clauses cl(3, nvertices, true);
    vector<bool> used(nvertices, false);
    
    add_cycle(nvertices, cl);
    #if DEBUG_CLAUSES
    cl.writeSAT(stdout);
    #else
    backtrack_two_cycles(nvertices, cl, used, 0, 0, INT_MAX / 2);
    #endif
}

/* Generate random graph */
void generate_graphs_random(int nvertices) {
    Clauses cl(3, nvertices, true);
    vector<bool> used(nvertices, false); /* Vertex used in third matching? */	
	
	add_cycle(nvertices, cl);

	check_transitive(nvertices, cl);
	return;

	/* Random third matching */
    int rem = nvertices / 2;
    for (int i = 0; i < nvertices; ++i) {
        /* Skip if already matched */
        if (used[i]) {
            continue;
        } else {
            /* Choose matching vertex randomly */
            int cnt, idx;
            
            /* Note: Choosing a disjoint matching is no guaranteed <-- TODO: improve this */
            if (rem > 1 && !used[i + 1]) {
                cnt = rand() % (rem - 1) + 1;
            } else {
                cnt = rand() % rem;
            }
            idx = i + 1;
            
            /* Search for cnt'th free vertex */
            while (true) {
                if (! used[idx]) {
                    if (cnt <= 0) {
                        break;
                    }
                    cnt--;
                }
                idx += 2;
            }
            
            used[i] = true;
            used[idx] = true;
            rem--;
            cl.addEdge(2, i, idx);
        }
    }

	#if DEBUG_OPT
		check_transitive(nvertices, cl);
	#else
		#if DEBUG_GEN
		cl.writeGraph(stdout);
		#else
		check_graph(cl);
		#endif
	#endif
}

/* 
	Backtracking without constraints 
		used[matching][vertex]: Vertex already used in matching?
		adj[v1][v2]: v1 and v2 connected?
*/
void backtrack_no_cycle(int nvertices, Clauses& cl, vector<vector<bool> >& adj, 
						vector<vector<bool> >& used, int matching, int pos, int max_diff) {
	if (pos >= nvertices) {
		/* Now fill in last matching */
		if (matching == 1) {
			backtrack_no_cycle(nvertices, cl, adj, used, 2, 0, max_diff);
		} else {
			#if DEBUG_OPT
				check_transitive(nvertices, cl);
			#else
				#if DEBUG_GEN
				cl.writeGraph(stdout);
				#else
				check_graph(cl);
				#endif
			#endif
		}
	} else {
		/* Vertex already matched */
		if (used[matching][pos]) {
			backtrack_no_cycle(nvertices, cl, adj, used, matching, pos + 1, max_diff);
		} else {
			for (int i = pos + 1; i < nvertices; i += 2) {
				//cl.writeGraph(stderr);
				/* If other vertex not already used in current matching and edge does not already exist */
				if (!used[matching][i] && !adj[pos][i] && min(i - pos, nvertices - (i - pos)) <= max_diff) {
					used[matching][pos] = true;
					used[matching][i] = true;
					adj[pos][i] = true;
					adj[i][pos] = true;
					cl.addEdge(matching, pos, i);

					/* First edge of the first matching determines max_diff */
					if (pos != 0 || matching != 1) {
						backtrack_no_cycle(nvertices, cl, adj, used, matching, pos + 1, max_diff);
					} else {
						backtrack_no_cycle(nvertices, cl, adj, used, matching, pos + 1, min(i, nvertices - i));
					}

					used[matching][pos] = false;
					used[matching][i] = false;
					adj[pos][i] = false;
					adj[i][pos] = false;
					cl.removeLastEdge();
				}

				/* Rudimentary progress */
				if (matching == 1 && pos == 0 && i > 1) {
					printf("Progress: %.0f%%\n", 100 * (double) (i - 3) / (nvertices - 4));
				}
			}
		}
	}
}

/* Generate graphs consisting of three disjoint matchings without constraints */
void generate_graphs_no_cycle(int nvertices) {
	Clauses cl(3, nvertices, true);

	/* Is edge already used */
	vector<vector<bool> > adj(nvertices, vector<bool>(nvertices, false));
	vector<vector<bool> > used(3, vector<bool>(nvertices, false));

	/* First matching: 0 <-> 1, 2 <-> 3, ... */
	for (int i = 0; i < nvertices; i += 2) {
		cl.addEdge(0, i, i + 1);
		adj[i][i + 1] = true;
		adj[i + 1][i] = true;
	}
	
    backtrack_no_cycle(nvertices, cl, adj, used, 1, 0, INT_MAX / 2);
}

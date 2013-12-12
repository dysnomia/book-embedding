#include <vector>

#include "consts.h"
#include "clauses.h"
#include "reduce.h"
#include "checker.h"

using namespace std;

/* New index of i after deleting a and b */
int new_index(int i, int a, int b) {
    int m1 = min(a, b);
    int m2 = max(a, b);
    
    if (i < m1) {
        return i;
    } else if (i < m2) {
        return i - 1;
    } else {
        return i - 2;
    }
}

/* Add edges in reduced graph.
   Returns whether it was successful. */
bool create_reduced_graph(Clauses& cl, int nmatchings, int nvertices, int a, int b, const vector<vector<int> >& match) {
    bool success = true;
    vector<bool> has_edge(nvertices*nvertices, false);

    for (int i = 0; i < nmatchings; ++i) {
        vector<bool> used(nvertices, false);
        for (int j = 0; j < nvertices; ++j) {
            if (j != a && j != b && ! used[j]) {
                used[j] = true;
                
                /* New edge lies between v1 and v2 */
                int v1 = new_index(j, a, b), v2;
                if (match[i][j] == a) {
                    used[match[i][b]] = true;
                    v2 = new_index(match[i][b], a, b);
                } else if (match[i][j] == b) {
                    used[match[i][a]] = true;
                    v2 = new_index(match[i][a], a, b);
                } else {
                    used[match[i][j]] = true;
                    v2 = new_index(match[i][j], a, b);
                }
            
                /* Matchings must be disjoint! */
                if (has_edge[nvertices * min(v1, v2) + max(v1, v2)]) {
                    success = false;
                    goto end;
                } else {
                    has_edge[nvertices * min(v1, v2) + max(v1, v2)] = true;
                    cl.addEdge(i, v1, v2);
                }
            }
        }
    }

end:
    return success;
}

void reduce_and_test(istream& in, bool cycle) {
    int nmatchings, nvertices;
    
    /* Read all graphs in file and reduce them */
    while (in >> nmatchings >> nvertices) {
        vector<vector<int> > match(nmatchings, vector<int>(nvertices));
        
        for (int i = 0; i < nmatchings; ++i) {
            for (int j = 0; j < nvertices / 2; ++j) {
                int a, b;
                in >> a >> b;
                match[i][a] = b;
                match[i][b] = a;
            }
        }
        
        Clauses cl(nmatchings, nvertices - 2, false);
        
        /* Delete vertices (one odd, one even to guarantee bipartiteness) */
        for (int a = 0; a < nvertices; ++a) {
            for (int b = 0; b < a; ++b) {
                /* Delete adjacent vertices -> one cycle */
                if (cycle && (a - b) > 1) {
                    continue;
                }
            
                if ((a + b) % 2 == 1) {
                    Clauses cln(cl);
                    
                    /* Create smaller graph */
                    bool valid = create_reduced_graph(cln, nmatchings, nvertices, a, b, match);
                    
                    if (valid) {
                        #if DEBUG_GEN
                        cout << "Deleted: " << a << " and " << b << endl;
                        cln.writeGraph(stdout);
                        #else
                        check_graph(cln);
                        #endif
                    }
                }
            }        
        }
    }
}

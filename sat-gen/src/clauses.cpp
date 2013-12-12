#include <algorithm>
#include <iterator>
#include <fstream>

#include "clauses.h"

void Clauses::addEdge(int matching, int k, int l) {
    edge_t e;
    e.a = k;
    e.b = l;
    
    /* Create clauses */
    int cnt = 0;
    for (EdgeIterator it = adj[matching].begin(); it != adj[matching].end(); ++it) {
        int i = it->a;
        int j = it->b;
        
        /* i < k < j -> i < l < j*/
        addClause(-var(i,k), -var(k,j), var(i,l));
        addClause(-var(i,k), -var(k,j), var(l,j));
        
        /* i < l < j -> i < k < j */
        addClause(-var(i,l), -var(l,j), var(i,k));
        addClause(-var(i,l), -var(l,j), var(k,j));
		
		/* j < k < i -> j < l < i */
        addClause(-var(j,k), -var(k,i), var(j,l));
        addClause(-var(j,k), -var(k,i), var(l,i));
        
        /* j < l < i -> j < k < i */
        addClause(-var(j,l), -var(l,i), var(j,k));
        addClause(-var(j,l), -var(l,i), var(k,i));

  //      /* k < i < l -> k < j < l*/
  //      addClause(-var(k,i), -var(i,l), var(k,j));
  //      addClause(-var(k,i), -var(i,l), var(j,l));
  //      
  //      /* k < j < l -> k < i < l */
  //      addClause(-var(k,j), -var(j,l), var(k,i));
  //      addClause(-var(k,j), -var(j,l), var(i,l));
  //
  //      /* l < i < k -> l < j < k */
  //      addClause(-var(l,i), -var(i,k), var(l,j));
  //      addClause(-var(l,i), -var(i,k), var(j,k));
  //      
  //      /* l < j < k -> l < i < k */
  //      addClause(-var(l,j), -var(j,k), var(l,i));
  //      addClause(-var(l,j), -var(j,k), var(i,k));
        
        cnt += 8;
    }
    
    /* Add edge */
    adj[matching].push_back(e);
    edge_sizes.push_back(cnt);
    edge_matchings.push_back(matching);
}

void Clauses::removeLastEdge() {
    /* Remove clauses */
    clauses.erase(clauses.end() - edge_sizes.back(), clauses.end());
    edge_sizes.pop_back();
    
    /* Remove edge */
    adj[edge_matchings.back()].pop_back();
    edge_matchings.pop_back();
}

void Clauses::addStandardClauses(bool optimiseTransitive) {
    /* Optimisation: vertex 0 comes first */
    for (int i = 1; i < nvertices; ++i) {
        addClause(var(0, i));
    }
    
    /* Clauses for a strict order */
    
	/* Irreflexive */
	for (int i = 0; i < nvertices; ++i) {
		addClause(-var(i, i));
	}

    /* Anti-symmetry and totality */
    for (int i = 0; i < nvertices; ++i) {
        for (int j = i + 1; j < nvertices; ++j) {
            /* i < j xor j < i */
            addClause(var(i, j), var(j, i));
            addClause(-var(i, j), -var(j, i));
        }
    }

    /* Transitivity */
    if (optimiseTransitive) {
        for (int i = 0; i < nvertices; i += 2) {
            for (int j = 0; j < nvertices; j += 2) {
                for (int k = 0; k < nvertices; k += 2) {
                    /* i < j, j < k -> i < k */
                    addClause(-var(i, j), -var(j, k), var(i, k));
                    addClause(-var(i+1,j+1), -var(j+1,k+1), var(i+1,k+1));
                }
            }
        }
    } else {
        for (int i = 0; i < nvertices; ++i) {
            for (int j = 0; j < nvertices; ++j) {
                for (int k = 0; k < nvertices; ++k) {
                    /* i < j, j < k -> i < k */
                    addClause(-var(i, j), -var(j, k), var(i, k));
                }
            }
        }
    }
}

void Clauses::writeSAT(FILE* out) const {
    fprintf(out, "p cnf %d %d\n", nvertices*nvertices, (int) clauses.size());
    for (int i = 0; i < (int) clauses.size(); ++i) {
        fprintf(out, clauses[i].c_str());
    }
    fflush(out);

	/* Debugging the optimisation -> output dnf of negated transitive */
	//#if DEBUG_OPT
	//	ofstream fo("debug_opt.out");
	//	fo << "(p" << var(1, 2) << "*" << "!p" << var(1, 2) << ")";
	//	for (int i = 0; i < nvertices; ++i) {
	//		for (int j = 0; j < nvertices; ++j) {
	//			for (int k = 0; k < nvertices; ++k) {
	//				if ((i % 2) != (j % 2) || (i % 2) != (k % 2) || (j % 2) != (k % 2)) {
	//					fo << "+" << "(p" << var(i, j) << "*p" << var(j, k) << "*p" << var(k, i) << ")";
	//				}
	//			}
	//		}
	//	}
	//	fo << "\n";
	//	fo.close();
	//#endif
}

void Clauses::writeGraph(FILE* out) const {
    fprintf(out, "%d\n", nmatchings);
    fprintf(out, "%d\n", nvertices);
    
    for (int i = 0; i < nmatchings; ++i) {
        for (int j = 0; j < (int) adj[i].size(); ++j) {
            fprintf(out, "%d %d ", adj[i][j].a, adj[i][j].b);
        }
        fprintf(out, "\n");
    }
    fflush(out);
}

#ifndef __CLAUSE_H
#define __CLAUSE_H

#include <stdio.h>

#include <vector>
#include <sstream>
#include <boost/lexical_cast.hpp>

#include "consts.h"

using namespace std;
using namespace boost;

/* Simple (undirected) edge */
struct edge_t {
    int a, b;
};

/* Stores the set of SAT-clauses together with their underlying matchings */
class Clauses {
private:
    /* Edge lists of matchings */
    vector<vector<edge_t> > adj;
    typedef vector<edge_t>::iterator EdgeIterator;
    
    /* Number of matchings and order of graph */
    int nmatchings, nvertices;
    
    /* Clauses as strings */
    vector<string> clauses;
    /* Number of clauses added for a single edge and last matching added */
    vector<int> edge_sizes;
    vector<int> edge_matchings;

    /* Adds the standard clauses for strict orders */
    void addStandardClauses(bool optimise); 
    
public:
	/* Variable name corresponding to statement "i < j" (starts at 1) */
    inline int var(int i, int j) const {
        return nvertices * i + j + 1;
    }

    /* Adds a clause */
    inline void addClause(int v1, int v2, int v3) {
        clauses.push_back(lexical_cast<string>(v1)
                  + " " + lexical_cast<string>(v2)
                  + " " + lexical_cast<string>(v3) + " 0\n");
    }
    
    inline void addClause(int v1, int v2) {
        clauses.push_back(lexical_cast<string>(v1)
                  + " " + lexical_cast<string>(v2) + " 0\n");
    }
    
    inline void addClause(int v1) {
        clauses.push_back(lexical_cast<string>(v1) + " 0\n");
    }

	/* Removes one clause */
	inline void removeClause() {
		clauses.pop_back();
	}

    /* Adds a single edge in a matching and the corresponding constraints */
    void addEdge(int matching, int a, int b);
    
    /* Removes the last edge (must be called in order!) */
    void removeLastEdge();
    
    /* Creates new clauses object with given number
       of matchings and vertices */
    Clauses(int matchings, int vertices, bool optimise_transitive) : 
            adj(matchings, vector<edge_t>(0)), nmatchings(matchings), nvertices(vertices) {
        addStandardClauses(optimise_transitive);
    }
    
    Clauses(const Clauses& cl) : 
            adj(cl.adj), nmatchings(cl.nmatchings), nvertices(cl.nvertices),
            clauses(cl.clauses), edge_sizes(cl.edge_sizes), edge_matchings(cl.edge_matchings) {
    }
    
    /* Reads graph from file */
    Clauses(istream& in, bool optimise_transitive) : adj(0, vector<edge_t>(0)) {
        in >> nmatchings >> nvertices;
        string tmp;
        getline(in, tmp);
        
        addStandardClauses(optimise_transitive);
        adj.resize(nmatchings, vector<edge_t>(0));
        
        /* Read edges */
        for (int i = 0; i < nmatchings; ++i) {
            getline(in, tmp);
            stringstream ss(tmp);
            
            int a, b;
            while (ss >> a) {
                ss >> b;
                addEdge(i, a, b);
            }
        }
    }
    
    /* Writes the SAT program to a file */
    void writeSAT(FILE* out) const;
    
    /* Writes the graph to a file */
    void writeGraph(FILE* out) const;
};

#endif

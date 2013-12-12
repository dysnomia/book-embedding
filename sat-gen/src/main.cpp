#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <time.h>

#include <iostream>
#include <fstream>
#include <vector>

#include "consts.h"
#include "clauses.h"
#include "generate.h"
#include "checker.h"
#include "reduce.h"
#include "tools.h"

using namespace std;

/* Transform string array to int vector */
vector<int> get_sizes(char* arg[], int num) {
    /* Read sizes */
    vector<int> sizes;
    for (int i = 0; i < num; ++i) {
        try {
            int nvertices = lexical_cast<int>(arg[i]);
            sizes.push_back(nvertices);
        } catch (bad_lexical_cast&) {
            fprintf(stderr, "Graph orders must be integers\n");
            exit(EXIT_FAILURE);
        }
    }
    
    return sizes;
}

void usage() {
    printf("Usage: gen <command> <arguments>\n");
    printf("commands:\n");
    printf("   random <cnt> <size>: generate <cnt> random graphs of orders <sizes>\n");
    printf("   open <file>: read graph from <file> and test it\n");
    printf("   s0c <sizes>: search all graphs of the orders <sizes> (without cycle constraints)\n");
    printf("   s1c <sizes>: search all graphs of the orders <sizes> (one cycle)\n");
    printf("   s2c <sizes>: search all graphs of the orders <sizes> (two cycles)\n");
    printf("   knn <dir> <a> <b>: generate K_{n, n} in <dir> for n = <a>, ..., <b>\n");
    printf("   contract <file> <c>: reads graph from <file> and tests its reduced graphs\n");
    printf("                if <c> == 1: we require the reduced graph to have one cycle\n");
    printf("   bipartite <file>: checks whether graph in <file> is bipartite\n");
    exit(EXIT_FAILURE);
}

int main(int argc, char* argv[]) {
    if (argc <= 1) {
        usage();
    }
    
    srand(time(NULL));
    
    if (argv[1][0] == 'o') {
        if (argc <= 2) {
            usage();
        }
    
        /* Graph from file */
        ifstream fin(argv[2]);
        
        if (fin) {
            Clauses cl(fin, false);
            #if DEBUG_CLAUSES
            cl.writeSAT(stdout);
            return EXIT_SUCCESS;
            #else
            check_graph(cl);
            #endif
        } else {
            fprintf(stderr, "Error opening graph\n");
            return EXIT_FAILURE;
        }
    } else if (argv[1][0] == 'k') {
        if (argc <= 4) {
            usage();
        }
        
        int start = lexical_cast<int>(argv[3]);
        int end = lexical_cast<int>(argv[4]);
        gen_knn_cyclic(argv[2], start, end);
        
        return EXIT_SUCCESS;
    } else if (argv[1][0] == 's') {
        if (argc <= 2) {
            usage();
        }
    
        /* Fixed size graph */
        vector<int> sizes = get_sizes(argv + 2, argc - 2);

        /* Now generate graphs */
        vector<int>::iterator it;
        for (it = sizes.begin(); it != sizes.end(); ++it) {
            if (argv[1][1] == '0') {
                generate_graphs_no_cycle(*it);
            } else if (argv[1][1] == '1') {
                generate_graphs_one_cycle(*it);
            } else {
                generate_graphs_two_cycles(*it);
            }
        }
    } else if (argv[1][0] == 'r') {
        if (argc <= 3) {
            usage();
        }
    
        /* Random graph */
        int cnt = lexical_cast<int>(argv[2]), tmp;
        vector<int> sizes = get_sizes(argv + 3, argc - 3);
        
        for (int i = 0; i < cnt; ++i) {
            tmp = rand() % ((int) sizes.size());
            generate_graphs_random(sizes[tmp]);
        }
    } else if (argv[1][0] == 'c') {
        if (argc <= 3) {
            usage();
        }
    
        ifstream fin(argv[2]);
        bool one_cycle = (bool) lexical_cast<int>(argv[3]);
        
        if (fin) {
            reduce_and_test(fin, one_cycle);
        } else {
            fprintf(stderr, "Error opening graph\n");
            return EXIT_FAILURE;
        }
    } else if (argv[1][0] == 'b') {
        if (argc <= 2) {
            usage();
        }
    
        ifstream fin(argv[2]);
        
        if (fin) {
            if (check_bipartite(fin)) {
                cout << "Graph is bipartite" << endl;
            } else {
                cout << "Graph is not bipartite" << endl;
            }
            return EXIT_SUCCESS;
        } else {
            fprintf(stderr, "Error opening graph\n");
            return EXIT_FAILURE;
        }
    } else {
        fprintf(stderr, "Invalid command\n");
        return EXIT_FAILURE;
    }

    /* Wait for all children */
    int status;
    do {
        wait(&status);
        
        if (status == -1 && errno != ECHILD) {
            perror("Error during wait()\n");
            abort();
        }
    } while (status > 0);

    printf("Finished testing\n");
    
    return 0;
}


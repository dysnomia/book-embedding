#include <stdio.h>
#include <fstream>

#include "tools.h"

using namespace std;

void gen_knn_cyclic(char* dir, int start, int end) {
    /* Write Kn_n in dir */
    for (int n = start; n <= end; ++n) {
        char filename[128];
        sprintf(filename, "%s/K%d_%d", dir, n, n);
        ofstream out(filename);
        
        out << n << endl;
        out << 2 * n << endl;
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                out << j << " " << n + (j + i) % (n) << " ";
            }
            out << endl;
        }
    } 
}

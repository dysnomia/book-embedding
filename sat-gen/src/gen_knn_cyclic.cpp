/* Generate matching on K_{n,n} cyclically */
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>

using namespace std;

int main(int argc, char* argv[]) {
    /* Read parameters */
    if (argc <= 3) {
        cout << "Usage: gen_knn_cyclic <dir> <start> <end>" << endl;
        return 1;
    }
    
    int start, end;
    sscanf(argv[2], "%d", &start);
    sscanf(argv[3], "%d", &end);
    
    /* Write Kn_n in dir */
    for (int n = start; n <= end; ++n) {
        char filename[128];
        sprintf(filename, "%s/K%d_%d", argv[1], n, n);
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
    
    return 0;
}

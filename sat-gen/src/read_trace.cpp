#include <iostream>
#include <map>

/* Reads a simple sat proof trace */

using namespace std;

/* A single line statement */
struct statement {
    int cont[3];
    int cnt;
};

int n;

/* Write a single variable of the form i < j */
void write_var(int j) {
    if (j < 0) {
        cout << "-";
        j = -j;
    }
    cout << "(";
    cout << (j - 1) / n << " < " << (j - 1) % n;
    cout << ")";
}

void write_statement(const statement& s) {
    if (s.cnt == 3) {
        write_var(-s.cont[0]);
        cout << " and ";
        write_var(-s.cont[1]);
        cout << " =>  ";
        write_var(s.cont[2]);
    } else if (s.cnt == 2) {
        write_var(s.cont[0]);
        cout << " or ";
        write_var(s.cont[1]);
    } else {
        write_var(s.cont[0]);
    }
}

map<int, statement> resolve; 

int main() {
    cin >> n;

    int idx;
    while (cin >> idx) {
        statement s;
        s.cnt = 0;
        
        int i;
        for(;;) {
            cin >> i;
            if (i == 0) {
                break;
            }
            s.cont[s.cnt] = i;
            s.cnt++;
            
            /* Make cnt == 3 canonical */
            if (s.cnt == 3) {
                if (s.cont[0] > 0) {
                    swap(s.cont[0], s.cont[2]); 
                } else if (s.cont[1] > 0) {
                    swap(s.cont[1], s.cont[2]);
                }
                if (((-s.cont[0]) - 1) % n != ((-s.cont[1]) - 1) / n) {
                    swap(s.cont[0], s.cont[1]);
                }
            }
        }
        
        /* Insert into map */
        resolve[idx] = s;
        
        /* Now print it */
        write_statement(s);
        
        bool first = true;
        for (;;) {
            cin >> i;
            if (i == 0) {
                if (first) {
                    cout << "\n";
                }
                break;
            }
            if (first) {
                first = false;
                cout << " because of\n";
            }            
            
            cout << "    ";
            write_statement(resolve[i]);
            cout << "\n";
        }
    }
    
    return 0;
}

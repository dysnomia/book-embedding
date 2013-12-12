#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <errno.h>

#include <iostream>
#include <queue>

#include "checker.h"

using namespace std;

int nprocesses = 0;

/* Wait until enough children have finished */
void wait_for_children() {    
    /* Do not use too many processes */
    if (nprocesses >= MAX_PROCESSES) {
        /* Wait till a lot of processes are available */
        //cout << nprocesses << "Throttling..." << endl;
        while (nprocesses >= MAX_PROCESSES / 2) {
            int tmp;
            wait(&tmp);
            
            /* Error in child */
            if (WEXITSTATUS(tmp) != EXIT_SUCCESS) {
                fprintf(stderr, "error in child process\n");
                exit(EXIT_FAILURE);
            }
            nprocesses--;
        }
    }
}

/* fork() with error detection */
pid_t try_fork() {
    pid_t pid = fork();
    if (pid < 0) {
        if (errno == EAGAIN) {
            fprintf(stderr, "fork() failed: EAGAIN\n");
        } else if (errno == ENOMEM) {
            fprintf(stderr, "fork() failed: ENOMEM\n");
        }
        exit(EXIT_FAILURE);
    }
    return pid;
}

void check_graph(const Clauses& cl, int reverse) {
    wait_for_children();
    nprocesses += 2;
    
    pid_t pid = try_fork();
    
    /* Child process checks graph */
    if (pid == 0) {
        /* Pipe to use with minisat */
        int pipefd[2];
        if (pipe(pipefd) == -1) {
            cerr << "failed to create pipe\n";
            exit(EXIT_FAILURE);
        }
    
        /* Call minisat */
        pid = try_fork();
        if (pid == 0) {
            /* Read from pipe as stdin */
            close(pipefd[1]);
            fclose(stdout);
            dup2(pipefd[0], 0);
            
            if (execl(MINISAT_PATH, "minisat", NULL) != 0) {
               fprintf(stderr, "minisat could not be started\n");
               exit(EXIT_FAILURE);
            }
        } else {
            /* Write into pipe */
            close(pipefd[0]);
            FILE* out = fdopen(pipefd[1], "w");
            cl.writeSAT(out);
            fclose(out);
            
            /* Get exit code */
            int status;
            waitpid(pid, &status, 0);
            //cout << "Status: " << WEXITSTATUS(status) << endl;
            
            /* Not satisfiable (status code 20) -> write graph into OUTPUT_FILE */
            if ((WEXITSTATUS(status) == 20) ^ reverse) {
                cout << "Found counterexample!\n";
                FILE* out = fopen(OUTPUT_FILE, "a");
                if (out == NULL) {
                    cout << "could not open output file\n";
                    exit(EXIT_FAILURE);
                }
                
                flock(fileno(out), LOCK_SH);
                cl.writeGraph(out);
                flock(fileno(out), LOCK_UN);
                
                fclose(out);
            }
            
            exit(EXIT_SUCCESS);
        }
    }
}

/* Checks whether a graph is bipartite */
struct bfs_t {
    int node, color;
    
    bfs_t(int n, int c): node(n), color(c) {
    }
};

bool check_bipartite(istream& in) {
    int nmatchings, nvertices;
    in >> nmatchings >> nvertices;
    
    /* Create graph */
    vector<vector<int> > adj(nvertices);
    for (int i = 0; i < nmatchings; ++i) {
        for (int j = 0; j < nvertices / 2; ++j) {
            int a, b;
            in >> a >> b;
            adj[a].push_back(b);
            adj[b].push_back(a);
        }
    }
    
    /* BFS to check for two-colourability */
    vector<int> color(nvertices, -1), visited(nvertices, 0);
    queue<bfs_t> q;
    for (int start = 0; start < nvertices; ++start) {
        q.push(bfs_t(start, 0));
        while (! q.empty()) {
            bfs_t b = q.front();
            int v = b.node, c = b.color;
            q.pop();

            if (! visited[v]) {
                visited[v] = 1;
                color[v] = c;
                
                for (int i = 0; i < (int) adj[v].size(); ++i) {
                    int w = adj[v][i];
                    if (color[w] == c) {
                        return false;
                    }
                    q.push(bfs_t(w, (c + 1) % 2));
                }
            }
        }
    }
    
    return true;
}

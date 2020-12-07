//
// Created by 蒋李琦 on 2020/8/4.
//
// Edmonds-Karp algorithm
// to compute maximum flow ... COMP9024 20T2
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include "WGraph.h"
#include "queue.h"

#define NODES 6
#define VERY_HIGH_VALUE 999999

int flow[NODES][NODES];

// Find an augmenting src-dest path with fewest number of edges
bool bfs(Graph g, Vertex src, Vertex dest, int *visited) {
    Vertex v, w;
    int nV = numOfVertices(g);

    for (v = 0; v < nV; v++)
        visited[v] = -1;

    queue Q = newQueue();
    QueueEnqueue(Q, src);
    visited[src] = src;
    while (!QueueIsEmpty(Q)) {
        if ((v = QueueDequeue(Q)) == dest) {
            dropQueue(Q);
            return true;
        }
        for (w = 0; w < nV; w++) {
            int capacity = adjacent(g, v, w);
            if (visited[w] == -1 && capacity > flow[v][w]) {
                visited[w] = v;
                QueueEnqueue(Q, w);
            }
        }
    }
    dropQueue(Q);
    return false;
}

int edmondsKarp(Graph g, Vertex source, Vertex sink) {
    Vertex v, w;
    int nV = numOfVertices(g);
    int *visited = malloc(nV * sizeof(int));
    assert(visited != NULL);

    for (v = 0; v < nV; v++)
        for (w = 0; w < nV; w++)
            flow[v][w] = 0;

    int maxflow = 0;
    while (bfs(g, source, sink, visited)) {  // found augmenting path
        int df = VERY_HIGH_VALUE;             // determine how much we can send
        for (v = sink; v != source; v = visited[v]) {
            int residual = adjacent(g, visited[v], v) - flow[visited[v]][v];
            if (df > residual)
                df = residual;
        }
        // adjust flow[][] so as to represent residual graph
        for (v = sink; v != source; v = visited[v]) {
            flow[visited[v]][v] = flow[visited[v]][v] + df;
            flow[v][visited[v]] = flow[v][visited[v]] - df;
        }
        maxflow = maxflow + df;               // augment maximum flow
    }

    free(visited);
    return maxflow;
}

int main(void) {
    int nE = 8;
    Graph g = newGraph(NODES);

    Edge edges[nE];
    edges[0].v = 0; edges[0].w = 1; edges[0].weight = 2;
    edges[1].v = 0; edges[1].w = 2; edges[1].weight = 3;
    edges[2].v = 1; edges[2].w = 3; edges[2].weight = 3;
    edges[3].v = 1; edges[3].w = 4; edges[3].weight = 1;
    edges[4].v = 2; edges[4].w = 3; edges[4].weight = 1;
    edges[5].v = 2; edges[5].w = 4; edges[5].weight = 1;
    edges[6].v = 3; edges[6].w = 5; edges[6].weight = 2;
    edges[7].v = 4; edges[7].w = 5; edges[7].weight = 3;

    int i, j;
    for (i = 0; i < nE; i++) {
        insertEdge(g, edges[i]);
    }
    showGraph(g);
    putchar('\n');

    int max = edmondsKarp(g, 0, 5);
    printf("Maximum flow = %d\n", max);
    for (i = 0; i < NODES; i++)
        for (j = 0; j < NODES; j++)
            if (flow[i][j] > 0)
                printf("%d -> %d: %d\n", i, j, flow[i][j]);
    freeGraph(g);

    return 0;
}


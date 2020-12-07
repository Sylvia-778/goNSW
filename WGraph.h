// Weighted Graph ADT interface ... COMP9024 20T2

typedef struct GraphRep *Graph;

// vertices are ints
typedef int Vertex;
typedef int Schedule_no;
/*
typedef struct Weight {
    int schedule_no;
    int time;
}Weight;
 */

// edges are pairs of vertices (end-points) with a weight
typedef struct Edge {
   Vertex v;
   Vertex w;
   int weight;
   Schedule_no schedule_no;

} Edge;

Graph newGraph(int);
int   numOfVertices(Graph);
void  insertEdge(Graph, Edge);
void  removeEdge(Graph, Edge);
int   adjacent(Graph, Vertex, Vertex);  // returns weight, or 0 if not adjacent
void  showGraph(Graph, Schedule_no);
void  freeGraph(Graph);

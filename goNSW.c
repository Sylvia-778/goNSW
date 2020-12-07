//Created by z5253065 Liqi Jiang on 2020/7/21.
/*
   Time complexity for he worst-case asymptotic running time of my program is O((n^2)*(m^3)*(k^2))
       the number of stops, n
       the number of schedules, m
       the maximum number k of stops on a single train, bus or light rail line.
 */

/*
   My program has 8 functions including the main function.
   1. Input the stops, schedule and stops in each schedule in the main function, generating the graph and following
      important data structure: char **stop_name(stores the stop name and corresponding vertex number in the graph),
      int ***stop_sche_time(stores the time of stop in specific schedule). My program uses find_index() to find the
      vertex num in the graph corresponding to the stop name. List stop_sche stores the stop_name_index*10+schedule_no.
   2. Update the graph in the update_graph(), adding the edges between the same stops in the different schedule.
   3. Find the path using the data structures generated above. It's the most time-consuming part. My program initialize
      the dist[], pred[] in the find_path(), then calls thr generate path() to generate the path and compare the path
      with paths generated before to find the "depart latest" and "arrive earliest" path. generate_path_helper() and
      generate_path_helper_sub() helps to generate the path using stack and queue.
   4. Then passing the path generated above to show_queue(), using char **stop_name and int ***stop_sche_time, print the
      expected output.
 */


#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "WGraph.h"
#include <string.h>
#include "list.h"
#include "PQueue.h"
#include "queue.h"
#include "stack.h"
#define MAX_NAME_LEN 32
#define VERY_HIGH_VALUE 99999

typedef struct Node {
    int         v;
    struct Node *next;
} Node;

typedef struct node {
    int data;
    struct node *next;
} NodeT;

typedef struct StackRep {
    int    height;   // #elements on stack
    NodeT *top;      // ptr to first element
} StackRep;

typedef struct QueueRep {
    int   length;
    NodeT *head;
    NodeT *tail;
} QueueRep;

typedef struct GraphRep {
    int ***edges;  // adjacency matrix storing positive weights
    // 0 if nodes not adjacent
    int nV;       // #vertices
    int nE;       // #edges
} GraphRep;

// store hhmm and stop_name of each schedule
typedef struct Stoptime {
    int time;
    char stopname[MAX_NAME_LEN];
}Stoptime;

// find the stop_name index in **stop_name
int find_index(char **stop_name, int num_of_stop, char stopname[]){
    int i;
    for (i = 0; i < num_of_stop; i++){
        if (strcmp(stop_name[i], stopname) == 0)
            return i;
    }
    return -1;
}

// iter
stack generate_path_helper_sub(int **pred, int arrive_stop, stack path_reverse){
    if (pred[arrive_stop/10][arrive_stop%10] != arrive_stop){
        StackPush(path_reverse, pred[arrive_stop/10][arrive_stop%10]);
        generate_path_helper_sub(pred, pred[arrive_stop/10][arrive_stop%10], path_reverse);
    }
    return path_reverse;
}

// generate the path using stack, and then pop to queue
queue generate_path_helper(int **pred, int arrive_stop,stack path_reverse){
    path_reverse = generate_path_helper_sub(pred, arrive_stop,path_reverse);
    queue path = newQueue();
    int height = path_reverse->height;
    for (int i = 0; i < height; i++)
        QueueEnqueue(path, StackPop(path_reverse));
    return path;
}

// generate the queue path and then compare with paths before if exists
queue generate_path(int **pred, int **stop_sche_time,int end,int num_of_schedule, queue path){
    for (int i = 0; i < num_of_schedule; i++){
        int arrive_time;
        int arrive_stop;
        int depart_time;
        if (QueueIsEmpty(path)){
            arrive_time = VERY_HIGH_VALUE;
            arrive_stop = -1;
            depart_time = -1;
        }
        else{
            arrive_time = stop_sche_time[path->tail->data/10][path->tail->data%10];
            arrive_stop = path->tail->data;
            depart_time = stop_sche_time[path->head->data/10][path->head->data%10];
        }
        if (pred[end][i] != -1){
            if (stop_sche_time[end][i] < arrive_time) {
                arrive_stop = end*10+i;
                stack path_reverse = newStack();
                StackPush(path_reverse, arrive_stop);
                path = generate_path_helper(pred, arrive_stop,path_reverse);
                dropStack(path_reverse);
            }
            else if(stop_sche_time[end][i] == arrive_time){
                stack path_reverse = newStack();
                StackPush(path_reverse, end*10+i);
                queue path_temp = generate_path_helper(pred, arrive_stop, path_reverse);
                if (stop_sche_time[path_temp->head->data/10][path_temp->head->data%10] > depart_time)
                    path = path_temp;
            }
        }
    }
    return path;
}

// generate the path
queue find_path(int from, int to, int depart_time, int **stop_sche_time, int ***edges, int num_of_schedule, List stop_sche, int num_of_stop) {
    queue path = newQueue();
    List from_list = NULL;
    for (Node *q = stop_sche; q != NULL; q = q->next){
        if ((q->v/10) == from && stop_sche_time[q->v/10][q->v%10] >= depart_time){
            from_list = insertLL(from_list, q->v);
        }
    }
    if (from_list == NULL){
        return path;
    }
    int **dist;
    int **pred;
    dist = malloc(num_of_stop*sizeof(int*));
    pred = malloc(num_of_stop*sizeof(int*));
    for (int e = 0; e < num_of_stop; e++){
        dist[e] = malloc(num_of_schedule*sizeof(int));
        pred[e] = malloc(num_of_schedule*sizeof(int));
    }
    //bool vSet[num_of_stop][num_of_schedule];
    for (Node *n = from_list; n != NULL; n = n->next){
        PQueueInit();
        for (Node *p = stop_sche; p != NULL; p = p->next){
            if (stop_sche_time[n->v/10][n->v%10] <= stop_sche_time[p->v/10][p->v%10]) {
                joinPQueue(p->v);
                //vSet[p->v/10][p->v%10] = true;
            }
        }
        for (int i = 0; i < num_of_stop; i++){
            for (int j = 0; j < num_of_schedule; j++){
                dist[i][j] = VERY_HIGH_VALUE;
                pred[i][j] = -1;
            }
        }
        dist[n->v/10][n->v%10] = stop_sche_time[n->v/10][n->v%10];
        pred[n->v/10][n->v%10] = n->v;
        while (!PQueueIsEmpty()){
            int s = leavePQueue((int **) dist);
            if (dist[s/10][s%10] == VERY_HIGH_VALUE)
                continue;
            for (int j = 0; j < num_of_stop; j++){
                if (s/10 != j) {
                    if (edges[s/10][j][s%10] != 0) {
                        dist[j][s%10] = stop_sche_time[j][s%10];
                        pred[j][s%10] = s;
                    }
                }
                else {
                    for (int k = 0; k < num_of_schedule; k++){
                        if (edges[s/10][j][k] != 0 && k != s%10){
                            dist[j][k] = stop_sche_time[j][k];
                            pred[j][k] = s;
                        }
                    }
                }
            }
        }
        path = generate_path((int **)pred, stop_sche_time,to,num_of_schedule,path);
    }
    for(int i = 0; i < num_of_stop; i++){
        free(dist[i]);
        free(pred[i]);
    }
    free(dist);
    free(pred);
    return path;
}

void upgrade_graph(Graph g, int **stop_sche_time, int num_of_stop, int num_of_schedule){
    for (int i = 0; i < num_of_stop; i++){
        for (int k = 0; k < num_of_schedule-1; k++) {
            if (stop_sche_time[i][k] != -1) {
                for (int j = k + 1; j < num_of_schedule; j++) {
                    if (stop_sche_time[i][j] != -1) {
                        Edge *e;
                        e = malloc(sizeof(Edge));
                        if (stop_sche_time[i][k] <= stop_sche_time[i][j]) {
                            e->v = i;
                            e->w = i;
                            e->schedule_no = j;
                            e->weight = 1;
                            insertEdge(g, *e);
                        } else {
                            e->v = i;
                            e->w = i;
                            e->schedule_no = k;
                            e->weight = 1;
                            insertEdge(g, *e);
                        }
                        free(e);
                    }
                }
            }
        }
    }
}

void showqueue(queue path, char **stop_name, int **stop_sche_time){
    if (QueueIsEmpty(path)){
        printf("No connection found.\n");
    }
    else {
        NodeT *q = NULL;
        for (NodeT *p = path->head; p != NULL; p = p->next) {
            if (q != NULL) {
                if ((q->data / 10) == (p->data / 10)) {
                    printf("Change at %s\n", stop_name[p->data / 10]);
                }
            }
            printf("%04d %s\n", stop_sche_time[p->data / 10][p->data % 10], stop_name[p->data / 10]);
            q = p;
        }
    }
}

int main(void){
    // input
    int num_of_stop;  // number of stops on the network
    char **stop_name;  // pointer to pointer that points to stop names that on the network
    printf("Enter the total number of stops on the network: ");
    scanf("%d", &num_of_stop);
    Graph g = newGraph(num_of_stop); // create a weighted graph, g->nV = num_of_stop

    // allocate memory for num_of_stop pointers to beginning of rows
    stop_name = malloc(num_of_stop * sizeof(char*));
    assert(stop_name != NULL);
    // allocate memory for the elements in each row
    int i;
    for (i = 0; i < num_of_stop; i++) {
        stop_name[i] = malloc(MAX_NAME_LEN * sizeof(char));
        assert(stop_name[i] != NULL);
    }
    //read a single line with a stop name you may use
    for (i = 0; i < num_of_stop; i++)
        scanf("%s", stop_name[i]);
    //printf("%c", *stop_name[2]);
    //printf("%s", stop_name[2]);

    // ask number of schedules and number of stops of each schedule
    int num_of_schedule;  //  number of busses, trains and light rail vehicles running during a day
    int num_of_schedule_stop; // each schedule requires the number of stops
    int j, k;
    List stop_sche = NULL; // a list that store stop and its schedule_no
    printf("Enter the number of schedules: ");
    scanf("%d", &num_of_schedule);
    int **stop_sche_time;  // store stop time according to schedule
    stop_sche_time = malloc(num_of_stop * sizeof(int *));
    for (i = 0; i < num_of_stop; i++){
        stop_sche_time[i] = malloc(num_of_schedule*sizeof(int));
        assert(stop_sche_time[i] != NULL);
    }
    // initialize stop_sche_time
    for (i = 0; i < num_of_stop; i++)
        for (j = 0; j < num_of_schedule; j++)
            stop_sche_time[i][j] = -1;
    // input number of stops of each schedule, hh – hour, mm – minute and stop-name
    for (i = 0; i < num_of_schedule; i++) {
        printf("Enter the number of stops: ");
        scanf("%d", &num_of_schedule_stop);
        Stoptime **stoptime;
        stoptime = (Stoptime **)malloc(num_of_schedule_stop * sizeof(Stoptime *));
        assert(stoptime != NULL);
        // allocate memory for the elements in each row
        for (k = 0; k < num_of_schedule_stop; k++) {
            stoptime[k] = malloc(sizeof(Stoptime));
            assert(stoptime[k] != NULL);
            scanf("%d", &((*stoptime[k]).time));
            scanf("%s", (*stoptime[k]).stopname);
            int index = find_index(stop_name, num_of_stop, (*stoptime[k]).stopname);
            stop_sche_time[index][i] = (*stoptime[k]).time;
            stop_sche = insertLL(stop_sche, index*10+i);
            // bulid an edge between two adjacent stops
            if ( k != 0 ){
                Edge *edge;
                edge = malloc(sizeof(Edge));
                edge->v = find_index(stop_name, num_of_stop, (*stoptime[k-1]).stopname);
                edge->w = index;
                edge->schedule_no = i;
                edge->weight = 1;
                insertEdge(g, *edge);
                free(edge);
            }
        }
        free(stoptime);
    }
    upgrade_graph(g,stop_sche_time,num_of_stop,num_of_schedule);
    //showGraph(g, num_of_schedule);
    // input ended

    char from_stop[MAX_NAME_LEN];
    char to_stop[MAX_NAME_LEN];
    int depart_time;
    printf("\n");
    printf("From: ");
    scanf("%s", from_stop);
    while(strcmp(from_stop, "done\0") != 0){
        printf("To: ");
        scanf("%s", to_stop);
        printf("Depart at: ");
        scanf("%d", &depart_time);
        printf("\n");
        int index_from, index_to;
        index_from = find_index(stop_name, num_of_stop, from_stop);
        index_to = find_index(stop_name, num_of_stop, to_stop);
        queue path = find_path(index_from,index_to,depart_time,stop_sche_time,g->edges,num_of_schedule,stop_sche,num_of_stop);
        showqueue(path,stop_name,stop_sche_time);
        printf("\n");
        printf("From: ");
        scanf("%s", from_stop);
    }
    printf("Thank you for using goNSW.\n");
    freeGraph(g);
    freeLL(stop_sche);
    for (i = 0; i < num_of_stop; i++){
        free(stop_sche_time[i]);
        free(stop_name[i]);
    }
    free(stop_name);
    free(stop_sche_time);
    return 0;
}

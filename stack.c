// Stack ADT implementation ... COMP9024 20T2

#include <stdlib.h>
#include <assert.h>
#include "stack.h"

typedef struct n {
    int data;
    struct n *next;
} NodeS;

typedef struct StackRep {
    int    height;   // #elements on stack
    NodeS *top;      // ptr to first element
} StackRep;

// set up empty stack
stack newStack() {
    stack S = malloc(sizeof(StackRep));
    assert(S != NULL);
    S->height = 0;
    S->top = NULL;
    return S;
}

// remove unwanted stack
void dropStack(stack S) {
    NodeS *curr = S->top;
    while (curr != NULL) {
        NodeS *temp = curr->next;
        free(curr);
        curr = temp;
    }
    free(S);
}

// check whether stack is empty
bool StackIsEmpty(stack S) {
    return (S->height == 0);
}

// insert an int on top of stack
void StackPush(stack S, int v) {
    NodeS *new = malloc(sizeof(NodeS));
    assert(new != NULL);
    new->data = v;
    new->next = S->top;
    S->top = new;
    S->height++;
}

// remove int from top of stack
int StackPop(stack S) {
    assert(S->height > 0);
    NodeS *head = S->top;
    S->top = S->top->next;
    S->height--;
    int d = head->data;
    free(head);
    return d;
}

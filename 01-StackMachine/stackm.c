#ifndef STACKM_H
#define STACKM_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct node {
    int value;
    struct node* next;
} node;

typedef struct stackm {
    struct node *top;
} stackm;


void smInit(struct stackm *myStack){
    (myStack->top) = NULL;
}


int smSize(struct stackm *myStack){
    while(myStack->top->next != NULL) {
        
    }
    return myStack->top;
}


int smPush(struct stackm *myStack, int toStore){
    node node;
    node.value = toStore;
    
    *(myStack->top->next) = node;
}


int smPop(struct stackm *myStack){

}


int smTop(struct stackm *myStack, int* toStore){

}


void smClear(struct stackm *myStack){

}


void smPrint(struct stackm *myStack){

}


int smAdd(struct stackm* myStack){

}


int smSub(struct stackm* myStack){

}


int smMult(struct stackm* myStack){

}


int smRotate(struct stackm* myStack, int depth){

}

#endif
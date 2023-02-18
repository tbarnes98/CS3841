#ifndef STACKM_H
#define STACKM_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct node {
    int value;
    struct node *next;
} node;

typedef struct stackm {
    struct node *top;
} stackm;


void smInit(struct stackm *myStack){
    myStack->top = NULL;
}


int smSize(struct stackm *myStack){
    node *current = myStack->top;
    int count = 0;
    while(current != NULL) {
        count++;
        current = current->next;
    }
    return count;
}


int smPush(struct stackm *myStack, int toStore){

    node *newNode = (node *)malloc(sizeof(node));
    if (newNode == NULL) {
        return 0;
    } else {
        newNode->value = toStore;
        newNode->next = myStack->top;
        myStack->top = newNode;
        return 1;
    }
}


int smPop(struct stackm *myStack){
    if(myStack->top == NULL) {
        return 0;
    }
    node *temp = myStack->top;
    myStack->top = temp->next;
    free(temp);
    return 1;
}


int smTop(struct stackm *myStack, int* toStore){
    if (myStack->top == NULL || toStore == NULL) {
        return 0;
    } else {
        toStore = (int*)myStack->top;
        return 1;
    }
}


void smClear(struct stackm *myStack){
    while (myStack->top != NULL) {
        node *temp = myStack->top;
        myStack->top = temp->next;
        free(temp);
    }
}


void smPrint(struct stackm *myStack){
    node *temp = myStack->top;
    printf("==Stack Contents==\n");
    printf("Top -> ");
    while (temp != NULL) {
        printf("%d ", temp->value);
        temp = temp->next;
        printf("\n");
    }
}


int smAdd(struct stackm* myStack){
    if(smSize(myStack) <= 2){
        return 0;
    } else {
        int sum = (myStack->top->value) + (myStack->top->next->value);
        smPush(myStack,sum);
        return 1;
    }
}


int smSub(struct stackm* myStack){
    if(smSize(myStack) <= 2){
        return 0;
    } else {
        int result = (myStack->top->value) - (myStack->top->next->value);
        smPop(myStack);
        smPop(myStack);
        smPush(myStack,result);
        return 1;
    }
}


int smMult(struct stackm* myStack){
    if(smSize(myStack) <= 2){
        return 0;
    } else {
        int sum = (myStack->top->value) * (myStack->top->next->value);
        smPush(myStack,sum);
        return 1;
    }
}


int smRotate(struct stackm* myStack, int depth){
    if(depth == 0){
        return 0;
    } else {
        node *temp = myStack->top;
        int count = 1;
        while (count < depth && temp != NULL) {
            temp = temp->next;
            count++;
        }
        if (temp == NULL) {
            return 0;
        }
        node *nNode = temp;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = myStack->top;
        myStack->top = nNode->next;
        nNode->next = NULL;
        return 1;
    }
}

#endif
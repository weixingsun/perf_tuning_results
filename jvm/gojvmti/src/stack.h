#ifndef STACK_H
#define STACK_H

#include <stdio.h>
#include <stdlib.h>
#define STACK_MAX 10

struct Stack {
    char*   data[STACK_MAX];
    int     size;
} Stack;

struct Stack* Stack_Init(){
	//struct Stack* S = malloc(sizeof(struct Stack));
	struct Stack* S = &Stack;
    S->size = 0;
	return S;
}

char* Stack_Top(struct Stack *S){
    if (S->size == 0) {
        printf("Error: stack empty\n");
        return NULL;
    } 
    return S->data[S->size-1];
}

void Stack_Push(struct Stack *S, char* d){
    if (S->size < STACK_MAX)
        S->data[S->size++] = d;
    else
        printf("Error: stack full\n");
}

char* Stack_Pop(struct Stack *S){
    if (S->size == 0){
        printf("Error: stack empty\n");
		return NULL;
    }else{
        S->size--;
		return S->data[S->size];
	}
}

void Stack_Iter(struct Stack *S, void (*func)(char*) ){
	int T = S->size;
	//printf("Stack_Iter:%d\n",T);
	for (int i=0;i<T;i++){
		func(Stack_Pop(S));
	}
}

void Stack_Free(struct Stack *S){
	free(S);
}

void print_string(char* str){
	printf("Stack pop: %s\n",str);
}
/*
int main2() {
	char* str1 = "First";
	char* str2 = "Second";
	char* str3 = "Third";
	struct Stack* stack = Stack_Init();
	printf("The stack size : %d\n", stack->size);
	Stack_Push(stack, str1);
	Stack_Push(stack, str2);
	//Stack_Push(stack, str3);
	printf("Push stack size : %d\n", stack->size);
	Stack_Iter(stack,print_string);
	//Stack_Free(stack);

   return (0);
}*/
#endif // STACK_H
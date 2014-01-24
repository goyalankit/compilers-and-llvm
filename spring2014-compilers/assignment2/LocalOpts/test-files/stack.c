/*

Following file implements stack in C.

Author: Ankit Goyal

*/

#include<stdio.h>
#include<stdlib.h>

struct stack{
  int value;
  struct stack * next;
};

//defining a global head pointer to the stack queue
struct stack *head;

//push function for stack.
void push(int a){
  struct stack *node = (struct stack *)malloc(sizeof(struct stack));
  if(node==NULL){
    printf("failed to assign memory");
  }
  node -> next = head;
  node -> value = a;
  head = node;
}

int pop(){
  struct stack *poped;
  int popedValue;
  poped = head;
  popedValue = poped -> value;
  head = head -> next;
  free(poped);
  return popedValue;
}

void printLinkList(){
  struct stack *temp = head;
  while(temp!=NULL){
    printf("hello from list number %d\n", temp->value);
    temp = temp -> next;
  }
}

int main(void){
  head = (struct stack *)malloc(sizeof(struct stack));

  if(head==NULL){
    printf("failed to assign memory");
  }
  head -> value = 2;
  head -> next = NULL;


  push(3);
  printLinkList();

  printf("poping value from the stack\n");
  printf("popped the value - %d\n",pop());
  printf("stack after poping the value\n");

  printLinkList();
  return 0;
}



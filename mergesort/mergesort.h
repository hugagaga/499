#ifndef MERGESORT
#define MERGESORT

class Node
{ 
public:
  int data;
  Node* next;
  Node();
};

class LinkedList
{
  public:
  Node* headptr;
  LinkedList();
  void push(int);
  void print();
};

void split(Node*, Node*& , Node*&);
Node* merge(Node*, Node*);
void merge_sort(Node*& head);
#endif 

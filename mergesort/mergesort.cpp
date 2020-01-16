#include <iostream>
#include "mergesort.h"

using namespace std;

Node::Node() {
      data = 0;
      next = nullptr;
}

LinkedList::LinkedList() {
    headptr = nullptr;
}
  
void LinkedList::push(int data) {
    Node* n = new Node();
    n->data = data;
    n->next = headptr;
    headptr = n;
}

void LinkedList::print() {
    Node* node = headptr;
    while (node != nullptr) {
      cout << node->data << " ";
      node = node -> next;
    }
    cout << "\n";
}

void split(Node* head, Node*& left, Node*& right) {
    Node* fast = new Node();
    Node* slow = new Node();
    slow = head;
    fast = head->next;

    while(fast != nullptr) {
      fast = fast->next;
      if (fast != nullptr) {
        slow = slow -> next;
        fast = fast -> next;
      }
    }
    left = head;
    right = slow -> next;
    slow -> next = nullptr;
}

Node* merge(Node* left, Node* right) {
  Node* dummy = new Node();
  Node* curr = new Node();
  curr = dummy;
  while(left != nullptr && right!= nullptr) {
    Node* n = new Node();
    if (left->data > right->data) {
        n->data = right->data;
        right = right->next;
    } else {
        n->data = left->data;
        left = left->next;
    }
    curr->next = n;
    curr = curr->next;
  }
  while (left != nullptr) {
      Node* n = new Node();
      n->data = left->data;
      left = left -> next;
      curr -> next = n;
      curr = curr->next;
  }
  while (right != nullptr) { 
      Node* n = new Node();
      n->data = right->data;
      right = right -> next;
      curr -> next = n;
      curr = curr->next;
  }
  return dummy->next;
}

void merge_sort(Node*& head){
    Node *left, *right;
    if (head == nullptr || head->next == NULL) {
      return;
    }
    split(head, left, right);
    merge_sort(left);
    merge_sort(right);
    head = merge(left, right);
  }


int main() {
  LinkedList* l = new LinkedList();
  l->push(1);
  l->push(3);
  l->push(7);
  l->push(2);
  l->push(6);
  l->print();
  merge_sort(l->headptr);
  l->print();
  free(l);
  return 0;
}

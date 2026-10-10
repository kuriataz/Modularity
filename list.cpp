#include <iostream>

struct Node {
  int data;
  Node *next;

  Node(int data) : data(data) { next = nullptr; }
};

struct List {
  Node *head;

  List() { head = nullptr; }

  void push_back(int data) {
    Node *n = new Node(data);
    if (head == nullptr) {
      head = n;
      return;
    }
    Node *current = head;

    while (current->next != nullptr) {
      current = current->next;
    }
    current->next = n;
  }

  int pop_front() {
    int res = head->data;

    Node *new_head = head->next;
    delete (head);
    head = new_head;

    return res;
  }
};

int main() {
  List *list = new List();

  list->push_back(5);

  std::cout << list->pop_front() << "\n";
}

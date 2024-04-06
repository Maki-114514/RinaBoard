#ifndef DOUBLY_LINKED_LIST_H
#define DOUBLY_LINKED_LIST_H

#include <Arduino.h>

class DoublyLinkedList {
public:
    struct Node {
        String data;
        Node* prev;
        Node* next;
        explicit Node(String  data);
    };

private:
    Node* head;
public:
    DoublyLinkedList();
    ~DoublyLinkedList();

    void insert(const String& data);
    bool remove(const String& data);
    bool search(const String& data) const;
    void clear();
    bool isEmpty() const;
    Node* getHead() const;
};

#endif

// DoublyLinkedList.cpp

#include "DoublyLinkedList.h"

#include <utility>

DoublyLinkedList::Node::Node(String data) : data(std::move(data)), prev(nullptr), next(nullptr)
{}

DoublyLinkedList::DoublyLinkedList() : head(nullptr)
{}

DoublyLinkedList::~DoublyLinkedList()
{
    clear();
}

void DoublyLinkedList::insert(const String &data)
{
    Node *newNode = new Node(data);
    if (!head)
    {
        head = newNode;
        newNode->next = newNode;
        newNode->prev = newNode;
    } else
    {
        newNode->next = head;
        newNode->prev = head->prev;
        head->prev->next = newNode;
        head->prev = newNode;
        head = newNode;
    }
}

bool DoublyLinkedList::remove(const String &data)
{
    if (!head)
    {
        return false; // 空链表
    }

    Node *current = head;
    do
    {
        if (current->data == data)
        {
            if (current == head && head->next == head)
            {
                delete head;
                head = nullptr;
            } else
            {
                current->prev->next = current->next;
                current->next->prev = current->prev;
                if (current == head)
                {
                    head = current->next;
                }
                delete current;
            }
            return true;
        }
        current = current->next;
    } while (current != head);

    return false; // 未找到数据
}

bool DoublyLinkedList::search(const String &data) const
{
    if (!head)
    {
        return false; // 空链表
    }

    Node *current = head;
    do
    {
        if (current->data == data)
        {
            return true;
        }
        current = current->next;
    } while (current != head);

    return false; // 未找到数据
}

void DoublyLinkedList::clear()
{
    if (!head)
    {
        return; // 空链表
    }

    Node *current = head;
    do
    {
        Node *temp = current;
        current = current->next;
        delete temp;
    } while (current != head);

    head = nullptr;
}

bool DoublyLinkedList::isEmpty() const
{
    return head == nullptr;
}

DoublyLinkedList::Node *DoublyLinkedList::getHead() const
{
    return head; // 返回头节点指针
}


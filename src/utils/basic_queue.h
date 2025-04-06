#ifndef __BASIC_QUEUE__
#define __BASIC_QUEUE__

#include <Arduino.h>

template <typename T>
class BasicQueue
{
public:
    class Node;
private:
    uint8_t count;
    Node *head;
    void remove(int index)
    {
        if (index < 0 || index >= count)
            return;
        Node **pp = &head;
        while (index)
        {
            pp = &((*pp)->next);
            index--;
        }
        Node *p = *pp;
        *pp = p->next;
        delete p;
        count--;
    };
    void clear()
    {
        while (head)
        {
            Node *p = head;
            head = head->next;
            delete p;
        }
        this->count = 0;
    }
    void insert(int index, T value)
    {
        if (index < 0)
            return;
        else if (index > count)
            index = count;
        Node **pp = &head;
        while (index)
        {
            pp = &((*pp)->next);
            index--;
        }
        *pp = new Node(value, *pp);
        count++;
    };
public:
    BasicQueue(): count(0), head(nullptr) {}
    ~BasicQueue() { clear(); }
    T getFirstIdx() { return head->data; }
    void push_back(T value)
    {
        insert(count, value);
    };
    T pop()
    {
        // get at idx
        T val;
        if (this->count > 0)
        {
            val = head->data;
            remove(0);
        }
        return val;
    }
public: 
  class Node
  {
    private:
        T data;
        Node* next;
        friend class BasicQueue<T>;
    public:
        Node(): next(nullptr) {};
        Node(T data, Node *next): data(data), next(next) {};
  };
};
#endif
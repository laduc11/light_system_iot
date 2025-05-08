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
    SemaphoreHandle_t mutex;
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
    BasicQueue(): count(0), head(nullptr), mutex(xSemaphoreCreateMutex()) {}
    ~BasicQueue() { clear(); }
    T getFirstIdx() { return head->data; }
    bool isEmpty() { return count == 0; }
    void push_back(T value)
    {
        if (xSemaphoreTake(this->mutex, (TickType_t)10) == pdTRUE)
        {
            insert(count, value);
            xSemaphoreGive(mutex);
        }    
    };
    T pop()
    {
        // get at idx
        T val;
        if (xSemaphoreTake(this->mutex, (TickType_t)10) == pdTRUE)
        {
            if (this->count > 0)
            {
                val = head->data;
                remove(0);
            }
            xSemaphoreGive(this->mutex);
        }
        return val;
    }
    bool find(const T& value)
    {
        if (xSemaphoreTake(this->mutex, (TickType_t)10) == pdTRUE)
        {
            Node *p = head;
            while (p != nullptr)
            {
                if (p->data == value)
                {
                    xSemaphoreGive(this->mutex);
                    return true;
                }
                p = p->next;
            }
            xSemaphoreGive(this->mutex);
        }
        return false;
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
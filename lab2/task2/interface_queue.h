#ifndef LAB2_INTERFACE_QUEUE_H
#define LAB2_INTERFACE_QUEUE_H

#include <iostream>


template <typename T>
class IQueue {
public:
    virtual void push(T t) = 0;
    virtual bool pop(T &t) = 0;
};

#endif //LAB2_INTERFACE_QUEUE_H

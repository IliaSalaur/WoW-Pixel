#ifndef SMART_ARRAY_H
#define SMART_ARRAY_H

#include <stdlib.h>

template <typename T>
class SmartArray
{
private:
    T* _arrayPtr;
    size_t _size;
public:
    SmartArray(size_t s, bool memset0 = 1) : _size(s), _arrayPtr(new T[s])
    {
        if(memset0)
        {
            memset(_arrayPtr, 0, _size);
        }
    }

    size_t getSize()
    {
        return _size;
    }

    operator T*()
    {
        return _arrayPtr;
    }

    T* getPtr()
    {
        return _arrayPtr;
    }
    /*
    @brief calls the delete[] operator 
    */
    void clearHeap()
    {
        delete[] _arrayPtr;
    }

    ~SmartArray(){}
};

#endif
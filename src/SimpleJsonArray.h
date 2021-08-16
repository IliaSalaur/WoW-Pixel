#ifndef SIMPLE_JSON_ARRAY_H
#define SIMPLE_JSON_ARRAY_H

#include <Arduino.h>
class SimpleJsonArray
{
private:
    String _jsonStr;
    bool first = 1;
public:
    SimpleJsonArray()
    {
        this->clear();
    }

    void add(String val)
    {
        _jsonStr += (first) ? "\"" : ",\"";
        _jsonStr += val;
        _jsonStr += "\"";
        
        first = 0;
    }

    String getString()
    {
        String json = _jsonStr;
        json += "]";
        return json;
    }

    void clear()
    {   
        first = 1;
        _jsonStr = "[";
    }

    ~SimpleJsonArray() 
    {
        _jsonStr.~String();
    }
};

#endif

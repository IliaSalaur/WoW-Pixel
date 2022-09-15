#ifndef SIMPLEFB_MAIN_H
#define SIMPLEFB_MAIN_H

#include <Arduino.h>
#include "FirebaseClient.h"
#include <utility>
#include <string>

static const uint8_t simplefb_max_callbacks = 12;

class SimpleFirebase
{
private:
    static SimpleFirebase* pSingletonInstance;
    FirebaseClient* _client;
    std::pair<std::string, std::function<void(const char*)>> _path_callbacks[simplefb_max_callbacks];

    void _onDisconnect()
    {

    }

    void _onMsgCb(char* buf) // check path obtained from data that is asigned with a cb 
    {
        //Serial.flush();
        Fm("msgCb buf:%s\n\n", buf)
        for(uint8_t i = 0; i < simplefb_max_callbacks; i++)
        {
            if(this->_path_callbacks[i].second)// && this->_path_callbacks[i].first != "")
            {
                char value[TinyJsonConfig::k_value_size];
                TinyJson::getValue(this->_path_callbacks[i].first.c_str(), buf, value);
                if(value[0] != TinyJsonConfig::k_fail[0])
                {
                    //if(this->_path_callbacks[i].first[this->_path_callbacks[i].first.length() - 1] == '/')
                    {
                        //j = TinyJson::getIndexOf(buf, this->_path_callbacks[i].first.c_str(), 0);
                        for(int j = TinyJson::getIndexOf(buf, this->_path_callbacks[i].first.c_str(), 0); TinyJson::getIndexOf(buf, this->_path_callbacks[i].first.c_str(), j) > -1; j++)
                        {                          
                            auto nodeBuf = std::make_unique<char[]>(TinyJsonConfig::k_node_size);                            
                            nodeBuf.get()[0] = 0;
                            TinyJson::getNode(buf, this->_path_callbacks[i].first.c_str(), nodeBuf.get(), j);
                            
                            j += this->_path_callbacks[i].first.length();
                            //Fm("msgCb getNode:%s\nj = %u \t tinyJson = %d\n", nodeBuf.get(), j, TinyJson::getIndexOf(buf, this->_path_callbacks[i].first.c_str(), j))
                            this->_path_callbacks[i].second(nodeBuf.get());
                            
                        }
                    }
                }
            }
            else { Fm("msgCb: Callback not found, i:%u\n", i)}
        }

        Fm("msgCb ended\n")
    }

public:
    static void onMsgCb(char* s)
    {
        if(pSingletonInstance) pSingletonInstance->_onMsgCb(s);
    }

    SimpleFirebase(const char* host, const char* token) :  _client(new FirebaseClientImpl(host, token)) // initializing the client
    {
        pSingletonInstance = this;
        for(size_t i = 0; i < simplefb_max_callbacks; i++)
        {
            _path_callbacks[i].first = "";
            //_path_callbacks[i].second;
        }
    }

    bool begin()
    {
        //_client.reset();
        _client->onReceive(SimpleFirebase::onMsgCb);
        return _client->begin();
    }

    void beginStream(const char* path)
    {
        _client->beginStream(path);
    }

    void on(const char* path, std::function<void(const char*)> cb)
    {
        for(size_t i = 0; i < simplefb_max_callbacks; i++)
        {
            if(_path_callbacks[i].second == NULL)
            {
                _path_callbacks[i].first = path;
                _path_callbacks[i].second = cb;
                return;
            }
        }
    }

    void handle()
    {
        _client->handle();
    }

    void requestData(const char* path)
    {
        _client->requestData(path);
    }
    
    FirebaseClient& getClient()
    {
        return *_client;
    }
    ~SimpleFirebase(){}
};


SimpleFirebase* SimpleFirebase::pSingletonInstance = 0; // singleton instance for callbacks
#endif
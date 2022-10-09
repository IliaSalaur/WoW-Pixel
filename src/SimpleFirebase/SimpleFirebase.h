#ifndef SIMPLEFB_MAIN_H
#define SIMPLEFB_MAIN_H

#include <Arduino.h>
#include "FirebaseClient.h"
#include <utility>
#include <string>
#include "StatePersistence.h"

static const uint8_t simplefb_max_callbacks = 12;
static const size_t simplefb_persist_buf_size = 704;
static const uint8_t simplefb_persist_max_entries = 12;

class SimpleFirebase
{
private:
    static SimpleFirebase* pSingletonInstance;
    FirebaseClient* _client;
    IStatePersist* _statePersist;
    std::pair<std::string, std::function<void(const char*)>> _path_callbacks[simplefb_max_callbacks];
    std::pair<std::string, std::string> _persistEntries[simplefb_persist_max_entries];

    void _onDisconnect()
    {

    }

    void _executeAsignedCallbacks(char* buf) // check path obtained from data that is asigned with a cb
    {
        //Serial.flush();
        //Fm("msgCb buf:%s\n\n", buf)
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

    void _onMsgCb(char* buf) 
    {
        _persistData(buf);
        _executeAsignedCallbacks(buf);
    }    

    void _persistData(const char* buf){
        if(strlen(buf) < 2) return;
        std::string entriesBuf = "";
        entriesBuf.reserve(k_max_posible_rtcmem_size);
        for(size_t i = 0; i < simplefb_persist_max_entries; i++)
        {
            if(_persistEntries[i].first == "") continue;
            if(TinyJson::getIndexOf(buf, _persistEntries[i].first.c_str()) != -1)
            {
                char nodeBuf[TinyJsonConfig::k_node_size]{0};
                TinyJson::getNode(buf, _persistEntries[i].first.c_str(), nodeBuf);
                _persistEntries[i].second = TinyJson::value(nodeBuf);
                Fm("Entry to be persisted:%s\n", _persistEntries[i].first.c_str())                    
            }

            if(_persistEntries[i].second != "")
            {
                entriesBuf += _persistEntries[i].first;
                entriesBuf += "\t";
                entriesBuf += _persistEntries[i].second;
                entriesBuf += "\n";
            }                
        }
        Fm("All entries: %s\nSize:%u\n", entriesBuf.c_str(), entriesBuf.size() + 1)
        _statePersist->persist(entriesBuf.c_str(), entriesBuf.size() + 1);
    }

public:
    static void onMsgCb(char* s)
    {
        if(pSingletonInstance) pSingletonInstance->_onMsgCb(s);
    }

    bool retrievePersistedState()
    {
        char buf[simplefb_persist_buf_size]{0};
        size_t dataSize = _statePersist->getPersistedData(buf);             
        if(dataSize >= k_max_posible_rtcmem_size) return false;
        Fm("Persist data: %s\nData size:%u\n", buf, dataSize);   
        _executeAsignedCallbacks(buf);
        return true;
    }

    SimpleFirebase(const char* host, const char* token) :  _client(new FirebaseClientImpl(host, token)), _statePersist(new RTCStatePersist) // initializing the client
    {
        pSingletonInstance = this;
        for(size_t i = 0; i < simplefb_max_callbacks; i++)
        {
            _path_callbacks[i].first = "";
            //_path_callbacks[i].second;
        }

        for(size_t i = 0; i < simplefb_persist_max_entries; i++)
        {
            _persistEntries[i].first = "";
            _persistEntries[i].second = "";
        }
    }

    SimpleFirebase(const char* host, const char* token, IStatePersist* statePersist) :  _client(new FirebaseClientImpl(host, token)), _statePersist(statePersist) // initializing the client
    {
        pSingletonInstance = this;
        for(size_t i = 0; i < simplefb_max_callbacks; i++)
        {
            _path_callbacks[i].first = "";
            //_path_callbacks[i].second;
        }

        for(size_t i = 0; i < simplefb_persist_max_entries; i++)
        {
            _persistEntries[i].first = "";
            _persistEntries[i].second = "";
        }
    }

    bool begin()
    {        
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

    // @param entry path to data in RTDB to be persisted
    void persistEntry(const char* entry)
    {
        for(size_t i = 0; i < simplefb_persist_max_entries; i++)
        {
            if(_persistEntries[i].first == entry) return; //already persisting
            if(_persistEntries[i].first == ""){                
                _persistEntries[i].first = entry;
                Fm("Entry persist requeired: %s\n", _persistEntries[i].first.c_str());
                return;
            }
        }
    }
    
    FirebaseClient& getClient()
    {
        return *_client;
    }
    ~SimpleFirebase(){}
};


SimpleFirebase* SimpleFirebase::pSingletonInstance = 0; // singleton instance for callbacks
#endif
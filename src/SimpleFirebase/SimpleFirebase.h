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

    void _onMsgCb(char* buf) // check path obtained from data that is asigned with a cb 
    {
        Dln(buf)
        for(uint8_t i = 0; i < simplefb_max_callbacks; i++)
        {
            if(this->_path_callbacks[i].second != NULL)// && this->_path_callbacks[i].first != "")
            {
                char value[64];
                TinyJson::getValue(this->_path_callbacks[i].first.c_str(), buf, value);
                if(value[0] != '\2')
                {
                    //if(this->_path_callbacks[i].first[this->_path_callbacks[i].first.length() - 1] == '/')
                    {
                        // int index = 0;
                        // index = TinyJson::getIndexOf(buf, this->_path_callbacks[i].first.c_str(), 0);
                        // for(size_t i = 0; i != -1; i = TinyJson::getIndexOf(buf, this->_path_callbacks[i].first.c_str(), i))
                        // {
                        //     this->_path_callbacks[i].second(TinyJson::getNode(buf, this->_path_callbacks[i].first.c_str()).c_str());
                        // }
                        for(size_t i_ = TinyJson::getIndexOf(buf, this->_path_callbacks[i].first.c_str(), 0); i_ != -1; i_ = TinyJson::getIndexOf(buf, this->_path_callbacks[i].first.c_str(), i_+strlen(this->_path_callbacks[i].first.c_str())))
                        {
                            Dln(TinyJson::getNode(buf, this->_path_callbacks[i].first.c_str(), i_).c_str())
                            this->_path_callbacks[i].second(TinyJson::getNode(buf, this->_path_callbacks[i].first.c_str(), i_).c_str());

                        }
                    }
                    ///this->_path_callbacks[i].second(TinyJson::getNode(buf, this->_path_callbacks[i].first.c_str()).c_str());
                    ///if(this->_path_callbacks[i].first[this->_path_callbacks[i].first.length() -1] == '/') i--;
                }
            }
            else { Dln(__LINE__) return;}
        }
    }

    void _onMsgCbJson(DynamicJsonDocument& doc)
    {
        for(uint8_t i = 0; i < simplefb_max_callbacks; ++i)
        {
            if(this->_path_callbacks[i].second != NULL)
            {
                Dln(i)
                std::vector<std::string> strings = split(_path_callbacks[i].first, '/');
                std::string val = _getDoc(doc, strings, 1).as<std::string>();
                // D(_path_callbacks[i].first.c_str())
                // D(" : ")
                // D(val)
                // D('\n')
            }
        }
    }

    std::vector<std::string> split(std::string& s, char seperator)
    {
    std::vector<std::string> output;

        std::string::size_type prev_pos = 0, pos = 0;

        while((pos = s.find(seperator, pos)) != std::string::npos)
        {
            std::string substring( s.substr(prev_pos, pos-prev_pos) );

            output.push_back(substring);

            prev_pos = ++pos;
        }

        output.push_back(s.substr(prev_pos, pos-prev_pos)); // Last word

        return output;
    }

    DynamicJsonDocument& _getDoc(DynamicJsonDocument& doc, std::vector<std::string>& paths, size_t index)
    {
        if(doc[paths[index]].isNull())
        {
            return doc;
        }
        return _getDoc(doc, paths, ++index);
    }

public:
    static void onMsgCb(char* s)
    {
        if(pSingletonInstance) pSingletonInstance->_onMsgCb(s);
    }

    static void onMsgCbJson(DynamicJsonDocument& doc)
    {
        if(pSingletonInstance) pSingletonInstance->_onMsgCbJson(doc);
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
        _client->onReceive(SimpleFirebase::onMsgCbJson);
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
                //Dln(uint32_t(cb))
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
    

    ~SimpleFirebase()
    {
        delete _client;
    }
};


SimpleFirebase* SimpleFirebase::pSingletonInstance = 0; // singleton instance for callbacks
#endif
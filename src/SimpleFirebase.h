#ifndef SIMPLE_FIREBASE_H
#define SIMPLE_FIREBASE_H

#include <FirebaseESP8266.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#include <vector>

struct PathData
{
    PathData(String p, String d)
    {
        path = p; 
        data = d;
    }   
    String path;
    String data;
};

/*
struct PushData
{
    PushData(String p, String d, String dt)
    {
        path = p; 
        data = d;
        dataType = dt;
    }
    String path;
    String data;
    String dataType;
};
*/
struct PathCallback
{
    void (*funcptr)(PathData data);
    String path;
};


/*
struct SendQuery
{
    StreamData *data;
};*/

using namespace std;

void tokenHandler(TokenInfo info)
{
    static int count = 0;    

    if (info.status == token_status_error)
    {
        count++;
        Serial.printf("Token info: type = %s, status = %s\n", getTokenType(info).c_str(), getTokenStatus(info).c_str());
        Serial.printf("Token error: %s\n", getTokenError(info).c_str());
    }
    else
    {
        Serial.printf("Token info: type = %s, status = %s\n", getTokenType(info).c_str(), getTokenStatus(info).c_str());
    }

    if(count >= 4)
    {
        DEBUG("Count full, reset");
        Serial.flush();
        ESP.reset();
    }
}

class SimpleFirebase
{
private:
    FirebaseData _fbdo;
    vector<PathCallback> _pathcalls;

    static SimpleFirebase *pSingletonInstance;

    static void streamCallback(StreamData data)
    {
        if(pSingletonInstance)  pSingletonInstance->_streamCallback(data);
    }

    static void streamTimeoutCallback(bool tout)
    {
        if(pSingletonInstance && tout)  pSingletonInstance->_streamTimeoutCallback(tout);
    }

    void _streamCallback(StreamData data)
    {
        DEBUG(data.dataPath())
        DEBUG(data.stringData())
        DEBUG(data.dataType())

        if(data.dataType() == "json")
        {
            _parseJson(&data);
        }
        else 
        {
            _handleCallbacks(data);
        }
    }

    void _streamTimeoutCallback(bool tout)
    {
        DEBUG("TIMEOUT")
    }

    void _handleCallbacks(StreamData data)
    {
        for(PathCallback pc : _pathcalls) 
            {
                //if(pc.path == data.dataPath())
                if(data.dataPath().indexOf(pc.path) != -1)
                {
                    if(pc.funcptr)  pc.funcptr(PathData(data.dataPath(), data.stringData()));
                    break;
                }
            }
    }

    void _handleCallbacks(PathData data)
    {
        for(PathCallback pc : _pathcalls) 
            {
                //if(pc.path == data.dataPath())
                if(pc.path.indexOf(data.path) != -1)
                {
                    if(pc.funcptr)  pc.funcptr(data);
                    break;
                }
                else if(data.path.indexOf(pc.path) != -1)
                {
                    if(pc.funcptr)  pc.funcptr(data);
                    break;
                }
            }
    }

    void _parseJson(FirebaseJson* json)
    {
        //SimpleJSON js(data->stringData());
        String commonPath;
        if(1)
        {           
            for(size_t i = 0; i < json->iteratorBegin(); i++)
            {
                String key, value;
                int type = 0;

                json->iteratorGet(i, type, key, value);

                if(value.indexOf("{") == -1) 
                {
                    _handleCallbacks(PathData(commonPath + key, value)); //PushData's datatype should be int instead of string. But to be honest, this library is a piece of shit, really. I would rewrite it if i had time
                    // data->dataTypeEnum returns uint8_t but should return Enum. And it's just one example, i had a "little bit" more
                    /*
                        По этому если честно надо либо написать свою библиотеку, либо переписать это дело на TCP но тогда нужно будет перерыть всё мобильное приложение (если оно написано с использованием ФП)

                        Заменил PushData на PathData (тоже самое но без dataType) потому что iteratorGet 'return не тип даных (строка, инт) а какой-от бред бесполезный
                    */
                    DEBUG(commonPath + key)
                    DEBUG(value)
                }
                else 
                {
                    commonPath = String("/");
                    commonPath += key;
                    commonPath += String("/");
                    //DEBUG("Array")
                    //DEBUG(commonPath)
                    //DEBUG(value)
                }
            }
            json->iteratorEnd();
        }
    }
    void _parseJson(StreamData* data)
    {
        FirebaseJson* json = data->jsonObjectPtr();
        String commonPath = data->dataPath();
        commonPath += String("/");
        if(1)
        {   DEBUG("iterator begin");
            for(size_t i = 0; i < json->iteratorBegin(); i++)
            {
                String key, value;
                int type = 0;
                DEBUG("iterator get")
                json->iteratorGet(i, type, key, value);
                DEBUG("iterator getted")
                if(value.indexOf("{") != -1)
                {
                    commonPath = data->dataPath();
                    commonPath += key;
                    commonPath += String("/");
                }
                DEBUG(commonPath + key)
                DEBUG(value)
                DEBUG("calback start")
                _handleCallbacks(PathData(commonPath + key, value));
                DEBUG("calback end")
            }
            json->iteratorEnd();
        }
    }
        
public:
    SimpleFirebase()
    {
        _pathcalls.reserve(10);
    }

    void begin(FirebaseConfig *config, FirebaseAuth *auth, String path)
    {
        DEBUG(String("Path is ") + path)
        pSingletonInstance = this;
        Firebase.begin(config, auth);
        Firebase.reconnectWiFi(true);
        _fbdo.setBSSLBufferSize(4096, 4096);//2048, 2048
        _fbdo.setResponseSize(4096);//2048
        if (!Firebase.beginStream(_fbdo, path))
        {
          DEBUG("------------------------------------");
          DEBUG("Can't begin stream connection...");
          DEBUG("REASON: " + _fbdo.errorReason());
          DEBUG("------------------------------------");
          DEBUG();
        }
        Firebase.setStreamCallback(_fbdo, SimpleFirebase::streamCallback, SimpleFirebase::streamTimeoutCallback);

    }

    void on(String path, void(*funcptr)(PathData data))
    {   
        PathCallback pathcallback;
        pathcallback.funcptr = funcptr;
        pathcallback.path = path;

        _pathcalls.push_back(pathcallback);
    }

    void handle()
    {
    }
    /*
    void get(String path)
    {
        DEBUG("Getting JSON")
        //Firebase.getJSON(_fbdo, path);
        if(!Firebase.get(_fbdo, path))
        {
            DEBUG(_fbdo.errorReason())
        }

        DEBUG("Getting json from _fbdo")
        FirebaseJson *json = _fbdo.jsonObjectPtr();
        
        DEBUG("Start parsing")
        this->_parseJson(json);
    }*/
};

SimpleFirebase * SimpleFirebase::pSingletonInstance = 0;

#endif
#ifndef SIMPLE_FIREBASE_H
#define SIMPLE_FIREBASE_H

#include <FirebaseESP8266.h>
#include <vector>

#define DEBUG_SIMPLE_FIREBASE
#ifdef DEBUG_SIMPLE_FIREBASE
#define DEBUG(x) Serial.println(x);
#else 
#define DEBUG(x)
#endif

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

class SimpleFirebase
{
private:
    FirebaseData _fbdo;
    vector<PathCallback> _pathcalls;
    vector<PathData*> _sendquery;

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

    void _addQuery(PathData* data)
    {
        _sendquery.push_back(data);
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
                if(data.path.indexOf(pc.path) != -1)
                {
                    if(pc.funcptr)  pc.funcptr(data);
                    break;
                }
            }
    }

    void _handleQuery()
    {
        for(uint16_t i = 0; i < _sendquery.size(); i++)
        {
            if(Firebase.ready())
            {
                Firebase.set(_fbdo, _sendquery[i]->path, _sendquery[i]->data);
                DEBUG("Query sent:")
                DEBUG(int(_sendquery[i]))
                _sendquery.erase(_sendquery.begin() + i);                
            }
            else break;
        }
    }

    void _parseJson(StreamData* data)
    {
        //SimpleJSON js(data->stringData());
        String commonPath;
        if(data->dataType() == "json")
        {
            FirebaseJson *json = data->jsonObjectPtr();            
            for(int i = 0; i < json->iteratorBegin(); i++)
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

                        Заменил PushData на PathData (тоже самое но без dataType) потому что iteratorGet возврощал не тип даных (строка, инт) а какой-от бред бесполезный
                    */
                    DEBUG(type)
                    DEBUG(key)
                    DEBUG(value)
                }
                else 
                {
                    commonPath = String("/");
                    commonPath += key;
                    commonPath += String("/");
                }
            }
            json->iteratorEnd();
        }
    }
        
public:
    SimpleFirebase()
    {

    }

    void begin(FirebaseConfig *config, FirebaseAuth *auth)
    {
        pSingletonInstance = this;        
        Firebase.begin(config, auth);
        Firebase.reconnectWiFi(true);
        _fbdo.setBSSLBufferSize(2048, 2048);
        _fbdo.setResponseSize(2048);
        if (!Firebase.beginStream(_fbdo, "/WowPixelMini/IlyaTtest"))
        {
          //Serial.println("------------------------------------");
          //Serial.println("Can't begin stream connection...");
          //Serial.println("REASON: " + _fbdo.errorReason());
          //Serial.println("------------------------------------");
          //Serial.println();
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
        _handleQuery();
    }
};

SimpleFirebase * SimpleFirebase::pSingletonInstance = 0;

#endif
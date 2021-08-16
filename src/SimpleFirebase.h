#ifndef SIMPLE_FIREBASE_H
#define SIMPLE_FIREBASE_H

#include <Firebase_ESP_Client.h>
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
    PathData()
    {
        path = "";
        data = "";
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
    FirebaseStream *data;
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

    static void streamCallback(FirebaseStream data)
    {
        if(pSingletonInstance)  pSingletonInstance->_streamCallback(data);
    }

    static void streamTimeoutCallback(bool tout)
    {
        if(pSingletonInstance && tout)  pSingletonInstance->_streamTimeoutCallback(tout);
    }

    void _streamCallback(FirebaseStream data)
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
            _handleCallbacks(&data);
        }
    }

    void _streamTimeoutCallback(bool tout)
    {
        DEBUG("TIMEOUT")
    }

    void _handleCallbacks(FirebaseStream* data)
    {
        this->_handleCallbacks(this->_castFirebaseStream(data));
    }

    PathData _castFirebaseStream(FirebaseStream* data)
    {
        DEBUG("Cast start")
        PathData pd;
        pd.path = data->dataPath();
        switch(data->dataTypeEnum())
        {
        case fb_esp_rtdb_data_type_integer:
            pd.data = String(data->to<int>());
            DEBUG("Cast type: int")
            break;

        case fb_esp_rtdb_data_type_string:
            pd.data = data->to<String>();
            DEBUG("Cast type: string")
            break;
        }
        DEBUG(pd.path)
        DEBUG(pd.data)
        return pd;
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

    void _parseJson(FirebaseStream* data)
    {
        FirebaseJson* json = data->jsonObjectPtr();
        String commonPath = data->dataPath();
        commonPath += String("/");
        if(1)
        {   //DEBUG("iterator begin");
            for(size_t i = 0; i < json->iteratorBegin(); i++)
            {
                String key, value;
                int type = 0;
                //DEBUG("iterator get")
                json->iteratorGet(i, type, key, value);
                //DEBUG("iterator getted")

                if(value.startsWith("\""))
                {
                    DEBUG("Bad string start")
                    value.remove(0, 1);
                    if(value.endsWith("\""))
                    {
                        DEBUG("Bad string end")
                        value.remove(value.length() - 1, 1);
                    }
                }

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
        pSingletonInstance = this;        
        Firebase.begin(config, auth);
        Firebase.reconnectWiFi(true);
        _fbdo.setBSSLBufferSize(4096, 4096);//2048, 2048
        _fbdo.setResponseSize(4096);//2048
        if (!Firebase.RTDB.beginStream(&_fbdo, path.c_str()))
        {
          //Serial.println("------------------------------------");
          //Serial.println("Can't begin stream connection...");
          //Serial.println("REASON: " + _fbdo.errorReason());
          //Serial.println("------------------------------------");
          //Serial.println();
        }
        Firebase.RTDB.setStreamCallback(&_fbdo, SimpleFirebase::streamCallback, SimpleFirebase::streamTimeoutCallback);

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
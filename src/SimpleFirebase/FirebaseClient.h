#ifndef SIMPLEFB_CLIENT_H
#define SIMPLEFB_CLIENT_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecureBearSSL.h>
#include "TinyJsonFinal.h"
#include "SmartArray.h"
#include <ArduinoJson.h>

static const uint16_t bearssl_client_timeout = 5000;
static const uint16_t bearssl_client_buffersize = 4096;//2048;

class FirebaseClient
{
protected:
    const char* _token;
    const char* _host;
    void(*_onMessageCallback)(DynamicJsonDocument&);
    
public:
    FirebaseClient(const char* host, const char* token) : _host(host), _token(token){}
    virtual void onReceive(void(*cb)(DynamicJsonDocument&)) = 0;
    virtual bool begin() = 0;
    virtual void handle() = 0;
    virtual bool beginStream(const char* path) = 0;
    virtual void requestData(const char* path) = 0;

    virtual ~FirebaseClient() = default;
};

class FirebaseClientImpl : public FirebaseClient
{
private:
    WiFiClientSecure _client;
public:
    FirebaseClientImpl(const char* host, const char* token) : FirebaseClient(host, token){}

    bool begin()
    {
        Dln(_host)
        Dln(_token)

        _client.setInsecure();
        _client.setNoDelay(true);
        _client.setTimeout(bearssl_client_timeout);
        return _client.connect(_host, 443);
    }

    bool beginStream(const char* path) override
    {
        char s[strlen(_token) + strlen(path) + 30] = "GET ";
        strcat(s, path);
        strcat(s, ".json?auth=");
        strcat(s, _token);
        strcat(s," HTTP/1.1\r\n");
        
        char h[strlen(_host) + 10] = "Host: ";
        strcat(h, _host);
        strcat(h, "\r\n");

        _client.print(s); //GET request with auth
        _client.print(h); //Host header
        _client.print("Accept: text/event-stream\r\n");
        _client.print("Connection: keep-alive\r\n\r\n");

        return true; // hard code, should return real state of stream begining
    }

    void handle() override
    {
        if(_client.available() && _onMessageCallback)
        {
            SmartArray<char> buf(bearssl_client_buffersize);
            _client.readBytesUntil('\n', buf, bearssl_client_buffersize);
            if(TinyJson::getIndexOf(buf, "data") == 0)
            {
                Dln(buf);
                //SmartArray<char> json(3000);
                //TinyJson::createJson(buf + 6, json, 3000);
                DynamicJsonDocument doc(3096);
                DeserializationError error = deserializeJson(doc, buf + 6);                
                if (error) {
                    Serial.print(F("deserializeJson() failed: "));
                    Serial.println(error.c_str());
                }
                buf.clearHeap();
                _onMessageCallback(doc);
                //json.clearHeap();
            }
            else buf.clearHeap();
        }
    }

    void onReceive(void(*cb)(DynamicJsonDocument&)) override
    {
        _onMessageCallback = cb;
    }

    void requestData(const char* path)
    {
        char s[strlen(_token) + strlen(path) + 30] = "GET ";
        strcat(s, path);
        strcat(s, ".json?auth=");
        strcat(s, _token);
        strcat(s," HTTP/1.1\r\n");
        
        char h[strlen(_host) + 10] = "Host: ";
        strcat(h, _host);
        strcat(h, "\r\n");

        _client.print(s); //GET request with auth
        _client.print(h); //Host header
        _client.print("Accept: application/\r\n");
        _client.print("Connection: keep-alive\r\n\r\n");
    }
    

    ~FirebaseClientImpl(){}
};


/*
@brief Class for TESTING SimpleFirebase cb handling 
*/
// class FirebaseClientTest : public FirebaseClient
// {
// public:
//     FirebaseClientTest(const char* host, const char* token) : FirebaseClient(host, token){}

//     bool begin() override
//     {
//         #warning Test cilent!

//         Dln("Begin")
//         return true;
//     }

//     void handle() override
//     {
        
//         static uint32_t timer = millis() * 2;
//         if(millis() - timer >= 2000)
//         {
//             Dln("CB called")
//             timer = millis();
//             SmartArray<char> buf(2048);
//             //char* buf = new char[2048];
//             //memset(buf, 0, 2048);
//             //strcpy(buf,"{\"path\":\"/\",\"data\":{\"Case\":{\"Case\":24},\"Control\":{\"Brig\":50,\"Digits\":{\"BackColor\":\"#1121\",\"Color\":\"#1010\",\"Text\":\"19\"},\"Text\":{\"BackColor\":\"#0\",\"Color\":\"00ff00\",\"Scroll\":-9,\"Text\":\"\"}},\"Matrix\":{\"Led0\":\"#82ff09\",\"Led1\":\"#82ff00\",\"Led10\":\"#00fbff\",\"Led11\":\"#00fbff\",\"Led12\":\"#00ffe5\",\"Led13\":\"#00ffe5\",\"Led14\":\"#00ffa7\",\"Led15\":\"#00ffa7\",\"Led16\":\"#008fff\",\"Led17\":\"#008fff\",\"Led18\":\"#0053ff\",\"Led19\":\"#0053ff\",\"Led2\":\"#14ff00\",\"Led20\":\"#0037ff\",\"Led21\":\"#0037ff\",\"Led22\":\"#0015ff\",\"Led23\":\"#0015ff\",\"Led24\":\"#8a00ff\",\"Led25\":\"#8a00ff\",\"Led26\":\"#5800ff\",\"Led27\":\"#5800ff\",\"Led28\":\"#4200ff\",\"Led29\":\"#4200ff\",\"Led3\":\"#14ff00\",\"Led30\":\"#1c00ff\",\"Led31\":\"#1c00ff\",\"Led32\":\"#bb00ff\",\"Led33\":\"#bb00ff\",\"Led34\":\"#d800ff\",\"Led35\":\"#d800ff\",\"Led36\":\"#ff00d8\",\"Led37\":\"#ff00d8\",\"Led38\":\"#ff00a4\",\"Led39\":\"#ff00a4\",\"Led4\":\"#00ff35\",\"Led40\":\"#ff4a00\",\"Led41\":\"#ff4a00\",\"Led42\":\"#ff1100\",\"Led43\":\"#ff1100\",\"Led44\":\"#ff0030\",\"Led45\":\"#ff0030\",\"Led46\":\"#ff0070\",\"Led47\":\"#ff0070\",\"Led48\":\"#ff7e00\",\"Led49\":\"#ff7e00\",\"Led5\":\"#00ff35\",\"Led50\":\"#ffab00\",\"Led51\":\"#ffc200\",\"Led52\":\"#ffd400\",\"Led53\":\"#ffd400\",\"Led54\":\"#fff600\",\"Led55\":\"#fff600\",\"Led56\":\"#09ff00\",\"Led57\":\"#09ff00\",\"Led58\":\"#74ff00\",\"Led59\":\"#74ff00\",\"Led6\":\"#00ff71\",\"Led60\":\"#b5ff00\",\"Led61\":\"#b5ff00\",\"Led62\":\"#e6ff00\",\"Led63\":\"#e6ff00\",\"Led7\":\"#00ff71\",\"Led8\":\"#00b6ff\",\"Led9\":\"#00b6ff\"}}}"); 
//             strcpy(buf, "{\"data\":{\"Case\":{\"Case\":24},\"Control\":{\"Brig\":50,\"Digits\":{\"BackColor\":\"#1121\",\"Color\":\"#1010\",\"Text\":\"19\"}}}}}");
//             SmartArray<char> json(1024);
//             //char* json = new char[1024];
//             //memset(json, 0, 1024);
//             TinyJson::createJson(buf, json, 1024);
//             //delete[] buf;
//             buf.clearHeap();
//             if(_onMessageCallback) _onMessageCallback(json);
//             //delete[] json;
//             json.clearHeap();
//         }
//     }

//     void onReceive(void(*cb)(char*)) override
//     {
//         Dln("onReceive")
//         _onMessageCallback = cb;
//     }

//     void requestData(const char* path){}

//     bool beginStream(const char* path) override
//     {
//         Dln("beginStream")
//         return true;
//     }
// };

#endif
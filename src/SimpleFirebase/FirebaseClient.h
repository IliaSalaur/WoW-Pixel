#ifndef SIMPLEFB_CLIENT_H
#define SIMPLEFB_CLIENT_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecureBearSSL.h>
#include "TinyJsonFinal.h"
#include <memory>

static const uint16_t bearssl_client_reconnect_timeout = 20000;
static const uint32_t bearssl_client_keepalive_timeout = 45000;
static const uint16_t bearssl_client_timeout = 10000;
static const uint16_t bearssl_client_buffersize = 5000;//2048;
static const uint16_t bearssl_client_json_size = 6144;
static const uint16_t bearssl_init_path_size = 128;

class FirebaseClient
{
protected:    
    const char* _host;
    const char* _token;
    void(*_onMessageCallback)(char*);
    void(*_onDisconnectCallback)();
    
public:
    FirebaseClient(const char* host, const char* token) : _host(host), _token(token), _onMessageCallback(nullptr), _onDisconnectCallback(nullptr){}
    virtual void onReceive(void(*cb)(char*)) = 0;
    virtual void onDisconnect(void(*cb)()) = 0;
    virtual bool begin() = 0;
    virtual void handle() = 0;
    virtual bool beginStream(const char* path) = 0;
    virtual void requestData(const char* path) = 0;

    virtual ~FirebaseClient() = default;
};

class FirebaseClientImpl : public FirebaseClient
{
private:
    bool _reconnecting;
    WiFiClientSecure* _client;
public:
    FirebaseClientImpl(const char* host, const char* token) : FirebaseClient(host, token), _reconnecting(false), _client(new WiFiClientSecure){}

    bool begin()
    {
        Fm("Host: %s\nToken: %s\n", _host, _token)
        _client->stop();
        _client->setInsecure();
        _client->setNoDelay(true);
        _client->setTimeout(bearssl_client_timeout);
        return _client->connect(_host, 443);
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

        _client->print(s); //GET request with auth
        _client->print(h); //Host header
        _client->print("Accept: text/event-stream\r\n");
        _client->print("Connection: keep-alive\r\n\r\n");

        return true; // hard code, should return real state of stream begining
    }

    void handle() override
    {
        static uint32_t _reconnectTimer = millis();
        static uint32_t _lastKeepAlive = millis();
        if((!_client->connected() || _reconnecting) && millis() - _reconnectTimer > bearssl_client_reconnect_timeout)
        {
            _reconnectTimer = millis();
            _reconnecting = 0;
            if(_onDisconnectCallback) _onDisconnectCallback();
        }

        if(_client->available())
        {
            _lastKeepAlive = millis();

            auto buf = std::make_unique<char[]>(bearssl_client_buffersize);
            buf.get()[0] = 0;
            _client->readBytesUntil('\n', buf.get(), bearssl_client_buffersize);
            if(TinyJson::getIndexOf(buf.get(), "data") == 0)
            {
                Fm("handle buf.get():\n%s\n\n", buf.get())
                const uint16_t bufSize = strlen(buf.get());
                Fm("buf size:%u\n\n", bufSize)
                auto json = std::make_unique<char[]>(bearssl_client_json_size);
                json.get()[0] = 0;
                char initialPath[bearssl_init_path_size]{0};
                int pI = TinyJson::getIndexOf(buf.get() + 6, "path");
                if(pI != -1)
                {
                    strncpy(initialPath, buf.get() + 6 + pI + 7, TinyJson::getIndexOf(buf.get() + 6, "\",", pI + 6) - pI - 7);
                }
                TinyJson::createJson(buf.get() + 6, json.get(), bearssl_client_json_size, initialPath);
                Fm("json size:%u\n", strlen(json.get()))
                if(_onMessageCallback)_onMessageCallback(json.get());
            }
        }

        if(millis() - _lastKeepAlive > bearssl_client_keepalive_timeout){
            _reconnecting = true;            
        }
    }

    void onReceive(void(*cb)(char*)) override
    {
        _onMessageCallback = cb;
    }

    void onDisconnect(void(*cb)()) override
    {
        _onDisconnectCallback = cb;
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

        _client->print(s); //GET request with auth
        _client->print(h); //Host header
        _client->print("Accept: application/\r\n");
        _client->print("Connection: keep-alive\r\n\r\n");
    }
    

    ~FirebaseClientImpl(){}
};

#endif
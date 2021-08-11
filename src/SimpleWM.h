#ifndef SIMPLE_WM_H
#define SIMPLE_WM_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

#define EEP_KEY_START 125
#define EEP_KEY_STOP 21


struct WiFiConfig
{
    char ssid[32];
    char pass[32];
    WiFiConfig()
    {
        for(int i = 0; i < 32; i++)
        {
            ssid[i] = '\0';
            pass[i] = '\0';
        }
    }
};


//  dnsServer.processNextRequest();
//  webServer->handleClient();

class SimpleWM
{
private:
    const uint8_t DNS_PORT = 53;
    IPAddress *apIP;
    DNSServer dnsServer;
    ESP8266WebServer *webServer;
    void(*funcPtr)() = nullptr;
    bool connected = 0;
    String html ="<!DOCTYPE html> <html> <head> <title>WowPixel</title> </head> <body> <h2>Connect to WiFi</h2> <form action=\"/connect\"> <label for=\"ssid\">Ssid:</label><br> <input type=\"text\" id=\"ssid\" name=\"ssid\" value=\"\"><br> <label for=\"pass\">Password:</label><br> <input type=\"text\" id=\"pass\" name=\"pass\" value=\"\"><br><br> <input type=\"submit\" value=\"Connect\"> </form> </body> </html>";

    void handleRoot()
    {
        webServer->send(200, "text/html", html);
    }

    void handleConnect()
    {
        this->connectWiFi(webServer->arg(String("ssid")), webServer->arg(String("pass")));
    }

    void connectWiFi(String ssid, String pass)
    {
        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid, pass);
        while(!WiFi.isConnected()){Serial.print("~"); delay(300);}
        Serial.println(WiFi.localIP());
        if( funcPtr)  funcPtr();
        webServer->send(200, "text/plain", "Succes");
        connected = 1;

        saveEEPROM(ssid, pass);
    }

    bool checkEEPROM()
    {
        if(EEPROM.read(0) == EEP_KEY_START)
        {
            WiFiConfig wconf;
            EEPROM.get(1, wconf);
            DEBUG(wconf.ssid)
            DEBUG(wconf.pass)
            connectWiFi(wconf.ssid, wconf.pass);
            return 1;
        }
        return 0;
    }

    void saveEEPROM(String ssid, String pass)
    {
        EEPROM.write(0, EEP_KEY_START);
        WiFiConfig conf;
        ssid.toCharArray(conf.ssid, 32);
        pass.toCharArray(conf.pass, 32);
        EEPROM.put(1, conf);
        EEPROM.commit();
    }

    void startCaptivePortal(const char* ap_ssid)
    {
        WiFi.mode(WIFI_AP);
        WiFi.softAPConfig(*apIP, *apIP, IPAddress(255, 255, 255, 0));
        WiFi.softAP(ap_ssid);
        dnsServer.start(DNS_PORT, "*", *apIP);
        webServer->onNotFound(std::bind(&SimpleWM::handleRoot, this));
        webServer->on(String(F("/connect")), std::bind(&SimpleWM::handleConnect, this));

        webServer->begin();
    }

public:

    SimpleWM()
    {
        apIP = new IPAddress(192, 168, 43, 1);
        webServer = new ESP8266WebServer(80);
    }

    SimpleWM onConnect(void(*funcptr)())
    {
        funcPtr = funcptr;
    }

    void begin(const char* ap_ssid)
    {
        EEPROM.begin(100);
        if(!checkEEPROM())
        {
            startCaptivePortal(ap_ssid);
        }
        
        while(!connected)
        {
            this->handle();
            yield();
        }
    }

    void handle()
    {
        if(connected == 0)
        {
            dnsServer.processNextRequest();
            webServer->handleClient();
        }
    }

    bool getState()
    {
        return connected;
    }
};


#endif
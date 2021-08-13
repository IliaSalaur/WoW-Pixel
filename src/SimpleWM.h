#ifndef SIMPLE_WM_H
#define SIMPLE_WM_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <FS.h>
#include <Arduino_JSON.h>

#define EEP_KEY_START 120
#define EEP_KEY_STOP 21
#define WAIT_FOT_CONNECTION 30 //seconds

#define HTML_SIZE 204
const char* htmlP PROGMEM ="<!DOCTYPE html> <html > <head> <title>WowPixel</title> </head> </body> </html> <script> window.document.onload = redirect(); function redirect() { location.replace(\"/\"); console.log(\"redir\"); } </script>";

/*
const char * htmlP PROGMEM = ("<!DOCTYPE html> <html onload = \" redir()\"> <head> <title>WowPixel</title> </head> <body> <h2>Connect to WiFi</h2> <form action=\"/connect\"> <label for=\"ssid\">Ssid:</label><br> <input type=\"text\" id=\"ssid\" name=\"ssid\" value=\"\"><br> <label for=\"pass\">Password:</label><br> <input type=\"text\" id=\"pass\" name=\"pass\" value=\"\"><br><br> <input type=\"submit\" value=\"Connect\"> </form> </body> <script> function redir(){location.replace(\"/\");}</script></html>");*/

struct WiFiConfig
{
    bool isValid = 0;
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

    WiFiConfig(String ssid, String pass, bool isV)
    {
        ssid.toCharArray(this->ssid, 32);
        pass.toCharArray(this->pass, 32);
        this->isValid = isV;
    }
};


//  dnsServer.processNextRequest();
//  webServer->handleClient();

class SimpleWM
{
private:
    const uint8_t DNS_PORT = 53;
    IPAddress apIP;
    std::unique_ptr<DNSServer> dnsServer;
    std::unique_ptr<ESP8266WebServer> webServer;

    JSONVar nets;

    void(*funcPtr)() = nullptr;
    bool connected = 0;
    uint32_t _startTimer = 0;    

    void handleRoot()
    {
        char html[HTML_SIZE];
        strcpy_P(html, htmlP);
        webServer->send(200, "text/html", html);
    }

    void handleConnect()
    {
        WiFiConfig conf = WiFiConfig(webServer->arg(String("ssid")), webServer->arg(String("pass")), 0);
        this->connectWiFi(conf);
        //this->connectWiFi(webServer->arg(String("ssid")), webServer->arg(String("pass")));
    }

    void handleNets()
    {
        this->scanWiFi();
        webServer->send(200, "application/json", JSON.stringify(nets));
    }

    void scanWiFi()
    {
        for(int i = 0; i < WiFi.scanNetworks(); i++)
        {
            nets[i] = WiFi.SSID(i);
        }
    }

    void connectWiFi(WiFiConfig conf)
    {
        _startTimer = millis();
        WiFi.mode(WIFI_STA);
        WiFi.begin(conf.ssid, conf.pass);

        while(1)
        {
            Serial.print("~"); delay(300);
            if(WiFi.isConnected())
            {
                connected = 1;
                saveEEPROM(conf);
                Serial.println(WiFi.localIP());
                if( funcPtr)  funcPtr();
                break;
            }
            else if(millis() - _startTimer >= WAIT_FOT_CONNECTION * 1000)
            {
                DEBUG("Bad credentials, reset")
                connected = 0;
                startCaptivePortal(AP_NAME);
                break;
            }
        }        
    }

    WiFiConfig checkEEPROM()
    {
        if(EEPROM.read(0) == EEP_KEY_START)
        {
            WiFiConfig wconf;
            EEPROM.get(1, wconf);
            DEBUG(wconf.ssid)
            DEBUG(wconf.pass)
            wconf.isValid = 1;
            return wconf;
        }
        return WiFiConfig("", "", 0);
    }

    void saveEEPROM(WiFiConfig conf)
    {
        if(conf.isValid == 0)
        {
            EEPROM.write(0, EEP_KEY_START);
            EEPROM.put(1, conf);
            EEPROM.commit();
            DEBUG("EEPROM Saved")
        }                
    }

    void startCaptivePortal(const char* ap_ssid)
    {
        DEBUG("CaptivePortal started")
        if(!SPIFFS.begin())
        {
            DEBUG("Spiffs failed");
        }
        WiFi.mode(WIFI_AP);
        apIP = IPAddress(192, 168, 43, 1);
        WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
        WiFi.softAP(ap_ssid);

        dnsServer.reset(new DNSServer);
        webServer.reset(new ESP8266WebServer(80));
        dnsServer->start(DNS_PORT, "*", *apIP);
        webServer->on(String(F("/nets")), std::bind(&SimpleWM::handleNets, this));
        webServer->onNotFound(std::bind(&SimpleWM::handleRoot, this));
        webServer->on(String(F("/connect")), std::bind(&SimpleWM::handleConnect, this));
        webServer->serveStatic("/", SPIFFS, "/Page.html");
        webServer->begin();
    }

public:

    SimpleWM()
    {}

    SimpleWM onConnect(void(*funcptr)())
    {
        funcPtr = funcptr;
    }

    void begin(const char* ap_ssid)
    {
        EEPROM.begin(100);
        WiFiConfig conf = checkEEPROM();

        if(!conf.isValid)
        {
            startCaptivePortal(ap_ssid);
        }
        else
        {
            connectWiFi(conf);
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
            dnsServer->processNextRequest();
            webServer->handleClient();
        }
    }

    bool getState()
    {
        return connected;
    }
};


#endif
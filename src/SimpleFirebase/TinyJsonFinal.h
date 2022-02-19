#ifndef TINY_JSON_FINAL_H
#define TINY_JSON_FINAL_H

#include <stdio.h>
#include <string.h>
#include "SmartArray.h"
#include <Arduino.h>
#include <string>

#ifdef FB_DEBUG
#define Fm(x...) Serial.printf(x);
#define D(x)  Serial.print(x);
#define Dln(x) Serial.print("FB_DEB: ");  Serial.println(x); Serial.flush();
#define Line() Serial.println(__LINE__);
#else 
#define Fm(x...)
#define D(x)
#define Dln(x)
#define Line()
#endif

class TinyJson
{
public:
    static void oneLapDeserialization(char* jsonStr, char* buf, char* path)
    {
        size_t len = strlen(jsonStr);
        // char basepath[16] = {0};
        // static bool founded = 0;
        if(jsonStr[0] == '{' && jsonStr[len-1] == '}')
        {
            // if(TinyJson::path(buf)[0] != '\2')
            // {
            //     strcpy(basepath, path);
            //     founded = 1;
            // }
            // else if(!founded)
            // {
            //     strcpy(basepath, TinyJson::path(buf).c_str());
            // }
            for(size_t i = 0; i < len-2; i++)
            {
                size_t kStartI = i;
                size_t kStopI = 0;
                
                kStopI = getIndexOf(jsonStr, "\":", i);
                kStartI = getIndexOf(jsonStr, "\"", kStopI-1, 0);
                
                SmartArray<char> key(kStopI-kStartI+1);
                strncpy(key, jsonStr + kStartI + 1, kStopI - kStartI - 1);

                //VALUE
                int vStartI = kStopI+2;
                int vStopI = 0, valStrEnd = vStartI, t1, t2;
                bool ignoreCitate = false;
                if(jsonStr[vStartI] == '\"')
                {
                    valStrEnd = getIndexOf(jsonStr, "\"", vStartI+1);
                    t1 = getIndexOf(jsonStr, ",", valStrEnd);
                    t2 = getIndexOf(jsonStr, "}", valStrEnd);
                
                    vStopI = (t1 > -1) ? t1:t2;   
                    ignoreCitate = true;
                }
                else if(jsonStr[vStartI] == '{')
                {
                    int braces = 1;
                    int j = vStartI + 1;
                    while(braces != 0)
                    {
                        if(jsonStr[j] == '}')
                        {
                            braces--;
                        }
                        else if(jsonStr[j] == '{')
                        {
                            braces++;
                        }
                        j++;
                    }
                    vStopI = j;
                }
                else
                {
                    t1 = getIndexOf(jsonStr, ",", valStrEnd);
                    t2 = getIndexOf(jsonStr, "}", valStrEnd);
                
                    vStopI = (t1 > -1) ? t1:t2;   
                }
                
                SmartArray<char> value(vStopI-vStartI+1);
                strncpy(value, jsonStr + vStartI + int(ignoreCitate), vStopI - vStartI - ((ignoreCitate) ? 2:0));
                i = vStopI;

                switch (value[0])
                {
                case '{':
                    {
                        char allPath[64] = "";
                        strcat(allPath, path);                        
                        if(!strstr("/data", key)) {Dln(key)strcat(allPath, "/"); strcat(allPath, key);}
                        oneLapDeserialization(value, buf, allPath);

                    }break;
                
                default:
                    strcat(buf, path);
                    strcat(buf, "/");
                    strcat(buf, key);
                    strcat(buf, "\t");
                    strcat(buf, value);
                    strcat(buf, "\n"); 
                    break;
                }
                value.clearHeap();
                key.clearHeap();
            }
        }
        //else //Dln("FAIL");
    }

    static uint16_t countSize(const char* jsonStr)
    {
        static uint16_t size = 0;
        size_t len = strlen(jsonStr);
        if(jsonStr[0] == '{' && jsonStr[len-1] == '}')
        for(size_t i = 0; i < len-2; i++)
        {
            size_t kStartI = i;
            size_t kStopI = 0;
            
            kStopI = getIndexOf(jsonStr, "\":", i);
            kStartI = getIndexOf(jsonStr, "\"", kStopI-1, 0);
            
            size += kStopI-kStartI+1;
            
            //VALUE
            int vStartI = kStopI+2;
            int vStopI = 0, valStrEnd = vStartI, t1, t2;
            if(jsonStr[vStartI] == '\"')
            {
                valStrEnd = getIndexOf(jsonStr, "\"", vStartI+1);
                t1 = getIndexOf(jsonStr, ",", valStrEnd);
                t2 = getIndexOf(jsonStr, "}", valStrEnd);
            
                vStopI = (t1 > -1) ? t1:t2;   
            }
            else if(jsonStr[vStartI] == '{')
            {
                int braces = 1;
                int j = vStartI + 1;
                while(braces != 0)
                {
                    if(jsonStr[j] == '}')
                    {
                        braces--;
                    }
                    else if(jsonStr[j] == '{')
                    {
                        braces++;
                    }
                    j++;
                }
                vStopI = j;
            }
            else
            {
                t1 = getIndexOf(jsonStr, ",", valStrEnd);
                t2 = getIndexOf(jsonStr, "}", valStrEnd);
            
                vStopI = (t1 > -1) ? t1:t2;   
            }
            
            size += vStopI-vStartI+1;
            i = vStopI;
        }
        return size;
    }

    static int getIndexOf(const char* str, const char* find, int start = 0, int stop = -1)
    {
        size_t strLen = strlen(str);
        size_t findLen = strlen(find);
        int step = 1;
        
        if(stop != -1 && start > stop) {step = -1;}
        else stop = strLen;

        
        for(size_t i = start; (step == -1) ? (i > stop):(i < stop); i += step)
        {
            if(str[i] == find[0])
            {
                bool f = 1;
                for(size_t j = 0; j < findLen; j++)
                {
                if(str[i + j] != find[j])
                {
                    f = 0;
                    break;
                }
                }
                if(f == 1)
                {
                    return (step == -1) ? i:i;
                }
            }
        }
        return -1;
    }

    static void pathPrep(char* json, size_t s)
    {
        SmartArray<char> path(64);
        getValue("/path", json, path);
        if(strlen(path) < 5) return;
        SmartArray<char> data(32);
        getValue("/data", json, data);
        memset(json, 0, s);

        strcat(json, path);
        strcat(json, "\t");
        strcat(json, data);
        strcat(json, "\n"); 
        data.clearHeap();
        path.clearHeap();
    }

    static void createJson(char* json, char* buf, size_t s, char* path = "")
    {
        memset(buf, 0, s);
        oneLapDeserialization(json, buf, path);
        //pathPrep(json, s);
    }
    
    static void getValue(const char* key, const char* json, char* buf)
    {
        memset(buf, 0, 64);
        int kStartI = getIndexOf(json, key);
        int kStopI = getIndexOf(json, "\t", kStartI);
        int vStopI = getIndexOf(json, "\n", kStopI);
        if(kStartI == -1 || kStopI == -1 || vStopI == -1)
        {
            strcpy(buf, "\2");
            return;
        }
        strncpy(buf, json + kStopI + 1, vStopI - kStopI - 1);
    }

    static void getValue(const char* key, char* json, char* buf)
    {
        memset(buf, 0, 64);
        int kStartI = getIndexOf(json, key);
        int kStopI = getIndexOf(json, "\t", kStartI);
        int vStopI = getIndexOf(json, "\n", kStopI);
        if(kStartI == -1 || kStopI == -1 || vStopI == -1)
        {
            strcpy(buf, "\2");
            return;
        }
        strncpy(buf, json + kStopI + 1, vStopI - kStopI - 1);
    }

    // static std::string getValue(const char* key, char* json)
    // {
    //     std::string s;
    //     int kStartI = getIndexOf(json, key);
    //     int kStopI = getIndexOf(json, "\t", kStartI);
    //     int vStopI = getIndexOf(json, "\n", kStopI);
    //     if(kStartI == -1 || kStopI == -1 || vStopI == -1)
    //     {
    //         return std::string("\2");
    //     }
    //     strncpy(s.data(), json + kStopI + 1, vStopI - kStopI - 1);
    //     return s;
    // }

    static std::string path(const char* json)
    {
        SmartArray<char> buf(64);
        getValue("/path", json, buf);
        std::string s;
        s += buf;
        buf.clearHeap();
        return s;
    }

    static std::string value(const char* json)
    {
        SmartArray<char> buf(64);
        getValue("/data", json, buf);
        std::string s;
        s += buf;
        buf.clearHeap();
        return s;
    }

    static std::string getNode(const char* json, const char* nodePath, int index = 0)
    {
        char* buf = new char[64];
        memset(buf, 0 ,64);
        int pStartI = getIndexOf(json, nodePath, index);
        strncpy(buf, json + pStartI, getIndexOf(json, "\t", pStartI) - pStartI);
        
        std::string s = "/path";
        s.reserve(128);
        s += "\t";
        //if(json[getIndexOf(json, "/path", index) + 7] != '\n') s += path(json);
        s += buf;
        s += "\n";
        
        memset(buf, 0, 64);
        getValue(nodePath, json + index, buf);
        
        s += "/data";
        s += "\t";
        s += buf;
        s += "\n";

        delete[] buf;
        return s;
    }
    //#error "Fix the parsing of patch request."
};


#endif
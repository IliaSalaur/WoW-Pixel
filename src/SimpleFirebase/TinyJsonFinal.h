#ifndef TINY_JSON_FINAL_H
#define TINY_JSON_FINAL_H

#include <stdio.h>
#include <string.h>
#include <Arduino.h>
#include <string>
#include <memory>

#ifdef FB_DEBUG
#define Fm(x...) Serial.printf(x);
#define Line() Serial.println(__LINE__);
#else 
#define Fm(x...)
#define Line()
#endif

//Constants
namespace TinyJsonConfig{
    const uint8_t k_value_size = 96;
    const uint8_t k_path_size = 128;
    const uint8_t k_node_size = k_value_size + k_path_size;
    const char * k_fail = "?"; //if an operation fails (getValue, getPath, getNode), the k_fail wil be returned
}

class TinyJson
{
public:
    static void oneLapDeserialization(char* jsonStr, char* buf, char* path)
    {
        size_t len = strlen(jsonStr);
        if(jsonStr[0] == '{' && jsonStr[len-1] == '}')
        {
            for(size_t i = 0; i < len-2; i++)
            {
                size_t kStartI = i;
                size_t kStopI = 0;
                
                kStopI = getIndexOf(jsonStr, "\":", i);
                kStartI = getIndexOf(jsonStr, "\"", kStopI-1, 0);
                
                auto key = std::make_unique<char[]>(kStopI-kStartI+1);
                key.get()[0] = 0;
                strncpy(key.get(), jsonStr + kStartI + 1, kStopI - kStartI - 1);

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
                
                auto value = std::make_unique<char[]>(vStopI-vStartI+1);
                value.get()[0] = 0;
                strncpy(value.get(), jsonStr + vStartI + int(ignoreCitate), vStopI - vStartI - ((ignoreCitate) ? 2:0));                
                i = vStopI;

                switch (value.get()[0])
                {
                case '{':
                    {
                        auto allPath = std::make_unique<char[]>(TinyJsonConfig::k_path_size);
                        allPath.get()[0] = 0;
                        strcat(allPath.get(), path);                        
                        if(!strstr("/data", key.get())) {strcat(allPath.get(), "/"); strcat(allPath.get(), key.get());}
                        oneLapDeserialization(value.get(), buf, allPath.get());

                    }break;
                
                default:
                    strcat(buf, path);
                    strcat(buf, "/");
                    strcat(buf, key.get());
                    strcat(buf, "\t");
                    strcat(buf, value.get());
                    strcat(buf, "\n"); 
                    break;
                }
            }
        }
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

        
        for(int i = start; (step == -1) ? (i > stop):(i < stop); i += step)
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

    static void pathPrep(char* json, size_t s)//SmartArray
    {
        char path[TinyJsonConfig::k_path_size];
        getValue("/path", json, path);
        if(strlen(path) < 5) return;
        char data[TinyJsonConfig::k_value_size];
        getValue("/data", json, data);
        memset(json, 0, s);

        strcat(json, path);
        strcat(json, "\t");
        strcat(json, data);
        strcat(json, "\n"); 
        // data.clearHeap();
        // path.clearHeap();
    }

    static void createJson(char* json, char* buf, size_t s, char* path = {0})
    {
        memset(buf, 0, s);
        oneLapDeserialization(json, buf, path);
        //pathPrep(json, s);
    }
    
    static void getValue(const char* key, const char* json, char* buf)
    {
        memset(buf, 0, TinyJsonConfig::k_value_size);
        int kStartI = getIndexOf(json, key);
        int kStopI = getIndexOf(json, "\t", kStartI);
        int vStopI = getIndexOf(json, "\n", kStopI);
        if(kStartI == -1 || kStopI == -1 || vStopI == -1)
        {
            strcpy(buf, TinyJsonConfig::k_fail);
            return;
        }
        strncpy(buf, json + kStopI + 1, vStopI - kStopI - 1);
    }

    static void getValue(const char* key, char* json, char* buf)
    {
        memset(buf, 0, TinyJsonConfig::k_value_size);
        int kStartI = getIndexOf(json, key);
        int kStopI = getIndexOf(json, "\t", kStartI);
        int vStopI = getIndexOf(json, "\n", kStopI);
        if(kStartI == -1 || kStopI == -1 || vStopI == -1)
        {
            strcpy(buf, TinyJsonConfig::k_fail);
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

    static std::string path(const char* json)//SmartArray
    {
        char buf[TinyJsonConfig::k_path_size]{0};
        getValue("/path", json, buf);
        std::string s;
        s += buf;
        // buf.clearHeap();
        return s;
    }

    static std::string value(const char* json)
    {
        char buf[TinyJsonConfig::k_value_size]{0};
        getValue("/data", json, buf);
        std::string s;
        s += buf;
        // buf.clearHeap();
        return s;
    }

    static void getNode(const char* json, const char* nodePath, char* nodeBuf, int index = 0)
    {
        char buf[TinyJsonConfig::k_value_size]{0};
        memset(buf, 0 ,TinyJsonConfig::k_value_size);
        int pStartI = getIndexOf(json, nodePath, index);
        strncpy(buf, json + pStartI, getIndexOf(json, "\t", pStartI) - pStartI);
        
        sprintf(nodeBuf, "/path\t%s\n", buf);
        
        memset(buf, 0, TinyJsonConfig::k_value_size);
        getValue(nodePath, json + index, buf);
        
        strcat(nodeBuf, "/data\t");
        strcat(nodeBuf, buf);
        strcat(nodeBuf, "\n");
    }
    //#error "Fix the parsing of patch request."
};


#endif
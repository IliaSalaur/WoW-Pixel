#ifndef STATE_PERSISTENCE_H
#define STATE_PERSISTENCE_H

extern "C"{
    #include "user_interface.h"
}

#include "TinyJsonFinal.h"

const size_t k_max_posible_rtcmem_size = 512;
namespace {
    const size_t k_rtc_persist_mem_start = 65; // Start point of rtc memory
    const size_t k_rtc_persist_memsize_start = 64; // start point of rtc memory's saved data size
};

class IStatePersist 
{
public:
    virtual size_t getPersistedData(void*) = 0;
    virtual void persist(const void*, size_t) = 0;
};

class RTCStatePersist : public IStatePersist
{
public:
    RTCStatePersist(){}

    size_t getPersistedData(void* dataP) override
    {
        size_t size = 0;
        system_rtc_mem_read(k_rtc_persist_memsize_start, &size, sizeof(size));
        system_rtc_mem_read(k_rtc_persist_mem_start, dataP, size);
        return size;
    }

    void persist(const void* dataP, size_t size) override{
        if(k_rtc_persist_mem_start * 4 + size > k_max_posible_rtcmem_size - 4) return;
        if(size % 4 == 0) system_rtc_mem_write(k_rtc_persist_mem_start, dataP, size);
        else{
            uint8_t buckets = size / 4 + 1;
            uint32 dataBuckets[buckets];
            memset(dataBuckets, int(' '), buckets * 4);
            memcpy(dataBuckets, dataP, size);
            system_rtc_mem_write(k_rtc_persist_mem_start, dataBuckets, buckets);
        }
        system_rtc_mem_write(k_rtc_persist_mem_start, dataP, size);
        
        size_t memSize = k_rtc_persist_mem_start * 4 + size;
        system_rtc_mem_write(k_rtc_persist_memsize_start, &memSize, 4);
        Fm("Persist entry: %s\nTotal size: %u\n", (char*)dataP, memSize);
    }
};

#endif
//
//  debug_time_log.hpp
//  Example
//
//  Created by Apple on 17/10/11.
//  Copyright © 2017年 Apple. All rights reserved.
//

#ifndef debug_time_log_hpp
#define debug_time_log_hpp

#include <stdio.h>
#include <stdint.h>
#include <mutex>
#include <atomic>

#define LOGE(...) printf(__VA_ARGS__)

class DebugTimeLog{
public:
    explicit DebugTimeLog(int size);
    
    bool log(const char* method_name, const char* class_name, uint64_t time, uint32_t tid, uint8_t type);
    
    bool flush(const char* path);
    
    static DebugTimeLog* getInstance();
    
private:

    static DebugTimeLog* instance_;

    volatile bool is_flushed_;
    
    volatile bool is_logging_;
    
    void* buffer_;
    
    std::atomic<uint32_t> offset_;
    
    uint32_t size_;
    
    std::mutex lock_;
    
    
};

class ScopeMethodTime{
public:
    ScopeMethodTime(const char* method_name, const char* class_name);
    
    ~ScopeMethodTime();
    
private:
    const char* method_name_;
    const char* class_name_;
    uint32_t tid_;
};

#endif /* debug_time_log_hpp */

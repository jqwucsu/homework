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

class DebugTimeLog{
public:
    explicit DebugTimeLog(int size);
    bool log(const char* msg, uint64_t time, uint32_t tid, uint8_t type);
    bool flush(const char* path);
    
private:
    void* buffer_;
    uint32_t offset_;
};

class ScopeMethodTime{
public:
    ScopeMethodTime(const char* methodName);
    
    ~ScopeMethodTime();
    
private:
    const char* method_name;
};

#endif /* debug_time_log_hpp */

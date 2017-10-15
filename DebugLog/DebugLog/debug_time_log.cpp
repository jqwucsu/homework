//
//  debug_time_log.cpp
//  Example
//
//  Created by Apple on 17/10/11.
//  Copyright © 2017年 Apple. All rights reserved.
//
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>

#include "debug_time_log.h"

DebugTimeLog::DebugTimeLog(int size):offset_(0),size_(size),is_flushed_(false),is_logging_(false){
    buffer_ = malloc(size);
}

DebugTimeLog* t = new DebugTimeLog(10*1024*1024);

DebugTimeLog* DebugTimeLog::getInstance(){
    return t;
}

bool DebugTimeLog::log(const char *msg, const char* class_name, uint64_t time, uint32_t tid, uint8_t type){
    if(is_flushed_){
        return false;
    }
    is_logging_ = true;
    if(offset_ + 1000 > size_){
        is_logging_ = false;
        return false;
    }
    uint32_t msg_size = (uint32_t)strlen(msg);
    uint32_t name_size = (uint32_t)strlen(class_name);
    
    
    int current_offset = offset_.fetch_add(4 + msg_size + 4+ name_size + 8 + 4 + 1);
    
    *((uint32_t*)((char*)buffer_+current_offset)) = msg_size;
    current_offset += 4;
    
    memcpy((char*)buffer_ + current_offset, msg, msg_size);
    current_offset += msg_size;
    
    *((uint32_t*)((char*)buffer_+current_offset)) = name_size;
    current_offset += 4;
    
     memcpy((char*)buffer_ + current_offset, class_name, name_size);
    current_offset += name_size;
    
    *((uint64_t*)((char*)buffer_+current_offset)) = time;
    current_offset += 8;
    
    *((uint32_t*)((char*)buffer_+current_offset)) = tid;
    current_offset += 4;
    
    *((uint8_t*)((char*)buffer_+current_offset)) = type;
    is_logging_ = false;
    
    return true;
}

bool DebugTimeLog::flush(const char *path){
    is_flushed_ = true;
    while(is_logging_){
        usleep(10);
    }
    int current_offset = offset_;
    FILE *file = fopen(path, "w+");
    if(!file){
        LOGE("file open failed , %s", strerror(errno));
        return false;
    }
    uint64_t size = fwrite(buffer_, 1, current_offset, file);
    if(size != current_offset){
        LOGE("file write error, %s" , strerror(errno));
        fclose(file);
    }
    fclose(file);
    LOGE("file flush sucess, path is %s", path);
    return true;
}



ScopeMethodTime::ScopeMethodTime(const char* method_name, const char* class_name){
    uint64_t self = (uint64_t)pthread_self();
    uint32_t tid = (uint32_t)self;
    tid_ = tid;
    method_name_ = method_name;
    class_name_ = class_name;
    struct timeval tv;
    struct timezone tz;
    gettimeofday(&tv, &tz);
    uint64_t utime = tv.tv_sec*1000000 + tv.tv_usec;
    DebugTimeLog::getInstance()->log(method_name, class_name, utime, tid, 0);
}

ScopeMethodTime::~ScopeMethodTime(){
    struct timeval tv;
    struct timezone tz;
    gettimeofday(&tv, &tz);
    uint64_t utime = tv.tv_sec*1000000 + tv.tv_usec;
    DebugTimeLog::getInstance()->log(method_name_, class_name_, utime, tid_, 1);
}


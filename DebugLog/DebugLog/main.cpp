//
//  main.cpp
//  DebugLog
//
//  Created by WuJunqing on 17/10/15.
//  Copyright (c) 2017年 WuJunqing. All rights reserved.
//

#include <stdio.h>
#include "debug_time_log.h"
#include <time.h>
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>


void test2(){
    ScopeMethodTime time(__FUNCTION__, "haha");
    usleep(2000);
}

void test(){
    ScopeMethodTime time(__FUNCTION__, "hehe");
    test2();
    usleep(1000);
    
}

void* run(void* arg){
    int times = 1000;
    while(times > 0){
        test();
        times--;
    }
    printf("run finish !!! \n");
    return NULL;
}



int main(){
    const char* path = "/Users/wujunqing/oat/data.txt";
    
    pthread_t key_1;
    pthread_t key_2;
    pthread_t key_3;
    pthread_t key_4;
    
    pthread_create(&key_1, NULL, run, NULL);
    pthread_create(&key_2, NULL, run, NULL);
    pthread_create(&key_3, NULL, run, NULL);
    pthread_create(&key_4, NULL, run, NULL);
    
    pthread_join(key_1, NULL);
    pthread_join(key_2, NULL);
    pthread_join(key_3, NULL);
    pthread_join(key_4, NULL);
    
    DebugTimeLog::getInstance()->flush(path);
    
    FILE *file = fopen(path, "r");
    if(file == NULL){
        printf("open error , %s \n", strerror(errno));
        return 0;
    }
    int filesize = -1;
    struct stat statbuff;
    if(stat(path, &statbuff) < 0){
        printf("file size get error \n");
    }else{
        filesize = (int)statbuff.st_size;
    }
    printf("\n");
    printf("filesize is %d \n", filesize);
    char buffer[filesize];
    uint64_t read_size = fread(buffer, 1, filesize, file);
    if(read_size != filesize){
        printf("file read failed %s \n", strerror(errno));
        return 0;
    }
    unsigned long read_offset = 0;
    while(read_offset < filesize){
        uint32_t msg_size =  *((uint32_t*)(buffer+read_offset));
        read_offset = read_offset+4;
        char *msg = (char*)malloc(msg_size+1);
        memcpy(msg, buffer+read_offset, msg_size);
        msg[msg_size] = '\0';
        read_offset += msg_size;
        
        uint32_t class_size =  *((uint32_t*)(buffer+read_offset));
        read_offset += 4;
        char *class_name = (char*)malloc(class_size+1);
        memcpy(class_name, buffer+read_offset, class_size);
        class_name[class_size] = '\0';
        read_offset += class_size;
        
        uint64_t time =  *((uint64_t*)(buffer+read_offset));
        read_offset += 8;
        uint32_t tid =  *((uint32_t*)(buffer+read_offset));
        read_offset += 4;
        uint8_t type =  *((uint8_t*)(buffer+read_offset));
        read_offset += 1;
        
        printf("msg: %s , class: %s, time:%lld, tid:%u, type:%u \n", msg, class_name, time, tid, type);
        free(msg);
        
    }
    
    printf("DONE !!!!!!!! \n");
    
}
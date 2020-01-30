/*************************************************************************
   > File Name: /home/sylar/src/util.h
   > Author: Joey/Cao
   > Mail: flyingrhino@163.com
   > Created Time: Thu 30 Jan 2020 07:49:34 PM CST
 ************************************************************************/

#ifndef _SYLAR_UTIL_H
#define _SYLAR_UTIL_H

#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <stdint.h>

namespace sylar {

pid_t GetThreadId();
uint32_t GetFiberId();

}


#endif

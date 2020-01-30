/*************************************************************************
   > File Name: /home/sylar/src/util.cc
   > Author: Joey/Cao
   > Mail: flyingrhino@163.com
   > Created Time: Thu 30 Jan 2020 07:27:20 PM CST
 ************************************************************************/

#include "util.h"

namespace sylar {

pid_t GetThreadId() {
    return syscall(SYS_gettid);
}

uint32_t GetFiberId() {
    return 0;
}

}

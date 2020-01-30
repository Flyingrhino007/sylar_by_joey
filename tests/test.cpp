/*************************************************************************
   > File Name: test.cpp
   > Author: Joey/Cao
   > Mail: flyingrhino@163.com
   > Created Time: Tue 28 Jan 2020 02:10:35 PM CST
 ************************************************************************/

#include "../src/log.h"
//#include "src/util.h"

int main(int argc, char** argv) {
    sylar::Logger::ptr logger(new sylar::Logger);
    logger->addAppender(sylar::LogAppender::ptr(new sylar::StdoutLogAppender));

    sylar::LogEvent::ptr event(new sylar::LogEvent(__FILE__, __LINE__, 0, 1, 2, time(0)));

    logger->log(sylar::LogLevel::DEBUG, event); 
    return 0;
}








/*************************************************************************
   > File Name: test.cpp
   > Author: Joey/Cao
   > Mail: flyingrhino@163.com
   > Created Time: Tue 28 Jan 2020 02:10:35 PM CST
 ************************************************************************/
#include <iostream>
#include <thread>
#include "../src/log.h"
#include "../src/util.h"

int main(int argc, char** argv) {
    sylar::Logger::ptr logger(new sylar::Logger);
    logger->addAppender(sylar::LogAppender::ptr(new sylar::StdoutLogAppender));

    sylar::FileLogAppender::ptr file_appender(new sylar::FileLogAppender("./log.txt"));
    
    sylar::LogFormatter::ptr fmt(new sylar::LogFormatter("%d%T%n"));
    file_appender->setFormatter(fmt);
    
    logger->addAppender(file_appender);


//    sylar::LogEvent::ptr event(new sylar::LogEvent(__FILE__, __LINE__, 0, sylar::GetThreadId(), sylar::GetFiberId(), time(0)));
//    event->getSS() << "Hello Sylar bu joey" << std::endl;

//    logger->log(sylar::LogLevel::DEBUG, event); 
//    std::cout << "Sylar by joey" << std::endl;
    
    SYLAR_LOG_INFO(logger) << "test marco";
    SYLAR_LOG_WARN(logger) << "WARN test";

    SYLAR_LOG_FMT_ERROR(logger, "test_marco fmt error %s", "aa");
    return 0;
}


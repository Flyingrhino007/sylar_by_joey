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
    
    sylar::LogFormatter::ptr fmt(new sylar::LogFormatter("%d%T%p%T%m%n"));
    file_appender->setFormatter(fmt);
    file_appender->setLevel(sylar::LogLevel::ERROR);
    
    logger->addAppender(file_appender);


//    sylar::LogEvent::ptr event(new sylar::LogEvent(__FILE__, __LINE__, 0, sylar::GetThreadId(), sylar::GetFiberId(), time(0)));
//    event->getSS() << "Hello Sylar bu joey" << std::endl;

//    logger->log(sylar::LogLevel::DEBUG, event); 
//    std::cout << "Sylar by joey" << std::endl;
    
    SYLAR_LOG_INFO(logger) << "test marco";
    SYLAR_LOG_WARN(logger) << "WARN test";

    SYLAR_LOG_FMT_ERROR(logger, "test_marco fmt error %s", "aa");

    auto ll = sylar::LoggerMgr::GetInstance()->getLogger("xx");
    SYLAR_LOG_INFO(ll) << "jjjjjjjjjjoey";

    return 0;
}


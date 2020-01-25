#ifndef __SYLAR_LOG_H__
#define __SYLAR_LOG_H__

#include <string>
#include <stdint.h>
#include <memory>
#include <list>
#include <stringstream>
#include <fstream>

namespace sylar {

// Log event
class LogEvent {
public:
    typedef std::shared_ptr<LogEvent> ptr;
    LogEvent();
private:
    const char* m_file = nullptr;   // file name
    int32_t m_line = 0;             // line number
    uint32_t m_elapse = 0;          // milliseconds since program start
    uint32_t m_threadId = 0;        // thread id
    uint32_t m_fiberId = 0;         // coroutine id
    uint64_t m_time;                // timestamp
    std::string m_content;
};

// Log level
class LogLevel {
public:
    enum Level {
        DEBUG = 1,
        INFO = 2,
        WARN = 3,
        ERROR = 4,
        FATAL = 5
    };
};

// Log formatter
class LogFormatter {
public:
    typedef std::shared_ptr<LogFormatter> ptr;

    std::string format(LogEvent::ptr event);
private:

};


// Log output path
class LogAppender {
public:
    typedef std::shared_ptr<LogAppender> ptr;
    virtual ~LogAppender() {}

    virtual void log(LogLevel::Level level, LogEvent::ptr event) = 0;

    void setFormatter(LogFormatter::ptr val) { m_formatter = val;}
    LogFormatter::ptr getFormatter() const { return m_formatter};
protected:
    LogLevel::Level m_level;            // protected 子类可用
    LogFormatter::ptr m_formatter;
};

// Logger
class Logger {
public:
    typedef std::shared_ptr<Logger> ptr;

    Logger(const std::string& name = "root");
    void log(LogLevel::Level level, LogEvent::ptr event);

    void debug(LogEvent::ptr event);
    void info(LogEvent::ptr event);
    void warn(LogEvent::ptr event);
    void error(LogEvent::ptr event);
    void fatal(LogEvent::ptr event);

    void addAppender(LogEvent::ptr appender);
    void delAppender(LogEvent::ptr appender);
    LogLevel::Level getLevel() const {return m_level;}
    void setLevel(LogLevel::Level val) {m_level = val;}
private:
    std::string m_name;         // Logger name
    LogLevel::Level m_level;    // log level
    std::list<LogAppender::ptr> m_appenders;   // Appender set

};

// Appender to console
class StdoutLogAppender : public LogAppender {
public:
    typedef std::shared_ptr<StdoutLogAppender> ptr;
    void log(LogLevel::Level level, LogEvent::ptr event) override;   // override描述从虚基类继承出来的
};

// Appender to file
class FileLogAppender : public LogAppender {
public:
    typedef std::shared_ptr<FileLogAppender> ptr;
    FileLogAppender(const std::string& flename);
    void log(LogLevel::Level level, LogEvent::ptr event) override;
    void reopen();
private:
    std::string m_filename;
    std::ofstream m_filestream;
};



}
#endif

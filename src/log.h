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

    const char* getFile() const { return m_file;}
    int32_t getLine() const { return m_line;}
    uint32_t getElapse() const { return m_elapse;}
    uint32_t getThreadId() const { return m_threadId;}
    uint32_t getFiberId() const { return m_fiberId;}
    uint64_t getTime() const { return m_time;}
    const std::string& getContent() const { return m_content;}

        
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
        UNKNOW = 0,
        DEBUG = 1,
        INFO = 2,
        WARN = 3,
        ERROR = 4,
        FATAL = 5
    };

    static const char* ToString(LogLevel::Level level);
};

// Log formatter
class LogFormatter {
public:
    typedef std::shared_ptr<LogFormatter> ptr;
    LogFormatter(const std::string& pattern);       // 实例化的时候输入pattern

    // %t   %thread_id %m%n
    std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);
    // 日志解析子模块
public:
    class FormatItem {
    public:
        typedef std::shared_ptr<FormatItem> ptr;    // 智能指针
        FormatItem(const std::string& fmt = "") {};
        virtual ~FormatItem() {}
        virtual void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
    };

    void init();                                // 解析pattern
private:
    std::string m_pattern;                      // 输出的结果
    std::vector<FormatItem::ptr> m_items;       // 定义很多不同的子类，每个子类负责输出对应的内容

private:

};


// Log output path
class LogAppender {
public:
    typedef std::shared_ptr<LogAppender> ptr;
    virtual ~LogAppender() {}

    virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;

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

    const std::string& getName() const { return m_name;}
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
    // 重新打开文件，文件打开成功，返回true
    bool reopen();
private:
    std::string m_filename;
    std::ofstream m_filestream;
};



}
#endif

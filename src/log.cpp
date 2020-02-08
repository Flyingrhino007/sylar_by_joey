#include "log.h"
#include <map>          // 关联容器
#include <string>
#include <iostream>
#include <functional>
#include "config.h"

//#include <cstring>

namespace sylar {

class Logger;

const char* LogLevel::ToString(LogLevel::Level level) {
    switch(level) {
#define XX(name) \
    case LogLevel::name: \
        return #name; \
        break;

    XX(DEBUG);
    XX(INFO);
    XX(WARN);
    XX(ERROR);
    XX(FATAL);
#undef XX
    default:
        return "UNKOWN";
    }
    return "UNKOWN";
}

LogEvent::LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level, const char* file, int32_t line, uint32_t elapse
            , uint32_t thread_id, uint32_t fiber_id, uint64_t time)
    :m_file(file)
    ,m_line(line)
    ,m_elapse(elapse)
    ,m_threadId(thread_id)
    ,m_fiberId(fiber_id)
    ,m_time(time) 
    ,m_logger(logger)
    ,m_level(level) {
}

void LogEvent::format(const char* fmt, ...) {
    va_list al;
    va_start(al, fmt);
    format(fmt, al);
    va_end(al);
}

void LogEvent::format(const char* fmt, va_list al) {
    char* buf = nullptr;
    int len = vasprintf(&buf, fmt, al);    // 成功len为buf的长度，失败len = -1
    if(len != -1) {
        m_ss << std::string(buf, len);      // 先写入到类的变量 m_ss 中
        free(buf);                          // 记得要free掉buf
    }
}

LogEventWrap::LogEventWrap(LogEvent::ptr e) 
    :m_event(e) {
}

LogEventWrap::~LogEventWrap() {
    m_event->getLogger()->log(m_event->getLevel(), m_event);
}

std::stringstream& LogEventWrap::getSS() {
    return m_event->getSS();
}

LogFormatter::LogFormatter(const std::string& pattern) 
    : m_pattern(pattern) {
        init();
}

// 返回 string 变量
std::string LogFormatter::format (std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) {
    std::stringstream ss;
    for(auto&i : m_items) {
        i->format(ss, logger, level, event);
    }
    return ss.str();
}


class MessageFormatItem : public  LogFormatter::FormatItem {
public:
    MessageFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getContent(); 
    }
};

class LevelFormatItem : public  LogFormatter::FormatItem {
public:
    LevelFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << LogLevel::ToString(level);
    }
};

class ElapseFormatItem : public  LogFormatter::FormatItem {
public:
    ElapseFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getElapse();
    }
};

class NameFormatItem : public  LogFormatter::FormatItem {
public:
    NameFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getLogger()->getName();
    }
};

class ThreadIdFormatItem : public  LogFormatter::FormatItem {
public:
    ThreadIdFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getThreadId();
    }
};

class FiberIdFormatItem : public  LogFormatter::FormatItem {
public:
    FiberIdFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getFiberId();
    }
};

class DateTimeFormatItem : public  LogFormatter::FormatItem {
public:
    DateTimeFormatItem(const std::string& format = "%Y-%m-%d %H:%M:%S") 
        :m_format(format) {
        if(m_format.empty()) {
            m_format = "%Y-%m-%d %H:%M:%S";
        }
    }

    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        struct tm tm;
        time_t time = event->getTime();
        localtime_r(&time, &tm);
        char buf[64];
        strftime(buf, sizeof(buf), m_format.c_str(), &tm);      // 把时间戳tm以m_format的形式写入到buf
        os << buf;
//        os << event->getTime();
    }
private:
    std::string m_format;
};

class FilenameFormatItem : public  LogFormatter::FormatItem {
public:
    FilenameFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getFile();
    }
};


class LineFormatItem : public  LogFormatter::FormatItem {
public:
    LineFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getLine();
    }
};

class NewLineFormatItem : public  LogFormatter::FormatItem {
public:
    NewLineFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << std::endl;
    }
};

class StringFormatItem : public  LogFormatter::FormatItem {
public:
    StringFormatItem(const std::string& str)
        : m_string(str) {}
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << m_string;
    }
private:
    std::string m_string;
};

class TabFormatItem : public  LogFormatter::FormatItem {
public:
    TabFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << "\t";
    }
private:
    std::string m_string;
};

void LogFormatter::init() {
    // 日志格式的解析，3种格式分别是  %xxx  %xxx{xxx}  %%
    //str, format, type 三元组
    std::vector<std::tuple<std::string, std::string, int> >vec;
    std::string nstr;                       // 存储当前的string
    for(size_t i = 0; i < m_pattern.size(); ++i) {
        if(m_pattern[i] != '%') {           // 如果当前字母不是 %，则放入到 str
            nstr.append(1, m_pattern[i]);   // 1表示添加1次，内容为m_pattern[i]
            continue;                       // 继续下一个字母
        }

        if((i + 1) < m_pattern.size()) {
            if(m_pattern[i + 1] == '%') {
                nstr.append(1, '%');            // 连续2个%，表示其本身就是个%
                continue;
            }
        }

        size_t n = i + 1;
        int fmt_status = 0;             // 记录当前字段的状态
        size_t fmt_begin = 0;           // 记录 { 括号后第一个字母的位置

        std::string str;
        std::string fmt;
        while(n < m_pattern.size()) {
            // 持续检索下一个字母，直到遇到空格
            if(!fmt_status && (!isalpha(m_pattern[n]) && m_pattern[n] != '{' && m_pattern[n] != '}')) {
                // fmt_status为0，且该位置不是字母，不是{，也不是}
                str = m_pattern.substr(i + 1, n - i - 1);
                break;
            }

            if(fmt_status == 0) {
                if(m_pattern[n] == '{') {
                    str = m_pattern.substr(i + 1, n - i - 1);   // str 保存左大括号左侧的字符串
                    fmt_status = 1;                             // 解析格式
                    fmt_begin = n;                              // fmt_begin 记录的是左大括号的位置
                    ++n;
                    continue;
                }
            } else if(fmt_status == 1) {
                if(m_pattern[n] == '}') {
                    fmt = m_pattern.substr(fmt_begin + 1, n - fmt_begin - 1);
                    fmt_status = 0;
                    ++n;
                    break;                    
                }
            }
            ++n;
            if(n == m_pattern.size()) {
                if(str.empty()) {
                    str = m_pattern.substr(i + 1);
                }
            }
        }

        // 根据解析结果存储相应的内容
        if (fmt_status == 0) {
            if(!nstr.empty()) {
                vec.push_back(std::make_tuple(nstr, std::string(), 0));
                nstr.clear();
            }
            vec.push_back(std::make_tuple(str, fmt, 1));
            i = n - 1;              // for 循环要 ++i，所以将i重置为n - 1
        } else if(fmt_status == 1) { 
            // 错误的字段，不作任何事情 
            std::cout << "pattern parse error: " << m_pattern << "-" << m_pattern.substr(i) << std::endl;
            m_error = true;
            vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
        }        
    }
    if(!nstr.empty()) {
        vec.push_back(std::make_tuple(nstr, "", 0));
    }

    static std::map<std::string, std::function<FormatItem::ptr(const std::string& str)> > s_format_items = {
#define XX(str, C) \
        {#str, [](const std::string& fmt) { return FormatItem::ptr(new C(fmt));}}

        XX(m, MessageFormatItem),       // %m -- 消息体
        XX(p, LevelFormatItem),         // %p -- level
        XX(r, ElapseFormatItem),        // %r -- 启动后的时间
        XX(c, NameFormatItem),          // %c -- 日志名称
        XX(t, ThreadIdFormatItem),      // %t -- 线程id
        XX(n, NewLineFormatItem),       // %n -- 换行符
        XX(d, DateTimeFormatItem),      // %d -- 时间
        XX(f, FilenameFormatItem),      // %f -- 文件名
        XX(l, LineFormatItem),          // %l -- 行号
        XX(T, TabFormatItem),           // %T -- Tab
        XX(F, FiberIdFormatItem),       // %F -- 协程id
#undef XX
    };

    for(auto& i : vec) {
        if(std::get<2>(i) == 0) {
            // 对应fmt_status == 0
            // string 类型
            m_items.push_back(FormatItem::ptr(new StringFormatItem(std::get<0>(i))));
        } else {
            auto it = s_format_items.find(std::get<0>(i));
            if (it == s_format_items.end()) {
                m_items.push_back(FormatItem::ptr(new StringFormatItem("<<error_format %" + std::get<0>(i) + ">>")));
                m_error = true;
                std::cout << "****** Log parse error ******" << std::endl;
            } else {
                m_items.push_back(it->second(std::get<1>(i)));
            }
        }
//        std::cout << "(" << std::get<0>(i) << ") - (" << std::get<1>(i) << ") - (" << std::get<2>(i) << ")" << std::endl; 
    }
//    std::cout << m_items.size() << std::endl;
}

Logger::Logger(const std::string& name)
    : m_name(name)
    ,m_level(LogLevel::DEBUG) {
        // 日志级别初始化默认为最低级
        m_formatter.reset(new LogFormatter("%d{%Y-%m-%d %H:%M:%S}%T%t%T%F%T[%p]%T[%c]%T%f:%l%T%m%T%n"));
}

void Logger::setFormatter(LogFormatter::ptr val) {
    m_formatter = val;
}

void Logger::setFormatter(const std::string& val) {
    sylar::LogFormatter::ptr new_val(new sylar::LogFormatter(val));
    if(new_val->isError()) {
        // 防止出错
        return;
    }
    m_formatter = new_val;
}

LogFormatter::ptr Logger::getFormatter() {

}


void StdoutLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) {
    if (level >= m_level) {
        // 如果此事件的级别高于等于默认的m_level，才输出
        // 输出到控制台打印
        // 调用父类LogAppender的protected类成员LogFormatter m_formatter
        std::cout << m_formatter->format(logger, level, event);
    }
}

FileLogAppender::FileLogAppender(const std::string& filename) 
    : m_filename(filename) {
        reopen();
}

void FileLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) {
    if(level >= m_level) {
        // 将event事件按照m_formatter的格式，输出到string类型的m_filestream中
        m_filestream << m_formatter->format(logger, level, event);
    }
}

bool FileLogAppender::reopen() {
    if (m_filestream) {
        m_filestream.close();
    } 
    m_filestream.open(m_filename, std::ios::app);
    return !!m_filestream;      // ??????????????????????
}

void Logger::addAppender(LogAppender::ptr appender) {
    if(!appender->getFormatter()) {
        // 如果当前appender的格式为空，就把Logger自己的格式指针给appender
        appender->setFormatter(m_formatter);
    }
    m_appenders.push_back(appender);
}

void Logger::delAppender(LogAppender::ptr appender) {
    for(auto it = m_appenders.begin();
            it != m_appenders.end(); ++it) {
        if (*it == appender) {
            m_appenders.erase(it);
            break;
        }
    }
}

void Logger::clearAppenders() {
    m_appenders.clear();
}

void Logger::log(LogLevel::Level level, LogEvent::ptr event) {
    if (level >= m_level) {
        auto self = shared_from_this();
        if(!m_appenders.empty()) {
            for(auto& i : m_appenders) {
                i->log(self, level, event);     // 将事件进行登记
            }
        } else if(m_root) {
            m_root->log(level, event);
        }
    }
}

void Logger::debug(LogEvent::ptr event) {
    log(LogLevel::DEBUG, event);
}
    
void Logger::info(LogEvent::ptr event) {
    log(LogLevel::INFO, event);
}

void Logger::warn(LogEvent::ptr event) {
    log(LogLevel::WARN, event);
}

void Logger::error(LogEvent::ptr event) {
    log(LogLevel::ERROR, event);
}

void Logger::fatal(LogEvent::ptr event) {
    log(LogLevel::FATAL, event);
}


LoggerManager::LoggerManager(){
    m_root.reset(new Logger);
    m_root->addAppender(LogAppender::ptr(new StdoutLogAppender));

    init();
}

Logger::ptr LoggerManager::getLogger(const std::string& name) {
    auto it = m_logger.find(name);
    if(it != m_logger.end()) {
        return it->second;
    }

    Logger::ptr logger(new Logger(name));
    logger->m_root = m_root;
    m_loggers[name] = logger;
    return logger;
}

struct LogAppenderDefine {
    int type = 0;   // 1 File, 2 Stdout
    LogLevel::Level level = LogLevel::UNKOWN;
    std::string formatter;
    std::string file;

    bool operator==(const LogAppenderDefine& oth) const {
        return type == oth.type
            && level == oth.type
            && formatter == oth.formatter
            && file == oth.file;
    }
};

struct LogDefine {
    std::string name;
    LogLevel::Level level = LogLevel::UNKOWN;
    std::string formatter;
    std::vector<LogAppenderDefine> appenders;

    bool operator==(const LogDefine& oth) const {
        return name == oth.name
            && level == oth.level
            && formatter == oth.formatter
            && appenders == oth.appenders;
    }
    
    bool operator<(const LogDefine& oth) const {
        return name < oth.name;
    }
};

// 用set而不用vector，去重好一些
// 就需要重载operator < 函数
sylar::ConfigVar<std::set<LogDefine> > g_log_defines = 
    sylar::Config::Lookup("logs", std::set<LogDefine>(), "logs config");

// main 函数之前执行，全局对象！全局对象在main函数之前构造，其在构建时会调用其构造函数
// 可在其构造函数内加一些事件

struct LogIniter {
    LogIniter() {
        g_log_defines->addListener(0xF1E231, [](const std::set<LogDefine>& old_Value,
                    const std::set<LogDefine>& new_value){
                // 新增日志
            for(auto& i : new_value) {
                auto it = old_value.find(i);
                if(it == old_value.end()) {
                    // 新增logge
                sylar::Logger::ptr logger (new sylar::Logger(i.name));
                logger->setLevel(i.level);
                if(!i.formatter.empty()) {
                    logger->
                }
                logger->
                } else {
                    if(!(i == *it)) {
                        // 修改的logger，在old_value里有，但是和new_value里对应的不同，所以要修改
                    }
                }
            }
            for(auto& i : old_value) {
                auto it = new_value.find(i);
                if(it == new_value.end()) {
                    // 删除logger，不是真正的删除，关闭文件或者把日志级别设置得很高
                    auto logger =  SYLAR_LOG_NAME(i.name);
                    logger->setLevel((LogLevel::Level)100);
                    logger->clearAppenders();
                }
            }
        });
    }
};

void LoggerManager::init() {

}

}


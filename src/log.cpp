#include "log.h"
#include <map>          // 关联容器

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

class MessageFormatItem : public  LogFormatter::FormatItem {
public:
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getContent(); 
    }
};

class LevelFormatItem : public  LogFormatter::FormatItem {
public:
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << LogLevel::ToString(level);
    }
};

class ElapseFormatItem : public  LogFormatter::FormatItem {
public:
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << LogLevel::getElapse();
    }
};

class NameFormatItem : public  LogFormatter::FormatItem {
public:
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << logger->getName();
    }
};

class ThreadIdFormatItem : public  LogFormatter::FormatItem {
public:
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getThreadId();
    }
};

class FiberIdFormatItem : public  LogFormatter::FormatItem {
public:
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getFiberId();
    }
};

class DateTimeFormatItem : public  LogFormatter::FormatItem {
public:
    DateTimeFormatItem(const std::string& format = "%Y:%m:%d %H:%M:%S") 
        :m_format(format) {
    }

    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getTime();
    }
};

class FilenameFormatItem : public  LogFormatter::FormatItem {
public:
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getFile();
    }
};


class LineFormatItem : public  LogFormatter::FormatItem {
public:
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getLine();
    }
};

class NewLineFormatItem : public  LogFormatter::FormatItem {
public:
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << std::endl;
    }
};

class StringFormatItem : public  LogFormatter::FormatItem {
public:
    StringFormatItem(const syd::string& str)
        : FormatItem(str), m_string(str) {}
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << m_string;
    }
private:
    std::string m_string;
};

Logger::Logger(const std::string& name)
    : m_name(name) {
}

void Logger::addAppender(LogEvent::ptr appender) {
    m_appenders.push_back(appender);
}

void Logger::delAppender(LogEvent::ptr appender) {
    for(auto it = m_appenders.begin();
            it != m_appenders.end(); ++it) {
        if (*it == appender) {
            m_appenders.erase(it);
            break;
        }
    }
}


void Logger::log(LogLevel::Level level, LogEvent::ptr event) {
    if (level >= m_level) {
        for(auto& i : m_level) {
            i->log(level, event);
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

FileLogAppender::FileLogAppender(const std::string filename) 
    : m_filename(filename) {
}

void FileLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) {
    if(level >= m_level) {
        m_filestream << m_formatter.format(logger, level, event);
    }
}

bool FileLogAppender::reopen() {
    if (m_filestream) {
        m_filestream.close();
    } 
    m_filestream.open(m_filename);
    return !!m_filestream;      // ??????????????????????
}

void StdoutLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) {
    if (level >= m_level) {
        std::cout << m_formatter.format(logger, level, event);
    }
}

LogFormatter::LogFormatter(const std::string& pattern) 
    : m_pattern(pattern) {
}

std::string LogFormatter::format (std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) {
    std::stringstream ss;
    for(auto&i : m_items) {
        i->format(ss, logger, level, event);
    }
    return ss.str();
}

void LogFormatter::init() {
    // 日志格式的解析，3种格式分别是  %xxx  %xxx{xxx}  %%
    //str, format, type 三元组
    std::vector<std::tuple<std::string, std::string, int> >vec;
    std::string nstr;                    // 存储当前的string
    size_t last_pos = 0;
    for(size_t i = 0; i< m_pattern.size(); ++i) {
        if(m_pattern[i] != '%') {       // 如果当前字母bushi %，则放入到 str
            nstr.append(1, m_pattern[i]);
            continue;                   // 继续下一个字母
        }

        if((i + 1) < m_pattern.size()) {
            if(m_pattern[i + 1] == '%') {
                nstr.append(1. '%');    // 连续2个%，表示其本身就是个%
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
            if(isspace(m_pattern[n])) {
                // 空格表示中断，退出
                break;
            }
            if(fmt_status == 0) {
                if(m_pattern[n] == '{') {
                //
                    str = m_pattern.substr(i + 1, n - i - 1);
                    fmt_status = 1;     // 解析格式
                    ++n;
                    fmt_begin = n;
                    continue;
                }
            }
            if(fmt_status == 1) {
                if(m_pattern[n] == '}') {
                    fmt = m_pattern.substr(fmt_begin + 1, n - fmt_begin - 1);
                    fmt_status = 2;     // 解析结束
                    ++n;
                    break;
                }
            }
        }

        // 根据解析结果存储相应的内容
        if (fmt_status == 0) {
            if(!nstr.empty()) {
                vec.push_back(std::make_tuple(nstr, "", 0));
            }
            str = m_pattern.substr(i + 1, n -i -1);     // 正常的
            vec.push_back(std::make_tuple(str, fmt, 1));
            i = n;
        } else if(fmt_status == 1) { 
            // 错误的字段，不作任何事情 
            std::cout << "pattern parse error: " << m_pattern << "-" << m_pattern.substr(i) << std::endl;
            vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
        } else if(fmt_status == 2) {
            if(!nstr.empty()) {
                vec.push_back(std::make_tuple(nstr, "", 0));
            }
            vec.push_back(std::make_tuple(str, fmt, 1));
            i = n;
        }
    }
    if(!nstr.empty()) {
        vec.push_back(std::make_tuple(nstr, "", 0));
    }

    static std::map<std::string, std::function<FormatItem::ptr(const std::string& str)> > s_format_items = {
#define XX(str, C) \
        {str, [](const std::string& fmt) { return FormatItem::ptr(new C(fmt));}}

        XX(m, MessageFormatItem);
        XX(p, LevelFormatItem);
        XX(r, ElapseFormatItem);
        XX(c, NameFormatItem);
        XX(t, ThreadIdFormatItem);
        XX(n, NewLineFormatItem);
        XX(d, DateTimeFormatItem);
        XX(f, FilenameFormatItem);
        XX(l, LineFormatItem);
#undef XX
    };

    for(auto& i : vec) {
        if(std::get<2>(i) == 0) {
            // string 类型
            m_items.push_back(FormatItem::ptr(new StringFormatItem(std::get<0>(i))));
        } else {
            auto it = s_format_items.find(std::get<0>(i));
            if (it == s_format_items.end()) {
                m_items.push_back(FormatItem::ptr(new StringFormatItem("<<error_format>>" + std::get<0>(i) + ">>")));
            } else {
                m_items.push_back(it->second(std::get<1>(i)));
            }
        }

        std::cout << std::get<0>(i) << " - " << std::get<1>(i) << " - " << std::get<2>(i) << std::endl; 
    }
    // %m -- 消息体
    // %p -- level
    // %r -- 启动后的时间
    // %c -- 日志名称
    // %t -- 线程id
    // %n -- 回车换行
    // %d -- 时间
    // %f -- 文件名
    // %l -- 行号
}


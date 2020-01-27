#include "log.h"

namespace sylar {

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

void FileLogAppender::log(LogLevel::Level level, LogEvent::ptr event) {
    if(level >= m_level) {
        m_filestream << m_formatter.format(event);
    }
}

bool FileLogAppender::reopen() {
    if (m_filestream) {
        m_filestream.close();
    } 
    m_filestream.open(m_filename);
    return !!m_filestream;      // ??????????????????????
}

void StdoutLogAppender::log(LogLevel::Level level, LogEvent::ptr event) {
    if (level >= m_level) {
        std::cout << m_formatter.format(event);
    }
}

LogFormatter::LogFormatter(const std::string& pattern) 
    : m_pattern(pattern) {
}

void LogFormatter::format(std::ostream& os, LogEvent::ptr event) {
    std::stringstream ss;
    for(auto&i : m_items) {
        i->format(ss, event);
    }
    return ss.str();
}

void LogFormatter::init() {
    //str, format, type
    std::vector<std::tuple<std::string, std::string, int> >vec;
    std::string str;
    size_t last_pos = 0;
    for(size_t i = 0; i< m_pattern.size(); ++i) {
        if(m_pattern[i] != '%') {
            str.append(1, m_pattern[i]);
            continue;
        }
        size_t n = i + 1;
        int fmt_status = 0;
        while(n < m_pattern.size()) {
            if(isspace(m_pattern[n])) {
                break;
            }
            if(m_pattern[n] == '{') {
            }
            if((i + 1) < m_pattern.size(); ++i) {


}

}

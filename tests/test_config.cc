/*************************************************************************
   > File Name: ../tests/test_config.cc
   > Author: Joey/Cao
   > Mail: flyingrhino@163.com
   > Created Time: Fri 31 Jan 2020 05:39:15 PM CST
 ************************************************************************/

#include "../src/log.h"
#include "../src/config.h"

sylar::ConfigVar<int>::ptr g_int_value_config = 
    sylar::Config::Lookup("system.port", (int)8080, "system port");

sylar::ConfigVar<float>::ptr g_float_value_config = 
    sylar::Config::Lookup("system.float", (float)23.456, "system float");

int main(int argc, char** argv) {

    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << g_int_value_config->getValue();
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << g_float_value_config->toString();

    return 0;
}

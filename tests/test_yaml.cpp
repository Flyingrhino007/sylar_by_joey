/*************************************************************************
   > File Name: test_yaml.cc
   > Author: Joey/Cao
   > Mail: flyingrhino@163.com
   > Created Time: Mon 03 Feb 2020 02:18:58 PM CST
 ************************************************************************/
#include "yaml-cpp/yaml.h"
#include "../src/config.h"
#include "../src/log.h"

void print_yaml(const YAML::Node& node, int level) {
    if(node.IsScalar()) {
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << std::string(level * 4, ' ')
            << node.Scalar() << " - " << node.Type() << " - " << level;
    } else if(node.IsNull()) {
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << std::string(level * 4, ' ')
            << "NULL - " << node.Type() << " - " << level;
    } else if(node.IsMap()) {
        for(auto it = node.begin();
                it != node.end(); ++it) {
            SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << std::string(level * 4, ' ')
                << it->first << " - " << it->second.Type() << " - " << level;
            print_yaml(it->second, level + 1);
        }
    } else if(node.IsSequence()) {
        for(size_t i = 0; i < node.size(); ++i) {
            SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << std::string(level * 4, ' ')
                << i << " - " << node[i].Type() << " - " << level;
            print_yaml(node[i], level + 1);                                                                                        }
    }
}

int main(int argc, char** argv) {

    YAML::Node root  = YAML::LoadFile("/home/sylar/bin/conf/log.yml");

    print_yaml(root, 0);
    return 0;
}

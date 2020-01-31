/*************************************************************************
   > File Name: test.cc
   > Author: Joey/Cao
   > Mail: flyingrhino@163.com
   > Created Time: Fri 31 Jan 2020 01:20:35 PM CST
 ************************************************************************/

#include <fstream>
#include <iostream>

int main(int argc, char** argv) {
    std::ofstream m_filestream;
    m_filestream.open("./joey.txt");

    m_filestream << "joey tasdasdasdest\n" << "line eight\n\n\n\n\n\n*****";

    m_filestream.close();
    return 0;
}

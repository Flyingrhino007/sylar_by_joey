/*************************************************************************
   > File Name: ../src/singleton.h
   > Author: Joey/Cao
   > Mail: flyingrhino@163.com
   > Created Time: Fri 31 Jan 2020 02:28:14 PM CST
 ************************************************************************/

#ifndef _SINGLETON_H
#define _SINGLETON_H

namespace sylar {

template<class T, class X = void, int N = 0>
class Singleton {
public:
    static T* GetInstance() {
        static T v;
        return &v;
    }
};

template<class T, class X = void, int N = 0>
class SingletonPtr {
public:
    static std::shared_ptr<T> GetInstance() {
        static std::shared_ptr<T> v(new T);
        return v;
    }
};

}

#endif

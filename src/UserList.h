#ifndef USERLIST_H
#define USERLIST_H

#include <jsoncpp/json/json.h>
#include <unordered_map>

class UserList {
public:
    // 局部静态特性的方式实现单实例
    static UserList *GetInstance();

private:
    UserList();

    ~UserList();
};

#endif
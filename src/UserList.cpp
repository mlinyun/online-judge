#include "UserList.h"
#include "MongoDataBase.h"
#include "RedisDataBase.h"
#include <iostream>

using namespace std;

UserList *UserList::GetInstance() {
    static UserList userlist;
    return &userlist;
}

UserList::UserList() {
}

UserList::~UserList() {
}
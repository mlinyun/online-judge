#include "db/redis_database.h"

using namespace std;

// 局部静态特性的方式实现单实例模式
ReDB *ReDB::GetInstance() {
    static ReDB redis_database;
    return &redis_database;
}

ReDB::ReDB() {
    // 构造函数实现
}

ReDB::~ReDB() {
    // 析构函数实现
}
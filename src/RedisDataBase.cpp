#include "RedisDataBase.h"

using namespace std;

ReDB *ReDB::GetInstance() {
    static ReDB redb;
    return &redb;
}

ReDB::ReDB() {
}

ReDB::~ReDB() {
}

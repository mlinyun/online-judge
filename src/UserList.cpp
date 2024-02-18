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

Json::Value UserList::RegisterUser(Json::Value &registerjson) {
    return MoDB::GetInstance()->RegisterUser(registerjson);
}

Json::Value UserList::LoginUser(Json::Value &loginjson) {
    return MoDB::GetInstance()->LoginUser(loginjson);
}

bool UserList::UpdateUserProblemInfo(Json::Value &updatejson) {
    return MoDB::GetInstance()->UpdateUserProblemInfo(updatejson);
}

Json::Value UserList::SelectUserRank(Json::Value &queryjson) {
    return MoDB::GetInstance()->SelectUserRank(queryjson);
}

Json::Value UserList::SelectUserInfo(Json::Value &queryjson) {
    return MoDB::GetInstance()->SelectUserInfo(queryjson);
}

Json::Value UserList::UpdateUserInfo(Json::Value &updatejson) {
    return MoDB::GetInstance()->UpdateUserInfo(updatejson);
}

Json::Value UserList::SelectUserUpdateInfo(Json::Value &queryjson) {
    return MoDB::GetInstance()->SelectUserUpdateInfo(queryjson);
}

Json::Value UserList::SelectUserSetInfo(Json::Value &queryjson) {
    return MoDB::GetInstance()->SelectUserSetInfo(queryjson);
}

Json::Value UserList::DeleteUser(Json::Value &deletejson) {
    return MoDB::GetInstance()->DeleteUser(deletejson);
}
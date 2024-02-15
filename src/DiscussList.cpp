#include "DiscussList.h"
#include "MongoDataBase.h"

DiscussList *DiscussList::GetInstance() {
    static DiscussList disscusslist;
    return &disscusslist;
}

DiscussList::DiscussList() {
}

DiscussList::~DiscussList() {
}
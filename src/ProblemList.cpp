#include "ProblemList.h"
#include "MongoDataBase.h"
#include "RedisDataBase.h"
#include <iostream>
#include <fstream>
#include <unistd.h>

using namespace std;

ProblemList *ProblemList::GetInstance() {
    static ProblemList problemlist;
    return &problemlist;
}

ProblemList::ProblemList() {
}

ProblemList::~ProblemList() {
}
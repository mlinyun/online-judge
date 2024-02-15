#include "SolutionList.h"
#include "MongoDataBase.h"

// 局部静态特性的方式实现单实例
SolutionList *SolutionList::GetInstance() {
    static SolutionList solutionlist;
    return &solutionlist;
}

SolutionList::SolutionList() {
}

SolutionList::~SolutionList() {
}
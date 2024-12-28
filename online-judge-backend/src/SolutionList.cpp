#include "SolutionList.h"
#include "MongoDataBase.h"

// 局部静态特性的方式实现单实例
SolutionList *SolutionList::GetInstance()
{
    static SolutionList solutionlist;
    return &solutionlist;
}

// 添加题解
Json::Value SolutionList::InsertSolution(Json::Value &insertjson)
{
    return MoDB::GetInstance()->InsertSolution(insertjson);
}

// 分页查询题解
Json::Value SolutionList::SelectSolutionList(Json::Value &queryjson)
{
    return MoDB::GetInstance()->SelectSolutionList(queryjson);
}

// 管理员查询
Json::Value SolutionList::SelectSolutionListByAdmin(Json::Value &queryjson)
{
    return MoDB::GetInstance()->SelectSolutionListByAdmin(queryjson);
}

// 查询题解的详细信息，主要是编辑时的查询
Json::Value SolutionList::SelectSolutionByEdit(Json::Value &queryjson)
{
    return MoDB::GetInstance()->SelectSolutionByEdit(queryjson);
}

// 查询题解的内容，并且将其浏览量加一
Json::Value SolutionList::SelectSolution(Json::Value &queryjson)
{
    return MoDB::GetInstance()->SelectSolution(queryjson);
}

// 修改评论数的数量
bool SolutionList::UpdateSolutionComments(Json::Value &updatejson)
{
    return MoDB::GetInstance()->UpdateSolutionComments(updatejson);
}

// 更新题解
Json::Value SolutionList::UpdateSolution(Json::Value &updatejson)
{
    return MoDB::GetInstance()->UpdateSolution(updatejson);
}

// 删除题解
Json::Value SolutionList::DeleteSolution(Json::Value &deletejson)
{
    return MoDB::GetInstance()->DeleteSolution(deletejson);
}

SolutionList::SolutionList()
{
}

SolutionList::~SolutionList()
{
}
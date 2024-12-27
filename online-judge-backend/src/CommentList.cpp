#include "CommentList.h"
#include "MongoDataBase.h"

CommentList *CommentList::GetInstance()
{
    static CommentList commentlist;
    return &commentlist;
}

CommentList::CommentList()
{
}

CommentList::~CommentList()
{
}
#ifndef REDISDATABASE_H
#define REDISDATABASE_H

class ReDB {
public:
    // 局部静态特性的方式实现单实例
    static ReDB *GetInstance();

private:
    ReDB();

    ~ReDB();
};

#endif
#ifndef REDIS_DATABASE_H
#define REDIS_DATABASE_H

/**
 * Redis 数据库操作类头文件
 */
class ReDB {
private:
    ReDB();

    ~ReDB();

public:
    // 局部静态特性的方式实现单实例模式
    static ReDB *GetInstance();
};

#endif  // REDIS_DATABASE_H
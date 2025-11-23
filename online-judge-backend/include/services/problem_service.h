#ifndef PROBLEM_SERVICE_H
#define PROBLEM_SERVICE_H

/**
 * 题目服务类头文件
 */
class ProblemService {
private:
    ProblemService();

    ~ProblemService();

public:
    // 局部静态特性的方式实现单实例模式
    static ProblemService *GetInstance();
};

#endif  // PROBLEM_SERVICE_H
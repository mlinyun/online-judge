#ifndef SOLUTION_SERVICE_H
#define SOLUTION_SERVICE_H

/**
 * 题解服务类头文件
 */
class SolutionService {
private:
    SolutionService();

    ~SolutionService();

public:
    // 局部静态特性的方式实现单实例模式
    static SolutionService *GetInstance();
};

#endif  // SOLUTION_SERVICE_H
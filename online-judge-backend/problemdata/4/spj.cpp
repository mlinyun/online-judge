#include <stdio.h>
#include <fstream>
using namespace std;

int main(int argc, char *argv[])
{
    FILE *f_out = fopen(argv[2], "r");  // 测试输出
    FILE *f_user = fopen(argv[3], "r"); // 用户输出
    int ret = 0;                        // AC=0, WA=1

    int a, b;
    // 获取输入输出
    fscanf(f_out, "%d", &a);
    fscanf(f_user, "%d", &b);
    // 比较答案
    if (a == b)
        ret = 0;
    else
        ret = 1;

    fclose(f_out);
    fclose(f_user);

    return ret; // 返回结果，返回值为0时，答案正确，为1时，答案错误
}
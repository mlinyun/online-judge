#include<stdio.h>
#include <fstream>

using namespace std;

int main(int argc, char *argv[]) {
    FILE *f_in = fopen(argv[1], "r"); //测试输入
    FILE *f_out = fopen(argv[2], "r"); //测试输出
    FILE *f_user = fopen(argv[3], "r"); //用户输出
    int ret = 0; //AC=0, WA=1, 其他均为 System Error

    // 以下是一个a+b的例子

    int a, b, c, d;
    // 获取输入输出
    fscanf(f_in, "%d %d", &a, &b);
    fscanf(f_out, "%d", &c);
    fscanf(f_user, "%d", &d);
    // 比较答案
    if (c == d)
        ret = 0;
    else
        ret = 1;

    fclose(f_in);
    fclose(f_out);
    fclose(f_user);

    return ret;//返回结果，返回值为0时，答案正确，为1时，答案错误，返回值为其他时，会报System Error
}
#include <fstream>
#include <iostream>
#include <string>
#include <cctype>

using namespace std;

int main(int argc, char *argv[]) {
    ifstream f_out(argv[2]);
    ifstream f_user(argv[3]);

    string s1, s2;
    getline(f_out, s1);
    getline(f_user, s2);

    int ret = 0;

    /*****spj代码区域*******/
    // 以下是一个a+b的例子

    if (s1.size() != s2.size()) {
        ret = 1;
    } else {
        for (size_t i = 0; i < s1.size(); i++) {
            if (tolower(s1[i]) != tolower(s2[i])) {
                ret = 1;
            }
        }
    }

    /*****spj-end********/
    f_out.close();
    f_user.close();

    return ret; // 返回结果，返回值为0时，答案正确，为1时，答案错误，返回值为其他时，会报System Error
}
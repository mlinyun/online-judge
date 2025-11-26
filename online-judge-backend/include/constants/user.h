#ifndef USER_H
#define USER_H

namespace constants {
namespace user {
// 用户权限常量
constexpr int USER_AUTHORITY_GUEST = 1;          // 游客权限
constexpr int USER_AUTHORITY_ORDINARY = 3;       // 普通用户权限
constexpr int USER_AUTHORITY_ADMINISTRATOR = 5;  // 管理员权限
}  // namespace user
}  // namespace constants

#endif  // USER_H
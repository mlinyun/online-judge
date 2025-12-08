# Axios 封装使用指南

## 📋 概述

本项目使用企业级的 Axios 封装，提供统一的请求/响应处理、错误处理、消息提示等功能。

## 🎯 主要特性

- ✅ 统一的响应结构处理
- ✅ 自动 Token 管理
- ✅ 大整数精度处理（int64）
- ✅ 完善的错误处理与消息提示
- ✅ TypeScript 类型安全
- ✅ 支持请求/响应拦截
- ✅ 开发环境请求日志

## 📦 响应结构

### 后端统一响应格式

```typescript
{
  "success": true,        // 是否成功
  "code": 0,              // 状态码（0 表示成功）
  "message": "操作成功",  // 说明消息
  "data": { ... }         // 响应数据（可选）
}
```

### 前端类型定义

```typescript
interface ApiResponse<T = unknown> {
    success: boolean;
    code: number;
    message: string;
    data?: T;
}
```

## 🔧 基础用法

### 1. GET 请求

```typescript
import { get } from "@/http";
import type { ApiResponse } from "@/types/common/response";

// 简单查询
const getUserInfo = async (userId: string) => {
    const response = await get<UserInfo>("/user/info", { UserId: userId });
    
    // response.data 的类型是 ApiResponse<UserInfo>
    if (response.data.success) {
        const userInfo = response.data.data; // UserInfo 类型
        console.log(userInfo);
    }
};
```

### 2. POST 请求

```typescript
import { post } from "@/http";

// 提交数据
const login = async (data: LoginParams) => {
    const response = await post<LoginResult>("/user/login", data, {
        showSuccessMessage: true,
        successMessage: "登录成功！"
    });
    
    if (response.data.success) {
        const result = response.data.data; // LoginResult 类型
        // 处理登录结果
    }
};
```

### 3. DELETE 请求

```typescript
import { del } from "@/http";

// 删除数据
const deleteUser = async (userId: string) => {
    const response = await del("/user/delete", { UserId: userId }, {
        showSuccessMessage: true,
        successMessage: "删除成功！"
    });
    
    return response.data.success;
};
```

### 4. PUT/PATCH 请求

```typescript
import { put, patch } from "@/http";

// 更新数据
const updateUser = async (data: UpdateUserParams) => {
    const response = await put<UpdateResult>("/user/update", data);
    return response.data;
};
```

## ⚙️ 请求配置选项

```typescript
interface RequestConfig {
    /** 是否显示错误提示，默认 true */
    showErrorMessage?: boolean;
    
    /** 是否显示成功提示，默认 false */
    showSuccessMessage?: boolean;
    
    /** 自定义成功提示信息 */
    successMessage?: string;
    
    /** 是否显示 Loading 加载状态，默认 false */
    showLoading?: boolean;
}
```

### 使用示例

```typescript
// 显示成功提示
await post("/user/update", data, {
    showSuccessMessage: true,
    successMessage: "更新成功！"
});

// 不显示错误提示（静默失败）
await get("/user/info", { id: "123" }, {
    showErrorMessage: false
});

// 同时配置多个选项
await post("/problem/submit", code, {
    showSuccessMessage: true,
    successMessage: "提交成功！",
    showLoading: true
});
```

## 🎨 API 接口定义规范

### 推荐的接口定义方式

```typescript
// src/api/user.ts
import { get, post } from "@/http";
import type { AxiosResponse } from "http";
import type { ApiResponse } from "@/types/common/response";

/**
 * 用户登录
 * @param data 登录参数
 * @returns 登录响应
 */
export const userLogin = (
    data: Api.User.UserLoginParams
): Promise<AxiosResponse<ApiResponse<Api.User.UserLoginResult>>> => {
    return post<Api.User.UserLoginResult>("/user/login", data, {
        showSuccessMessage: true,
        successMessage: "登录成功！"
    });
};

/**
 * 查询用户信息
 * @param userId 用户 ID
 * @returns 用户信息
 */
export const selectUserInfo = (
    userId: string
): Promise<AxiosResponse<ApiResponse<Api.User.SelectUserInfoResult>>> => {
    return get<Api.User.SelectUserInfoResult>("/user/info", { UserId: userId });
};
```

### 在组件中使用

```typescript
import { userLogin, selectUserInfo } from "@/api/user";

// 登录
const handleLogin = async () => {
    try {
        const response = await userLogin({ Account: "test", PassWord: "123456" });
        
        if (response.data.success) {
            const loginData = response.data.data;
            // 保存 Token
            localStorage.setItem("token", loginData.Token);
            // 跳转页面
            router.push("/home");
        }
    } catch (error) {
        // 错误已经在拦截器中处理并提示
        console.error("登录失败", error);
    }
};

// 查询用户
const loadUserInfo = async (userId: string) => {
    try {
        const response = await selectUserInfo(userId);
        
        if (response.data.success) {
            userInfo.value = response.data.data;
        }
    } catch (error) {
        console.error("查询失败", error);
    }
};
```

## 🚨 错误处理

### 自动错误处理

默认情况下，所有请求错误都会自动显示错误提示消息：

- **HTTP 错误**：401/403/404/500 等
- **业务错误**：根据后端返回的 `code` 和 `message`
- **网络错误**：超时、连接失败等

### 错误码说明

```typescript
export enum BusinessErrorCode {
    SUCCESS = 0,                    // 成功
    
    // 通用错误 (1xxx)
    UNKNOWN_ERROR = 1000,           // 未知错误
    BAD_REQUEST = 1001,             // 请求参数错误
    UNAUTHORIZED = 1002,            // 未授权
    FORBIDDEN = 1003,               // 无权限
    NOT_FOUND = 1004,               // 资源不存在
    INTERNAL_ERROR = 1005,          // 服务器内部错误
    DATABASE_ERROR = 1006,          // 数据库错误
    RATE_LIMIT = 1007,              // 请求频率过高
    
    // 用户模块错误 (2xxx)
    USER_ACCOUNT_EXISTS = 2001,     // 账户已存在
    USER_NICKNAME_EXISTS = 2002,    // 昵称已存在
    USER_LOGIN_FAILED = 2003,       // 登录失败
    USER_NOT_FOUND = 2004,          // 用户不存在
    USER_TOKEN_INVALID = 2007,      // Token 无效
    
    // ... 其他模块错误码
}
```

### 特殊错误处理

某些错误会触发特殊行为：

- **UNAUTHORIZED / USER_TOKEN_INVALID**：自动清除 Token 并跳转到登录页
- **FORBIDDEN**：显示权限不足提示
- **RATE_LIMIT**：显示警告提示（黄色）

### 手动错误处理

```typescript
// 方式1：通过 success 判断
const response = await get<UserInfo>("/user/info", { id: "123" });
if (!response.data.success) {
    // 手动处理错误
    console.error("错误码:", response.data.code);
    console.error("错误信息:", response.data.message);
}

// 方式2：使用 try-catch
try {
    const response = await post("/user/update", data);
    // 处理成功逻辑
} catch (error) {
    // 处理异常（网络错误、HTTP 错误等）
    console.error(error);
}
```

## 🔐 Token 管理

### 自动 Token 处理

所有请求会自动携带 Token（如果存在）：

```typescript
// 请求头会自动添加
headers: {
    "Authorization": "your-token",
    "Token": "your-token"  // 兼容后端
}
```

### Token 操作方法

```typescript
import { setToken, removeToken } from "@/http";

// 设置 Token（登录成功后）
setToken("your-token-string");

// 移除 Token（登出时）
removeToken();
```

### Token 失效处理

当 Token 失效时，会自动：

1. 显示错误提示："登录已过期，请重新登录"
2. 清除本地 Token
3. 跳转到登录页

## 📊 开发调试

### 开发环境日志

开发模式下，所有请求和响应都会在控制台输出：

```
[Request] POST /user/login { Account: "test", PassWord: "..." }
[Response] /user/login { success: true, code: 0, message: "登录成功", data: {...} }
```

### 生产环境

生产环境不会输出请求日志，仅在发生错误时输出错误信息。

## 🌐 环境配置

在 `.env` 文件中配置 API 基础地址：

```bash
# .env.development
VITE_API_BASE_URL=/api

# .env.production
VITE_API_BASE_URL=https://api.example.com
```

## 📝 最佳实践

### 1. 统一在 `src/api` 目录定义接口

```
src/api/
  ├── user.ts          # 用户相关接口
  ├── problem.ts       # 题目相关接口
  ├── discussion.ts       # 讨论相关接口
  └── index.ts         # 统一导出
```

### 2. 使用 TypeScript 类型约束

```typescript
// 定义请求参数类型
interface LoginParams {
    Account: string;
    PassWord: string;
}

// 定义响应数据类型
interface LoginResult {
    Token: string;
    UserId: string;
    NickName: string;
}

// 使用类型约束
export const userLogin = (data: LoginParams) => {
    return post<LoginResult>("/user/login", data);
};
```

### 3. 合理使用消息提示

- **查询类操作**：不显示成功提示
- **修改类操作**：显示成功提示
- **删除类操作**：显示成功提示并二次确认

```typescript
// 查询 - 不提示
await get("/user/info", { id: "123" });

// 更新 - 提示成功
await post("/user/update", data, {
    showSuccessMessage: true,
    successMessage: "更新成功！"
});

// 删除 - 二次确认 + 提示
await ElMessageBox.confirm("确定删除吗？", "提示");
await del("/user/delete", { id: "123" }, {
    showSuccessMessage: true,
    successMessage: "删除成功！"
});
```

### 4. 使用统一的错误处理

```typescript
// 推荐：依赖拦截器自动处理
const loadData = async () => {
    try {
        const response = await get("/data/list");
        if (response.data.success) {
            list.value = response.data.data;
        }
    } catch (error) {
        // 错误已自动提示，这里只需处理业务逻辑
        console.error(error);
    }
};

// 不推荐：手动处理每个错误
const loadData = async () => {
    try {
        const response = await get("/data/list");
        if (!response.data.success) {
            ElMessage.error(response.data.message); // 重复提示
        }
    } catch (error) {
        ElMessage.error("请求失败"); // 重复提示
    }
};
```

## 🔄 迁移指南

### 从旧版本迁移

如果你的代码使用了旧的响应结构（`Result`/`Reason`/`Data`），需要调整：

**旧版本：**

```typescript
const response = await post("/user/login", data);
if (response.data.Result === "200") {
    const user = response.data.Data;
}
```

**新版本：**

```typescript
const response = await post<LoginResult>("/user/login", data);
if (response.data.success) {
    const user = response.data.data;
}
```

## 📚 相关文件

- `src/axios/index.ts` - Axios 封装主文件
- `src/types/common/response.ts` - 响应类型定义
- `src/types/api/api.d.ts` - API 接口类型定义
- `include/constants/error_code.h` - 后端错误码定义
- `include/utils/response.h` - 后端响应工具

## 🤝 贡献

如有问题或建议，请提交 Issue 或 Pull Request。

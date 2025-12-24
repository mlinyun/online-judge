/**
 * 用户模块 API 接口
 *
 * @description 提供用户注册、登录、信息管理等接口
 *
 * ## 主要功能
 *
 * - 用户注册与登录
 * - 用户信息查询与更新
 * - 用户排名查询
 * - 用户管理（管理员权限）
 * - 通过 Token 鉴权登录
 * - 用户修改密码
 * - 用户退出登录
 *
 * @module api/user
 */

import { get, post, put, del } from "@/utils/http";
import type { AxiosResponse } from "axios";
import type { Api } from "@/types/api/api";

/**
 * 用户注册
 * @name userRegister
 * @tags 用户模块
 * @description 注册新用户
 * @request POST `/user/register`
 * @param data 用户注册请求参数，详见 {@link Api.User.UserRegisterParams}
 * @returns 用户注册响应参数，详见 {@link Api.User.UserRegisterResponse}
 */
export const userRegister = (
    data: Api.User.UserRegisterParams
): Promise<AxiosResponse<Api.User.UserRegisterResponse>> => {
    return post<Api.User.UserRegisterResult>("/user/register", data, {
        showSuccessMessage: true,
        successMessage: "注册成功！",
    });
};

/**
 * 用户登录
 * @name userLogin
 * @tags 用户模块
 * @description 用户登录，登录成功后返回 Token
 * @request POST `/user/login`
 * @param data 用户登录请求参数，详见 {@link Api.User.UserLoginParams}
 * @returns 用户登录响应，详见 {@link Api.User.UserLoginResponse}
 */
export const userLogin = (data: Api.User.UserLoginParams): Promise<AxiosResponse<Api.User.UserLoginResponse>> => {
    return post<Api.User.UserLoginResult>("/user/login", data, {
        showSuccessMessage: true,
        successMessage: "登录成功！",
    });
};

/**
 * 查询用户信息
 * @name selectUserInfo
 * @tags 用户模块
 * @description 获取用户详细信息
 * @request GET `/user/info`
 * @param userId 用户 ID，详见 {@link Api.User.UserId}
 * @returns 用户详细信息，详见 {@link Api.User.SelectUserInfoResponse}
 */
export const selectUserInfo = (userId: Api.User.UserId): Promise<AxiosResponse<Api.User.SelectUserInfoResponse>> => {
    return get<Api.User.SelectUserInfoResult>("/user/info", { UserId: userId });
};

/**
 * 查询用户信息（在设置页面修改用户时使用）
 * @name selectUserUpdateInfo
 * @tags 用户模块
 * @description 获取用户详细信息（用于设置页面）
 * @request GET `/user/select/info`
 * @param userId 用户 ID，详见 {@link Api.User.UserId}
 * @returns 用户详细信息，详见 {@link Api.User.SelectUserUpdateInfoResponse}
 */
export const selectUserUpdateInfo = (
    userId: Api.User.UserId
): Promise<AxiosResponse<Api.User.SelectUserUpdateInfoResponse>> => {
    return get<Api.User.SelectUserUpdateInfoResult>("/user/select/info", { UserId: userId });
};

/**
 * 更新用户信息
 * @name updateUserInfo
 * @tags 用户模块
 * @description 更新用户信息
 * @request POST `/user/update`
 * @param data 更新用户信息请求参数，详见 {@link Api.User.UpdateUserInfoParams}
 * @returns 更新用户信息响应，详见 {@link Api.User.UpdateUserInfoResponse}
 */
export const updateUserInfo = (
    data: Api.User.UpdateUserInfoParams
): Promise<AxiosResponse<Api.User.UpdateUserInfoResponse>> => {
    return post<Api.Common.OperationResult>("/user/update", data, {
        showSuccessMessage: true,
        successMessage: "更新成功！",
    });
};

/**
 * 删除用户（管理员权限）
 * @name deleteUser
 * @tags 用户模块
 * @description 删除用户（管理员权限）
 * @request DELETE `/admin/user/delete`
 * @param userId 用户 ID，详见 {@link Api.User.UserId}
 * @returns 删除用户响应，详见 {@link Api.User.DeleteUserResponse}
 */
export const deleteUser = (userId: Api.User.UserId): Promise<AxiosResponse<Api.User.DeleteUserResponse>> => {
    return del<Api.Common.OperationResult>(
        "/admin/user/delete",
        { UserId: userId },
        {
            showSuccessMessage: true,
            successMessage: "用户删除成功！",
        }
    );
};

/**
 * 获取用户排名列表
 * @name selectUserRank
 * @tags 用户模块
 * @description 分页查询用户排名列表
 * @request GET `/user/rank`
 * @param params 分页参数，详见 {@link Api.Common.PaginationParams}
 * @returns 用户排名列表，详见 {@link Api.User.UserRankListResponse}
 */
export const selectUserRank = (
    params: Api.Common.PaginationParams
): Promise<AxiosResponse<Api.User.UserRankListResponse>> => {
    return get<Api.User.UserRankListResult>("/user/rank", { ...params });
};

/**
 * 分页查询用户列表（管理员权限）
 * @name selectUserList
 * @tags 用户模块
 * @description 分页查询用户列表（管理员权限）
 * @request POST `/admin/user/list`
 * @param data 分页参数，详见 {@link Api.User.UserManageListParams}
 * @returns 用户列表，详见 {@link Api.User.UserManageListResponse}
 */
export const selectUserList = (
    data: Api.User.UserManageListParams
): Promise<AxiosResponse<Api.User.UserManageListResponse>> => {
    return post<Api.User.UserManageListResult>("/admin/user/list", data);
};

/**
 * 通过 Token 登录用户
 * @name loginUserByToken
 * @tags 用户模块
 * @description 通过 Token 鉴权登录用户
 * @request Get `/user/auth`
 * @returns 通过 Token 登录用户响应参数，详见 {@link Api.User.LoginUserByTokenResponse}
 */
export const loginUserByToken = (): Promise<AxiosResponse<Api.User.LoginUserByTokenResponse>> => {
    return get<Api.User.LoginUserByTokenResult>("/user/auth");
};

/**
 * 用户修改密码
 * @name updateUserPassword
 * @tags 用户模块
 * @description 用户修改密码
 * @request POST `/user/password`
 * @param data 用户修改密码请求参数，详见 {@link Api.User.UpdateUserPasswordParams}
 * @returns 用户修改密码响应参数，详见 {@link Api.User.UpdateUserPasswordResponse}
 */
export const updateUserPassword = (
    data: Api.User.UpdateUserPasswordParams
): Promise<AxiosResponse<Api.User.UpdateUserPasswordResponse>> => {
    return put<Api.Common.OperationResult>("/user/password", data, {
        showSuccessMessage: true,
        successMessage: "密码修改成功！",
    });
};

/**
 * 用户退出登录
 * @name userLogout
 * @tags 用户模块
 * @description 用户退出登录
 * @request POST `/user/logout`
 * @returns 用户退出登录响应参数，详见 {@link Api.User.UserLogoutResponse}
 */
export const userLogout = (): Promise<AxiosResponse<Api.User.UserLogoutResponse>> => {
    return post<Api.Common.OperationResult>("/user/logout", null, {
        showSuccessMessage: true,
        successMessage: "退出登录成功！",
    });
};

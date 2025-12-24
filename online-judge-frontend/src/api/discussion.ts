/**
 * 讨论模块 API 接口
 *
 * @description 包含讨论的增删改查等接口
 *
 * ## 主要功能
 * - 添加讨论
 * - 查询讨论详细内容
 * - 更新讨论
 * - 删除讨论
 * - 分页查询讨论列表
 * - 分页查询讨论列表（管理员权限）
 *
 * @module api/discuss
 */

import { get, post, del } from "@/utils/http";
import type { AxiosResponse } from "axios";
import type { Api } from "@/types/api/api";

/**
 * 添加讨论
 * @name insertDiscuss
 * @tags 讨论模块
 * @description 添加讨论
 * @request POST `/discussion/insert`
 * @param data 添加讨论请求参数，详见 {@link Api.Discuss.InsertDiscussParams}
 * @return 添加讨论响应，详见 {@link Api.Discuss.InsertDiscussResponse}
 */
export const insertDiscuss = (
    data: Api.Discuss.InsertDiscussParams
): Promise<AxiosResponse<Api.Discuss.InsertDiscussResponse>> => {
    return post<Api.Discuss.InsertDiscussResult>("/discussion/insert", data, {
        showSuccessMessage: true,
        successMessage: "讨论添加成功！",
    });
};

/**
 * 查询讨论的详细内容
 * @name selectDiscuss
 * @tags 讨论模块
 * @description 查询讨论的详细内容，并且将其浏览量加 1
 * @request GET `/discussion/info`
 * @param discussId 讨论 ID，详见 {@link Api.Discuss.DiscussId}
 * @return 讨论详细信息，详见 {@link Api.Discuss.SelectDiscussResponse}
 */
export const selectDiscuss = (
    discussId: Api.Discuss.DiscussId
): Promise<AxiosResponse<Api.Discuss.SelectDiscussResponse>> => {
    return get<Api.Discuss.SelectDiscussResult>("/discussion/info", {
        DiscussId: discussId,
    });
};

/**
 * 查询讨论的详细信息，主要是编辑时的查询
 * @name selectDiscussByEdit
 * @tags 讨论模块
 * @description 查询讨论的详细信息，主要是编辑时的查询
 * @request GET `/discussion/select/info`
 * @param discussId 讨论 ID，详见 {@link Api.Discuss.DiscussId}
 * @return 讨论详细信息，详见 {@link Api.Discuss.SelectDiscussByEditResponse}
 */
export const selectDiscussByEdit = (
    discussId: Api.Discuss.DiscussId
): Promise<AxiosResponse<Api.Discuss.SelectDiscussByEditResponse>> => {
    return get<Api.Discuss.SelectDiscussByEditResult>("/discussion/select/info", {
        DiscussId: discussId,
    });
};

/**
 * 更新讨论
 * @name updateDiscuss
 * @tags 讨论模块
 * @description 更新讨论内容
 * @request POST `/discussion/update`
 * @param data 更新讨论请求参数，详见 {@link Api.Discuss.UpdateDiscussParams}
 * @return 更新讨论响应，详见 {@link Api.Discuss.UpdateDiscussResponse}
 */
export const updateDiscuss = (
    data: Api.Discuss.UpdateDiscussParams
): Promise<AxiosResponse<Api.Discuss.UpdateDiscussResponse>> => {
    return post<Api.Common.OperationResult>("/discussion/update", data, {
        showSuccessMessage: true,
        successMessage: "讨论更新成功！",
    });
};

/**
 * 删除讨论
 * @name deleteDiscuss
 * @tags 讨论模块
 * @description 删除指定讨论
 * @request DELETE `/discussion/delete`
 * @param discussId 讨论 ID，详见 {@link Api.Discuss.DiscussId}
 * @return 删除讨论响应，详见 {@link Api.Discuss.DeleteDiscussResponse}
 */
export const deleteDiscuss = (
    discussId: Api.Discuss.DiscussId
): Promise<AxiosResponse<Api.Discuss.DeleteDiscussResponse>> => {
    return del<Api.Common.OperationResult>(
        "/discussion/delete",
        {
            DiscussId: discussId,
        },
        {
            showSuccessMessage: true,
            successMessage: "讨论删除成功！",
        }
    );
};

/**
 * 分页查询讨论列表
 * @name selectDiscussList
 * @tags 讨论模块
 * @description 分页查询讨论列表
 * @request POST `/discussion/list`
 * @param data 分页查询参数，详见 {@link Api.Discuss.SelectDiscussListParams}
 * @return 分页查询响应，详见 {@link Api.Discuss.SelectDiscussListResponse}
 */
export const selectDiscussList = (
    data: Api.Discuss.SelectDiscussListParams
): Promise<AxiosResponse<Api.Discuss.SelectDiscussListResponse>> => {
    return post<Api.Discuss.SelectDiscussListResult>("/discussion/list", data);
};

/**
 * 分页查询讨论列表（管理员权限）
 * @name selectDiscussListByAdmin
 * @tags 讨论模块
 * @description 分页查询讨论列表（管理员权限）
 * @request POST `/admin/discussion/list`
 * @param data 分页查询参数，详见 {@link Api.Discuss.SelectDiscussListByAdminParams}
 * @return 分页查询响应，详见 {@link Api.Discuss.SelectDiscussListResponse}
 */
export const selectDiscussListByAdmin = (
    data: Api.Discuss.SelectDiscussListByAdminParams
): Promise<AxiosResponse<Api.Discuss.SelectDiscussListResponse>> => {
    return post<Api.Discuss.SelectDiscussListResult>("/admin/discussion/list", data);
};

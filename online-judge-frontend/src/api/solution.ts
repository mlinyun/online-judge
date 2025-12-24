/**
 * 题解模块 API 接口
 *
 * @description 包含题解的增删改查等接口
 *
 * ## 主要功能
 * - 添加题解
 * - 查询题解详细内容
 * - 查询题解详细信息（编辑时）
 * - 更新题解
 * - 删除题解
 * - 分页查询题解列表
 * - 分页查询题解列表（管理员权限）
 *
 * @module api/solution
 */

import { get, post, del } from "@/utils/http";
import type { AxiosResponse } from "axios";
import type { Api } from "@/types/api/api";

/**
 * 添加题解
 * @name insertSolution
 * @tags 题解模块
 * @description 添加新的题解
 * @request POST `/solution/insert`
 * @param data 题解信息，详见 {@link Api.Solution.InsertSolutionParams}
 * @returns 新增结果，详见 {@link Api.Solution.InsertSolutionResponse}
 */
export const insertSolution = (
    data: Api.Solution.InsertSolutionParams
): Promise<AxiosResponse<Api.Solution.InsertSolutionResponse>> => {
    return post<Api.Solution.InsertSolutionResult>("/solution/insert", data, {
        showSuccessMessage: true,
        successMessage: "题解添加成功！",
    });
};

/**
 * 查询题解的详细内容，并且将其浏览量加 1
 * @name selectSolution
 * @tags 题解模块
 * @description 查询题解的详细内容，并且将其浏览量加 1
 * @request GET `/solution/info`
 * @param solutionId 题解 ID，详见 {@link Api.Solution.SolutionId}
 * @returns 题解详细信息，详见 {@link Api.Solution.SelectSolutionResponse}
 */
export const selectSolution = (
    solutionId: Api.Solution.SolutionId
): Promise<AxiosResponse<Api.Solution.SelectSolutionResponse>> => {
    return get<Api.Solution.SelectSolutionResult>("/solution/info", {
        SolutionId: solutionId,
    });
};

/**
 * 查询题解的详细信息，主要是编辑时的查询
 * @name selectSolutionByEdit
 * @tags 题解模块
 * @description 查询题解的详细信息，主要是编辑时的查询
 * @request GET `/solution/select/info`
 * @param solutionId 题解 ID，详见 {@link Api.Solution.SolutionId}
 * @returns 题解详细信息，详见 {@link Api.Solution.SelectSolutionByEditResponse}
 */
export const selectSolutionByEdit = (
    solutionId: Api.Solution.SolutionId
): Promise<AxiosResponse<Api.Solution.SelectSolutionByEditResponse>> => {
    return get<Api.Solution.SelectSolutionByEditResult>("/solution/select/info", {
        SolutionId: solutionId,
    });
};

/**
 * 更新题解
 * @name updateSolution
 * @tags 题解模块
 * @description 更新题解信息
 * @request POST `/solution/update`
 * @param data 题解更新请求参数，详见 {@link Api.Solution.UpdateSolutionParams}
 * @returns 题解更新响应参数，详见 {@link Api.Solution.UpdateSolutionResponse}
 */
export const updateSolution = (
    data: Api.Solution.UpdateSolutionParams
): Promise<AxiosResponse<Api.Solution.UpdateSolutionResponse>> => {
    return post<Api.Common.OperationResult>("/solution/update", data, {
        showSuccessMessage: true,
        successMessage: "题解更新成功！",
    });
};

/**
 * 删除题解
 * @name deleteSolution
 * @tags 题解模块
 * @description 删除指定题解
 * @request DELETE `/solution/delete`
 * @param solutionId 题解 ID，详见 {@link Api.Solution.SolutionId}
 * @returns 删除结果，详见 {@link Api.Solution.DeleteSolutionResponse}
 */
export const deleteSolution = (
    solutionId: Api.Solution.SolutionId
): Promise<AxiosResponse<Api.Solution.DeleteSolutionResponse>> => {
    return del<Api.Common.OperationResult>("/solution/delete", {
        SolutionId: solutionId,
        showSuccessMessage: true,
        successMessage: "题解删除成功！",
    });
};

/**
 * 分页查询题解列表
 * @name selectSolutionList
 * @tags 题解模块
 * @description 分页查询题解列表
 * @request POST `/solution/list`
 * @param data 查询参数，详见 {@link Api.Solution.SelectSolutionListParams}
 * @returns 题解列表，详见 {@link Api.Solution.SelectSolutionListResponse}
 */
export const selectSolutionList = (
    data: Api.Solution.SelectSolutionListParams
): Promise<AxiosResponse<Api.Solution.SelectSolutionListResponse>> => {
    return post<Api.Solution.SelectSolutionListResult>("/solution/list", data);
};

/**
 * 分页查询题解列表（管理员权限）
 * @name selectSolutionListByAdmin
 * @tags 题解模块
 * @description 分页查询题解列表（管理员权限）
 * @request POST `/admin/solution/list`
 * @param data 查询参数，详见 {@link Api.Solution.SelectSolutionListByAdminParams}
 * @returns 题解列表，详见 {@link Api.Solution.SelectSolutionListResponse}
 */
export const selectSolutionListByAdmin = (
    data: Api.Solution.SelectSolutionListByAdminParams
): Promise<AxiosResponse<Api.Solution.SelectSolutionListByAdminResponse>> => {
    return post<Api.Solution.SelectSolutionListByAdminResult>("/admin/solution/list", data);
};

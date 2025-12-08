/**
 * 题目模块 API 接口
 *
 * @description 包含题目的增删改查等接口
 *
 * ## 主要功能
 * - 获取题目详情
 * - 编辑题目（新增/更新）
 * - 删除题目（管理员权限）
 * - 获取题目列表
 *
 * @module api/problem
 */

import { get, post, del } from "@/utils/http";
import type { AxiosResponse } from "axios";
import type { Api } from "@/types/api/api";

/**
 * 查询题目信息（单条）
 * @name selectProblemInfo
 * @tags 题目模块
 * @description 查询指定题目的信息
 * @request GET `/problem/info`
 * @param problemId 题目 ID，详见 {@link Api.Problem.ProblemId}
 * @returns 题目详细信息，详见 {@link Api.Problem.SelectProblemResponse}
 */
export const selectProblemInfo = (
    problemId: Api.Problem.ProblemId
): Promise<AxiosResponse<Api.Problem.SelectProblemResponse>> => {
    return get<Api.Problem.SelectProblemResult>("/problem/info", {
        ProblemId: problemId,
    });
};

/**
 * 查询题目信息（单条，管理员权限）
 * @name selectProblemInfoByAdmin
 * @tags 题目模块
 * @description 管理员查询指定题目的信息
 * @request GET `/admin/problem/info`
 * @param problemId 题目 ID，详见 {@link Api.Problem.ProblemId}
 * @returns 题目详细信息，详见 {@link Api.Problem.SelectProblemByAdminResponse}
 */
export const selectProblemInfoByAdmin = (
    problemId: Api.Problem.ProblemId
): Promise<AxiosResponse<Api.Problem.SelectProblemByAdminResponse>> => {
    return get<Api.Problem.SelectProblemByAdminResult>("/admin/problem/info", {
        ProblemId: problemId,
    });
};

/**
 * 编辑题目（新增/更新，管理员权限）
 * @name editProblem
 * @tags 题目模块
 * @description 新增或更新题目信息
 * @request POST `/admin/problem/edit`
 * @param data 题目编辑请求参数，详见 {@link Api.Problem.EditProblemParams}
 * @returns 题目编辑响应参数，详见 {@link Api.Problem.EditProblemResponse}
 */
export const editProblem = (
    data: Api.Problem.EditProblemParams
): Promise<AxiosResponse<Api.Problem.EditProblemResponse>> => {
    return post<Api.Problem.EditProblemResult>("/admin/problem/edit", data, {
        showSuccessMessage: true,
        successMessage: "题目编辑成功！",
    });
};

/**
 * 删除题目（管理员权限）
 * @name deleteProblem
 * @tags 题目模块
 * @description 管理员删除指定题目
 * @request DELETE `/admin/problem/delete`
 * @param problemId 题目 ID，详见 {@link Api.Problem.ProblemId}
 * @returns 删除结果，详见 {@link Api.Problem.DeleteProblemResponse}
 */
export const deleteProblem = (
    problemId: Api.Problem.ProblemId
): Promise<AxiosResponse<Api.Problem.DeleteProblemResponse>> => {
    return del<Api.Common.OperationResult>(
        "/admin/problem/delete",
        {
            ProblemId: problemId,
        },
        {
            showSuccessMessage: true,
            successMessage: "题目删除成功！",
        }
    );
};

/**
 * 分页获取题目列表
 * @name selectProblemList
 * @tags 题目模块
 * @description 分页获取题目列表
 * @request GET `/problem/list`
 * @param params 题目列表查询参数，详见 {@link Api.Problem.SelectProblemListParams}
 * @returns 题目列表，详见 {@link Api.Problem.SelectProblemListResponse}
 */
export const selectProblemList = (
    params: Api.Problem.SelectProblemListParams
): Promise<AxiosResponse<Api.Problem.SelectProblemListResponse>> => {
    return get<Api.Problem.SelectProblemListResult>("/problem/list", { ...params });
};

/**
 * 分页获取题目列表（管理员权限）
 * @name selectProblemListByAdmin
 * @tags 题目模块
 * @description 管理员分页获取题目列表
 * @request GET `/admin/problem/list`
 * @param params 分页查询请求参数，详见 {@link Api.Common.PaginationParams}
 * @returns 题目列表，详见 {@link Api.Problem.SelectProblemListResponse}
 */
export const selectProblemListByAdmin = (
    params: Api.Common.PaginationParams
): Promise<AxiosResponse<Api.Problem.SelectProblemListResponse>> => {
    return get<Api.Problem.SelectProblemListResult>("/admin/problem/list", { ...params });
};

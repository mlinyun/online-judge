/**
 * 测评记录模块 API 接口
 *
 * @description 包含测评记录的查询等接口
 *
 * ## 主要功能
 * - 查询测评记录详情
 * - 分页获取测评记录列表
 *
 * @module api/status
 */

import { get, post } from "@/utils/http";
import type { AxiosResponse } from "axios";
import type { Api } from "@/types/api/api";

/**
 * 查询一条状态记录的详细信息
 * @name selectStatusRecord
 * @tags 测评记录模块
 * @description 查询一条状态记录的详细信息
 * @request GET `/status/info`
 * @param statusRecordId 状态记录 ID，详见 {@link Api.Status.StatusRecordId}
 * @returns 状态记录详细信息，详见 {@link Api.Status.SelectStatusRecordResponse}
 */
export const selectStatusRecord = (
    statusRecordId: Api.Status.StatusRecordId
): Promise<AxiosResponse<Api.Status.SelectStatusRecordResponse>> => {
    return get<Api.Status.SelectStatusRecordResult>("/status/record/info", {
        StatusRecordId: statusRecordId,
    });
};

/**
 * 分页获取状态记录列表
 * @name selectStatusRecordList
 * @tags 测评记录模块
 * @description 分页获取状态记录列表
 * @request POST `/status/info`
 * @param data 查询参数，详见 {@link Api.Status.SelectStatusRecordListParams}
 * @returns 状态记录列表，详见 {@link Api.Status.SelectStatusRecordListResponse}
 */
export const selectStatusRecordList = (
    data: Api.Status.SelectStatusRecordListParams
): Promise<AxiosResponse<Api.Status.SelectStatusRecordListResponse>> => {
    return post<Api.Status.SelectStatusRecordListResult>("/status/record/list", data);
};

/**
 * 代码测评模块 API 接口
 *
 * @description 包含代码提交测评的接口
 *
 * ## 主要功能
 * - 提交代码进行测评
 *
 * @module api/judge
 */

import { post } from "@/utils/http";
import type { AxiosResponse } from "axios";
import type { Api } from "@/types/api/api";

/**
 * 提交代码进行测评
 * @name submitJudgeCode
 * @tags 代码测评模块
 * @description 提交代码进行测评，返回测评结果
 * @request POST `/judge/code`
 * @param data 代码提交请求参数，详见 {@link Api.Judge.SubmitJudgeCodeParams}
 * @returns 代码提交响应参数，详见 {@link Api.Judge.SubmitJudgeCodeResponse}
 */
export const submitJudgeCode = (
    data: Api.Judge.SubmitJudgeCodeParams
): Promise<AxiosResponse<Api.Judge.SubmitJudgeCodeResponse>> => {
    return post<Api.Judge.SubmitJudgeCodeResult>("/judge/code", data);
};

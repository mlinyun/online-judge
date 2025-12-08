/**
 * 标签模块 API 接口
 *
 * @description 包含标签的查询等接口
 *
 * ## 主要功能
 * - 获取题目的所有标签
 *
 * @module api/tag
 */

import { get } from "@/utils/http";
import type { AxiosResponse } from "axios";
import type { Api } from "@/types/api/api";

/**
 * 获取题目的所有标签
 * @name getProblemTags
 * @tags 标签模块
 * @description 获取题目的所有标签
 * @request GET `/tags`
 * @param tagType 标签类型，详见 {@link Api.Tag.GetProblemTagsParams}
 * @returns 标签列表，详见 {@link Api.Tag.GetProblemTagsResponse}
 */
export const getProblemTags = (
    tagType: Api.Tag.GetProblemTagsParams
): Promise<AxiosResponse<Api.Tag.GetProblemTagsResponse>> => {
    return get<Api.Tag.GetProblemTagsResult>("/tags", {
        TagType: tagType,
    });
};

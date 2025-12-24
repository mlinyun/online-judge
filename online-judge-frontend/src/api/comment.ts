/**
 * 评论模块 API 接口
 *
 * @description 包含评论的增删查等接口
 *
 * ## 主要功能
 * - 添加评论
 * - 分页获取评论列表
 * - 分页获取评论列表（管理员权限）
 * - 删除评论
 *
 * @module api/comment
 */

import { post, del } from "@/utils/http";
import type { AxiosResponse } from "axios";
import type { Api } from "@/types/api/api";

/**
 * 添加评论
 * @name insertComment
 * @tags 评论模块
 * @description 添加新的评论
 * @request POST `/comment/insert`
 * @param data 评论信息，详见 {@link Api.Comment.InsertCommentParams}
 * @returns 新增结果，详见 {@link Api.Comment.InsertCommentResponse}
 */
export const insertComment = (
    data: Api.Comment.InsertCommentParams
): Promise<AxiosResponse<Api.Comment.InsertCommentResponse>> => {
    return post<Api.Comment.InsertCommentResult>("/comment/insert", data, {
        showSuccessMessage: true,
        successMessage: "评论添加成功！",
    });
};

/**
 * 分页查询评论列表
 * @name selectCommentList
 * @tags 评论模块
 * @description 分页查询评论列表
 * @request POST `/comment/info`
 * @param data 查询参数，详见 {@link Api.Comment.SelectCommentListParams}
 * @returns 评论列表，详见 {@link Api.Comment.SelectCommentListResponse}
 */
export const selectCommentList = (
    data: Api.Comment.SelectCommentListParams
): Promise<AxiosResponse<Api.Comment.SelectCommentListResponse>> => {
    return post<Api.Comment.SelectCommentListResult>("/comment/info", data);
};

/**
 * 分页查询评论列表（管理员权限）
 * @name selectCommentListByAdmin
 * @tags 评论模块
 * @description 分页查询评论列表（管理员权限）
 * @request POST `/comment/admin/info`
 * @param data 查询参数，详见 {@link Api.Comment.SelectCommentListByAdminParams}
 * @returns 评论列表，详见 {@link Api.Comment.SelectCommentListByAdminResponse}
 */
export const selectCommentListByAdmin = (
    data: Api.Comment.SelectCommentListByAdminParams
): Promise<AxiosResponse<Api.Comment.SelectCommentListByAdminResponse>> => {
    return post<Api.Comment.SelectCommentListByAdminResult>("/admin/comment/list", data);
};

/**
 * 删除评论
 * @name deleteComment
 * @tags 评论模块
 * @description 删除评论
 * @request DELETE `/comment/delete`
 * @param commentId 评论 ID，详见 {@link Api.Comment.CommentId}
 * @returns 删除结果，详见 {@link Api.Comment.DeleteCommentResponse}
 */
export const deleteComment = (
    commentId: Api.Comment.CommentId
): Promise<AxiosResponse<Api.Comment.DeleteCommentResponse>> => {
    return del<Api.Common.OperationResult>(
        "/comment/delete",
        {
            CommentId: commentId,
        },
        {
            showSuccessMessage: true,
            successMessage: "评论删除成功！",
        }
    );
};

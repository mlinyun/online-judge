<script setup lang="ts">
import { computed, onMounted, reactive, ref, watch } from "vue";
import { ElMessage } from "element-plus";
import { storeToRefs } from "pinia";

import { deleteComment, insertComment, selectCommentList, toggleCommentLike } from "@/api/comment";
import { useUserStore } from "@/stores/modules/user";
import emoji from "@/assets/emoji/emoji";
import type { Api } from "@/types/api/api";

// UndrawUI types are optional at compile-time; keep them type-only.
import type {
    CommentApi as UndrawCommentApi,
    ConfigApi as UndrawConfigApi,
    CommentSubmitApi as UndrawSubmitParamApi,
    CommentReplyPageApi as UndrawReplyPageParamApi,
    ReplyApi as UndrawReplyApi,
    UserApi as UndrawUserApi,
} from "undraw-ui";

defineOptions({ name: "OjComment" });

type ParentType = Api.Comment.InsertCommentParams["ParentType"];

type Props = {
    /** 文章/对象 ID（公告/讨论/题解） */
    parentId: string;
    /** 文章类型（公告/讨论/题解） */
    parentType: Exclude<ParentType, "Comment">;
    /** 父评论分页大小 */
    pageSize?: number;
    /** 父评论列表里，默认展示多少条子评论 */
    sonNum?: number;
};

const props = withDefaults(defineProps<Props>(), {
    pageSize: 10,
    sonNum: 3,
});

const userStore = useUserStore();
const { userInfo, isLoggedIn } = storeToRefs(userStore);

const currentPage = ref(1);
const total = ref(0);
const loading = ref(false);

const undrawUser = computed<UndrawUserApi>(() => {
    if (!isLoggedIn.value) {
        return {
            id: "",
            username: "",
            avatar: "",
            likeIds: [],
        };
    }
    return {
        id: userInfo.value._id,
        username: userInfo.value.NickName,
        avatar: userInfo.value.Avatar,
        likeIds: userInfo.value.CommentLikes,
    };
});

type UndrawShowApi = NonNullable<UndrawConfigApi["show"]>;

const config = reactive<UndrawConfigApi>({
    user: undrawUser.value,
    // 表情包资源：public/static/emoji + 本地 emoji.ts
    emoji: emoji as unknown as UndrawConfigApi["emoji"],
    comments: [],
    show: {
        form: isLoggedIn.value,
    } as UndrawShowApi,
});

watch(
    undrawUser,
    (val) => {
        config.user = val;
    },
    { immediate: true }
);

watch(
    isLoggedIn,
    (val) => {
        config.show = {
            ...(config.show ?? ({} as UndrawShowApi)),
            form: val,
        };
    },
    { immediate: true }
);

const pickFirstUser = (u: Api.User.SimpleUserInfo[] | Api.User.SimpleUserInfo | null | undefined) => {
    if (!u) return undefined;
    return Array.isArray(u) ? u[0] : u;
};

const mapUser = (
    u: Api.User.SimpleUserInfo[] | Api.User.SimpleUserInfo | null | undefined
): UndrawCommentApi["user"] => {
    const first = pickFirstUser(u);
    return {
        username: first?.NickName ?? "",
        avatar: first?.Avatar ?? "",
    };
};

const mapUid = (u: Api.User.SimpleUserInfo[] | Api.User.SimpleUserInfo | null | undefined) => {
    const first = pickFirstUser(u);
    return first?._id ?? "0";
};

const mapChildComment = (fatherId: Api.Comment.CommentId, item: Api.Comment.ChildCommentListItem): UndrawCommentApi => {
    return {
        id: item._id,
        parentId: fatherId,
        uid: mapUid(item.User),
        content: item.Content,
        likes: item.Likes,
        createTime: item.CreateTime,
        user: mapUser(item.User),
        reply: null,
    } as unknown as UndrawCommentApi;
};

const mapFatherComment = (item: Api.Comment.SelectCommentListItem): UndrawCommentApi => {
    const fatherId = item._id;
    const replyList = (item.Child_Comments ?? []).map((c) => mapChildComment(fatherId, c));
    const replyTotal = Number(item.Child_Total ?? 0);

    const reply: UndrawReplyApi | null =
        replyTotal > 0 ? ({ total: replyTotal, list: replyList } as UndrawReplyApi) : null;

    return {
        id: fatherId,
        parentId: null,
        uid: mapUid(item.User),
        content: item.Content,
        likes: item.Likes,
        createTime: item.CreateTime,
        user: mapUser(item.User),
        reply,
    } as unknown as UndrawCommentApi;
};

const refreshFatherComments = async () => {
    loading.value = true;
    try {
        const res = await selectCommentList({
            Page: currentPage.value,
            PageSize: props.pageSize,
            ParentId: props.parentId as Api.Comment.SelectCommentListParams["ParentId"],
            CommentType: "Father",
            SonNum: props.sonNum,
        });

        if (res.data.code === 0 && res.data.data) {
            total.value = Number(res.data.data.Total ?? 0);
            config.comments = (res.data.data.List ?? []).map(mapFatherComment);
        } else {
            total.value = 0;
            config.comments = [];
            ElMessage.error(res.data.message || "获取评论失败");
        }
    } catch (e) {
        console.error("获取评论失败:", e);
        ElMessage.error("网络异常，请稍后重试");
    } finally {
        loading.value = false;
    }
};

const handleSubmit = async ({ content, parentId, finish }: UndrawSubmitParamApi) => {
    if (!isLoggedIn.value) {
        ElMessage.warning("请先登录");
        return;
    }

    const isReply = parentId != null;

    const payload: Api.Comment.InsertCommentParams = {
        ParentId: (isReply ? String(parentId) : props.parentId) as Api.Comment.InsertCommentParams["ParentId"],
        ParentType: (isReply ? "Comment" : props.parentType) as Api.Comment.InsertCommentParams["ParentType"],
        CommentType: isReply ? "Reply" : "Father",
        Content: content,
    };

    try {
        const res = await insertComment(payload);
        if (res.data.code === 0 && res.data.data) {
            const created: UndrawCommentApi = {
                id: res.data.data.CommentId,
                parentId: isReply ? String(parentId) : null,
                uid: undrawUser.value.id,
                content,
                likes: 0,
                createTime: res.data.data.CreateTime,
                user: {
                    username: undrawUser.value.username,
                    avatar: undrawUser.value.avatar,
                },
                reply: null,
            };

            finish(created);

            // 父评论提交后刷新一次，确保分页/子评论统计保持一致
            if (!isReply) {
                await refreshFatherComments();
            }
        } else {
            ElMessage.error(res.data.message || "评论失败");
        }
    } catch (e) {
        console.error("提交评论失败:", e);
        ElMessage.error("网络异常，请稍后重试");
    }
};

const handleReplyPage = async ({ parentId, current, size, finish }: UndrawReplyPageParamApi) => {
    try {
        const res = await selectCommentList({
            Page: Number(current) || 1,
            PageSize: size,
            ParentId: String(parentId) as unknown as Api.Comment.SelectCommentListParams["ParentId"],
            CommentType: "Reply",
        });

        if (res.data.code === 0 && res.data.data) {
            const list = (res.data.data.List ?? []).map((it) => {
                return {
                    id: it._id,
                    parentId: String(parentId),
                    uid: mapUid(it.User),
                    content: it.Content,
                    likes: it.Likes,
                    createTime: it.CreateTime,
                    user: mapUser(it.User),
                    reply: null,
                } as unknown as UndrawCommentApi;
            });

            finish({ total: Number(res.data.data.Total ?? 0), list } as unknown as UndrawReplyApi);
        } else {
            finish({ total: 0, list: [] } as unknown as UndrawReplyApi);
        }
    } catch (e) {
        console.error("加载回复失败:", e);
        finish({ total: 0, list: [] } as unknown as UndrawReplyApi);
    }
};

const syncLikeIdsToStore = (nextLikeIds: string[]) => {
    if (!isLoggedIn.value) return;
    userStore.setUserInfo({
        ...userInfo.value,
        CommentLikes: nextLikeIds,
    });
};

const ensureLikeId = (id: string, liked: boolean) => {
    const likeIds = (config.user.likeIds ?? []).map((x) => String(x));
    const idStr = String(id);
    const idx = likeIds.findIndex((x) => x === idStr);
    if (liked && idx === -1) likeIds.push(idStr);
    if (!liked && idx !== -1) likeIds.splice(idx, 1);
    config.user.likeIds = likeIds;
    syncLikeIdsToStore(likeIds);
};

const setCommentLikes = (id: string, likes: number) => {
    const idStr = String(id);
    for (const comment of config.comments) {
        if (String(comment.id) === idStr) {
            comment.likes = likes;
            return;
        }
        const replyList = comment.reply?.list ?? [];
        const found = replyList.find((c) => String(c.id) === idStr);
        if (found) {
            found.likes = likes;
            return;
        }
    }
};

const handleLike = async (id: string, finish: () => void) => {
    if (!isLoggedIn.value) {
        ElMessage.warning("请先登录");
        return;
    }

    try {
        const payload = {
            CommentId: String(id) as Api.Comment.CommentId,
        };
        const res = await toggleCommentLike(payload);
        if (res.data.code === 0 && res.data.data) {
            // 先让 UndrawUI 完成它自己的交互闭环（会自动 +1/-1 & 更新 likeIds）
            finish();

            // 再用后端返回的最终状态进行校准，避免出现偏差
            ensureLikeId(String(id), res.data.data.Liked);
            setCommentLikes(String(id), res.data.data.Likes);
        } else {
            ElMessage.error(res.data.message || "点赞失败");
        }
    } catch (e) {
        console.error("点赞失败:", e);
        ElMessage.error("网络异常，请稍后重试");
    }
};

const handleRemove = async (id: string, finish: () => void) => {
    try {
        const res = await deleteComment(String(id) as Api.Comment.CommentId);
        if (res.data.code === 0) {
            finish();
            // 删除后刷新父评论列表，避免统计不一致
            await refreshFatherComments();
        } else {
            ElMessage.error(res.data.message || "删除失败");
        }
    } catch (e) {
        console.error("删除评论失败:", e);
        ElMessage.error("网络异常，请稍后重试");
    }
};

watch(
    () => props.parentId,
    () => {
        currentPage.value = 1;
        void refreshFatherComments();
    }
);

onMounted(() => {
    void refreshFatherComments();
});
</script>

<template>
    <section class="oj-comment" aria-label="评论">
        <div class="oj-comment-body">
            <!--
                UndrawUI 评论组件
                - page: 开启回复分页
            -->
            <u-comment :config="config" page :loading="loading" @submit="handleSubmit" @reply-page="handleReplyPage"
                @like="handleLike" @remove="handleRemove" />
        </div>

        <div class="oj-comment-pagination" v-if="total > 0">
            <el-pagination v-model:current-page="currentPage" :page-size="pageSize" layout="total, prev, pager, next"
                :total="total" background @current-change="refreshFatherComments" />
        </div>
    </section>
</template>

<style scoped>
.oj-comment {
    width: 100%;
}

.oj-comment-body {
    width: 100%;
}

.oj-comment-pagination {
    display: flex;
    justify-content: center;
    padding: var(--oj-spacing-4) 0 0;
}
</style>

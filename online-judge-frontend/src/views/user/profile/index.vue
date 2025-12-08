<script setup lang="ts">
import { selectUserInfo } from "@/api/user";
import { useUserStore } from "@/stores/modules/user";
import type { Api } from "@/types/api/api";

defineOptions({ name: "UserProfile" });

// 用户状态管理
const userStore = useUserStore();
// 获取用户信息
const userInfo = computed(() => userStore.getUserInfo);

// 处理查询用户信息
const handleQuery = async () => {
    // 获取登录用户 ID
    const userId: Api.User.UserId = userInfo.value._id;
    // 查询用户信息
    const response = await selectUserInfo(userId);
    // 判断查询是否成功
    if (response.data.success && response.data.data) {
        // 从控制台输出查询到的用户信息
        console.log("查询用户信息成功:", response.data.data);
    }
};
</script>

<template>
    <div class="user-profile">
        <h1>UserProfile</h1>
        <el-button type="primary" @click="handleQuery">Query</el-button>
    </div>
</template>

<style scoped></style>

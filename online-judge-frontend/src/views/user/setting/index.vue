<script setup lang="ts">
defineOptions({ name: "UserSetting" });
import { useUserStore } from "@/stores/modules/user";

const userStore = useUserStore();

const userInfo = userStore.getUserInfo;
const isLoggedIn = userStore.getIsLoggedIn;
const accessToken = userStore.getAccessToken;
</script>

<template>
    <div class="user-setting">
        <h1>UserSetting</h1>
        <p class="placeholder-text">UserInfo: {{ userInfo }}</p>
        <p class="placeholder-text">IsLoggedIn: {{ isLoggedIn }}</p>
        <p class="placeholder-text">AccessToken: {{ accessToken }}</p>
    </div>
</template>

<style scoped>
.placeholder-text {
    color: var(--oj-text-muted);
}
</style>

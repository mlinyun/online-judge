<script setup lang="ts">
defineOptions({ name: "AnnouncementDetail" });
import { useRoute } from "vue-router";

// 获取路由参数 id
const route = useRoute();
const announcementId = route.params.id as string;
</script>

<template>
    <div class="announcement-detail">
        <h1>AnnouncementDetail</h1>
        <p>公告 ID: {{ announcementId }}</p>
    </div>
</template>

<style scoped></style>

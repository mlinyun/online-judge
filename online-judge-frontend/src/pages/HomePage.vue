<template>
    <el-card class="box-card">
        <template #header>
        <div class="card-header">
            <span id="homepagetitle">欢迎来到XDOJ</span>
        </div>
        </template>
        <AnnouncementList></AnnouncementList>
    </el-card>
</template>
<script setup>
import AnnouncementList from '../components/HomePage/AnnouncementList.vue'
</script>

<style scoped>
#homepagetitle{
    font-family:Arial;
    font-size:30px;
    font-weight:400;
}

.card-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
}
.box-card{
    margin: 10px;
}
</style>
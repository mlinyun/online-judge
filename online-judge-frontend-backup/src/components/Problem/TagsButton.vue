<template>
    <h2>标签：</h2>
    <div>
        <el-button 
            v-for="(name,index) in store.state.ProblemTags"
            :key="index"
            type="default"
        >
        {{ name }}</el-button>
    </div>

</template>

<script setup>
import store from '../../store'

</script>
<template>
    <el-form
      ref="formRef"
      :model="dynamicValidateForm"
      label-width="120px"
      class="demo-dynamic"
    >
        <el-form-item
            v-for="(domain, index) in dynamicValidateForm.domains"
            :key="domain.key"
            :label="'测试点' + (index+1)"
            :prop="'domains.' + index + '.value'"
        >
            <el-input v-model="domain.value" type="textarea" :autosize="{ minRows: 1, maxRows:10 }"/>
            <el-button class="mt-2" @click.prevent="removeDomain(domain)"
            >删除</el-button
            >
        </el-form-item>
        <el-form-item>
            <el-button @click="addDomain">增加新测试点</el-button>
        </el-form-item>
    </el-form>
</template>
  
<script lang="ts" setup>
import { reactive, ref } from 'vue'
  
const dynamicValidateForm = reactive<{
    domains: DomainItem[]
}>({
    domains: [

    ],
})

interface DomainItem {
    key: number
    value: string
}
// --------------接口-----------------------
// 提交表单
const submitForm = () => {
    return dynamicValidateForm.domains
}
// 设置表单
function setForm(info:Array<string>)
{
    for(var i = 0; i < info.length;i++){
        dynamicValidateForm.domains.push({
            key: Date.now(),
            value: info[i]
        })
    }
}
defineExpose({
    submitForm,
    setForm
})
// -------------------------------------------
// 删除某一项
const removeDomain = (item: DomainItem) => {
    const index = dynamicValidateForm.domains.indexOf(item)
    if (index !== -1) {
        dynamicValidateForm.domains.splice(index, 1)
    }
}

// 添加某一项
const addDomain = () => {
    dynamicValidateForm.domains.push({
        key: Date.now(),
        value: '',
    })
}
</script>
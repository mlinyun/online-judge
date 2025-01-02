<template>
    <el-tag
        v-for="tag in dynamicTags"
        :key="tag"
        class="mx-1"
        closable
        :disable-transitions="false"
        @close="handleClose(tag)"
    >
        {{ tag }}
    </el-tag>
    <el-input
        v-if="inputVisible"
        ref="InputRef"
        v-model="inputValue"
        class="ml-1 w-20"
        size="small"
        @keyup.enter="handleInputConfirm"
        @blur="handleInputConfirm"
    />
    <el-button v-else class="button-new-tag ml-1" size="small" @click="showInput">
        + New Tag
    </el-button>
</template>
  
<script lang="ts" setup>
import { nextTick, ref } from 'vue'
import { ElInput } from 'element-plus'

const inputValue = ref('')
const dynamicTags = ref(['默认'])
const inputVisible = ref(false)
const InputRef = ref<InstanceType<typeof ElInput>>()

// --------------接口---------------------
// 返回数据
function GetTagsData()
{
    return dynamicTags.value
}
// 设置数据
function SetTagsData(Info:Array<string>)
{
    dynamicTags.value.splice(0)
    for(var i = 0; i < Info.length;i++)
    {
        dynamicTags.value.push(Info[i])
    }
}
// -----------------------------------
defineExpose({
    GetTagsData,
    SetTagsData
})
const handleClose = (tag: string) => {
    dynamicTags.value.splice(dynamicTags.value.indexOf(tag), 1)
}

const showInput = () => {
    inputVisible.value = true
    nextTick(() => {
        InputRef.value!.input!.focus()
    })
}

const handleInputConfirm = () => {
    if (inputValue.value) {
        dynamicTags.value.push(inputValue.value)
    }
    inputVisible.value = false
    inputValue.value = ''
}
</script>
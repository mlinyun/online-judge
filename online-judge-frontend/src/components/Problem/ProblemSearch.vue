<template>
    <div class="m-4">
        <el-row :gutter="10">
            <el-col :span="4">
                <el-input v-model="searchid" placeholder="ID" maxlength=5 />
            </el-col>
            <el-col :span="7">
                <el-input v-model="searchtitle" placeholder="标题" />
            </el-col>
            <el-col :span="11">
                <el-select
                    v-model="selectvalue"
                    multiple
                    placeholder="标签"
                    style="width: 480px"
                >
                    <el-option
                        v-for="item in options.array"
                        :key="item.value"
                        :label="item.label"
                        :value="item.value"
                    />
                </el-select>
            </el-col>
            <el-col :span="2">
                <el-button type="primary" :icon="Search" @click="searchbutton">搜索</el-button>
            </el-col>
        </el-row>
    </div>
</template>
  
<script setup>
import { ref ,reactive,onMounted} from 'vue'
import store from '../../store'
import { ElMessage } from 'element-plus'
import { Search} from '@element-plus/icons-vue'

// 保存搜索的标签内容
const options = reactive({array:[]})

const pointmessage = ref('')

const selectvalue = ref([])

const searchid = ref('')
const searchtitle = ref('')

const props = defineProps(['SearchProblemSet'])
function GetSearchInfo()
{
    let Info={
        Id:searchid.value,
        Title:searchtitle.value,
        Tags:selectvalue.value
    }
    return Info
}

function searchbutton()
{
    props.SearchProblemSet()
}

function SetTags()
{
    options.array.splice(0)
    for(var i = 0; i < store.state.ProblemTags.length; i++){
        options.array.push({value:store.state.ProblemTags[i],label:store.state.ProblemTags[i]})
    }
}
onMounted(()=>{
    SetTags()
})

defineExpose({
    GetSearchInfo
})
// 发送成功消息
const SuccessMessage = () => {
    ElMessage({
        showClose: true,
        message: pointmessage.value,
        type: 'success',
    })
}
// 发送警告消息
const WaringMessage = () => {
    ElMessage({
        showClose: true,
        message: pointmessage.value,
        type: 'warning',
    })
}
// 发送错误消息
const ErrorMessage = () => {
    ElMessage({
        showClose: true,
        message: pointmessage.value,
        type: 'error',
    })
}
</script>
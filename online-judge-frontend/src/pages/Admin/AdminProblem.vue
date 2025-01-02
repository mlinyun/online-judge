<template>
    <h1>题目</h1>
    <el-button type="primary" @click="AddProblem">添加题目</el-button>
    <el-table :data="problemsetdata.array" style="width: 100%" border>
        <el-table-column prop="ProblemId" label="ID" width="180" />
        <el-table-column prop="Title" label="Title" width="180" />
        <el-table-column prop="SubmitNum" label="提交次数" width="180"/>
        <el-table-column prop="ACNum" label="通过次数" />
        <el-table-column label="操作">
            <template #default="scope">
                <el-button size="small" @click="handleCheck(scope.row)">查看</el-button>
                <el-button size="small" @click="handleEdit(scope.row)">编辑</el-button>
                <el-button size="small" type="danger"
                @click="handleDelete(scope.row)">删除</el-button>
            </template>
        </el-table-column>
    </el-table>

    <div class="demo-pagination-block">
        <el-pagination
        v-model:current-page="currentPage"
        v-model:page-size="pageSize"
        :page-sizes="[20, 40, 60]"
        :small="small"
        :disabled="disabled"
        :background="background"
        layout="total, sizes, prev, pager, next, jumper"
        :total="totalsize"
        @size-change="handleSizeChange"
        @current-change="handleCurrentChange"
        />
    </div>
</template>

<script setup>
import service from '../../axios'
import store from '../../store'
import {reactive,ref,onMounted} from 'vue'
import { useRouter} from 'vue-router'
import { ElMessage } from 'element-plus'
const router = useRouter()
const pointmessage = ref('')

let currentPage = ref(1) // 当前页数
let pageSize = ref(20) // 当前页的数量
let totalsize = ref(20)
const small = ref(false)
const background = ref(false)
const disabled = ref(false)

// --------------------------------------
// 添加题目
function AddProblem()
{
    router.push({
        name: "ProblemEditor",
        query: { ProblemId: "0" ,edittype:"Insert"}
    });
}
// 查看题目
function handleCheck(row){
    router.push({
        name: "Problem",
        query: { ProblemId: row.ProblemId }
    });
}
// 编辑题目
function handleEdit(row){
    console.log('点击编辑',row)
    router.push({
        name: "ProblemEditor",
        query: { ProblemId: row.ProblemId ,edittype:"Update"}
    });
}
// 删除题目
function handleDelete(row){
    console.log('点击删除',row)
    service.delete(`/api/problem`,{
        params:{
            ProblemId:row.ProblemId
        }
    }).then(
        response => {
            console.log('请求成功了',response.data)
            if(response.data.Result=="Success"){
                pointmessage.value = "删除成功"
                SuccessMessage()
                router.go(0)
            }
        },
        error => {
            console.log('请求失败了',error.data)
        }
    )
}
// -------------------------------------------------------
const handleSizeChange = (val) => {
    console.log(`${val} items per page`)
    pageSize.value = val;
    GetProblemSetInfo()
}
const handleCurrentChange = (val) => {
    console.log(`current page: ${val}`)
    currentPage.value = val;
    GetProblemSetInfo()
}
// 题目信息列表
let problemsetdata = reactive({'array':[]})

function GetProblemSetInfo(){
    service.get(`/api/problemlist/admin`,{
        params: {
            Page : currentPage.value,
            PageSize : pageSize.value
        },
    }).then(
        response => {
            console.log('请求成功了',response.data)
            problemsetdata.array = response.data.ArrayInfo
            totalsize.value = Number(response.data.TotalNum)
        },
        error => {
            console.log('请求失败了',error.data)
        }
    )
}

onMounted(()=>{
    GetProblemSetInfo()
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

<style scoped>
.demo-pagination-block + .demo-pagination-block {
  margin-top: 10px;
}
.demo-pagination-block .demonstration {
  margin-bottom: 16px;
}
</style>
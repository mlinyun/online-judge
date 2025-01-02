<template>
    <el-card class="box-card">
        <div>
            <StatusRecordSearch ref="statusrecordsearch"
                :SearchStatusRecordList="SearchStatusRecordList"
                v-show="searchshow"
                id="statusrecordsearch"
            ></StatusRecordSearch>
        </div>
        <el-table :data="statusrecorddata.array" style="width: 100%" @cell-click="(row, column, cell, event)=>statusrecordclick(row, column, cell, event)">
            <el-table-column prop="_id" label="ID" width="150" />
            <el-table-column prop="UserNickName" label="User" width="150" />
            <el-table-column prop="ProblemTitle" label="Title" width="300"/>
            <el-table-column prop="Status" label="Status" width="200">
                <template #default="scope">
                    <el-tag size="medium"
                            :type="ChangeStatusToType(scope.row.Status)"
                            disable-transitions
                            hit>
                        {{ ChangeStatusToTitle(scope.row.Status) }}
                    </el-tag>
                </template>
            </el-table-column>
            <el-table-column prop="Language" label="语言" width="100"/>
            <el-table-column prop="RunTime" label="RunTime" width="120"/>
            <el-table-column prop="RunMemory" label="Memory" width="120"/>
            <el-table-column prop="Length" label="Length" width="120"/>
            <el-table-column prop="SubmitTime" label="SubmitTime" />
        </el-table>
        <ViewStatus ref="viewstatusdialog"></ViewStatus>
        <div class="demo-pagination-block">
            <el-pagination
            v-model:current-page="currentPage"
            v-model:page-size="pageSize"
            :page-sizes="[15, 20, 30, 40]"
            :small="small"
            :disabled="disabled"
            :background="background"
            layout="total, sizes, prev, pager, next, jumper"
            :total="totalsize"
            @size-change="handleSizeChange"
            @current-change="handleCurrentChange"
            />
        </div>
    </el-card>
    
</template>

<script setup>
import {reactive,ref,onMounted} from 'vue'
import { useRoute,onBeforeRouteUpdate } from 'vue-router'
import ViewStatus from '../components/Dialog/ViewStatus.vue'
import StatusRecordSearch from '../components/StatusRecord/StatusRecordSearch.vue'
import service from '../axios'

const route = useRoute()

const viewstatusdialog = ref()
const statusrecordsearch = ref()
const searchshow = ref(true)

const searchinfo = reactive({
    ProblemId: '',
    UserId : '',
    ProblemTitle:'',
    Status:'',
    Language:''
})

let currentPage = ref(1) // 当前页数
let pageSize = ref(15) // 当前页的数量
let totalsize = ref(0)
const small = ref(false)
const background = ref(false)
const disabled = ref(false)

const handleSizeChange = (val) => {
    pageSize.value = val;
    GetStatusRecordInfo()
}
const handleCurrentChange = (val) => {
    currentPage.value = val;
    GetStatusRecordInfo()
}

// 题目信息列表
let statusrecorddata = reactive({'array':[]})

function GetStatusRecordInfo(){

    service.get(`/api/statusrecordlist`,{
        params: {
            SearchInfo : JSON.stringify(searchinfo),
            Page : currentPage.value,
            PageSize : pageSize.value
        },
    }).then(
        response => {
            console.log('请求成功了',response.data)
            statusrecorddata.array = response.data.ArrayInfo
            totalsize.value = Number(response.data.TotalNum)
        },
        error => {
            console.log('请求失败了',error.data)
        }
    )
}

function SearchStatusRecordList()
{
    let Info = statusrecordsearch.value.GetSearchInfo()
    searchinfo.ProblemId = Info.ProblemId
    searchinfo.ProblemTitle = Info.ProblemTitle
    searchinfo.Status = Info.Status
    searchinfo.Language = Info.Language
    GetStatusRecordInfo()
}
function ChangeStatusToType(status)
{
    if(status == 0) return "info"
    else if(status == 2) return "success"
    else if(status == 3)return "error"
    else return "warning"
}

function ChangeStatusToTitle(status)
{
    if(status == 0)
        return "Pending"
    else if(status == 1)
        return "Compile Error"
    else if(status == 2)
        return "Accepted"
    else if(status == 3)
        return  "Wrong Answer"
    else if(status == 4)
        return "Runtime Error"
    else if(status == 5)
        return "Time Limit Exceeded"
    else if(status == 6)
        return "Memory Limit Exceeded"
    else if(status == 7)
        return "System Error"
}
// 点击单元格
function statusrecordclick(row, column, cell, event) {
    viewstatusdialog.value.opendialog(row._id)
}

function InitSearchInfo(ProblemId,UserId)
{
    searchinfo.ProblemId = ''
    searchinfo.ProblemTitle = ''
    searchinfo.Status = ''
    searchinfo.UserId = ''
    searchinfo.Language = ''
    searchshow.value = true
    if(ProblemId!=null){
        searchinfo.ProblemId = ProblemId
        searchshow.value = false
    }
        

    if(UserId!=null){
        searchinfo.UserId = UserId
        searchshow.value = false
    }
        
}
// 检测组件是否更新
onBeforeRouteUpdate(to=>{
    InitSearchInfo(to.query.ProblemId,to.query.UserId)
    GetStatusRecordInfo()
})
// 组件初始化
onMounted(()=>{
    InitSearchInfo(route.query.ProblemId,route.query.UserId)
    GetStatusRecordInfo()
})

</script>


<style scoped>
#statusrecordsearch{
    margin: 10px;
}
.demo-pagination-block{
    margin-top: 10px;
    display: flex;
    justify-content: center;
    align-items: center
}

.box-card{
    margin: 10px;
}
</style>
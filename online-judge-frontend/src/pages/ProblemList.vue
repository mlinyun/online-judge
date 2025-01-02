<template>
    <el-row>
        <el-col :span="18">
            <el-card class="box-card">
                <ProblemSearch ref="problemsearch" :SearchProblemSet="SearchProblemSet" id="problemsearch"></ProblemSearch>
            <el-table :data="problemsetdata.array"
                @cell-click="problemclick"
                @cell-mouse-enter="changepiechart"
                >
                <el-table-column label="状态" width="100">
                    <template #default="scope">
                        <el-icon v-if="store.state.Solves.includes(scope.row.ProblemId)" >
                            <Check />
                        </el-icon>
                    </template>
                </el-table-column>
                <el-table-column prop="ProblemId" label="ID" width="50" />
                <el-table-column prop="Title" label="题目" width="300" />
                <el-table-column
                    prop="Tags"
                    label="标签"
                    width="400"
                    >
                    <template #default="scope">
                        <el-tag
                        v-for="(name,index) in scope.row.Tags"
                        :key="index"
                        disable-transitions
                        size="medium"
                        >{{ name }}</el-tag>
                    </template>
                </el-table-column>
                <el-table-column prop="ACNum" label="通过次数" width="100"/>
                <el-table-column prop="SubmitNum" label="提交次数" width="100"/>
            </el-table>
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
        </el-col>
        <el-col :span="6">
            <el-card class="box-card">
                <PieChart ref="piechart"></PieChart>
            </el-card>
            <el-card class="box-card">
                <TagsButton></TagsButton>
            </el-card>
        </el-col>
    </el-row>

</template>

<script setup>
import service from '../axios'
import store from '../store'
import {reactive,ref,onMounted} from 'vue'
import { useRouter} from 'vue-router'
import PieChart from '../components/Chart/PieChart.vue'
import ProblemSearch from '../components/Problem/ProblemSearch.vue'
import TagsButton from '../components/Problem/TagsButton.vue'

const router = useRouter()

// 子组件
const piechart = ref()
const problemsearch = ref()

let currentPage = ref(1) // 当前页数
let pageSize = ref(15) // 当前页的数量
let totalsize = ref(0)
const small = ref(false)
const background = ref(false)
const disabled = ref(false)
const searchinfo = reactive({
    Id:'',
    Title:'',
    Tags:[]
})

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
    service.get(`/api/problemlist`,{
        params: {
            SearchInfo : JSON.stringify(searchinfo),
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
// 子组件的搜索按钮调用父组件的方法
function SearchProblemSet()
{
    let Info = problemsearch.value.GetSearchInfo()
    searchinfo.Id = Info.Id
    searchinfo.Title = Info.Title
    searchinfo.Tags = Info.Tags

    GetProblemSetInfo()
}
// 跳转函数
function problemclick(row, column, cell, event) {
    router.push({
        name: "Problem",
        query: { ProblemId: row.ProblemId }
    });
}
function changepiechart(row, column, cell, event){
    let info = {
        CENum : row.CENum,
        ACNum : row.ACNum,
        WANum : row.WANum,
        RENum : row.RENum,
        TLENum: row.TLENum,
        MLENum: row.MLENum,
        SENum : row.SENum,
        Title : row.Title
    }
    piechart.value.SetDataInfo(info)
}
onMounted(()=>{
    GetProblemSetInfo()
})
        
</script>

<style scoped>
#problemsearch{
    margin-bottom: 10px;
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
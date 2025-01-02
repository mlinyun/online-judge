<template>
    <el-card class="box-card">
        <center>
            <h1>{{ data.problemid }}.{{ data.title }}</h1>
        </center>
        <el-row>
            <el-col :span="20">
                <div class="content">
                    <v-md-preview :text="data.content"></v-md-preview>
                </div>
                <div :style="{boxShadow: `var(--el-box-shadow)`}">
                    <MonacoEditor ref="monacoeditor" :ProblemId=$route.query.ProblemId></MonacoEditor>
                </div>
                <br>
                <el-button type="primary" @click="SubmitCode()" :disabled="submitbutton" :loading="submitloading">提交</el-button>
                <div id="resultdiv">
                    <h4>代码运行状态： {{ ResultMsg() }}</h4>
                    <h4>错误提示： {{ reason }}</h4>
                    <h3>Tip：可以通过查看测评记录查看每个测试点详细信息</h3>
                </div>
            </el-col>
            <el-col :span="4">
                <div class="demo-collapse">
                    <el-collapse v-model="activeNames" @change="handleChange">
                        <el-collapse-item title="作者" name="1">
                            <div>
                                {{ data.nickname }}
                            </div>
                        </el-collapse-item>
                        <el-collapse-item title="时间限制" name="2">
                            <div>
                                {{ data.timelimit }} MS
                            </div>
                        </el-collapse-item>
                        <el-collapse-item title="空间限制" name="3">
                            <div>
                                {{ data.memorylimit }} MB
                            </div>
                        </el-collapse-item>
                        <el-collapse-item title="标签" name="4">
                            <div>
                                <el-tag v-for="(tag,index) in data.tags" :key="index">{{ tag }}</el-tag>
                            </div>
                        </el-collapse-item>
                    </el-collapse>
                </div>
                <br>
                <el-button type="primary" @click="ClickStatusRecord">提交记录</el-button>
                <el-button type="primary" @click="ClickSolution">题解</el-button>
                <el-button type="primary" @click="ClickDiscuss">讨论</el-button>
            </el-col>
        </el-row>
    </el-card>
</template>

<script setup>
import MonacoEditor from '../components/Problem/MonacoEditor.vue'
import service from '../axios'
import { ref,onMounted,reactive } from "vue"
import store from '../store'
import { useRoute,useRouter} from 'vue-router'


let submitbutton = ref(false)
let submitloading = ref(false)

const monacoeditor = ref()
const route = useRoute()
const router = useRouter()
// 创建题目描述，是否显示，获取题目数据
const data = reactive({
    problemid:'',
    title:'',
    content:'',
    timelimit:0,
    memorylimit:0,
    judgenum:0,
    submitnum:0,
    acnum:0,
    nickname:'',
    tags:[]
})

let result = ref(-1);
let reason = ref("");
// 请求当前题目详情
function GetProblem() {
    service
    .get(`/api/problem`, {
        params: {
            ProblemId: data.problemid,
        },
    })
    .then(
        (response) => {
            if(response.data.Result == "Success"){
                console.log("请求成功了！！！",response.data);
                SetDataInfo(response.data)
            }else{
                console.log('出错啦！')
            }
            
        },
        (error) => {
            console.log("请求失败了！！！");
            console.log(error.data);
        }
    );
}
function SetDataInfo(Info)
{
    data.title = Info.Title
    data.content = Info.Description
    data.timelimit = Info.TimeLimit
    data.memorylimit = Info.MemoryLimit
    data.judgenum = Info.JudgeNum
    data.submitnum = Info.SubmitNum
    data.acnum = Info.ACNum
    data.nickname = Info.UserNickName
    data.tags = Info.Tags
}
// 提交代码
function SubmitCode() {
    submitloading.value = true
    result.value = 0

    service
    .post(`/api/problemcode`, { 
        ProblemId: data.problemid,
        UserId:store.state.UserId,
        UserNickName:store.state.NickName,
        Code: monacoeditor.value.GetCode(),
        Language:monacoeditor.value.GetLanguage(),
        TimeLimit: data.timelimit,
        MemoryLimit: data.memorylimit,
        JudgeNum: data.judgenum,
        ProblemTitle: data.title
    })
    .then(
        (response) => {
            submitloading.value = false
            if(response.data.Result == "Success"){
                console.log("提交成功了！！！", response.data);
                result.value = Number(response.data.Status);
                reason.value = response.data.ComplierInfo;
                // 如果第一次AC
                if(response.data.IsFirstAC){
                    store.state.Solves.push(data.problemid)
                    console.log('solves:',store.state.Solves)
                }
            }else{
                console.log('提交失败',response.data)
            }
        },
        (error) => {
            console.log("提交失败了！！！");
            console.log(error.data);
            submitloading.value = false
        }
    );
}
function ResultMsg()
{
    if(result.value == -1)return ""
    else if(result.value == 0)return "Pending"
    else if(result.value == 1)return "Compile Error"
    else if(result.value == 2)return "Accepted"
    else if(result.value == 3)return "Wrong Answer"
    else if(result.value == 4)return "Runtime Error"
    else if(result.value == 5)return "Time Limit Exceeded"
    else if(result.value == 6)return "Memory Limit Exceeded"
    else if(result.value == 7)return "System Error"
}
function ClickStatusRecord()
{
    router.push({
        name:"StatusRecord",
        query:{
            ProblemId:route.query.ProblemId
        }
    })
}
function ClickSolution()
{
    router.push({
        name:"SolutionList",
        query:{
            ParentId:route.query.ProblemId
        }
    })
}

function ClickDiscuss()
{
    router.push({
        name:"DiscussList",
        query:{
            ParentId:route.query.ProblemId
        }
    })
}
onMounted(()=>{
    data.problemid = route.query.ProblemId
    GetProblem();
    if(store.state.UserId == '0')submitbutton.value = true
})
</script>

<style scoped>
.content{
    width: 1200px
}
.box-card{
    margin: 10px;
}
</style>
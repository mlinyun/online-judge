<template>
    <el-card class="box-card">
        <el-row>
            <el-col :span="24">
                <h5>标题：</h5>
                <el-input v-model="title" placeholder="标题" />
                <h5>题目描述（可全屏编辑）</h5>
                <MarkDownEditor ref="markdowneditor"></MarkDownEditor>
            </el-col>
        </el-row>
        <el-row>
            <el-col :span="12">
                <h7>输入</h7>
                <TestDataFrom ref="intestdatafrom"></TestDataFrom>
            </el-col>
            <el-col :span="12">
                <h7>输出</h7>
                <TestDataFrom ref="outtestdatafrom"></TestDataFrom>
            </el-col>
        </el-row>
        <el-row>
            <el-col :span="24">
                <h5>SPJ文件（如果要启用请设置开关开启）</h5>
                <el-switch v-model="isspj" />
                <el-input v-model="spjdata" placeholder="spj文件"  type="textarea" 
                :autosize="{ minRows: 10, maxRows: 30 }"
                />

                <h5>时间复杂度：单位MS</h5>
                <el-input v-model="timelimit" placeholder="时间复杂度" />

                <h5>空间复杂度：单位MB</h5>
                <el-input v-model="memorylimit" placeholder="空间复杂度" />

                <h5>标签</h5>
                <ProblemTags ref="problemtags"></ProblemTags>

                <h5>作者：</h5>
                <el-input v-model="usernickname" placeholder="作者" disabled/>

                <el-button type="primary" @click="Submit()">提交</el-button>
            </el-col>
        </el-row>
    </el-card>
</template>

<script lang="ts" setup>
import {  reactive, ref ,onMounted} from 'vue'
import TestDataFrom from '../../components/Problem/TestDataFrom.vue'
import ProblemTags from '../../components/Problem/ProblemTags.vue'
import MarkDownEditor from '../../components/MarkDownEditor.vue'
import store from '../../store'
import { ElMessage } from 'element-plus'
import service from '../../axios'
import { useRouter,useRoute } from 'vue-router'

const router = useRouter()
const route = useRoute()
const markdowneditor = ref()

// 组件
const intestdatafrom = ref()
const outtestdatafrom = ref()
const problemtags = ref()
// 提示信息
const pointmessage = ref('')

// 传递的信息
const problemid = ref('')
const edittype = ref('')

const title = ref('')

const spjdata = ref(`#include <stdio.h>
#include <fstream>
using namespace std;

int main(int argc, char *argv[])
{
    FILE *f_out = fopen(argv[2], "r");  // 测试输出
    FILE *f_user = fopen(argv[3], "r"); // 用户输出
    int ret = 0;                        // AC=0, WA=1

    int a, b;
    // 获取输入输出
    fscanf(f_out, "%d", &a);
    fscanf(f_user, "%d", &b);
    // 比较答案
    if (a == b)
        ret = 0;
    else
        ret = 1;

    fclose(f_out);
    fclose(f_user);

    return ret; // 返回结果，返回值为0时，答案正确，为1时，答案错误
}`)
const isspj = ref(false)
const timelimit = ref(2000)
const memorylimit = ref(128)
const usernickname = ref('')

interface TestInfo{
    in : string
    out : string
}
interface ProblemData {
    ProblemId:string
    EditType:string
    Title: string
    Description: string
    JudgeNum: number
    IsSPJ : boolean
    SPJ : string
    UserNickName : string
    TimeLimit : number
    MemoryLimit : number
    Tags : Array<string>
    TestInfo : Array<TestInfo>
}

function Submit()
{
    let indata = intestdatafrom.value.submitForm()
    let outdata = outtestdatafrom.value.submitForm()
    let tags = problemtags.value.GetTagsData()
    if(title.value.length < 2){
        pointmessage.value = "题目标题太短！"
        WaringMessage()
        return
    }
    if(indata.length != outdata.length){
        pointmessage.value = "输入输出数据的数量不一致！"
        WaringMessage()
        return
    }
    let testinfo =<Array<TestInfo>>[]
    for(var i = 0;i<indata.length;i++)
    {
        const info = <TestInfo>{}
        info.in = indata[i].value
        info.out = outdata[i].value
        testinfo.push(info)
    }

    let tagsinfo = <Array<string>>[]
    for(var i = 0;i<tags.length;i++){
        tagsinfo.push(tags[i])
    }

    const data=<ProblemData>{}
    data.EditType = edittype.value
    data.ProblemId = problemid.value
    data.Title = title.value
    data.Description = markdowneditor.value.GetContent()
    data.JudgeNum = indata.length
    data.IsSPJ = isspj.value
    data.SPJ = spjdata.value
    data.UserNickName = usernickname.value
    data.TimeLimit = timelimit.value
    data.MemoryLimit = memorylimit.value
    data.Tags = tagsinfo
    data.TestInfo = testinfo

    service
        .post(`/api/problem/edit`, {
            datainfo:data
        })
        .then(
        (response) => {
            console.log("提交成功了！！！", response.data);
            if(response.data.Result == "Success"){
                pointmessage.value = "题目插入成功！"
                SuccessMessage()
                router.go(-1)
            }else{
                pointmessage.value = response.data.Reason
                WaringMessage()
                return
            }
        },
        (error) => {
            console.log(error.data);
            pointmessage.value = "提交失败,可能网络出现问题！"
            ErrorMessage()
            return
        }
    )
}
function GetServerInfo()
{   
    service
        .get(`/api/problem/select`, {
            params: {
                ProblemId : problemid.value
            },
        })
        .then(
        (response) => {
            if(response.data.Result == "Success"){
                pointmessage.value = "题目获取成功！"
                SuccessMessage()
                handledata(response.data)
            }else{
                pointmessage.value = response.data.Reason
                WaringMessage()
                return
            }
        },
        (error) => {
            console.log(error.data);
            pointmessage.value = "提交失败,可能网络出现问题！"
            ErrorMessage()
            return
        }
    )
}
//Json(Result,Reason,_id,Title,Description,TimeLimit,MemoryLimit,IsSPJ,JudgeNum,Tags,TestInfo[{in,out}])
function handledata(Info:any)
{
    title.value = Info.Title
    markdowneditor.value.SetContent(Info.Description)
    timelimit.value = Number(Info.TimeLimit)
    memorylimit.value = Number(Info.MemoryLimit)
    isspj.value = Info.IsSPJ
    if(isspj.value)spjdata.value = Info.SPJ
    usernickname.value = Info.UserNickName
    let tagsinfo = <Array<string>>[]
    for(var i = 0; i < Info.Tags.length;i++){
        tagsinfo.push(Info.Tags[i])
    }
    problemtags.value.SetTagsData(tagsinfo)

    let testdatain = <Array<string>>[]
    let testdataout = <Array<string>>[]
    for(var i =0;i<Number(Info.JudgeNum);i++){
        testdatain.push(Info.TestInfo[i].in)
        testdataout.push(Info.TestInfo[i].out)
    }

    intestdatafrom.value.setForm(testdatain)
    outtestdatafrom.value.setForm(testdataout)
}
onMounted(()=>{
    problemid.value = String(route.query.ProblemId)
    edittype.value = String(route.query.edittype)
    if(edittype.value == "Update"){ // 如果是更新，需要向服务器获取数据,并更新数据
        GetServerInfo()
    }else if(edittype.value == "Insert"){ // 否则是添加
        usernickname.value = store.state.NickName
    }
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

<style lang="scss">
.el-row {
  margin-bottom: 20px;
}
.el-row:last-child {
  margin-bottom: 0;
}
.el-col {
  border-radius: 4px;
}
.box-card{
    margin: 10px;
}
.grid-content {
  border-radius: 4px;
  min-height: 36px;
}
</style>
<template>  
    <el-dialog id="dialog" v-model="dialogVisible" title="测评详情" :before-close="closedialog">
        <el-alert 
            :title="alerttitle"
            :type="alerttype"
            :description="alertdescription"
            :closable="false"
            show-icon
            :show-close="false">
        </el-alert>
        <MonacoView ref="monacoview"></MonacoView>
        <el-collapse>
            <el-collapse-item 
                v-for="(info,index) in testinfo.array"
                :key="index"
                :class="ChangeStatusToCollapseClass(info.Status)"
                >
                <template #title>
                    #Test{{ index+1 }}  {{ ChangeStatusToTitle(info.Status) }}
                </template>
                <div>
                    标准输入：
                    <el-input
                        v-model="info.StandardInput"
                        type="textarea"
                        placeholder="Please input"
                        :autosize="{ minRows: 1, maxRows:10 }"
                    />
                    标准输出：
                    <el-input
                        v-model="info.StandardOutput"
                        type="textarea"
                        placeholder="Please input"
                        :autosize="{ minRows: 1, maxRows:10 }"
                    />
                    你的输出：
                    <el-input
                        v-model="info.PersonalOutput"
                        type="textarea"
                        placeholder="Please input"
                        :autosize="{ minRows: 1, maxRows:10 }"
                    />
                    运行时间：{{ info.RunTime }}
                    运行内存：{{ info.RunMemory }}
                </div>
            </el-collapse-item>
        </el-collapse>
    </el-dialog>
</template>
  
<script setup>
import { reactive, ref } from 'vue'
import MonacoView from '../Problem/MonacoView.vue'
import service from '../../axios'
// 是否打开
const dialogVisible = ref(false)

const monacoview = ref()

const alerttype = ref('success')
const alerttitle = ref('')
const alertdescription = ref('')

const testinfo = reactive({array:[]})

function opendialog(submitid){
    dialogVisible.value = true;
    console.log('submitid',submitid)
    service
        .get(`/api/statusrecord`, {
            params:{
                SubmitId:submitid
            }
        })
        .then(
        (response) => {
            console.log(response.data)
            SetDataInfo(response.data)
        },
        (error) => {
            console.log(error.data);
        }
    );
}

function SetDataInfo(statusinfo)
{
    testinfo.array = statusinfo.TestInfo
    alertdescription.value = statusinfo.ComplierInfo
    // 显示代码
    let Info={
        Code:statusinfo.Code,
        Language:statusinfo.Language
    }
    monacoview.value.SetData(Info)
    if(statusinfo.Status == 2) alerttype.value = "success"
    else if(statusinfo.Status == 3)alerttype.value = "error"
    else alerttype.value = "warning"

    alerttitle.value = ChangeStatusToTitle(statusinfo.Status)
}

function closedialog()
{
    alerttitle.value = ''
    alertdescription.value = ''
    let Info={
        Code:'',
        Language:'C++'
    }
    monacoview.value.SetData(Info)
    testinfo.array=[]

    dialogVisible.value = false
}
function ChangeStatusToTitle(status)
{
    if(status == 0)
        return "Pending & Judging"
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
function ChangeStatusToCollapseClass(status)
{
    if(status == 2) return "el-collapse-success"
    else if(status == 3)return "el-collapse-error"
    else return "el-collapse-warning"
}

defineExpose({
    opendialog
})
</script>
<style scoped>
#dialog{
    width:800px;
}
.el-button--text {
margin-right: 15px;
}
.el-select {
width: 300px;
}
.el-input {
width: 300px;
}
.dialog-footer button:first-child {
margin-right: 10px;
}

.el-collapse-error{
  background-color:#FEF0F0; 
}

.el-collapse-success{
  background-color:#F0F9EB; 
}

.el-collapse-warning{
  background-color:#FDF6EC; 
}
</style>
<template>
    <el-dialog v-model="dialogFormVisible" title="注册">
        <el-form :model="form">
            <el-form-item label="昵称" :label-width="formLabelWidth">
                <el-input v-model="form.nickname"
                maxlength="15" show-word-limit
                autocomplete="off" placeholder="不少于2个字符的昵称 【必填】"/>
            </el-form-item>
            <el-form-item label="账号" :label-width="formLabelWidth">
                <el-input v-model="form.account" 
                maxlength="20" show-word-limit
                autocomplete="off" placeholder="不少于6个字符的账号 【必填】"/>
            </el-form-item>
            <el-form-item label="密码" :label-width="formLabelWidth">
                <el-input v-model="form.password"
                maxlength="20" show-word-limit show-password = true
                autocomplete="off" placeholder="不少于6个字符的密码 【必填】"/>
            </el-form-item>
            <el-form-item label="确认密码" :label-width="formLabelWidth">
                <el-input v-model="form.confirmpassword"
                maxlength="20" show-word-limit show-password = true
                autocomplete="off" placeholder="确认密码 【必填】"/>
            </el-form-item>
            <el-form-item label="学校" :label-width="formLabelWidth">
                <el-input v-model="form.school"
                maxlength="15" show-word-limit
                autocomplete="off" placeholder="你的学校名称 【选填】"/>
            </el-form-item>
            <el-form-item label="专业" :label-width="formLabelWidth">
                <el-input v-model="form.major"
                maxlength="15" show-word-limit
                autocomplete="off" placeholder="你的专业名称 【选填】"/>
            </el-form-item>
            <el-form-item label="个人简介" :label-width="formLabelWidth">
                <el-input v-model="form.personalprofile"
                maxlength="25" show-word-limit
                autocomplete="off" placeholder="你的个人简介 【选填】"/>
            </el-form-item>
        </el-form>
        <template #footer>
            <span class="dialog-footer">
            <el-button @click="dialogFormVisible = false">取消</el-button>
            <el-button type="primary" @click="SubmitInfo">确认</el-button>
            </span>
        </template>
    </el-dialog>
</template>

<script lang="ts" setup>
import { reactive, ref } from 'vue'
import { ElMessage } from 'element-plus'
import service from '../../axios'
const pointmessage = ref('')
// 对话框是否关闭
const dialogFormVisible = ref(false)
// 输入框的长度
const formLabelWidth = '200px'

const form = reactive({
    nickname:'',
    account:'',
    password:'',
    confirmpassword:'',
    personalprofile:'',
    school:'',
    major:''
})
// 打开对话框
function open(){
    dialogFormVisible.value = true;
}

// 提交表单
function SubmitInfo(){
    //判断必填字段是否为空
    if (
        !form.nickname ||
        !form.account  ||
        !form.password ||
        !form.confirmpassword
    ) {
        pointmessage.value = "必填字段不能为空！"
        WaringMessage()
        return
    }
    // 检查昵称长度
    if(form.nickname.length < 2){
        pointmessage.value = "昵称长度太短！"
        WaringMessage()
        return
    }
    // 检查账户长度
    if(form.account.length < 6){
        pointmessage.value = "账户长度太短！"
        WaringMessage()
        return
    }
    // 检查密码长度
    if(form.password.length < 6){
        pointmessage.value = "密码长度太短！"
        WaringMessage()
        return
    }
    // 检查确认密码长度
    if(form.confirmpassword.length < 6){
        pointmessage.value = "确认密码长度太短！"
        WaringMessage()
        return
    }
    // 检查昵称是否符合规范（是否全为空格）
    if(form.nickname.match(/^\s+$/)){
        pointmessage.value = "昵称不符合规范！"
        WaringMessage()
        return               
    }
    // 检查账户是否符合
    if(!checkstr(form.account)){
        pointmessage.value = "账户只能包含字母和数字！"
        WaringMessage()
        return
    }
    if(!checkstr(form.password)){
        pointmessage.value = "密码只能包含字母和数字！"
        WaringMessage()
        return
    }
    // 判断两次密码是否一致
    if(form.password != form.confirmpassword){
        pointmessage.value = "密码和确认密码不一致！"
        WaringMessage()
        return
    }
    /*
        功能：注册用户
        传入：Json(NickName,Account,PassWord,PersonalProfile,School,Major)
        传出：Json(Result,Reason)
    */
    service
        .post(`/api/user/register`, {
            NickName:form.nickname,
            Account:form.account,
            PassWord:form.password,
            PersonalProfile:form.personalprofile,
            School:form.school,
            Major:form.major
        })
        .then(
        (response) => {
            console.log("提交成功了！！！", response.data);
            if(response.data.Result == "Success"){
                pointmessage.value = "恭喜你！注册成功！成为XDOJ的一员！！！"
                SuccessMessage()
                dialogFormVisible.value = false
                // 本地保存账号和密码
                localStorage.setItem('Account',form.account)
                localStorage.setItem('PassWord',form.password)
                return
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
    );
}
// 正则判断字符串是否只包含字母和数字
function checkstr(value:string) {
    var Regx = /^[0-9A-Z]+$/i;
    if (Regx.test(value)) {
        return true;
    }
    else {
        return false;
    }
}
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
defineExpose({
    open
})
</script>
<style scoped>
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
</style>
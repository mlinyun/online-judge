<template>
    <el-dialog v-model="dialogFormVisible" title="登录">
        <el-form :model="form">
            <el-form-item label="账号" :label-width="formLabelWidth">
                <el-input v-model="form.account" 
                maxlength="20"
                autocomplete="off"/>
            </el-form-item>
            <el-form-item label="密码" :label-width="formLabelWidth">
                <el-input v-model="form.password"
                maxlength="20" show-password = true
                autocomplete="off"/>
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
import store from '../../store'
import { useRouter } from 'vue-router'
const router = useRouter()
const pointmessage = ref('')
// 对话框是否关闭
const dialogFormVisible = ref(false)
// 输入框的长度
const formLabelWidth = '200px'
// 信息
const form = reactive({
    account:'',
    password:'',
})
// 打开对话框
function open(){
    dialogFormVisible.value = true;
}

// 提交表单
function SubmitInfo(){
    //判断必填字段是否为空
    if (
        !form.account  ||
        !form.password 
    ) {
        pointmessage.value = "必填字段不能为空！"
        WaringMessage()
        return
    }
    // 检查账户长度
    if(form.account.length < 6){
        pointmessage.value = "账号长度太短！"
        WaringMessage()
        return
    }
    // 检查密码长度
    if(form.password.length < 6){
        pointmessage.value = "密码长度太短！"
        WaringMessage()
        return
    }
    /*
        功能：登录用户
        传入：Json(Account,PassWord)
        传出：Json(Result,Reason,Info(_id,NickName,Avatar,CommentLikes,Solves,Authority))
    */
    service
        .post(`/api/user/login`, {
            Account:form.account,
            PassWord:form.password,
        })
        .then(
        (response) => {
            console.log("提交成功了！！！", response.data);
            if(response.data.Result == "Success"){
                pointmessage.value = "登录成功！！！"
                SuccessMessage()
                dialogFormVisible.value = false

                store.commit('Login',response.data.Info)
                router.push({name:'HomePage'})
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
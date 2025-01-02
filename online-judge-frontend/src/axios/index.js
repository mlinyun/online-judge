import axios from 'axios'
import JSONbig from 'json-bigint'
import store from '../store'
import { ElMessage } from 'element-plus'

// 处理int64精度问题
const service = axios.create({
    timeout: 100000, // request timeout,
    transformResponse: [function (data) {
        try {
            // 如果转换成功则返回转换的数据结果
            return JSONbig.parse(data)
        } catch (err) {
            // 如果转换失败，则包装为统一数据格式并返回
            return {
                data
            }
        }
    }]
});

// 请求拦截器，每次请求都会先执行这里的代码
service.interceptors.request.use(
    function (config) {
        // 请求头携带localStorage里面的token
        config.headers.Authorization = localStorage.getItem('token')

        return config
    },
    function (err) {
        // 对请求错误做些什么
    }
);

// 响应拦截器，每次响应都会先执行这里的代码
service.interceptors.response.use(
    function (response) {
        // 对响应数据做点什么
        return response;
    }, function (error) {
        // 对响应错误做点什么

        if(error.response.status == 401){ // 检测到未登录或登录信息过期
            // 进行提示
            ErrorMessage('暂无权限！请查看是否登录！')
            // 将用户进行退出（删除Token）
            store.commit('Logout') 
        }else if(error.response.status == 400){ // 参数有误
            // 进行提示
            ErrorMessage('请求参数有误！')
        }else if(error.response.status == 500){ // 服务器出错
            // 进行提示
            ErrorMessage('服务器出错啦！')
        }
        
    }
);
// 发送错误消息
const ErrorMessage = (msg) => {
    ElMessage({
        showClose: true,
        message: msg,
        type: 'error',
    })
}

export default service;
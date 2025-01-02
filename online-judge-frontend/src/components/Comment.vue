<template>
    <div class="comment-view">
        <u-comment
            :config="config"
            :show-size="3"
            page
            @submit="submit"
            @like="like"
            @remove="remove"
            @report="report"
            @reply-page="replyPage"
        ></u-comment>
    </div>
    <div class="demo-pagination-block">
        <n-pagination
            v-model:page="currentPage"
            v-model:page-size="pageSize"
            :item-count="TotalNum"
            show-size-picker
            :page-sizes="pageSizes"
            @update:page="handlepage"
            @update:page-size="handlpagesize"
        />
    </div>
</template>

<script lang="ts" setup>
    import store from '../store'
    import service from '../axios'
    import { ElMessage } from 'element-plus'
    import { reactive, ref,onMounted } from 'vue'
    import { NPagination } from 'naive-ui'
    import { useRouter } from 'vue-router'
    import { UToast, ConfigApi, CommentApi, ReplyPageParam, CommentSubmitParam, ReplyApi } from 'undraw-ui'
    // 下载表情包资源emoji.zip https://gitee.com/undraw/undraw-ui/releases
    // static文件放在public下,引入emoji.ts文件可以移动到自定义位置
    import emoji from '../../public/emoji'
    // 父亲ID（讨论ID或者题解ID）
    const props = defineProps(['ParentId','ArticleType'])
    const pointmessage = ref('')

    const router = useRouter()

    const currentPage = ref(1) // 当前页数
    const pageSize = ref(5) // 当前页的数量
    const TotalNum = ref(0) // 总数
    const pageSizes = [
      {
        label: '5 每页',
        value: 5
      },
      {
        label: '10 每页',
        value: 10
      },
      {
        label: '15 每页',
        value: 15
      }
    ]
    // 获取父评论信息
    function getCommentInfo(ParentId:string){
        service
            .get(`/api/comment`, {
            params: {
                Type:"Father",
                ParentId: ParentId,
                Page:currentPage.value,
                PageSize:pageSize.value,
                SonNum:5,
            },
            })
            .then(
            (response) => {
                console.log("请求成功了！！！",response.data);
                TotalNum.value = response.data.TotalNum;
                setCommentInfo(response.data.ArryInfo)
            },
            (error) => {
                console.log("请求失败了！！！");
                console.log(error.data);
            }
        );
    }

    // 本人的信息 id，name，头像，点赞的
    const config = reactive<ConfigApi>({
        user: {
            id: store.state.UserId,
            username: store.state.NickName,
            avatar: store.state.Avatar,
            // 评论id数组 建议:存储方式用户id和文章id和评论id组成关系,根据用户id和文章id来获取对应点赞评论id,然后加入到数组中返回
            likeIds: store.state.CommentLikes
        },
        emoji: emoji,
        comments: []
    })

    function setCommentInfo(Info){
        // 将原数组清空
        config.comments = []
        // 循环获取父评论
        for(var i in Info){
            let fatherinfo = Info[i]
            // 获取父评论
            let Father:CommentApi = {
                id: String(fatherinfo["_id"]),
                parentId: fatherinfo["ParentId"],
                uid: String(fatherinfo["User"][0]["_id"]),
                address:'',
                content: fatherinfo["Content"],
                likes: fatherinfo["Likes"],
                createTime: fatherinfo["CreateTime"],
                user: {
                    username: fatherinfo["User"][0]["NickName"],
                    avatar: fatherinfo["User"][0]["Avatar"],
                    level: 0,
                    homeLink: ''
                },
                reply: null
            }
            // 判断是否有子评论
            if(fatherinfo["Child_Total"] > 0){
                let Son:ReplyApi={
                    total: fatherinfo["Child_Total"],
                    list: []
                }
                // 循环获取子评论
                for(var j in fatherinfo["Child_Comments"]){
                    let soninfo = fatherinfo["Child_Comments"][j]
                    let info = {
                        id: soninfo["_id"],
                        parentId: String(fatherinfo["ParentId"]),
                        uid: String(soninfo["User"][0]["_id"]),
                        address: '',
                        content: soninfo["Content"],
                        likes: soninfo["Likes"],
                        createTime: soninfo["CreateTime"],
                        user: {
                            username: soninfo["User"][0]["NickName"],
                            avatar:soninfo["User"][0]["Avatar"],
                            level: 0,
                            homeLink: ''
                        }
                    }
                    Son.list.push(info)
                }
                Father.reply = Son
            }
            config.comments.push(Father)
            
        }
    }

    // 提交评论事件
    const submit = ({ content, parentId, files, finish }: CommentSubmitParam) => {
        if(Number(config.user.id) == 0){
            pointmessage.value = "请先登录！"
            WaringMessage()
            return
        }

        console.log('提交评论: ' + content, parentId, files)
        let info={}
        if(parentId == null){ // 父评论
            info={
                Type:"Father",
                ArticleType:props.ArticleType,
                ArticleId:props.ParentId,
                ParentId: props.ParentId,
                Content:content,
                UserId:config.user.id,
            }
        }else{ // 子评论
            info={
                Type:"Son",
                ArticleType:props.ArticleType,
                ArticleId:props.ParentId,
                ParentId: parentId,
                Content:content,
                UserId:config.user.id,
            }
        }

        // Type:Father,ParentId:props.ParentId,
        service
            .post(`/api/comment/insert`, {
            Info: info,
            })
            .then(
            (response) => {
                console.log("请求成功了！！！",response.data);

                let comment: CommentApi = {
                    id: String((response.data._id)),
                    parentId: parentId,
                    uid: config.user.id,
                    address: '',
                    content: content,
                    likes: 0,
                    createTime: response.data.CreateTime,
                    contentImg: '',
                    user: {
                        username: config.user.username,
                        avatar: config.user.avatar,
                        level: 0,
                        homeLink: ``
                    },
                    reply: null
                }
                finish(comment)
                UToast({ message: '评论成功!', type: 'info' })
            },
            (error) => {
                console.log("请求失败了！！！");
                console.log(error.data);
            }
        );
    }
  
    // 删除评论
    const remove = (id: number, finish: () => void) => {
        service
            .delete(`/api/comment`, {
                params:{
                    ArticleId:props.ParentId,
                    CommentId:id
                }
            })
            .then(
            (response) => {
                console.log("请求成功了！！！",response.data);
                finish()
                alert(`删除成功: ${id}`)
                router.go(0)
            },
            (error) => {
                console.log("请求失败了！！！");
                console.log(error.data);
            }
        );
    }

    //举报用户
    const report = (id: number, finish: () => void) => {
        console.log(id)
        setTimeout(() => {
        finish()
        alert(`举报成功: ${id}`)
        }, 200)
    }

    // 点赞按钮事件 TOOD:暂时不能做，因为涉及到删除评论时，用户的数据也要删除
    const like = (id: number, finish: () => void) => {
        console.log(id)
        setTimeout(() => {
        finish()
        }, 200)
    }

    //子评论回复分页
    const replyPage = ({ parentId, pageNum, pageSize, finish }: ReplyPageParam) => {
        let childcommentinfo = reactive({'array':[]})
        let childcommentnum = ref(0)
        service
            .get(`/api/comment`, {
            params: {
                Type:"Son",
                ParentId: parentId,
                Page:pageNum,
                PageSize:pageSize,
                Skip:(pageNum - 1) * pageSize,
                Limit:pageSize,
                SonNum:5,
            },
            })
            .then(
            (response) => {
                console.log("请求成功了！！！",response.data);
                childcommentinfo.array = response.data.Child_Comments;
                childcommentnum.value = response.data.Child_Total;
                let tmp :ReplyApi= {
                    total: childcommentnum.value,
                    list: []
                }
                for(var tmpindex in childcommentinfo.array)
                {
                    let tmpinfo = childcommentinfo.array[tmpindex]
                    let info={
                        id: tmpinfo["_id"],
                        parentId: String(parentId),
                        uid: tmpinfo["User"][0]["_id"],
                        address: '',
                        content: tmpinfo["Content"],
                        likes: tmpinfo["Likes"],
                        createTime: tmpinfo["CreateTime"],
                        user: {
                            username: tmpinfo["User"][0]["NickName"],
                            avatar:tmpinfo["User"][0]["Avatar"],
                            level: 0,
                            homeLink: ''
                        }
                    }
                    tmp.list.push(info)
                }
                finish(tmp)
            },
            (error) => {
                console.log("请求失败了！！！");
                console.log(error.data);
            }
        );
        console.log('回复分页')
    }

// 处理页数发生改变
function handlepage(page: number){
    console.log('handlenext',page)
    currentPage.value = page
    getCommentInfo(props.ParentId)
}

// 处理每页数量发送改变
function handlpagesize(pagesize: number)
{
    console.log('handlpagesize',pagesize)
    pageSize.value = pagesize;
    getCommentInfo(props.ParentId)
}

// 发送警告消息
const WaringMessage = () => {
    ElMessage({
        showClose: true,
        message: pointmessage.value,
        type: 'warning',
    })
}
onMounted(()=>{
    // 调用函数
    getCommentInfo(props.ParentId)
})
</script>


  
<style lang="scss">
.comment-view{
    margin-top: 2rem;
}

.demo-pagination-block{
    margin-top: 10px;
    display: flex;
    justify-content: center;
    align-items: center
}

.vp-doc ul {
    list-style: none !important;
}

.vp-doc ul,
.vp-doc ol {
    padding-left: 0 !important;
    margin: 0 !important;
}

.vp-doc li + li {
    margin-top: 0px !important;
}
</style>
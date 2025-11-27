import { createStore } from 'vuex'
import service from '../axios'

export default createStore({
	// 存放数据
	state: {
		UserId:'0',
		NickName:'未登录',
		Avatar:'http://127.0.0.1:8081/image/1',
		CommentLikes:[],
		Solves:[],
		Authority:1,
		IsLogin:false,
		IsAdmin:false,
		ProblemTags:[]
	},
	// 存放函数
	mutations: {
		Login(state,userinfo){
			localStorage.setItem('token', userinfo.Token)
			state.UserId = userinfo._id
			state.NickName = userinfo.NickName
			state.Avatar = userinfo.Avatar
			state.CommentLikes = userinfo.CommentLikes
			state.Solves = userinfo.Solves
			state.Authority = userinfo.Authority
			state.IsLogin = true
			if(state.Authority==5)
				state.IsAdmin=true
		},
		SaveUserInfo(state,userinfo){
			state.UserId = userinfo._id
			state.NickName = userinfo.NickName
			state.Avatar = userinfo.Avatar
			state.CommentLikes = userinfo.CommentLikes
			state.Solves = userinfo.Solves
			state.Authority = userinfo.Authority
			state.IsLogin = true
			if(state.Authority==5)
				state.IsAdmin=true
		},
		Logout(state){
			state.UserId = '0'
			state.NickName = '未登录'
			state.Avatar = 'http://127.0.0.1:8081/image/1'
			state.CommentLikes = []
			state.Solves = []
			state.Authority = 1
			state.IsLogin = false
			state.IsAdmin = false
			// 移除浏览器中token
			localStorage.removeItem('token')
		},
		GetProblemTags(state){
			service
			.get(`/api/tags`, {
				params:{
					TagType:"Problem"
				}
			})
			.then(
				(response) => {
					state.ProblemTags = response.data.Tags
				},
				(error) => {
					
				}
			);
		}
	},
	actions: {
		
	},
	modules: {

	}
})


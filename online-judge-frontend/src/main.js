import { createApp } from 'vue'
import App from './App.vue'
// ---------- element UI -------
import ElementPlus from 'element-plus'
import 'element-plus/dist/index.css'
import * as ElementPlusIconsVue from '@element-plus/icons-vue'
// ---------- vuex ----------
import store from './store'
// -------------------------
import './assets/main.css'
// ---------- 路由 ----------
import router from './router'
// ---------- markdown ----------
import VMdPreview from '@kangc/v-md-editor/lib/preview';
import '@kangc/v-md-editor/lib/style/preview.css';
import githubTheme from '@kangc/v-md-editor/lib/theme/github.js';
import '@kangc/v-md-editor/lib/theme/style/github.css';

import VMdEditor from '@kangc/v-md-editor';
import '@kangc/v-md-editor/lib/style/base-editor.css';
// highlightjs
import hljs from 'highlight.js';

VMdPreview.use(githubTheme, {
  	Hljs: hljs,
});

VMdEditor.use(githubTheme, {
	Hljs: hljs,
});
// Tip 提示
import createTipPlugin from '@kangc/v-md-editor/lib/plugins/tip/index'
import '@kangc/v-md-editor/lib/plugins/tip/tip.css'
// emoji 表情
import createEmojiPlugin from '@kangc/v-md-editor/lib/plugins/emoji/index';
import '@kangc/v-md-editor/lib/plugins/emoji/emoji.css';
// katex
import createKatexPlugin from '@kangc/v-md-editor/lib/plugins/katex/cdn';

// 快速复制代码
import createCopyCodePlugin from '@kangc/v-md-editor/lib/plugins/copy-code/index';
import '@kangc/v-md-editor/lib/plugins/copy-code/copy-code.css';
VMdEditor.use(createTipPlugin())
VMdEditor.use(createEmojiPlugin())
VMdEditor.use(createKatexPlugin())
VMdEditor.use(createCopyCodePlugin())
VMdPreview.use(createTipPlugin())
VMdPreview.use(createEmojiPlugin())
VMdPreview.use(createKatexPlugin())
VMdPreview.use(createCopyCodePlugin())
// -------------------------

// ---------- 评论 ----------
import UndrawUi from 'undraw-ui'
import 'undraw-ui/dist/style.css'
// -------------------------

// 获取本地用户信息
import service from './axios'
let token = localStorage.getItem('token')
// 如果获取到Token则进行登录
if(token){
    service
    .get(`/api/user/tokenlogin`)
    .then(
    (response) => {
        if(response.data.Result == "Success"){
            // 登录成功
            store.commit('SaveUserInfo',response.data.Info)
        }else{
            // 登录失败
            localStorage.removeItem('token')
            return
        }
    },
    (error) => {
        console.log(error.data);
    }
    );
}

const app = createApp(App)

// 评论UI
app.use(UndrawUi)
// 路由
app.use(router)
// element-UI
app.use(ElementPlus)
// markdown
app.use(VMdPreview)
app.use(VMdEditor)
// vuex
app.use(store)

for (const [key, component] of Object.entries(ElementPlusIconsVue)) {
	app.component(key, component)
}
// 将app放置在最后
app.mount('#app')

#### 1、说明

firda gadget 模式支持如下四种模式：

   - Listen
   - Connect
   - Script
   - ScriptDirectory
   
我没有全部测试，根据使用目的不同，我现在只需要最后一种，主要用于大规模手机部署hook功能，为了把 libgadget.so 注入到进程，所以选择了 magisk + riru 的模式，通过自定义riru模块在riru的回调里面加载 libgadget.so 

[Riru-ModuleTemplate](https://github.com/RikkaApps/Riru-ModuleTemplate)

本项目使用的是 [API 10 (Riru v23)](https://github.com/RikkaApps/Riru-ModuleTemplate#api-10-riru-v23) 来编译的


#### 2、 目的 & 功能

- frida 持久化
- frida 代码能够hook同一个应用的不同进程
- 应用白名单（避免和其他hook框架冲突）
- 为了用于生产环境而不是调试环境

#### 3、适配Android版本

Android 9，Android 10 ，？

#### 4、安装

- 通过 twrp 刷入 magisk v21 - v22.1(或者官网推荐方式)
- 通过 magisk 刷入 riru ，目前测试过 v23.9 ~ v25.4.4 
- 通过 magisk 刷入 riru-FridaGadgetRiruMoudle-v14.2.12.9.zip
- 记得重启手机

#### 5、编译 

gradle assembleRelease

#### 6、配置

##### 6.1、白名单

`/data/local/tmp/_white_list.config`

我写的这个白名单是控制进程是不是需要加载 gadget 的so的，目的是为了在手机上同时兼容xposed，要不然一个进程启动的时候同时加载 xposed 的so和 gadget.so  这时候进程会卡死。

这个配置文件格式很简单，就是把app进程名(包名)用逗号隔开，比如：

```txt
com.twitter.twitter,com.github.testapp1,com.github.testapp2
```

##### 6.2、gadget scriptdirectory 配置

这个配置不同于上面的白名单配置，这个配置是用了控制当前已经加载了 gadget.so 的进程，是不是要加载和当前配置文件同名的js文件的。

https://frida.re/docs/gadget/#scriptdirectory

我把 gadget 的这个识别的目录硬编码指定在了 `/data/local/tmp/frida_scripts` 如果有需要，可以修改后自己编译当前项目。

那么以twitter 手机目录结构如下为例：
```
/data/local/tmp/_white_list.config
/data/local/tmp/frida_scripts/twitter.js
/data/local/tmp/frida_scripts/twitter.config
```

twitter.config  配置文件的目的是为了指定是否应该为某个 app 加载 twitter.js hook 脚本。twitter.js 就是普通的frida hook 脚本，twitter.config 文件格式大概如下：

```json
{
  "filter": {
    "executables": ["com.twitter.twitter"],
    "bundles": [],
    "objc_classes": []
  }
}
```

#### 7、TODO 

1、构建用于调试的工具 

2、开发一个图形界面用于配置配置文件和传输js脚本

3、app 图形界面控制gadget的动态库可选

4、整理当前ts 脚本，分离改机和系统函数监控
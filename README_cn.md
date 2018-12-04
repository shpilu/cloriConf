[English Version](README.md)

cloriConf<div id="top"></div>
====

在工程实践中，如果没有专门的基础架构团队负责配置中心的搭建与维护，业务团队往往需要自己引入或者开发配置解析库，而随着业务的迭代升级，配置系统往往有从一种配置格式升级为另外一种配置格式，最终升级到配置中心的需求，针对这一业务场景我们设计了cloriConf。

cloriConf通过统一所有配置形式的对外存取API及内部存储数据结构, 在简化配置读取操作的同时, 实现配置系统从本地配置文件到分布式配置中心的平滑升级——不论配置文件格式是gflags、ini、json、yaml或toml，还是将配置数据存放于zookeeper/etcd上，你都可以用cloriConf来操作。

在设计上，cloriConf在一定程度上参考了[Qconf](https://github.com/Qihoo360/QConf)(奇虎360公司开源的配置管理平台)，其核心数据结构是一个哈希表(用于配置节点查找)和多叉树(用于配置节点存储)，所有配置数据存放在树节点，其存储路径以某种标准格式存储在哈希表，并对外提供配置节点插入与查询接口。

* [特点](#features)
* [实例](#usage)
* [安装](#installation)
* [Zookeeper可视化界面](#dashboard)
* [API参考](#api)
* [关于JOML](#joml)
* [数组操作](#array)
* [CloriConf生产环境实践](#using)
* [相关项目](#related)
* [待完成列表](#todo)
* [作者](#authors)

## 特点<div id="features"></div>

与普通配置解析库相比, cloriConf的特别之处在于

* **配置格式无关** - cloriConf的配置操作接口与配置格式完全独立，不论配置文件格式是ini、json或toml(配置格式可自由扩展)，你都可以用同一套API来读取 
* **配置来源无关** - cloriConf同时支持加载配置字符串、配置文件或者从zookeeper加载配置数据(配置来源亦可自由扩展)

虽然cloriConf支持的功能多样化，但cloriConf的API被设计得很简单，与一个轻量级配置解析库没什么差别，很方便引入你的C++项目中。

## 实例<div id="usage"></div>

以下简要举例说明cloriConf的使用，具体API可参考[API参考](#api)一节

* 读取ini格式(joml格式)的配置文件

common.ini: 
```C++
    [[adslot=xxx]]
      splash=123
      popUp=345
      [vta]
      splash=222 #comment test ; semicolon test
      popUp=3415
```
C++ 代码: 
```C++
    #include <cloriconf/config.h>

    // SRC_LOCAL: 从本地磁盘文件读取配置 
    // FMT_JOML: 以joml格式解析配置文件 
    // CMT_SHARP: 将'#'视为注释符 
    Config* conf = Config::instance()->Load("../conf/common.ini", SRC_LOCAL | FMT_JOML | CMT_SHARP);
    if (conf) {
        std::cout << "parse ini config file success" << std::endl;
    } else {
        std::cout << "parse ini config file failed"; 
        return;
    }
    std::string val1 = conf->GetString("adslot.vta.popUp");
    ConfNode* node = conf->GetConfNode("adslot.vta.splash");
    if (node) {
        std::cout << "adslot.vta.splash=" << node->AsString() << std::endl;
    }
    int val2 = conf->GetInt32("/adslot/vta/splash");
    std::cout << "val1=" << val1 << std::endl;
    std::cout << "val2=" << val2 << std::endl;
```
* 访问json格式的配置文件 

common.json:
```C++
    {
        "adslot":{
            "splash":"123",
            "popUp":"345",
            "vta":{
                "splash":"222",
                "popUp":"3415"
            }   
        }   
    }
```
C++ 代码: 
```C++
    #include <cloriconf/config.h>

    // FMT_JSON: 以json格式解析配置文件 
    std::string err_msg;
    Config* conf = Config::instance()->Load("../conf/common.json", SRC_LOCAL | FMT_JSON, &err_msg);
    if (conf) {
        std::cout << "parse json config file success" << std::endl;
    } else {
        std::cout << "parse json config file failed, " << err_msg << std::endl;
    }   
    std::string val1 = conf->GetString("adslot.vta.popUp");
    const ConfNode* node = conf->GetConfNode("adslot.vta.splash");
    if (node) {
        std::cout << "adslot.vta.splash=" << node->AsString() << std::endl; // 222
        std::cout << "adslot.vta.popUp=" << conf->GetString("adslot.vta.popUp") << std::endl; // 3415
    }   

    // take 13456789 as default value when node "/adslot/vta/not_exist" not found
    int val2 = conf->GetInt32("/adslot/vta/not_exist", 13456789);
    std::cout << "val1=" << val1 << std::endl;
    std::cout << "val2=" << val2 << std::endl;
```
* 从zookeeper加载配置数据 

zk.ini(JOML-style):
```C++
    [zookeeper]
        # host=10.6.43.15:2181,10.6.43.16:2181,10.6.1.12:2181,10.6.1.13:2181
        host=localhost:2181
        timeout=3000
        interval=5000
        root=/online/commercial/ssp
    []
```
C++ 代码:
```C++
    #include <cloriconf/config.h>

    // SRC_ZK: 从zookeeper加载配置 
    std::string err_msg;
    Config* conf = Config::instance()->Load("../conf/zk.ini", SRC_ZK, &err_msg);
    if (conf) {
        std::cout << "run test success" << std::endl;
    } else {
        std::cout << "run test failed, " << err_msg << std::endl;
        return;
    }
    std::string val1 = conf->GetString("rules/popUp");
    std::cout << "value of rules/popUp=" + val1 << std::endl;

    ConfNode* node = conf->GetConfNode("rules/splash");
    if (node) {
        for (ConfNode::ChildrenIterator iter = node->begin(); iter != node->end(); ++iter) {
            std::cout << "zookeeper config node, key=" << iter->name() << ", value=" << iter->AsString() << std::endl;
        }   
    }
```
## 安装<div id="installation"></div>

在安装cloriConf之前需要注意
  * cloriConf目前只在Linux操作系统通过测试，要在其他操作系统使用cloriConf，你可能需要手动改一些代码和CMakeLists
  * cloriConf对其他库有一些依赖，以支持json解析和zookeeper加载，这些库包括RapidJSON和zookeeper。为简化安装过程，cloriConf默认不开启对json和zookeeper的支持功能

可以通过在cloriConf源码根路径执行以下命令来安装cloriConf
```C++
// (默认不支持json和zookeeper，不用依赖其它第三方库) 
mkdir build && cd build
cmake ..
make
sudo make install
```
如果想支持json解析，你需要先安装[RapidJSON](https://github.com/Tencent/rapidjson)，然后执行 
```C++
mkdir build && cd build
cmake .. -DENABLE_JSON=ON
make
sudo make install
```
如果想支持zookeeper加载，你需要先安装[zookeeper](https://www.apache.org/dyn/closer.cgi)，然后执行
```C++
mkdir build && cd build
cmake .. -DENABLE_ZOOKEEPER=ON
make
sudo make install
```
你可以通过指定CMAKE_INSTALL_PREFIX来自定义安装路径，一个完整的安装命令如下
```C++
mkdir build && cd build
cmake .. -DENABLE_JSON=ON -DENABLE_ZOOKEEPER=ON -DCMAKE_INSTALL_PREFIX=/usr/local/third_party
make
sudo make install
```
安装cloriConf以后在程序中使用cloriConf的编译命令类似于 (假设cloriConf安装于/home/weijian/cloriconf目录)
```C++
g++ tutorial.cc -I/home/weijian/cloriconf/include -L/home/weijian/cloriconf/lib -lcloriconf -o main -std=c++11 -Wl,-rpath=/home/weijian/cloriconf/lib
```
## Zookeeper可视化界面<div id="dashboard"></div>
cloriConf的特性使得它可以当配置中心来使用，不过需要你事先搭建一个zookeeper集群</br>
为简化zookeeper的操作，cloriConf自带一个简单的zookeeper控制面板(其源码在src/dashboard目录下)，你只需要一个nginx和php运行环境就可以搭建起一套zookeeper可视化界面，cloriConf zookeeper dashboard的nginx配置可以参考以下
```PHP
    # set "/home/weijian/github/cloriConf" to your own directory
    location ~ \.php$ {
        root    /home/weijian/github/cloriConf/src/dashboard;
        fastcgi_pass   127.0.0.1:9000;
        fastcgi_index  index.php;
        fastcgi_param  SCRIPT_FILENAME  $document_root$fastcgi_script_name;
        include        fastcgi_params;
    }   
    location ~ \.(css|js|html)$ {
        root        /home/weijian/github/cloriConf/src/dashboard;
    }
```
cloriConf zookeeper dashboard截图如下: 
![pics1](https://github.com/shpilu/cloriConf/blob/master/img/cloriconf.jpg)
你可以点击[这里](http://60.205.189.117/index.php?group_id=default&path=/online/commercial/ssp/rules)了解下cloriConf zookeeper dashboard实例 

cloriConf的zookeeper可视化界面实际是[Qconf](https://github.com/Qihoo360/QConf) dashboard的php实现，并在操作上做了一定优化，当然你可以使用其他zookeeper工具来管理配置

## API参考<div id="api"></div>

### instance
`Config* Config::instance()`

功能描述
>使用单例模式获取*Config*类实例

返回值
>一个指向*Config*类实例的指针
### Load
`Config* Load(const std::string& input, uint32_t mode, std::string* err_msg = NULL)`  

功能描述
>从字符串、配置文件或者zookeeper加载配置数据

参数
>*input* - 数据源，可能是配置字符串、配置文件名称或者zookeeper配置(由*mode*参数决定)
>
>*mode* - 加载模式，包括配置来源、配置数据格式和注释标识符三部分并由"|"隔开，参考以下表格
>
>*err_msg* - 可选值，用于加载配置失败时获取错误信息

*Mode* 表:

| Flag          | Type    | Description                                 |
|---------------|---------|---------------------------------------------|
| SRC_LOCAL     | source  | 从本地配置文件加载配置                      |
| SRC_DIRECT    | source  | 直接从配置字符串加载配置                    |
| SRC_ZK        | source  | 从zookeeper加载配置                         |
| FMT_JOML      | format  | 以ini/joml格式解析配置数据                  |
| FMT_JSON      | format  | 以json格式解析配置数据                      |
| CMT_SHARP     | comment | 以'#'作为注释标识符                         |
| CMT_SLASH     | comment | 以'//'作为注释标识符                        |
| CMT_SEMICOLON | comment | 以';'作为注释标识符                         |
| CMT_PERCENT   | comment | 以'%'作为注释标识符                         |

注意
>* Comment类型只有在format类型是FMT_JOML时有用
>* 当source类型是SRC_ZK时，format和comment类型失效 
>* Comment类型支持多个注释符的组合，比如"CMT_SHARP|CMT_SLASH"表示'#'和'//'都是注释标识符 

实例
>Config conf;</br>
>// Load config data from local file and take ';' as line comment identifier</br>
>conf.Load("conf.ini", SRC_LOCAL | FMT_JOML | CMT_SEMICOLON); </br>
>// Load config data from zookeeper </br>
>conf.Load("zk.ini", SRC_ZK); </br>
>// Load JSON-style string config data </br>
>std::string input_str("{\"myself\":{\"name\":\"WeiJian\", \"school\":\"BUAA\",\"dr\":[\"cloris\", \"apache\"]}}"); </br>
>conf.Load(input_str, SRC_DIRECT | FMT_JSON);

返回值
>一个指向config对象的指针，如果加载失败，返回一个空指针

### LoadEx
`bool LoadEx(const std::string& input, uint32_t mode, std::string* err_msg = NULL)`  

功能描述
>与*Load*功能相同，区别在于返回值是bool类型

返回值
>一个标识加载成功或失败的bool类型

实例
>Config conf;</br>
>// Load config data from local file and take ';' as line comment identifier</br>
>bool ok = conf.LoadEx("conf.ini", SRC_LOCAL | FMT_JOML | CMT_SEMICOLON); </br>
>// Load config data from zookeeper </br>
>bool ok = conf.LoadEx("zk.ini", SRC_ZK); </br>
>// Load JSON-style string config data </br>
>std::string input_str("{\"myself\":{\"name\":\"WeiJian\", \"school\":\"BUAA\",\"dr\":[\"cloris\", \"apache\"]}}"); </br>
>bool ok = conf.LoadEx(input_str, SRC_DIRECT | FMT_JSON);


### GetConfNode
`ConfNode* Config::GetConfNode(const std::string& name = "") const`</br>
`ConfNode* ConfNode::GetConfNode(const std::string& name = "") const`

功能描述
>从配置树获取指定的*ConfNode*结点 

参数
>*name* - 配置结点路径，支持两种书写方式:"p1.p2.p3" 或 "p1/p2/p3"

### GetString/GetInt32/GetInt64/GetDouble/GetBool
`std::string Config::GetString(const std::string& name, const std::string& default_value = "") const`</br>
`int32_t Config::GetInt32(const std::string& name, int32_t default_value = 0) const`</br>
`int64_t Config::GetInt64(const std::string& name, int64_t default_value = 0L) const`</br>
`double  Config::GetDouble(const std::string& name, double default_value = 0.0) const`</br>
`bool    Config::GetBool(const std::string& name, bool default_value = false) const`</br>

`std::string ConfNode::GetString(const std::string& name, const std::string& default_value = "") const`</br>
`int32_t ConfNode::GetInt32(const std::string& name, int32_t default_value = 0) const`</br>
`int64_t ConfNode::GetInt64(const std::string& name, int64_t default_value = 0L) const`</br>
`double  ConfNode::GetDouble(const std::string& name, double default_value = 0.0) const`</br>
`bool    ConfNode::GetBool(const std::string& name, bool default_value = false) const`</br>

功能描述
>从当前配置结点搜索子配置结点并将其值解析为string/int32/int64/double/bool类型

参数
>*name* - 子配置结点路径，支持两种书写方式:"p1.p2.p3" 或 "p1/p2/p3"
>
>*default_value* - 当子结点不存在时，作为默认值返回

### AsString/AsInt32/AsInt64/AsDouble/AsBool
`const std::string& ConfNode::AsString() const`</br>
`int32_t ConfNode::AsInt32() const`</br>
`int64_t ConfNode::AsInt64() const`</br>
`double  ConfNode::AsDouble() const`</br>
`bool    ConfNode::AsBool() const`</br>

功能描述
>将配置结点解析为string/int32/int64/double/bool类型

### Exists
`bool Config::Exists(const std::string& name) const`</br>
`bool ConfNode::Exists(const std::string& name) const`

功能描述
>判断配置结点是否存在

参数
>*name* - 配置结点路径

### name
`const std::string& ConfNode::name()` 

功能描述
>当前结点的名称

### begin
`ConstChildrenIterator ConfNode::begin() const`</br>
`ChildrenIterator begin()`

功能描述
>子配置结点迭代器的begin()

### end 
`ConstChildrenIterator ConfNode::end() const`</br>
`ChildrenIterator ConfNode::end()` 

功能描述
>子配置结点迭代器的end()

实例
```C++
// 使用begin和end遍历子结点
ConfNode* node = conf->GetConfNode("rules/splash");
if (node) { 
    for (ConfNode::ChildrenIterator iter = node->begin(); iter != node->end(); ++iter) {
        std::cout << "zookeeper config node, key=" << iter->name() << ", value=" << iter->AsString() << std::endl;
    }
}
```

## 关于JOML<div id="joml"></div>
**JOML**(**J**ames's **O**bvious **M**inimal **L**anguage) 是一种自创的配置格式，一个joml的实例如下:
```
# @ad_server config
[[[ad_server]]]
    [[inmobi]]
        # online config
        host=api.w.xxx.cn
        port=80
        uri=/showadxxxx/v3.1
        # test
        #host=192.168.19.229
        #port=7761
        #uri=/getAdTestIxxx
        timeout_connect=200
        timeout_receive=300 
        retry=0
        [os_config.ios]
            appid=1231
        [os_config.android]
            appid=1231
        [splash.android]
            id=1510609xxx
            bundle=so.xxx.xxx
        [splash.ios]
            id=15119685xxx6
            bundle=com.xxx.xxx
```
joml在ini的基础上做了扩展，以支持更加复杂的配置场景(可以把joml当做ini的超集)，与ini/yaml相比，joml有以下特征

* 比ini多了对多节嵌套的支持 - 比如"[[...]]" 是 "[...]"的上一层, 嵌套的'['越多，section的层级越高 
* 比yaml支持更加自由的书写风格 - 比如，要表达配置项"ad_server.inmobi.os_config.ios.appid=1231", 以下配置写法都可以
```C++
# case 1
[[[[ad_server]]]]
    [[[inmobi]]]
        [[os_config]]
            [ios]
                appid=1231
```
```C++
# case 2
[ad_server.inmobi.os_config]
    ios.appid=1231
```
```C++
# case 3
[[[ad_server.inmobi]]]
    [os_config]
        ios.appid=1231
```
```C++
# case 4
ad_server.inmobi.os_config.ios.appid=1231
```
* 支持自定义注释标识符 - 可以在*Load/LoadEx*函数中指定哪些字符是注释标识符 

## 数组操作<div id="array"></div>

在设计上，cloriConf主要参照的配置形式是没有数组概念的ini格式和zookeeper，而一些配置格式比如json、yaml、toml有数组的概念。为此，cloriConf通过将数组转换为*name=value*对的形式来实现对数组的支持，其中*name*从0，1，2……逐一递增

以下实例介绍如何访问json配置格式中的数组:
```C++
# conf.json:
{
    "id":"N1",
    "name":"shpilu",
    "info":{
        "month":["January","February", "March", "April", "May"]
    }
}
```
C++ code:
```C++
    std::string err_msg;
    Config* conf = Config::instance()->Load("conf.json", SRC_LOCAL | FMT_JSON, &err_msg);
    if (!conf) {
        std::cout << "run test failed, " << err_msg << std::endl;
        return;
    }   
    std::string third_month = conf->GetString("info.month.2");
    // output: the third month is March
    std::cout << "the third month is " << third_month << std::endl;
    ConfNode* node = conf->GetConfNode("info.month");
    if (node) {
        for (auto &p : *node) {
            // 0, January
            // 1, February
            // ...
            std::cout << p.name() << ", " << p.AsString() << std::endl;
        }
    }
```
## CloriConf生产环境实践<div id="using"></div>

* cloriConf目前应用于[ofo 小黄车](http://www.ofo.so/#/)，其多个服务模块通过使用cloriConf，在小而精的团队中实现配置系统的平滑升级

## 相关项目<div id="related"></div>
 * [Qihoo360/QConf](https://github.com/Qihoo360/QConf)-奇虎360版配置管理系统
 * [knightliao/disconf](https://github.com/knightliao/disconf)-百度开源的分布式配置中心

## 待完成列表<div id="todo"></div>
* 支持yaml风格的配置格式
* 支持toml风格的配置格式
* 支持gflags风格的配置格式
* 支持protobuf风格的配置格式
* 支持从etcd加载配置
* 支持基于zookeeper/etcd的配置自动更新与消息订阅/发布

## 作者<div id="authors"></div>

* James Wei (weijianlhp@163.com)  

[返回顶部](#top)


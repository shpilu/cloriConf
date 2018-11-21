[中文版](README_cn.md)

cloriConf<div id="top"></div>
====

Suppose you are programing in C++ and looking for a third-party configuration library, you may considering cloriConf, which has characters of both lightweight configuration library and distributed configuration center. 

* [Features](#features)
* [Usage](#usage)
* [Installation](#installation)
* [Build Up Zookeeper Dashboard by CloriConf](#dashboard)
* [API Reference](#api)
* [About JOML](#joml)
* [Array Access](#array)
* [Who Is Using CloriConf?](#using)
* [Related Projects](#related)
* [TODO List](#todo)
* [Authors](#authors)

## Features<div id="features"></div>

The most significant features that cloriConf exceeds general configuration libraries are 

* **Format-independent** - As designed, cloriConf's access APIs are completely separated from specific configuration format. Actually cloriConf trys to translate configuration items of any config format(e.g. ini, json, yaml) into nodes of a configuration tree, and what cloriConf's API need to do is just searching a configuration tree.The currently supported formats are json, joml((**James's** **Obvious** **Minimal** **Language**, a superset of INI format, see [About JOML](#joml) for detail), and more config format will be supported in the future.
* **Sourece-independent** - Not only can cloriConf load various configuration format from local file and read config from a string directly, but also accessing zookeeper(support for ectd will come soon) is OK. 

Multifunctional though cloriConf is, cloriConf's access API is designed to be as simple as a lightweight configuration library, Refer [Usage](#usage) and [API Reference](#api) for details.</br>

## Usage<div id="usage"></div>

The following will take you through how to use cloriConf in your program.Or you can refer [API Reference](#api) for detail of cloriConf APIs.

* Access INI-style(JOML-style) configuration file  
common.ini: 
```C++
    [[adslot=xxx]]
      splash=123
      popUp=345
      [vta]
      splash=222 #comment test ; semicolon test
      popUp=3415
```
C++ code: 
```C++
    // SRC_LOCAL: load config from local file
    // FMT_JOML: parse as INI-style(JOML-style) config 
    // CMT_SHARP: take '#' as line comment identifier
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
* Access JSON-style configuration file  
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
C++ code: 
```C++
    // FMT_JSON: parse as JSON-style config
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
* Load config from zookeeper  
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
C++ code:
```C++
    // SRC_ZK: load config from zookeeper
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
## Installation<div id="installation"></div>

Before installation, you have to confirm that
  * CloriConf currently has passed the test in Linux operation system **only**. To support other OS, you may need to modify CMakeLists.txt and do some other coding work.  
  * CloriConf is not a fully self-contained library, which has dependency on RapidJSON(for json support) and zookeeper(for zookeeper support). To simplify the installation of cloriConf, function for json/zookeeper parser is disabled by default.  
To install cloriConf, you can run the following command in root path of cloriConf source code:
```C++
// (without support for JSON-style and zookeeper parser) 
mkdir build && cd build
cmake ..
make
sudo make install
```
To support JSON-style parser, you need to install [RapidJSON](https://github.com/Tencent/rapidjson) firstly, and then run
```C++
mkdir build && cd build
cmake .. -DENABLE_JSON=ON
make
sudo make install
```
To support zookeeper, you need to install [zookeeper](https://www.apache.org/dyn/closer.cgi) firstly, and then run
```C++
mkdir build && cd build
cmake .. -DENABLE_ZOOKEEPER=ON
make
sudo make install
```
Alternatively you can specify the install prefix by setting CMAKE_INSTALL_PREFIX, in this case, a complete step seems like 
```C++
mkdir build && cd build
cmake .. -DENABLE_JSON=ON -DENABLE_ZOOKEEPER=ON -DCMAKE_INSTALL_PREFIX=/usr/local/third_party
make
sudo make install
```
After adding cloriConf to your program, you can compile like (assume cloriConf installed in /home/weijian/cloriconf)
```C++
g++ tutorial.cc -I/home/weijian/cloriconf/include -L/home/weijian/cloriconf/lib -lcloriconf -o main -std=c++11 -Wl,-rpath=/home/weijian/cloriconf/lib
```
## Build Up Zookeeper Dashboard by CloriConf<div id="dashboard"></div>
As cloriConf support loading config data from zookeeper, you can use it as a simple distributed configuration center to some extent. What you need is a zookeeper cluster.</br> 
To simplify zookeeper management, cloriConf contain a zookeeper dashboard module in directory src/dashboard. To use cloriConf dashboard, a PHP runtime environment and nginx are required, and then you can add the following nginx configuration into nginx.conf and restart nginx
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
A screenshot of dashboard looks like this:
![pics1](https://github.com/shpilu/cloriConf/blob/master/img/cloriconf.jpg)
You may click [here](http://60.205.189.117/index.php?group_id=default&path=/online/commercial/ssp/rules) to see an instance of cloriConf zookeeper dashboard.

## API Reference<div id="api"></div>

### instance
`Config* Config::instance()`

Description
>Get a *Config* class instance using singleton pattern

Return value
>A *Config* class pointer which pointer to a *Config* instance
### Load
`Config* Load(const std::string& input, uint32_t mode, std::string* err_msg = NULL)`  

Description
>Load configuration data from a string, local file or zookeeper   

Parameters
>*input* - config data source, maybe a config string, file name, or zookeeper config file, determined by parameter *mode* 
>
>*mode* - load mode, consists three parts: source, format and comment(joined by '|'), see the following table for detail
>
>*err_msg* - optional, filled with detailed error message when loading data error occurred

*Mode* and their descriptions:

| Flag          | Type    | Description                                 |
|---------------|---------|---------------------------------------------|
| SRC_LOCAL     | source  | Load config data from local file            |
| SRC_DIRECT    | source  | Load config data from input string directly |
| SRC_ZK        | source  | Load config data from zookeeper             |
| FMT_JOML      | format  | Parse data as JOML-style/INI-style          |
| FMT_JSON      | format  | Parse data as JSON-style                    |
| CMT_SHARP     | comment | Take '#' as line comment identifier         |
| CMT_SLASH     | comment | Take '//' as line comment identifier        |
| CMT_SEMICOLON | comment | Take ';' as line comment identifier         |
| CMT_PERCENT   | comment | Take '%' as line comment identifier         |

Note
>* Comment type is useful only when format type is FMT_JOML
>* When source type is SRC_ZK, format and comment type are disabled
>* Commment type support combine of multi comment, e.g. "CMT_SHARP|CMT_SLASH" means '#' and '//' are all regard as comment identifier

Example
>Config conf;</br>
>// Load config data from local file and take ';' as line comment identifier</br>
>conf.Load("conf.ini", SRC_LOCAL | FMT_JOML | CMT_SEMICOLON); </br>
>// Load config data from zookeeper </br>
>conf.Load("zk.ini", SRC_ZK); </br>
>// Load JSON-style string config data </br>
>std::string input_str("{\"myself\":{\"name\":\"WeiJian\", \"school\":\"BUAA\",\"dr\":[\"cloris\", \"apache\"]}}"); </br>
>conf.Load(input_str, SRC_DIRECT | FMT_JSON);

Return value
>A pointer to config object, if loading failed, a NULL pointer is returned

### LoadEx
`bool LoadEx(const std::string& input, uint32_t mode, std::string* err_msg = NULL)`  

Description
>Have the same function as *Load*, the difference is that return value tyle is bool.

Return value
>A bool value indicating success or failure.

Example
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

Description
>Get specific *ConfNode* from configuration tree

Parameters
>*name* - path of config node. CloriConf supports two kinds of path style:"p1.p2.p3" or "p1/p2/p3"

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

Description
>Search children config node from current ConfNode and parse children config node as string/int32/int64/double/bool

Parameters
>*name* - Child config node path
>
>*default_value* - Default return value when ConfNode not found

### AsString/AsInt32/AsInt64/AsDouble/AsBool
`const std::string& ConfNode::AsString() const`</br>
`int32_t ConfNode::AsInt32() const`</br>
`int64_t ConfNode::AsInt64() const`</br>
`double  ConfNode::AsDouble() const`</br>
`bool    ConfNode::AsBool() const`</br>

Description
>Parse ConfNode as string/int32/int64/double/bool

### Exists
`bool Config::Exists(const std::string& name) const`</br>
`bool ConfNode::Exists(const std::string& name) const`

Description
>Check if conf node exists

Parameters
>*name* - Config node path

### name
`const std::string& ConfNode::name()` 

Description
>Name of current conf node

### begin
`ConstChildrenIterator ConfNode::begin() const`</br>
`ChildrenIterator begin()`

Description
>Begin of children config node iterator

### end 
`ConstChildrenIterator ConfNode::end() const`</br>
`ChildrenIterator ConfNode::end()` 

Description
>End of children config node iterator

Example
```C++
// use begin and end to traverse children config nodes 
ConfNode* node = conf->GetConfNode("rules/splash");
if (node) { 
    for (ConfNode::ChildrenIterator iter = node->begin(); iter != node->end(); ++iter) {
        std::cout << "zookeeper config node, key=" << iter->name() << ", value=" << iter->AsString() << std::endl;
    }
}
```

## About JOML<div id="joml"></div>

**JOML**(**J**ames's **O**bvious **M**inimal **L**anguage) is a self-defined configuration description language inspired by ini and toml. An instance of joml: 
```ini
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
You can treat JOML as a superset of INI, compared with INI, JOML has the following features

* Section nesting support - e.g. "[[...]]" is the parent section of "[...]", the more brackets nested, the higher layer it is
* More slack writing style - for example, to express item "ad_server.inmobi.os_config.ios.appid=1231", the following styles are all OK
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
* More line comments identifier are supported - you can specify your own identifier in *Load/LoadEx* function

## Array Access<div id="array"></div>
As designed, cloriConf's main purpose is to support config style like joml and zookeeper which do not have concept of array. To support config style which has array definition(e.g. json,toml), cloriConf translate any array into *name=value* pair, where *name* counts from 0 until the end of the array.</br>

The following example shows how to access arry in JSON-style configuration file: </br>
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
            std::cout << "name=" << p.name() << ", value=" << p.AsString() << std::endl;
        }   
    }
```
## Who Is Using CloriConf?<div id="using"></div>

* [ofo 小黄车](http://www.ofo.so/#/) - ofo Inc., a Beijing-based bicycle sharing company

## Related Projects<div id="related"></div>
 * [Qihoo360/QConf](https://github.com/Qihoo360/QConf)-A distributed configuration management system
 * [knightliao/disconf](https://github.com/knightliao/disconf)-A distributed configuration center
To some extent, cloriConf is inspired by the upper projects
## TODO List<div id="todo"></div>
* Support YAML-style configuration file
* Support TOML-style configuration file 
* Support loading config from ectd
* Update config automatically and smoothly when config in zookeeper/ectd changed

## Authors<div id="authors"></div>

* James Wei (weijianlhp@163.com)  
Please contact me if you have trouble using cloriConf.

[Go back to top](#top)


cloriConf
====

Suppose you programing in C++ and looking for a third-party configuration library, I suggest you have a try on cloriConf, which has character of both lightweight configuration library and distributed configuration center. 

## Features

The most significant features cloriConf exceeds general configuration library are 

* Configuration **format-independent**. CloriConf's access APIs are independent from configuration format. The currently supported formats are json and joml(a superset of ini format), and more config format will be supported in future.
* Configuration **sourece-independent**. CloriConf not only can load various configuration format from file or string, but also accessing zookeeper is OK.

Multifunctional though cloriConf is, cloriConf's access API is designed to be as simple as a lightweight configuration library, see usage and API document for detail.

## Usage 

* Access INI-style configuration file  
common.ini ==>
```C++
[[adslot=xxx]]
splash=123
popUp=345
[vta]
splash=222 #comment test ; semicolon test
popUp=3415
```
C++ code ==> 
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
common.json ==>
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
C++ code ==>
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
    std::cout << "adslot.vta.splash=" << node->AsString() << std::endl;
    std::cout << "adslot.vta.popUp=" << conf->GetString("adslot.vta.popUp") << std::endl;
}   

// take 13456789 as default value when node "/adslot/vta/not_exist" not found
int val2 = conf->GetInt32("/adslot/vta/not_exist", 13456789);
std::cout << "val1=" << val1 << std::endl;
std::cout << "val2=" << val2 << std::endl;
```
* Load config from zookeeper  
zk.ini ==>
```C++
[zookeeper]
    # host=10.6.43.15:2181,10.6.43.16:2181,10.6.1.12:2181,10.6.1.13:2181
    host=localhost:2181
    timeout=3000
    interval=5000
    root=/online/commercial/ssp
[]
```
C++ code ==>
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
## Installation

CloriConf currently support Linux operation system only. To support other os, you may need to change CMakeLists.txt and some code.

```

## Who Is Using CloriConf? 

* [ofo 小黄车](http://www.ofo.so/#/) - ofo Inc., a Beijing-based bicycle sharing company

## Authors

* James Wei (weijianlhp@163.com)


# cloriConf
====

Suppose you programing in C++ and looking for a third-party configuration library, I suggest you have a try on cloriConf, which has character of both lightweight configuration library and distributed configuration center. 

## Features

The most significant features cloriConf exceeds general configuration library are 

* Configuration format-independent. CloriConf's access APIs are independent from configuration format. The currently supported formats are json and joml(a superset of ini format), and more config format will be supported in future.
* Configuration sourece-independent. CloriConf not only can load various configuration format from file or string, but also accessing zookeeper is OK.

Multifunctional though cloriConf is, cloriConf's access API is designed to be as simple as a lightweight configuration library, see usage and API document for detail.

## Usage 

Access INI-style configuration file:
```
# joml test
[[adslot=xxx]]
splash=123
popUp=345
[vta]
splash=222 #comment test ; semicolon test
popUp=3415
```
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
int val2 = conf->GetInt32("/adslot/vta/xxx");
std::cout << "val1=" << val1 << std::endl;
std::cout << "val2=" << val2 << std::endl;
```


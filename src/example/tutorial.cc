//
// The following shows basic usage of cloriConf
//
#include <iostream>
#include <unistd.h>
#include "../config.h"

using namespace cloris;

// Load joml-formatted configuration data from local file
void load_joml_from_file() {
    std::string err_msg;
    Config* conf = Config::instance()->Load("../conf/common.ini", SRC_LOCAL | FMT_JOML | CMT_SHARP, &err_msg);
    if (conf) {
        std::cout << "run test success" << std::endl;
    } else {
        std::cout << "run test failed, " << err_msg << std::endl;
    }
    std::string k1 = conf->GetString("adslot.vta.popUp");
    const ConfNode* node = conf->GetConfNode("adslot.vta.splash");
    if (node) {
        std::cout << "adslot.vta.splash=" << node->AsString() << std::endl;
    }
    int k2 = conf->GetInt32("/adslot/vta/xxx", 13456);
    std::cout << "k1=" << k1 << std::endl;
    std::cout << "k2=" << k2 << std::endl;
    std::cout << "[OK]local joml file test end..." << std::endl;
}

// Load json-formatted configuration data from local file
void load_json_from_file() {
    std::string err_msg;
    Config* conf = Config::instance()->Load("../conf/common.json", SRC_LOCAL | FMT_JSON, &err_msg);
    if (conf) {
        std::cout << "run test success" << std::endl;
    } else {
        std::cout << "run test failed, " << err_msg << std::endl;
    }
    std::string k1 = conf->GetString("adslot.vta.popUp");
    const ConfNode* node = conf->GetConfNode("adslot.vta.splash");
    if (node) {
        std::cout << "adslot.vta.splash=" << node->AsString() << std::endl;
        std::cout << "adslot.vta.popUp=" << conf->GetString("adslot.vta.popUp") << std::endl;
    }
    int k2 = conf->GetInt32("/adslot/vta/xxx", 13456789);
    std::cout << "k1=" << k1 << std::endl;
    std::cout << "k2=" << k2 << std::endl;
    std::cout << "[OK]local json file test end..." << std::endl;
}

// Event watcher (not complete yet, coming soon)
void Watch(ConfNode* node, const std::string& path, uint32_t event) {
    std::cout << "watch hit---" << std::endl;
}

// Load configuration data from zookeeper 
void test_zookeeper() {
    std::string err_msg;
    Config* conf = Config::instance()->Load("../conf/zk.ini", SRC_ZK, &err_msg);
    if (conf) {
        std::cout << "run test success" << std::endl;
    } else {
        std::cout << "run test failed, " << err_msg << std::endl;
        return;
    }
    EventHandler handler = std::bind(Watch, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    conf->Watch("rules", EVENT_INIT, handler);
    std::string k1 = conf->GetString("rules/popUp");
    std::cout << "value of rules/popUp=" + k1 << std::endl;

    ConfNode* node = conf->GetConfNode("rules/splash");
    if (node) {
        for (ConfNode::ChildrenIterator iter = node->begin(); iter != node->end(); ++iter) {
            std::cout << "zookeeper config node, key=" << iter->name() << ", value=" << iter->AsString() << std::endl;
        }
    }
    std::cout << "[OK]zookeeper test end..." << std::endl;
}

// Load json-formatted configuration data from a string directly
void test_direct_json() {
    std::string conf("{\"myself\":{\"name\":\"WeiJian\", \"school\":\"BUAA\",\"dr\":[\"cloris\", \"apache\"]}}");
    std::string err_msg("");
    if (Config::instance()->Load(conf, SRC_DIRECT | FMT_JSON, &err_msg)) {
        std::string my_name = Config::instance()->GetString("myself.name");
        const ConfNode* node = Config::instance()->GetConfNode("myself.dr");
        for (auto &p : *node) {
            std::cout << "dr, name=" << p.name() << ", value=" << p.AsString() << std::endl;
        }
        std::cout << "my name is " << my_name << std::endl;
        std::cout << "dr0=" << Config::instance()->GetString("myself.dr.0") << std::endl;
    } else {
        std::cout << "init direct config failed:" << err_msg << std::endl;
    }
    std::cout << "[OK]direct json test end..." << std::endl;
}

// Load joml-formatted configuration data from a string directly
void test_direct_joml() {
    std::string conf("[myself] \n name=James Wei\n school=BUAA \n company=ofo\n");
    std::string err_msg("");
    if (Config::instance()->Load(conf, SRC_DIRECT, &err_msg)) {
        std::string my_name = Config::instance()->GetString("myself.name");
        std::cout << "my name is " << my_name << std::endl;
    } else {
        std::cout << "init direct config failed:" << err_msg << std::endl;
    }
    std::cout << "[OK]direct joml test end..." << std::endl;
}

int main(int argc, char** argv) {
    load_joml_from_file();
    load_json_from_file();
    test_zookeeper();
    test_direct_json();
    test_direct_joml();
    return 0;
}

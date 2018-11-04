#include <iostream>
#include <unistd.h>
#include "../config.h"

// usage:
// conf = Config::instance().Load(path, SRC_LOCAL); // 加载本地文件
// conf = Config::instance().Load(path, SRC_DIRECT); // 直接将入参作为参数解析
// conf = Config::instance().Load(path, SRC_ZK); // 加载zookeeper
// conf = Config::instance().Load(path, SRC_HTTP); // 加载HTTP
// conf = Config::instance().Load(path, SRC_ZK, MD_LOOP); // 加载并监听, 如果有更新则更新conf本地
// conf = Config::instance().LoadConfig(configure); // 以配置文件的形式加载
// int k = Config::instance()->getInt32("/adslot/inmobi/weight");
// spect = std::bind(xxxx);

void test_local() {
    std::string err_msg;
    cloris::Config* conf = cloris::Config::instance()->Load("../conf/common.ini", SRC_LOCAL, &err_msg);
    if (conf) {
        std::cout << "run test success" << std::endl;
    } else {
        std::cout << "run test failed, " << err_msg << std::endl;
    }
    std::string k1 = conf->GetString("adslot.vta.popUp");
    const cloris::ConfNode* node = conf->GetConfNode("adslot.vta.splash");
    if (node) {
        std::cout << "adslot.vta.splash=" << node->AsString() << std::endl;
    }
    int k2 = conf->GetInt32("/adslot/vta/xxx", 13456);
    std::cout << "k1=" << k1 << std::endl;
    std::cout << "k2=" << k2 << std::endl;
}

void test_local_json() {
    std::string err_msg;
    cloris::Config* conf = cloris::Config::instance()->Load("../conf/common.json", SRC_LOCAL | FMT_JSON, &err_msg);
    if (conf) {
        std::cout << "run test success" << std::endl;
    } else {
        std::cout << "run test failed, " << err_msg << std::endl;
    }
    std::string k1 = conf->GetString("adslot.vta.popUp");
    const cloris::ConfNode* node = conf->GetConfNode("adslot.vta.splash");
    if (node) {
        std::cout << "adslot.vta.splash=" << node->AsString() << std::endl;
    }
    int k2 = conf->GetInt32("/adslot/vta/xxx", 13456);
    std::cout << "k1=" << k1 << std::endl;
    std::cout << "k2=" << k2 << std::endl;
}

void watch(cloris::ConfNode* node, const std::string& path, uint32_t event) {
    std::cout << "watch hit !!" << std::endl;
}

void test_zk() {
    std::string err_msg;
    cloris::Config* conf = cloris::Config::instance()->Load("../conf/zk.ini", SRC_ZK, &err_msg);
    if (conf) {
        std::cout << "run test success" << std::endl;
    } else {
        std::cout << "run test failed, " << err_msg << std::endl;
        return;
    }
    cloris::EventHandler handler = std::bind(watch, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    conf->Watch("rules", EVENT_INIT, handler);
    std::string k1 = conf->GetString("rules/popUp");
    std::cout << "value of rules/popUp=" + k1 << std::endl;

    const cloris::ConfNode* node = conf->GetConfNode("rules/splash");
    if (node) {
        std::cout << "ITYPE_ALL TEST--------" << std::endl;
        for (const cloris::ConfNodeIterator iter = node->begin(); iter != node->end(); ++iter) {
            std::cout << "node, key=" << iter->key() << ", value=" << iter->AsString() << std::endl;
        }
        std::cout << "ITYPE_LEAF TEST--------" << std::endl;
        for (const cloris::ConfNodeIterator iter = node->leaf_begin(); iter != node->leaf_end(); ++iter) {
            std::cout << "node, key=" << iter->key() << ", value=" << iter->AsString() << std::endl;
        }

        std::cout << "ITYPE_NONLEAF TEST--------" << std::endl;
        for (const cloris::ConfNodeIterator iter = node->non_leaf_begin(); iter != node->non_leaf_end(); ++iter) {
            std::cout << "node, key=" << iter->key() << ", value=" << iter->AsString() << std::endl;
        }
    }

}

#if 0
void test_direct_json() {
    std::string conf("{\"myself\":{\"name\":\"WeiJian\", \"school\":\"BUAA\",\"dr\":[\"cloris\", \"yt\"]}}");
    std::string err_msg("");
    if (cloris::Config::instance()->Load(conf, SRC_DIRECT|FMT_JSON, &err_msg)) {
        std::string my_name = cloris::Config::instance()->GetString("myself.name");
        const cloris::ConfNode* node = cloris::Config::instance()->GetConfNode("myself.dr");
        for (auto &p : *node) {
            std::cout << "dr, key=" << p.key() << ", value=" << p.AsString() << std::endl;
        }
        std::cout << "my name is " << my_name << std::endl;
        std::cout << "dr0=" << cloris::Config::instance()->GetString("myself.dr.0") << std::endl;
    } else {
        std::cout << "init direct config failed:" << err_msg << std::endl;
    }
}
#endif 

void test_direct_joml() {
    std::string conf("[myself] \n name=James Wei\n school=BUAA \n company=ofo\n");
    std::string err_msg("");
    if (cloris::Config::instance()->Load(conf, SRC_DIRECT, &err_msg)) {
        std::string my_name = cloris::Config::instance()->GetString("myself.name");
        std::cout << "my name is " << my_name << std::endl;
    } else {
        std::cout << "init direct config failed:" << err_msg << std::endl;
    }
}

int main(int argc, char** argv) {
    test_local();
    test_local_json();
    test_zk();
    test_direct_joml();
    // test_direct_json();
    return 0;
}

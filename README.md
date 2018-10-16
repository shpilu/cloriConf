# cloriConf
An Universal Configuration Middleware That Covered Both Local Config File Parsing and Distributed Configuration Center

usage:
conf = Config::instance().Load(path, SRC_LOCAL); // 加载本地文件
conf = Config::instance().Load(path, SRC_DIRECT); // 直接将入参作为参数解析
conf = Config::instance().Load(path, SRC_ZK); // 加载zookeeper
conf = Config::instance().Load(path, SRC_HTTP); // 加载HTTP
conf = Config::instance().Load(path, SRC_ZK, MD_LOOP); // 加载并监听, 如果有更新则更新配置
conf = Config::instance().LoadConfig(configure); // 以配置文件的形式加载


spect = std::bind(xxxx);
conf.Watch(path, spect); // 消息订阅, 
std::string name  = conf->getNode("path1.path2").asString(); // 读取path1/path2节点的配置并以string类型解析 
std::string name  = conf->getString("path1.path2"); // 读取path1/path2节点的配置并以string类型解析
int32_t year = conf->getInt32("path1.path2"); // 读取path1/path2节点的配置并以int32类型解析 

CNode *node = Config::instance()->getCNode(const std::string& key = "");
for (auto &p : dada) {
     LOG(p.asString());
}

cloriConf设计目标: 
(1) 从各个源(本地文件、直接配置、zookeeper、http接口)加载配置
(2) 兼容不同类型的配置格式(jini/ini, toml, json, xml, yaml, gflags)
(2) 快速的配置读取
(3) 配置热更新
(4) 消息订阅
(5) 配置格式转换
(6) 服务发现



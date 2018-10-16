
#include <set>
#include <deque>
#include "../../config_impl.h"
#include "../../config.h"
#include "config_zk.h"

#define BUF_LEN 1024000

namespace cloris {

ConfigZookeeper::ConfigZookeeper(ConfigImpl* impl) 
    : ConfigKeeper(impl),
      zh_(NULL),
      running_(false),
      timeout_(5000),
      interval_(10000) {
}

ConfigZookeeper::~ConfigZookeeper() {
    Stop();
}

void ConfigZookeeper::Start() {
    if (!running_) {
        running_ = true;
        worker_thread_.reset(new std::thread(&ConfigZookeeper::Job, this));
    }
}

void ConfigZookeeper::Stop() {
    if (running_) {
        running_ = false;
        worker_thread_->join();
        worker_thread_.reset();
    }
}

void ConfigZookeeper::Job() {
    std::string err_msg;
    while (running_) {
        std::this_thread::sleep_for(std::chrono::milliseconds(interval_));
        // std::cout << "running job" << std::endl;
        ScanZookeeper(err_msg);
        impl()->FlushWatcher();
    }
}

bool ConfigZookeeper::ScanZk(std::string& err_msg) {
    std::set<std::string> node_set;
    std::deque<std::string> queue;

    queue.push_back("");
    // 只有全部节点正常时才能进入下一步
    int zerr; 
    while (!queue.empty()) {
        std::string head = queue[0];
        struct String_vector str_vec;
        zerr = zoo_get_children(zh_, (root_ + head).c_str(), 0, &str_vec);
        if (zerr != ZOK) {
            err_msg = "zoo_get_children run failed, node=" + root_;
            return false;
        }
        for (int i = 0; i < str_vec.count; ++i) {
            std::string node_name = str_vec.data[i];
            std::string path = head + "/" + node_name;
            queue.push_back(path);
            node_set.insert(path);
        }
        queue.pop_front();
    }

    // now get config---
    std::string buf_str;
    buf_str.resize(BUF_LEN);
    char *pbuf = const_cast<char*>(buf_str.data());
    struct Stat stat;
    size_t new_node(0);

    size_t old_count = impl()->count();
    for (auto& p : node_set) {
        int buf_len(BUF_LEN);
        zerr = zoo_get(zh_, (root_ + p).c_str(), 0, pbuf, &buf_len, &stat);
        if (zerr != ZOK) {
            err_msg = "zoo_get error, node=" + p;
            return false;
        }
        buf_str[buf_len] = '\0';
        // if not found in old table or updated, do it
        bool noexist(false);
        if (impl()->CheckIfNotExistOrExpired(p, stat.mzxid, &noexist)) {
            std::string err_msg;
            impl()->Insert(p, pbuf, err_msg, stat.mzxid);
        }
        if (noexist) {
            ++new_node;
        }
    }
    if (old_count + new_node > node_set.size()) {
        impl()->DisableDeletedNode(node_set);
    }

    return true;
}

bool ConfigZookeeper::ConnectZk(std::string& err_msg) {
    zoo_set_debug_level(ZOO_LOG_LEVEL_WARN);
    zh_ = zookeeper_init(host_.c_str(), NULL, timeout_, NULL, NULL, 0);
    if (!zh_) {
        err_msg = "zookeeper init failed, host=" + host_;
        return false;
    } else {
        return true;
    }
}

void ConfigZookeeper::CloseZk() {
    zookeeper_close(zh_);
    zh_ = NULL;
}

bool ConfigZookeeper::ScanZookeeper(std::string& err_msg) {
    bool ret = this->ConnectZk(err_msg);
    if (ret) {
        ret = ScanZk(err_msg);
        CloseZk();
    }
    return ret;
}

bool ConfigZookeeper::LoadConfig(const std::string& src, std::string& err_msg) {
    static bool enable_update = true;
    // 会保存上次执行的错误信息
    std::shared_ptr<Config> conf = std::shared_ptr<Config>(new Config(src, SRC_LOCAL | FMT_JINI));
    if (!conf->ok()) {
        err_msg = conf->error();
        return false;
    }
    host_     = conf->getString("zookeeper.host");
    timeout_  = conf->getInt32("zookeeper.timeout");
    interval_ = conf->getInt32("zookeeper.interval");
    root_     = conf->getString("zookeeper.root");

    bool ret = this->ScanZookeeper(err_msg);
    if (ret && enable_update) {
        this->Start();
    }
    impl()->FlushWatcher();
    return ret;
}

} // namespace cloris

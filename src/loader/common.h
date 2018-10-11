
#ifndef CLORIS_LOADER_COMMON_H_
#define CLORIS_LOADER_COMMON_H_

#include <string> 

namespace cloris {

struct TraceNode {
    TraceNode(const std::string& s_id, int i_weight) 
        : id(s_id),
           weight(i_weight) { }

    std::string id;
    int weight;
};

} // namespace cloris

#endif // CLORIS_LOADER_COMMON_H_

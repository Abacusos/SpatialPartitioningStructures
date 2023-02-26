#pragma once

#include <vector>

#include "Types.h"

namespace sps {

    class KDTree {
    public:

        static KDTree buildTree(std::vector<V3f> points);

        void rangeSearch(const V3f& min, const V3f& max, std::vector<V3f>& result);

    private:
        
        using NodeIndex = u32;

        NodeIndex buildTreeLevel(std::vector<V3f> nodes, u32 depth);

        constexpr static NodeIndex invalidIndex = std::numeric_limits<NodeIndex>::max();
        struct Node {
            V3f data;
            NodeIndex left = invalidIndex;
            NodeIndex right = invalidIndex;
        };

        std::vector<Node> m_nodes;


    };
}
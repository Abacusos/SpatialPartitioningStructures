#include "KDTree.h"

#include <algorithm>
#include <deque>
#include <cassert>

namespace sps {

    namespace {
        enum class Axis : u8 {
            X, Y, Z, EnumMax
        };
    }

    KDTree KDTree::buildTree(std::vector<V3f> points)
    {
        KDTree result;
        auto numPoints = points.size();
        result.m_nodes.reserve(numPoints);
        result.buildTreeLevel(std::move(points), 0u);

        return result;
    }

    void KDTree::rangeSearch(const V3f& min, const V3f& max, std::vector<V3f>& result)
    {
        if (m_nodes.empty()) {
            return;
        }

        auto getF32OnAxis = [](const V3f& data, Axis axis) {
            switch (axis) {
            case Axis::X:
                return data.x;
            case Axis::Y:
                return data.y;
            case Axis::Z:
                return data.z;
            }
        };

        struct IndexWithDepth {
            NodeIndex index;
            u32 depth;
        };

        std::deque<IndexWithDepth> nodesToBeChecked;
        nodesToBeChecked.push_back(IndexWithDepth{ 0, 0 });

        while (!nodesToBeChecked.empty()) {
            auto [index, depth] = nodesToBeChecked.front();
            nodesToBeChecked.pop_front();

            assert(index < m_nodes.size());
            const auto& node = m_nodes[index];
            const auto& data = node.data;
            if ((min.x <= data.x && data.x <= max.x) &&
                (min.y <= data.y && data.y <= max.y) &&
                (min.z <= data.z && data.z <= max.z)) {
                result.emplace_back(data);
            }

            Axis axis = static_cast<Axis>(depth % static_cast<u32>(Axis::EnumMax));
            if (node.left != invalidIndex && getF32OnAxis(data, axis) >= getF32OnAxis(min, axis)) {
                nodesToBeChecked.push_back(IndexWithDepth{ node.left, depth + 1 });
            }

            if (node.right != invalidIndex && getF32OnAxis(data, axis) <= getF32OnAxis(max, axis)) {
                nodesToBeChecked.push_back(IndexWithDepth{ node.right, depth + 1 });
            }
        }
    }

    KDTree::NodeIndex KDTree::buildTreeLevel(std::vector<V3f> nodes, u32 depth)
    {
        if (nodes.empty()) {
            return invalidIndex;
        }

        auto median = nodes.begin();

        if (nodes.size() > 1)
        {
            Axis axis = static_cast<Axis>(depth % static_cast<u32>(Axis::EnumMax));
            std::advance(median, std::distance(nodes.begin(), nodes.end()) / 2);
            std::nth_element(nodes.begin(), median, nodes.end(),
                [axis](const V3f& left, const V3f& right) {
                    if (axis == Axis::X) {
                        return left.x < right.x;
                    }
                    else if (axis == Axis::Y) {
                        return left.y < right.y;
                    }
                    else {
                        return left.z < right.z;
                    }
                }
            );
        }

        NodeIndex nodeIndex = static_cast<uint32_t>(m_nodes.size());
        auto& node = m_nodes.emplace_back();
        node.data = *median;

        if (nodes.size() > 1)
        {
            std::vector<V3f> leftData;
            leftData.assign(nodes.begin(), median);
            node.left = buildTreeLevel(std::move(leftData), depth + 1);

            if (median != nodes.end()) {
                auto onePastMedian = median;
                std::advance(onePastMedian, 1u);

                std::vector<V3f> rightData;
                rightData.assign(onePastMedian, nodes.end());
                node.right = buildTreeLevel(std::move(rightData), depth + 1);
            }
        }

        return nodeIndex;
    }
}
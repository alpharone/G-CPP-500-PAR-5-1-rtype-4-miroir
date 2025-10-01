// /*
// ** EPITECH PROJECT, 2025
// ** bootstrap2
// ** File description:
// ** ZipperIterator
// */

#pragma once
#include <tuple>
#include <utility>
#include <algorithm>
#include "SparseArray.hpp"
#include "Entity.hpp"

namespace Ecs {

    template <typename... Containers>
    class IndexedZipper {
        public:
            IndexedZipper(Containers&... containers) : _containers(containers...) {}

            class iterator {
                public:
                    iterator(size_t idx, std::tuple<Containers&...>& containers) : _idx(idx), _containers(containers) {}

                    iterator& operator++()
                    {
                        _idx += 1;
                        return *this;
                    }
                    bool operator!=(iterator const& other) const
                    {
                        return _idx != other._idx;
                    }

                    auto operator*()
                    {
                        return deref(std::index_sequence_for<Containers...>{});
                    }
                
                private:
                    size_t _idx;
                    std::tuple<Containers&...>& _containers;
                
                    template <std::size_t... I>
                    auto deref(std::index_sequence<I...>)
                    {
                        return std::tuple<Entity, typename Containers::reference_type...>(
                            Entity(_idx), std::get<I>(_containers)[_idx]...
                        );
                    }
            };
            
            iterator begin()
            {
                return iterator(0, _containers);
            }
            iterator end()
            {
                return iterator(minSize(), _containers);
            }

        private:
            std::tuple<Containers&...> _containers;

            size_t minSize() const
            {
                std::array<size_t, sizeof...(Containers)> sizes = {
                    std::get<Containers&>(_containers).size()...
                };
                return *std::min_element(sizes.begin(), sizes.end());
            }
    };

    template <typename... Containers>
    IndexedZipper<Containers...> makeIndexedZipper(Containers&... c)
    {
        return IndexedZipper<Containers...>(c...);
    }
}

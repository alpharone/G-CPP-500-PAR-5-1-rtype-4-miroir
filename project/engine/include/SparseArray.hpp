/*
** EPITECH PROJECT, 2025
** bootstrap2
** File description:
** SparseArray
*/

#pragma once

#include <optional>
#include <vector>
#include <cstddef>
#include <utility>
#include <stdexcept>
#include <memory>

namespace Ecs {

    template <typename Component>
    class SparseArray {
        public:
            using value_type = std::optional<Component>;
            using reference_type = value_type&;
            using const_reference_type = value_type const&;
            using container_t = std::vector<value_type>;
            using size_type = typename container_t::size_type;
            using iterator = typename container_t::iterator;
            using const_iterator = typename container_t::const_iterator;

            SparseArray() = default;
            SparseArray(SparseArray const&) = default;
            SparseArray(SparseArray&&) noexcept = default;
            ~SparseArray() = default;
            SparseArray& operator=(SparseArray const&) = default;
            SparseArray& operator=(SparseArray&&) noexcept = default;

            reference_type operator[](size_t i)
            {
                if (i >= _data.size()) {
                    _data.resize(i + 1);
                }
                return _data[i];
            }

            const_reference_type operator[](size_t i) const
            {
                static const value_type empty{};
                return (i < _data.size()) ? _data[i] : empty;
            }

            iterator begin() noexcept
            {
                return _data.begin();
            }

            const_iterator begin() const noexcept
            {
                return _data.begin();
            }

            const_iterator cbegin() const noexcept
            {
                return _data.cbegin();
            }

            iterator end() noexcept
            {
                return _data.end();
            }

            const_iterator end() const noexcept
            {
                return _data.end();
            }

            const_iterator cend() const noexcept
            {
                return _data.cend();
            }

            size_type size() const noexcept
            {
                return _data.size();
            }

            reference_type insert_at(size_type pos, Component const& c)
            {
                if (pos >= _data.size()) {
                    _data.resize(pos + 1);
                }
                _data[pos] = c;
                return _data[pos];
            }
        
            reference_type insert_at(size_type pos, Component&& c)
            {
                if (pos >= _data.size()) {
                    _data.resize(pos + 1);
                }
                _data[pos] = std::move(c);
                return _data[pos];
            }
        
            template <class... Params>
            reference_type emplace_at(size_type pos, Params&&... params)
            {
                if (pos >= _data.size()) {
                    _data.resize(pos + 1);
                }
                _data[pos].emplace(std::forward<Params>(params)...);
                return _data[pos];
            }
        
            void erase(size_type pos) noexcept
            {
                if (pos < _data.size()) {
                    _data[pos].reset();
                }
            }
        
            size_type get_index(value_type const& val) const
            {
                if (_data.empty()) {
                    throw std::out_of_range("sparse_array::get_index: empty");
                }
                auto base = std::addressof(_data.front());
                auto ptr  = std::addressof(val);
                ptrdiff_t diff = ptr - base;
                if (diff < 0 || static_cast<size_type>(diff) >= _data.size())
                    throw std::out_of_range("sparse_array::get_index: not in container");
                return static_cast<size_type>(diff);
            }

        private:
            container_t _data;

    };

}
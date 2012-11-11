/*
 * small_vector.hpp
 *
 *  Created on: 2012-10-9
 *      Author: yanghuan
 */

#ifndef SMALL_VECTOR_HPP_
#define SMALL_VECTOR_HPP_

#include <algorithm>
#include <cassert>
#include <iterator>
#include <stdexcept>

#define noexcept

template<class Value, std::size_t RequestedMaxInline, bool ShouldUseHeap, class Alloc = std::allocator<Value>>
class small_vector {
public:
    typedef std::size_t size_type;
    typedef Value              value_type;
    typedef value_type&        reference;
    typedef value_type*        pointer;
    typedef const value_type*  const_pointer;
    typedef value_type const&  const_reference;
    typedef value_type*        iterator;
    typedef value_type const*  const_iterator;
    typedef std::ptrdiff_t     difference_type;
    typedef std::reverse_iterator<iterator>       reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
    typedef Alloc                               allocator_type;
public:
    small_vector() {
    }

    small_vector(const small_vector& o) : small_vector{o.begin(), o.end()} {

    }

    small_vector(small_vector&& o) {
        *this = std::move(o);
    }

    small_vector(std::initializer_list<value_type> l) : small_vector{l.beign(), l.end()} {
    }

    template<class InputIterator>
    small_vector(InputIterator first, InputIterator last) {
        assign(first, last);
    }

    small_vector& operator=(const small_vector& o) {
        assign(o.begin(), o.end());
        return *this;
    }

    small_vector& operator=(small_vector&& o) {
        clear();
        if (!o.size_policy_.isExtern()) {
            allocator_type& allocator = size_policy_;
            reserve(o.size());
            for (std::size_t i = 0; i < o.size(); ++i) {
                allocator.construct(data() + i, std::move(o[i]));
            }
            size_policy_.setSize(o.size());
        } else {
            swap(o);
        }
        return *this;
    }

    small_vector& operator=(std::initializer_list<value_type> o) {
        assign(o.begin(), o.end());
        return *this;
    }

    void assign(size_type n, const value_type& val) {
        clear();
        insert(end(), n, val);
    }

    void assign(std::initializer_list<value_type> l) {
        assign(l.begin(), l.end());
    }

    template<class InputIterator>
    void assign(InputIterator first, InputIterator last) {
        clear();
        insert(first, last);
    }

    iterator begin() noexcept {
        return data();
    }

    const_iterator begin() const noexcept {
        return data();
    }

    iterator end() noexcept {
        return data() + size();
    }

    const_iterator end() const noexcept {
        return data() + size();
    }

    reverse_iterator rbegin() noexcept {
        return reverse_iterator(end());
    }

    const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }

    reverse_iterator rend() noexcept {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }

    const_iterator cbegin() const noexcept {
        return begin();
    }

    const_iterator cend() const noexcept {
        return end();
    }

    const_reverse_iterator crbegin() const noexcept {
        return rbegin();
    }

    const_reverse_iterator crend() const noexcept {
        return rend();
    }

    size_type size() const noexcept {
        return size_policy_.size();
    }

    size_type max_size() const noexcept {
        return kShouldUseHeap ? size_policy_.policyMaxSize() : kRequestedMaxInline;
    }

    void resize(size_type new_size) {
        resize(new_size, value_type());
    }

    void resize(size_type new_size, const value_type& val) {
        const size_type old_size = size();
        if(new_size < old_size) {
            erase(begin() + new_size, end());
        }else if(new_size > old_size) {
            reserve(new_size);
            std::uninitialized_fill_n(end(), new_size - old_size, val);
            size_policy_.setSize(new_size);
        }
    }

    void shrink_to_fit() {
        if (size_policy_.isExtern()) {
            if (capacity() != size()) {
                small_vector tmp(begin(), end());
                tmp.swap(*this);
            }
        }
    }

    size_type capacity() const noexcept {
        return size_policy_.isExtern() ? u.heap_ptr_.capacity_ : RequestedMaxInline;
    }

    bool empty() const noexcept {
        return !size();
    }

    void reserve(size_type size) {
        if(size > max_size()) {
            throw std::length_error("small_vector::reserve");
        }

        if(capacity() < size) {
            allocator_type& allocator = size_policy_;
            pointer ptr = allocator.allocate(size);
            try {
                std::uninitialized_copy(
                        std::make_move_iterator(begin()),
                        std::make_move_iterator(end()),
                        ptr);
            }catch (...) {
                allocator.deallocate(ptr, size);
                throw;
            }

            for (auto& val : *this) {
                allocator.destroy(std::addressof(val));
            }

            freeHeap();

            size_policy_.setExtern(true);
            u.heap_ptr_.ptr_ = ptr;
            u.heap_ptr_.capacity_ = size;
        }
    }

    reference operator[](size_type i) noexcept {
        assert(i < size());
        return *(begin() + i);
    }

    const_reference operator[](size_type i) const noexcept {
        assert(i < size());
        return *(begin() + i);
    }

    reference at(size_type i) {
        if (i >= size()) {
            throw std::out_of_range("small_vector::at");
        }
        return (*this)[i];
    }

    const_reference at(size_type i) const {
        if (i >= size()) {
            throw std::out_of_range("small_vector::at");
        }
        return (*this)[i];
    }

    reference front() {
        assert(!empty());
        return *begin();
    }

    const_reference front() const {
        assert(!empty());
        return *begin();
    }

    reference back() {
        assert(!empty());
        return *(end() - 1);
    }

    reference back() const {
        assert(!empty());
        return *(end() - 1);
    }

    pointer data() noexcept {
        return size_policy_.isExtern() ? u.heap() : u.buffer();
    }

    const_pointer data() const noexcept {
        return size_policy_.isExtern() ? u.heap() : u.buffer();
    }

    void push_back(const value_type& val) {
        const size_type old_size = size();
        if(old_size == capacity()){
            reserve(next_capacity());
        }
        allocator_type& allocator = size_policy_;
        allocator.construct(end(), val);
        size_policy_.setSize(old_size + 1);
    }

    void push_back(value_type&& val) {
        emplace_back(std::move(val));
    }

    void pop_back() {
        erase(end() - 1);
    }

    template<class... Args>
    void emplace_back(Args&&... args) {
        const size_type old_size = size();
        if(old_size == capacity()) {
            reserve(next_capacity());
        }
        allocator_type& allocator = size_policy_;
        allocator.construct(end(), std::forward<Args>(args)...);
        size_policy_.setSize(old_size + 1);
    }

    template<typename... Args>
    iterator emplace(iterator position, Args&&... args) {
        if(position == end()) {
            emplace_back(std::forward<Args>(args)...);
            return end() -1;
        }

        allocator_type& allocator = size_policy_;
        const size_type offset = position - begin();
        const size_type old_size = size();

        reserve(old_size + 1);

        allocator.construct(data() + old_size, std::move(back()));
        std::move_backward(position, data() + old_size - 2, data() + old_size - 1);
        *position = value_type(std::forward<Args>(args)...);
        size_policy_.setSize(old_size + 1);
        return position + offset;
    }

    iterator insert(iterator position, const value_type& val) {
        return insert(position, value_type(val));
    }

    iterator insert(iterator position, value_type&& val) {
        return emplace(position, std::move(val));
    }

    void insert(iterator position, std::initializer_list<value_type> l) {
        insert(position, l.begin(), l.end());
    }

    void insert(iterator position, size_type n, const value_type& val) {
        const size_type new_size = size() + n;
        iterator finish_iterator = end();
        const size_type elems_after = finish_iterator - position;

        reserve(new_size);

        if(elems_after > n) {
            std::uninitialized_copy(
                           std::make_move_iterator(finish_iterator - n),
                           std::make_move_iterator(finish_iterator),
                           finish_iterator);
            std::move_backward(position, finish_iterator - n, finish_iterator);
            std::fill(position, position + n, val);
        }else {
            std::uninitialized_fill_n(finish_iterator, n - elems_after, val);
            std::uninitialized_copy(
                                   std::make_move_iterator(position),
                                   std::make_move_iterator(finish_iterator),
                                   finish_iterator + n - elems_after);
            std::fill(position, finish_iterator, val);
        }
        size_policy_.setSize(new_size);
    }

    template<class InputIterator>
    void insert(iterator position, InputIterator first, InputIterator last) {
        typedef typename std::iterator_traits<InputIterator>::iterator_category categ;
        if(std::is_same<categ, std::input_iterator_tag>::value) {
            while (first != last) {
              insert(position, *first);
              ++first;
            }
        } else {
            if(first != last) {
                const size_type n = std::distance(first, last);
                const size_type new_size = size() + n;
                iterator finish_iterator = end();
                const size_type elems_after = finish_iterator - position;

                reserve(new_size);

                if(elems_after > n) {
                    std::uninitialized_copy(
                            std::make_move_iterator(finish_iterator - n),
                            std::make_move_iterator(finish_iterator),
                            finish_iterator);
                    std::move_backward(position, finish_iterator - n, finish_iterator);
                    std::copy(first, last, position);
                }else {
                    InputIterator mid = first;
                    std::advance(mid, elems_after);
                    std::uninitialized_copy(mid, last, finish_iterator);
                    std::uninitialized_copy(
                            std::make_move_iterator(position),
                            std::make_move_iterator(finish_iterator),
                            finish_iterator + n - elems_after);
                    std::copy(first, mid, position);
                }
                size_policy_.setSize(new_size);
            }
        }
    }

    iterator erase(iterator position) {
        allocator_type& allocator = size_policy_;
        iterator new_end = std::move(position + 1, end(), position);
        allocator.destroy(std::addressof(*new_end));
        size_policy_.setSize(new_end - begin());
        return position;
    }

    iterator erase(iterator first, iterator last) {
        if(first != last) {
            allocator_type& allocator = size_policy_;
            iterator new_end = std::move(last, end(), first);
            for (iterator it = new_end; it != end(); ++it) {
                allocator.destroy(std::addressof(*it));
            }
            size_policy_.setSize(new_end - begin());
        }
        return first;
    }

    void swap(small_vector& v) {
        if(size_policy_.isExtern() && v.size_policy_.isExtern()) {
            std::swap(u.heap_ptr_, v.u.heap_ptr_);
        }else if(!size_policy_.isExtern() && !v.size_policy_.isExtern()) {
            using std::swap;  //Allow ADL on swap for our value_type.

            small_vector* smaller;
            small_vector* larger;
            if(size() < v.size()) {
                smaller = this;
                larger = &v;
            }else {
                smaller = &v;
                larger = this;
            }

            iterator first = larger->begin();
            for(auto& i : *smaller) {
                swap(i, *first++);
            }

            std::uninitialized_copy(
                    std::make_move_iterator(first),
                    std::make_move_iterator(larger->end()),
                    smaller->end()
                    );
        } else {
            small_vector* externer;
            small_vector* inliner;
            if(size_policy_.isExtern()) {
                externer = this;
                inliner = &v;
            }else {
                externer = &v;
                inliner = this;
            }

            HeapPtr heap_ptr = externer->u.heap_ptr_;
            try {
                std::uninitialized_copy(
                              std::make_move_iterator(inliner->begin()),
                              std::make_move_iterator(inliner->end()),
                              externer->u.buffer()
                              );
            }catch(...) {
                externer->u.heap_ptr_ = heap_ptr;
                throw;
            }
            inliner->u.heap_ptr_ = heap_ptr;
        }
        std::swap(size_policy_, v.size_policy_);
    }

    void clear() {
        erase(begin(), end());
    }

private:
    void freeHeap() {
        if(u.heap_ptr_.ptr_ != nullptr) {
            allocator_type& allocator = size_policy_;
            allocator.deallocate(u.heap_ptr_.ptr_, u.heap_ptr_.capacity_);
            u.heap_ptr_.ptr_ = nullptr;
        }
    }
    size_type next_capacity() const {
        return std::max(size_type(2), 3 * size() / 2);
    }
private:
    static const size_t kRequestedMaxInline = RequestedMaxInline;
    static const bool kShouldUseHeap = ShouldUseHeap;
    typedef unsigned char InlineStorageType[sizeof(value_type) * kRequestedMaxInline];
    struct HeapPtr {
        pointer ptr_;
        std::size_t capacity_;
    };

    class IntegralSizePolicy : public allocator_type {
    public:
        bool isExtern() const {
            return (kExternMask & size_) != 0;
        }

        void setExtern(bool b) {
            if (b) {
                size_ |= kExternMask;
            } else {
                size_ &= ~kExternMask;
            }
        }

        std::size_t size() const {
            return size_ & (~kExternMask);
        }

        void setSize(std::size_t sz) {
            assert(sz <= policyMaxSize());
            size_ = (kExternMask & size_) | sz;
        }

        std::size_t policyMaxSize() const {
            return ~kExternMask;
        }
    private:
        static const std::size_t kExternMask = kShouldUseHeap ? std::size_t(1) << (sizeof(std::size_t) * 8 - 1) : 0;
        std::size_t size_ = 0;
    };
private:
    IntegralSizePolicy size_policy_;
    union Data {
        HeapPtr heap_ptr_;
        InlineStorageType storage_;

        Data() { heap_ptr_.ptr_ = nullptr;  }

        pointer buffer() noexcept {
            void* data = &storage_;
            return static_cast<value_type*>(data);
        }

        const_pointer buffer() const noexcept {
            void* data = &storage_;
            return static_cast<value_type*>(data);
        }

        pointer heap() noexcept {
            return heap_ptr_.ptr_;
        }

        const_pointer heap() const noexcept {
            return heap_ptr_.ptr_;
        }
    } u;
};

template<class T, std::size_t RequestedMaxInline, bool ShouldUseHeap, class Alloc>
void swap(small_vector<T, RequestedMaxInline, ShouldUseHeap, Alloc>& a,
        small_vector<T, RequestedMaxInline, ShouldUseHeap, Alloc>& b) {
    a.swap(b);
}

#endif /* SMALL_VECTOR_HPP_ */

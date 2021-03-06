#ifndef BOOST_TEXT_DETAIL_ROPE_ITERATOR_HPP
#define BOOST_TEXT_DETAIL_ROPE_ITERATOR_HPP

#include <boost/text/detail/iterator.hpp>


namespace boost { namespace text {

    struct unencoded_rope_view;
    struct rope_view;

}}

namespace boost { namespace text { namespace detail {

    struct const_rope_iterator
    {
        using value_type = char;
        using difference_type = std::ptrdiff_t;
        using pointer = char const *;
        using reference = char;
        using iterator_category = std::random_access_iterator_tag;

        const_rope_iterator() noexcept :
            rope_(nullptr),
            n_(-1),
            leaf_(nullptr),
            leaf_start_(-1)
        {}

        const_rope_iterator(
            unencoded_rope const & r, difference_type n) noexcept :
            rope_(&r),
            n_(n),
            leaf_(nullptr),
            leaf_start_(0)
        {}

        reference operator*() const noexcept
        {
            if (leaf_) {
                return deref();
            } else {
                found_char found;
                find_char(rope_->ptr_, n_, found);
                leaf_ = found.leaf_.leaf_->as_leaf();
                leaf_start_ = n_ - found.leaf_.offset_;
                return found.c_;
            }
        }

        value_type operator[](difference_type n) const noexcept
        {
            auto it = *this;
            if (0 <= n)
                it += n;
            else
                it -= -n;
            return *it;
        }

        const_rope_iterator & operator++() noexcept
        {
            ++n_;
            if (leaf_ && n_ == leaf_start_ + leaf_->size())
                leaf_ = nullptr;
            return *this;
        }
        const_rope_iterator operator++(int)noexcept
        {
            const_rope_iterator retval = *this;
            ++*this;
            return retval;
        }
        const_rope_iterator & operator+=(difference_type n) noexcept
        {
            n_ += n;
            leaf_ = nullptr;
            return *this;
        }

        const_rope_iterator & operator--() noexcept
        {
            if (leaf_ && n_ == leaf_start_)
                leaf_ = nullptr;
            --n_;
            return *this;
        }
        const_rope_iterator operator--(int)noexcept
        {
            const_rope_iterator retval = *this;
            --*this;
            return retval;
        }
        const_rope_iterator & operator-=(difference_type n) noexcept
        {
            n_ -= n;
            leaf_ = nullptr;
            return *this;
        }

        friend bool
        operator==(const_rope_iterator lhs, const_rope_iterator rhs) noexcept
        {
            return lhs.rope_ == rhs.rope_ && lhs.n_ == rhs.n_;
        }
        friend bool
        operator!=(const_rope_iterator lhs, const_rope_iterator rhs) noexcept
        {
            return !(lhs == rhs);
        }
        friend bool
        operator<(const_rope_iterator lhs, const_rope_iterator rhs) noexcept
        {
            return lhs.rope_ == rhs.rope_ && lhs.n_ < rhs.n_;
        }
        friend bool
        operator<=(const_rope_iterator lhs, const_rope_iterator rhs) noexcept
        {
            return lhs == rhs || lhs < rhs;
        }
        friend bool
        operator>(const_rope_iterator lhs, const_rope_iterator rhs) noexcept
        {
            return rhs < lhs;
        }
        friend bool
        operator>=(const_rope_iterator lhs, const_rope_iterator rhs) noexcept
        {
            return rhs <= lhs;
        }

        friend const_rope_iterator
        operator+(const_rope_iterator lhs, difference_type rhs) noexcept
        {
            return lhs += rhs;
        }
        friend const_rope_iterator
        operator-(const_rope_iterator lhs, difference_type rhs) noexcept
        {
            return lhs -= rhs;
        }
        friend difference_type
        operator-(const_rope_iterator lhs, const_rope_iterator rhs) noexcept
        {
            BOOST_ASSERT(lhs.rope_ == rhs.rope_);
            return lhs.n_ - rhs.n_;
        }

    private:
        const_rope_iterator(
            unencoded_rope const * r, difference_type n) noexcept :
            rope_(r),
            n_(n),
            leaf_(nullptr),
            leaf_start_(0)
        {}

        char deref() const
        {
            switch (leaf_->which_) {
            case which::t: {
                string const * t = static_cast<string *>(leaf_->buf_ptr_);
                return *(t->begin() + (n_ - leaf_start_));
            }
            case which::rtv: {
                repeated_string_view const * rtv =
                    static_cast<repeated_string_view *>(leaf_->buf_ptr_);
                return *(rtv->begin() + (n_ - leaf_start_));
            }
            case which::ref: {
                detail::reference<rope_tag> const * ref =
                    static_cast<detail::reference<rope_tag> *>(leaf_->buf_ptr_);
                return *(ref->ref_.begin() + (n_ - leaf_start_));
            }
            default: BOOST_ASSERT(!"unhandled rope node case"); break;
            }
            return '\0'; // This should never execute.
        }

        unencoded_rope const * rope_;
        difference_type n_;
        mutable leaf_node_t<rope_tag> const * leaf_;
        mutable difference_type leaf_start_;

        friend struct ::boost::text::unencoded_rope_view;
        friend struct ::boost::text::rope_view;
    };

    struct const_rope_view_iterator
    {
        using value_type = char;
        using difference_type = std::ptrdiff_t;
        using pointer = char const *;
        using reference = char;
        using iterator_category = std::random_access_iterator_tag;

        const_rope_view_iterator() noexcept : which_(which::r) {}
        explicit const_rope_view_iterator(const_rope_iterator it) noexcept :
            r_(it),
            which_(which::r)
        {}
        explicit const_rope_view_iterator(char const * it) noexcept :
            tv_(it),
            which_(which::tv)
        {}
        explicit const_rope_view_iterator(
            const_repeated_chars_iterator it) noexcept :
            rtv_(it),
            which_(which::rtv)
        {}

        const_rope_iterator as_rope_iter() const
        {
            BOOST_ASSERT(which_ == which::r);
            return r_;
        }

        reference operator*() const noexcept
        {
            switch (which_) {
            case which::r: return *r_;
            case which::tv: return *tv_;
            case which::rtv: return *rtv_;
            }
            return '\0'; // This should never execute.
        }
        value_type operator[](difference_type n) const noexcept
        {
            switch (which_) {
            case which::r: return r_[n];
            case which::tv: return tv_[n];
            case which::rtv: return rtv_[n];
            }
            return '\0'; // This should never execute.
        }

        const_rope_view_iterator & operator++() noexcept
        {
            switch (which_) {
            case which::r: ++r_; break;
            case which::tv: ++tv_; break;
            case which::rtv: ++rtv_; break;
            }
            return *this;
        }
        const_rope_view_iterator operator++(int)noexcept
        {
            const_rope_view_iterator retval = *this;
            ++*this;
            return retval;
        }
        const_rope_view_iterator & operator+=(difference_type n) noexcept
        {
            switch (which_) {
            case which::r: r_ += n; break;
            case which::tv: tv_ += n; break;
            case which::rtv: rtv_ += n; break;
            }
            return *this;
        }

        const_rope_view_iterator & operator--() noexcept
        {
            switch (which_) {
            case which::r: --r_; break;
            case which::tv: --tv_; break;
            case which::rtv: --rtv_; break;
            }
            return *this;
        }
        const_rope_view_iterator operator--(int)noexcept
        {
            const_rope_view_iterator retval = *this;
            --*this;
            return retval;
        }
        const_rope_view_iterator & operator-=(difference_type n) noexcept
        {
            switch (which_) {
            case which::r: r_ -= n; break;
            case which::tv: tv_ -= n; break;
            case which::rtv: rtv_ -= n; break;
            }
            return *this;
        }

        friend bool operator==(
            const_rope_view_iterator lhs, const_rope_view_iterator rhs) noexcept
        {
            if (lhs.which_ != rhs.which_)
                return false;
            switch (lhs.which_) {
            case which::r: return lhs.r_ == rhs.r_;
            case which::tv: return lhs.tv_ == rhs.tv_;
            case which::rtv: return lhs.rtv_ == rhs.rtv_;
            }
            return false; // This should never execute.
        }
        friend bool operator!=(
            const_rope_view_iterator lhs, const_rope_view_iterator rhs) noexcept
        {
            return !(lhs == rhs);
        }
        friend bool operator<(
            const_rope_view_iterator lhs, const_rope_view_iterator rhs) noexcept
        {
            if (lhs.which_ != rhs.which_)
                return false;
            switch (lhs.which_) {
            case which::r: return lhs.r_ < rhs.r_;
            case which::tv: return lhs.tv_ < rhs.tv_;
            case which::rtv: return lhs.rtv_ < rhs.rtv_;
            }
            return false; // This should never execute.
        }
        friend bool operator<=(
            const_rope_view_iterator lhs, const_rope_view_iterator rhs) noexcept
        {
            return lhs < rhs || lhs == rhs;
        }
        friend bool operator>(
            const_rope_view_iterator lhs, const_rope_view_iterator rhs) noexcept
        {
            return rhs < lhs;
        }
        friend bool operator>=(
            const_rope_view_iterator lhs, const_rope_view_iterator rhs) noexcept
        {
            return lhs > rhs || lhs == rhs;
        }

        friend const_rope_view_iterator
        operator+(const_rope_view_iterator lhs, difference_type rhs) noexcept
        {
            return lhs += rhs;
        }
        friend const_rope_view_iterator
        operator-(const_rope_view_iterator lhs, difference_type rhs) noexcept
        {
            return lhs -= rhs;
        }
        friend difference_type operator-(
            const_rope_view_iterator lhs, const_rope_view_iterator rhs) noexcept
        {
            if (lhs.which_ != rhs.which_)
                return false;
            switch (lhs.which_) {
            case which::r: return lhs.r_ - rhs.r_;
            case which::tv: return lhs.tv_ - rhs.tv_;
            case which::rtv: return lhs.rtv_ - rhs.rtv_;
            }
            return 0; // This should never execute.
        }

    private:
        enum class which { r, tv, rtv };

        const_rope_iterator r_;
        char const * tv_;
        const_repeated_chars_iterator rtv_;

        which which_;
    };

}}}

#endif

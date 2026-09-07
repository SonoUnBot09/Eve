#include <memory>

template <typename T, typename Allocator = std::allocator<T>>
class DefaultNoInitAllocator : public Allocator 
{
    using a_t = std::allocator_traits<Allocator>;

    public:

        template<typename U> struct rebind 
        {
            using other = DefaultNoInitAllocator<U, typename a_t::template rebind_alloc<U>>;
        };

        using Allocator::Allocator;

        template <typename U>
        void construct(U* ptr) noexcept(std::is_nothrow_default_constructible<U>::value)
        {
            ::new (static_cast<void*>(ptr)) U;
        }

        template <typename U, typename... Args>
        void construct(U* ptr, Args&&... args) 
        {
            a_t::construct(static_cast<Allocator&>(*this), ptr, std::forward<Args>(args)...);
        }
};
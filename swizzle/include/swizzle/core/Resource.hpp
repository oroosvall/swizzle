#ifndef RESOURCE_HPP
#define RESOURCE_HPP

#include <memory>

namespace swizzle::core
{

    template<typename T> using Resource = std::shared_ptr<T>;

    template <class _Ty, class... _Types>
    _NODISCARD Resource<_Ty> CreateRef(_Types&&... _Args)
    {
        return std::make_shared<_Ty>(std::forward<_Types>(_Args)...);
    }

    class IResource
    {
    public:
        virtual void release() = 0;
    protected:
        virtual ~IResource() {}
    };

}

#endif
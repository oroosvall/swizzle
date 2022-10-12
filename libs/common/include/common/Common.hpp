#ifndef COMMON_HPP
#define COMMON_HPP

/* Include files */

#include <cstring>
#include <memory>
#include <array>
#include <vector>

/* Defines */

#define UNUSED_ARG(x) (void)x;
#define COUNT_OF(arr) (sizeof(arr) / sizeof(arr[0]))

/* Typedefs/enums */

typedef uint64_t U64;

typedef int32_t S32;
typedef uint32_t U32;

typedef int16_t S16;
typedef uint16_t U16;

typedef int8_t S8;
typedef uint8_t U8;

typedef char SwChar;
typedef wchar_t SwWChar;

typedef float F32;
typedef double F64;

typedef bool SwBool;

typedef float DeltaTime;

/* Forward Declared Structs/Classes */

namespace common
{
    template<typename T> using Resource = std::shared_ptr<T>;
    template<typename T> using Unique = std::unique_ptr<T>;
}

/* Struct Declaration */

namespace common
{
    template <typename R, typename T>
    struct Result
    {
        R result;
        Resource<T> obj;
    };
}

/* Class Declaration */

namespace common
{
    template <class _Ty, class... _Types>
    [[nodiscard]] Resource<_Ty> CreateRef(_Types&&... _Args)
    {
        return std::make_shared<_Ty>(std::forward<_Types>(_Args)...);
    }

    template<class _Ty, class... _Types>
    [[nodiscard]] Unique<_Ty> CreateUnique(_Types&&... _Args)
    {
        return std::make_unique<_Ty>(std::forward<_Types>(_Args)...);
    }

    class IResource
    {
    public:
        virtual void release() = 0;
    protected:
        virtual ~IResource() {}
    };

    template<typename T> using IterType = std::vector<T>;
}

/* Function Declaration */

#endif
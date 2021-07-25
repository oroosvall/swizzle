#ifndef BUFFER_READER_HPP
#define BUFFER_READER_HPP

/* Include files */

#include <cstdint>
#include <vector>
#include <memory>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

namespace utils
{
    class BufferReader
    {
    public:
        BufferReader(std::vector<uint8_t>& container)
            : mContainer(std::move(container))
            , mReadOffset(0u)
        {
        }

        template<typename T>
        T* read()
        {
            const size_t typeSize = sizeof(T);
            const size_t offset = mReadOffset;
            mReadOffset += typeSize;
            T* result = nullptr;

            if (offset + typeSize <= mContainer.size())
            {
                result = (T*)(mContainer.data() + offset);
            }

            return result;
        }

        template<typename T>
        bool readNum(T& t)
        {
            bool ok = false;

            const size_t typeSize = sizeof(T);
            const size_t offset = mReadOffset;
            mReadOffset += typeSize;

            if (offset + typeSize <= mContainer.size())
            {
                t = *(T*)(mContainer.data() + offset);
                ok = true;
            }

            return ok;
        }

        template<typename T>
        T* readArrayU32(U32 length)
        {
            const size_t totalSize = sizeof(T) * length;
            const size_t offset = mReadOffset;
            mReadOffset += totalSize;;
            T* result = nullptr;


            if (offset + totalSize <= mContainer.size())
            {
                result = (T*)(mContainer.data() + offset);
            }

            return result;
        }

    private:
        std::vector<uint8_t> mContainer;
        size_t mReadOffset;
    };
}

/* Function Declaration */

#endif
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
            , mWriteOffset(0u)
        {
        }

        template<typename T>
        T* read()
        {
            const size_t typeSize = sizeof(T);
            const size_t offset = mWriteOffset;
            mWriteOffset += typeSize;
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
            const size_t offset = mWriteOffset;
            mWriteOffset += typeSize;

            if (offset + typeSize <= mContainer.size())
            {
                t = *(T*)(mContainer.data() + offset);
                ok = true;
            }

            return ok;
        }

        template<typename T>
        T* readArrayU32(uint32_t length)
        {
            const size_t totalSize = sizeof(T) * length;
            const size_t offset = mWriteOffset;
            mWriteOffset += totalSize;;
            T* result = nullptr;


            if (offset + totalSize <= mContainer.size())
            {
                result = (T*)(mContainer.data() + offset);
            }

            return result;
        }

    private:
        std::vector<uint8_t> mContainer;
        size_t mWriteOffset;
    };
}

/* Function Declaration */

#endif
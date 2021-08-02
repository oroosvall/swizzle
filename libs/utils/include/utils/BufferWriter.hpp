#ifndef BUFFER_WRITER_HPP
#define BUFFER_WRITER_HPP

/* Include files */

#include <cstdint>
#include <vector>
#include <memory>
#include <fstream>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

namespace utils
{
    class BufferWriter
    {
    public:
        BufferWriter(std::ofstream&& file)
            : mFile(std::move(file))
        {
        }

        ~BufferWriter()
        {
        }

        template<typename T>
        void write(T value)
        {
            size_t s = sizeof(T);
            mFile.write((const char*)&value, s);
        }

        template<typename T>
        void writeArray(T* value, size_t size)
        {
            size_t s = sizeof(T) * size;
            mFile.write((const char*)value, s);
        }

    private:
        std::ofstream mFile;
    };
}

/* Function Declaration */

#endif
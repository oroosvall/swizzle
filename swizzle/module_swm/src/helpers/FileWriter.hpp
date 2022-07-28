#ifndef FILE_WRITER_HPP
#define FILE_WRITER_HPP

/* Include files */

#include <ostream>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

namespace swm::file
{
    class FileWriter
    {
    public:
        FileWriter(std::ostream& stream)
            : mStream(stream)
        {
        }

        template<typename T>
        bool write(T value)
        {
            size_t s = sizeof(T);
            mStream.write((const char*)&value, s);
            return mStream.good();
        }

        template<typename T>
        bool writeArray(T* value, size_t size)
        {
            size_t s = sizeof(T) * size;
            mStream.write((const char*)value, s);
            return mStream.good();
        }

    private:
        std::ostream& mStream;
    };
}

/* Function Declaration */

#endif

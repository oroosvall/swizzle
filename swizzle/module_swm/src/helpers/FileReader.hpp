#ifndef FILE_READER_HPP
#define FILE_READER_HPP

/* Include files */

#include <vector>
#include <fstream>
#include <istream>
#include <cstdint>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

/* Function Declaration */

namespace swm::file
{
    class FileReader
    {
    public:
        FileReader(std::istream& stream)
            : mStream(stream)
            , mReadCount(0ull)
            , mFileSize(0ull)
        {
            mStream.seekg(0, std::ios_base::end);
            mFileSize = mStream.tellg();
            mStream.seekg(0);
        }

        ~FileReader() = default;

        template<typename T>
        bool readNum(T& t)
        {
            mStream.read((char*)&t, sizeof(T));
            size_t readCount = mStream.gcount();
            mReadCount += readCount;
            return readCount == sizeof(T);
        }

        template<typename T>
        bool readArray(std::vector<T>& vec, uint64_t length)
        {
            // check that we can actually read amount of bytes we want
            if (getRemainingSize() >= length)
            {
                vec.resize(length);
                mStream.read((char*)vec.data(), length * sizeof(T));
                size_t readCount = mStream.gcount();
                mReadCount += readCount;
                return readCount == (length * sizeof(T));
            }
            else
            {
                return false;
            }
        }

        size_t getRemainingSize() const
        {
            return mFileSize - mReadCount;
        }

        size_t getTotalSize() const
        {
            return mFileSize;
        }

        size_t getReadCount() const
        {
            return mReadCount;
        }

    private:
        std::istream& mStream;

        size_t mReadCount;
        size_t mFileSize;

    };
}

#endif

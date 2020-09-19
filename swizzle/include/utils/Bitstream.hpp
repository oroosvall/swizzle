#ifndef BIT_STREAM_HPP
#define BIT_STREAM_HPP

#include <cstdint>

namespace utils
{
    /*
    uint16_t readBitsTo16(uint8_t numBits)
    {
      uint16_t v1 = 0;
      uint16_t v2 = 0;
      uint16_t res = 0;

      char* dd = mData;

      v1 = (uint8_t)dd[mOffset];
      v2 = (uint8_t)dd[mOffset + 1];

      res = v2 | v1 << 8;

      uint16_t mask = (1 << numBits) - 1;

      res >>= (16 - (mBitOffset + numBits));
      res &= mask;

      mBitOffset += numBits;
      if (mBitOffset >= 8)
      {
        mBitOffset -= 8;
        mOffset++;
      }

      return res;
    }
    */

    class BitStreamReader
    {
    public:
        BitStreamReader(uint8_t* data, size_t dataSize)
            : mData(data)
            , mSize(dataSize)
            , mCache(0U)
            , mBitOffset(0U)
            , mRead(0U)
        {
            read(mCache);
        }

        bool readBits(uint32_t& data, uint8_t nBits)
        {
            uint32_t mask = (1U << nBits) - 1U;
            uint64_t tmp;

            uint8_t* p1 = (uint8_t*)&tmp;
            uint8_t* p2 = (uint8_t*)&mCache;

            p1[0] = p2[7]; p1[1] = p2[6]; p1[2] = p2[5]; p1[3] = p2[4];
            p1[4] = p2[3]; p1[5] = p2[2]; p1[6] = p2[1]; p1[7] = p2[0];

            data = static_cast<uint32_t>(tmp >> (64U - (mBitOffset + nBits)));
            data &= mask;

            mBitOffset += nBits;
            if (mBitOffset > 32)
            {
                read(mCache);
                mBitOffset -= 32;
            }

            return true;
        }

    private:
        bool read(uint64_t& data)
        {
            memcpy(&data, &mData[mRead], sizeof(uint64_t));
            mRead += sizeof(uint32_t);

            return true;
        }

        uint8_t* mData;
        size_t mSize;

        uint64_t mCache;
        uint8_t mBitOffset;
        size_t mRead;
    };

    class BitStreamWriter
    {
    public:
        BitStreamWriter(uint8_t* data, size_t dataSize)
            : mData(data)
            , mSize(dataSize)
            , mCache(0U)
            , mBitOffset(0U)
            , mWritten(0U)
        {
        }

        // we are using little endian so we shift by number of bits written
        // we can then or it with the cache if cache is above 32 bits we flush it
        // and write 4 bytes to buffer
        bool writeBits(uint32_t data, uint8_t nBits)
        {
            
            uint64_t w = static_cast<uint64_t>(data);

            w <<= (static_cast<uint64_t>(64U) - (static_cast<uint64_t>(mBitOffset) + static_cast<uint64_t>(nBits)));

            uint64_t tmp = 0;
            uint8_t* p1 = (uint8_t*)&tmp;
            uint8_t* p2 = (uint8_t*)&w;

            p1[0] = p2[7]; p1[1] = p2[6]; p1[2] = p2[5]; p1[3] = p2[4];
            p1[4] = p2[3]; p1[5] = p2[2]; p1[6] = p2[1]; p1[7] = p2[0];

            mCache |= tmp;
            mBitOffset += nBits;
            if (mBitOffset >= 32)
            {
                uint32_t d = ((uint32_t*)&mCache)[0];

                write(d);
                mCache >>= 32;
                mBitOffset -= 32;
            }
            return true;
        }

        void flush()
        {
            size_t s = 0;
            uint8_t* d = (uint8_t*)&mCache;
            for (size_t i = 0; i < (mBitOffset + 7) / 8; i++)
            {
                mData[mWritten + i] = d[i];
                s++;
            }
            mWritten += s;
        }

        size_t getWrittenSize()
        {
            return mWritten;
        }

    private:
        bool write(uint32_t data)
        {
            uint32_t* d = (uint32_t*)(mData + mWritten);
            *d = data;

            mWritten += sizeof(uint32_t);

            return true;
        }

        uint8_t* mData;
        size_t mSize;

        uint64_t mCache;
        uint8_t mBitOffset;
        size_t mWritten;
    };

} // namespace utils

#endif
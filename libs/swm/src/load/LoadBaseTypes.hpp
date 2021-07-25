#ifndef BASE_TYPES_HPP
#define BASE_TYPES_HPP

/* Include files */

#include <common/Common.hpp>
#include <swm/Swm.hpp>
#include <utils/BufferReader.hpp>
#include <string>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

/* Function Declaration */

namespace swm_base
{
    SwBool ResultIsOk(swm::LoadResult result);

    swm::LoadResult LoadString(utils::BufferReader& br, std::string& str);

    template<typename T, swm::LoadResult err>
    swm::LoadResult LoadNumber(utils::BufferReader& br, T& num)
    {
        swm::LoadResult res = err;

        if (br.readNum<T>(num))
        {
            res = swm::LoadResult::Success;
        }

        return res;
    }

    template<typename T, swm::LoadResult err>
    swm::LoadResult LoadArray(utils::BufferReader& br, std::vector<T>& data, U32 count)
    {
        swm::LoadResult res = err;

        T* arr = br.readArrayU32<T>(count);
        if (arr)
        {
            data.resize(count);
            memcpy(data.data(), arr, sizeof(T) * count);
            res = swm::LoadResult::Success;
        }

        return res;
    }
}

#endif
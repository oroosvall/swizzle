
#pragma once

/* Includes */

#include <common/Common.hpp>

#include <string>

/* Defines */

/* Typedefs */

/* Function Declaration */

/* Forward Declared Structs/Classes */

namespace sb
{
    enum class DataType
    {
        dataTypeNone,
        dataTypeFloat,
        dataTypeInt
    };

    enum class DataMode
    {
        dataModeIn,
        dataModeOut
        //dataModeInOut // this might get tricky
    };
}

/* Struct Declaration */

/* Class Declaration */

namespace sb
{
    class ShaderSource
    {
    public:
        ShaderSource() = default;
        virtual ~ShaderSource() = default;

        virtual const char* getSource(U32& size) const = 0;
        virtual const char* getName() const = 0;
        virtual U32 getNumInputs() const
        {
            return 0ull;
        }

        virtual DataType getInputDataType(U32 index) const
        {
            UNUSED_ARG(index);
            return DataType::dataTypeNone;
        }

        virtual U32 getNumOutputs() const
        {
            return 0ull;
        }

        virtual DataType getOuptuDataType(U32 index) const
        {
            UNUSED_ARG(index);
            return DataType::dataTypeNone;
        }
    };

    class EmptySource : public ShaderSource
    {
        EmptySource() = default;
        virtual ~EmptySource() = default;

        virtual const char* getSource(U32& size) const override
        {
            size = 0ull;
            return nullptr;
        }

        virtual const char* getName() const override
        {
            return "Empty Source";
        }
    };

    class TextShaderSource : public ShaderSource
    {
    public:
        TextShaderSource(const char* text);
        virtual ~TextShaderSource() = default;

        virtual const char* getSource(U32& size) const override;
        virtual const char* getName() const override;

    private:
        std::string mText;
    };

} // namespace sb

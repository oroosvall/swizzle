
#pragma once

/* Includes */

#include <common/Common.hpp>

#include <string>

/* Defines */

/* Typedefs */

/* Function Declaration */

/* Forward Declared Structs/Classes */

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
    };

    class TextShaderSource : public ShaderSource
    {
    public:
        TextShaderSource(const char* text);
        virtual ~TextShaderSource() = default;

        virtual const char* getSource(U32& size) const override;

    private:
        std::string mText;
    };

} // namespace sb

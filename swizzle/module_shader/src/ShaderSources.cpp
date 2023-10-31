
/* Include files */

#include <shader/ShaderSources.hpp>

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

/* Class Public Function Definition */

namespace sb
{
    TextShaderSource::TextShaderSource(const char* text)
        : mText(text)
    {
    }

    const char* TextShaderSource::getSource(U32& size) const
    {
        size = static_cast<U32>(mText.size());
        return mText.c_str();
    }

    const char* TextShaderSource::getName() const
    {
        return "Text Source";
    }
} // namespace sb

/* Class Protected Function Definition */

/* Class Private Function Definition */
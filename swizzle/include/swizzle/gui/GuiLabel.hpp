#ifndef GUI_LABEL_HPP
#define GUI_LABEL_HPP

/* Include files */

#include <common/Types.hpp>
#include <common/Resource.hpp>
#include <swizzle/Api.hpp>

#include <swizzle/gfx/Context.hpp>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

namespace swizzle::gui
{

    class GuiLabel
    {
    public:
        virtual ~GuiLabel() {}

        // @TODO: Change to core::String once that is added
        virtual void setText(const SwChar* text) = 0;

        virtual core::Resource<gfx::Buffer> getBuffer() const = 0;

        virtual core::Resource<gfx::Texture> getTexture() const = 0;

    };

}

/* Function Declaration */

// Temporary
namespace swizzle::gui
{
    typedef GuiLabel* GuiLabelPtr;
    GuiLabelPtr SWIZZLE_API CreateLabel(core::Resource<gfx::GfxContext> context);
}

#endif
#ifndef LABEL_HPP
#define LABEL_HPP

/* Include files */

#include <common/Types.hpp>
#include <swizzle/gui/GuiLabel.hpp>

#include "Font.hpp"

#include <gfx/gfxvk/VkContext.hpp>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

namespace swizzle::gui
{

    class Label : public GuiLabel
    {
    public:
        Label(gfx::VkGfxContext* ctx);
        virtual ~Label() {}

        // @TODO: Change to core::String once that is added
        virtual void setText(const SwChar* text) override;

        virtual core::Resource<gfx::Buffer> getBuffer() const override;

        virtual core::Resource<gfx::Texture> getTexture() const override;

    private:

        // Temp
        gfx::VkGfxContext* mCtx;

        Font mFont;

        core::Resource<gfx::Buffer> mQuadBuffer;
    };
}

/* Function Declaration */

#endif
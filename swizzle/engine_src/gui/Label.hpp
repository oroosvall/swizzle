#ifndef LABEL_HPP
#define LABEL_HPP

/* Include files */

#include <common/Common.hpp>
#include <swizzle/gui/GuiLabel.hpp>

#include "Font.hpp"

#include <gfx/gfxvk2/VkContext.hpp>

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

        virtual common::Resource<gfx::Buffer> getBuffer() const override;

        virtual common::Resource<gfx::Texture> getTexture() const override;

    private:

        // Temp
        gfx::VkGfxContext* mCtx;

        Font mFont;

        common::Resource<gfx::Buffer> mQuadBuffer;
    };
}

/* Function Declaration */

#endif
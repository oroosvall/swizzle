#ifndef GUI_CONTEXT_HPP
#define GUI_CONTEXT_HPP

/* Include files */

#include <common/Common.hpp>
#include <swizzle/gfx/Context.hpp>

/* Defines */

/* Typedefs/enums */

namespace swizzle::gui
{
    enum class GuiItemType
    {
        Panel,
        Button
    };
}

/* Forward Declared Structs/Classes */

/* Struct Declaration */

/* Class Declaration */

namespace swizzle::gui
{
    class GuiItem
    {
    public:
        virtual GuiItemType getType() = 0;

    };

    class GuiContext
    {
    public:
        GuiContext(common::Resource<swizzle::gfx::GfxContext> gfxContext);


        void update(DeltaTime dt);

        void draw(common::Resource<swizzle::gfx::CommandBuffer> cmdBuffer);

    private:

        common::Resource<swizzle::gfx::Buffer> mGuiBuffer;

    };
}

/* Function Declaration */

#endif
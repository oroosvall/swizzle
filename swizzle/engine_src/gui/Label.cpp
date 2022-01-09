
/* Include files */

#include "Label.hpp"
#include <gfx/gfxvk2/VkContext.hpp>
#include <cstring>

#include <optick/optick.h>

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

namespace swizzle::gui
{
    GuiLabelPtr SWIZZLE_API CreateLabel(common::Resource<gfx::GfxContext> context)
    {
        gfx::VkGfxContext* ctx = (gfx::VkGfxContext*)(context.get());
        return new Label(ctx);
    }
}

/* Class Public Function Definition */

#ifdef SW_WINDOWS
const char* fontPath = "c:/windows/fonts/arial.ttf";
#elif SW_LINUX
const char* fontPath = "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf";
#endif

namespace swizzle::gui
{
    Label::Label(gfx::VkGfxContext* ctx)
        : mCtx(ctx)
        , mFont(fontPath, ctx)
        , mQuadBuffer(nullptr)
    {
        for (int i = 0; i < 512; ++i)
        {
            mFont.getGlyph(i);
        }

        mFont.rebuildTexture();

        mQuadBuffer = ctx->createBuffer(gfx::BufferType::Vertex);

    }

    void Label::setText(const SwChar* text)
    {
        OPTICK_EVENT("Label::setText");
        struct TextVertex
        {
            F32 x;
            F32 y;
            F32 z;
            F32 u;
            F32 v;
        };

        struct Quad
        {
            TextVertex v1;
            TextVertex v2;
            TextVertex v3;

            TextVertex v4;
            TextVertex v5;
            TextVertex v6;
        };

        const U64 numQuads = strlen(text);
        //Quad* quads = new Quad[numQuads];
        mQuadBuffer->setStride(sizeof(TextVertex));
        Quad* quads = (Quad*)mQuadBuffer->mapMemory(numQuads * sizeof(Quad));

        F32 x = 0.0F;
        F32 y = mFont.getFontSize();
        F32 scale = 1.0F;

        F32 dx = 0.0F;

        F32 yAdv = 0.0F;

        F32 fSize = mFont.getFontSize() + 1.0F;

        for (U32 i = 0u; i < numQuads; ++i)
        {
            // This is an ugly hack to force characters above 127 to not be treated as negative... seems to cause issues
            S32 c = S32(U8(text[i]));
            const FontGlyph& glyph = mFont.getGlyph(c);

            if (c == '\n')
            {
                if (yAdv)
                {
                    y += (fSize);
                }
                else
                {
                    y += fSize;
                }
                yAdv = 0.0F;
                x = dx;
                memset(&quads[i], 0, sizeof(Quad));
                continue;
            }
            else if (c == '\t')
            {
                x += ((F32)glyph.mWidth * scale * 2.0F);
                memset(&quads[i], 0, sizeof(Quad));
                continue;
            }
            else if (c == '\0')
            {
                memset(&quads[i], 0, sizeof(Quad));
                continue;
            }

            F32 xPos = x + glyph.mStartX; // +(glyph.mBearingX * scale);
            F32 yPos = y + mFont.getBaseLine() + glyph.mStartY; // -((glyph.mHeight - glyph.mBearingY) * scale);

            F32 w = glyph.mWidth * scale;
            F32 h = glyph.mHeight * scale;

            Quad& q = quads[i];

            q.v1 = { xPos    , yPos + h, -1.0F, glyph.mUStart, glyph.mVEnd };
            q.v2 = { xPos    , yPos    , -1.0F, glyph.mUStart, glyph.mVStart };
            q.v3 = { xPos + w, yPos    , -1.0F, glyph.mUEnd  , glyph.mVStart };

            q.v4 = { xPos    , yPos + h, -1.0F, glyph.mUStart, glyph.mVEnd };
            q.v5 = { xPos + w, yPos    , -1.0F, glyph.mUEnd  , glyph.mVStart };
            q.v6 = { xPos + w, yPos + h, -1.0F, glyph.mUEnd  , glyph.mVEnd };

            F32 kerning = 0.0F;
            if (text[i + 1] != 0)
            {
                kerning = mFont.getKerning(c, text[i + 1]);
            }

            x += ((F32)glyph.mAdvance * scale) + kerning;

        }

        /*mQuadBuffer->setBufferData(quads, numQuads * sizeof(Quad), sizeof(TextVertex));

        delete[] quads;*/

        mQuadBuffer->unmapMemory();
    }

    common::Resource<gfx::Buffer> Label::getBuffer() const
    {
        return mQuadBuffer;
    }

    common::Resource<gfx::Texture> Label::getTexture() const
    {
        return mFont.mFontTexture;
    }

}

/* Class Protected Function Definition */

/* Class Private Function Definition */

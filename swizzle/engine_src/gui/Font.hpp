#ifndef FONT_HPP
#define FONT_HPP

/* Include files */

#include <common/Common.hpp>

#include <utils/MemoryBuffer.hpp>

#include <unordered_map>
#include <stb/stb_truetype.h>

#include <gfx/gfxvk2/VkContext.hpp>

/* Defines */

/* Typedefs/enums */

/* Forward Declared Structs/Classes */

/* Struct Declaration */

namespace swizzle::gui
{
    struct FontGlyph
    {
        U32 mCodepoint;

        F32 mUStart, mVStart; // Texture start coordinate (top left)
        F32 mUEnd, mVEnd; // Texture start coordinate (bottom right)

        U32 mTextureChannel; // Texture channel for character

        S32 mWidth;
        S32 mHeight;
        U8* mGlyphRenderBuffer;

        S32 mStartX;
        S32 mStartY;

        S32 mAdvance;
        S32 mBearingX;
        S32 mBearingY;

    };
}

/* Class Declaration */

namespace swizzle::gui
{
    class Font
    {
    public:
        Font(const SwChar* fontPath, gfx::VkGfxContext* ctx);
        virtual ~Font();

        const FontGlyph& getGlyph(S32 codePoint) const;

        F32 getFontSize() const;
        F32 getBaseLine() const;

        SwBool shouldRebuildTexture() const;
        void rebuildTexture();

        F32 getKerning(S32 codePoint1, S32 codePoint2) const;

        S32 getCachedCodePoint(S32 codePoint) const;

        // This is temp
    //private:

        U8* mFileBuffer;
        stbtt_fontinfo mFontInfo;

        mutable S32 mCodePointCacheCommon[512];

        mutable std::unordered_map<S32, S32> mCodePointCache;
        mutable std::unordered_map<S32, FontGlyph> mGlyphCache;
        mutable std::unordered_map<U64, F32> mKerningCache;
        mutable SwBool mTextureInvalidated;

        F32 mFontSize;
        F32 mScale;
        S32 mAscent;
        S32 mBaseline;

        mutable utils::MemoryBuffer mGlyphPoolMemory;

        common::Resource<gfx::Texture> mFontTexture;

    };
}

/* Function Declaration */

#endif
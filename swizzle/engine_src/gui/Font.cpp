
/* Include files */

#include "Font.hpp"

#include <swizzle/core/Logging.hpp>

#include <fstream>

#include <utils/ShelfPack.hpp>
#include <algorithm>
#include <vector>

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

/* Class Public Function Definition */

namespace swizzle::gui
{
    Font::Font(const SwChar* fontPath, gfx::VkGfxContext* ctx)
        : mFileBuffer(nullptr)
        , mFontInfo()
        , mCodePointCache()
        , mGlyphCache()
        , mKerningCache()
        , mTextureInvalidated(true)
        , mFontSize(20.0F)
        , mScale(1.0F)
        , mAscent(1)
        , mBaseline(1)
        , mGlyphPoolMemory(1024 * 1024)
        , mFontTexture(core::CreateRef<gfx::VulkanTexture>(ctx->getVkContainer(), 100, 100, 1, nullptr))
    {
        // @TODO: replace with input parameter
        std::ifstream inputFile(fontPath, std::ios::binary | std::ios::ate);
        U64 size = inputFile.tellg();
        inputFile.clear();
        inputFile.seekg(0, std::ios::beg);

        mFileBuffer = new U8[size];
        inputFile.read((char*)mFileBuffer, size);

        if (stbtt_InitFont(&mFontInfo, mFileBuffer, 0u) == 0)
        {
            LOG_ERROR("Font failed to load");
        }

        mScale = stbtt_ScaleForPixelHeight(&mFontInfo, mFontSize);
        stbtt_GetFontVMetrics(&mFontInfo, &mAscent, 0, 0);
        mBaseline = (int)(mAscent * mScale);

    }

    const FontGlyph& Font::getGlyph(U32 codePoint) const
    {
        if (mCodePointCache.find(codePoint) == mCodePointCache.end())
        {
            mCodePointCache[codePoint] = stbtt_FindGlyphIndex(&mFontInfo, (S32)codePoint);
        }
        U32 glyphIndex = mCodePointCache[codePoint];


        if (mGlyphCache.find(glyphIndex) == mGlyphCache.end())
        {
            S32 advance = 0;
            S32 lsb = 0;

            stbtt_GetGlyphHMetrics(&mFontInfo, glyphIndex, &advance, &lsb);

            S32 x0 = 0;
            S32 x1 = 0;
            S32 y0 = 0;
            S32 y1 = 0;
            stbtt_GetGlyphBitmapBoxSubpixel(&mFontInfo, glyphIndex, mScale, mScale, 0.0F, 0.0F, &x0, &y0, &x1, &y1);

            S32 width = x1 - x0;
            S32 height = y1 - y0;

            U32 size = (U32)(width * height);

            U8* mem = mGlyphPoolMemory.allocMemory(size);

            stbtt_MakeGlyphBitmapSubpixel(&mFontInfo, mem, width, height, width, mScale, mScale, 0.0F, 0.0F, glyphIndex);
            /*
            int x0, y0, x1, y1;
            float x_shift = xpos - (float)floor(xpos);
            stbtt_GetCodepointBitmapBoxSubpixel(&font, text[ch], scale, scale, x_shift, 0, &x0, &y0, &x1, &y1);
            stbtt_MakeCodepointBitmapSubpixel(&font, &screen[baseline + y0][(int)xpos + x0], x1 - x0, y1 - y0, 79, scale, scale, x_shift, 0, text[ch]);
            // note that this stomps the old data, so where character boxes overlap (e.g. 'lj') it's wrong
            // because this API is really for baking character bitmaps into textures. if you want to render
            // a sequence of characters, you really need to render each bitmap to a temp buffer, then
            // "alpha blend" that into the working buffer
            xpos += (advance * scale);
            if (text[ch + 1])
                xpos += scale * stbtt_GetCodepointKernAdvance(&font, text[ch], text[ch + 1]);
            ++ch;

            */
            FontGlyph glyph = {};

            glyph.mCodepoint = glyphIndex;
            glyph.mGlyphRenderBuffer = mem;
            glyph.mTextureChannel = 0u;
            glyph.mWidth = width;
            glyph.mHeight = height;
            glyph.mAdvance = (S32)((F32)advance * mScale);
            glyph.mBearingX = (S32)((F32)lsb * mScale);
            glyph.mBearingY = 0;

            glyph.mStartX = x0;
            glyph.mStartY = y0;

            mGlyphCache[glyphIndex] = glyph;
            mTextureInvalidated = true;
        }

        return mGlyphCache[glyphIndex];
    }

    F32 Font::getFontSize() const
    {
        return mFontSize;
    }

    F32 Font::getBaseLine() const
    {
        return (F32)mBaseline;
    }

    SwBool Font::shouldRebuildTexture() const
    {
        return mTextureInvalidated;
    }

    void Font::rebuildTexture()
    {
        std::vector<FontGlyph*> characterList;
        characterList.reserve(mGlyphCache.size());

        for (auto& it : mGlyphCache)
        {
            characterList.push_back(&it.second);
        }

        std::sort(characterList.begin(), characterList.end(), [](FontGlyph* a, FontGlyph* b) -> bool {return a->mHeight > b->mHeight; });

        ShelfPack sp(512, 512);

        for (auto& it : characterList)
        {
            S32 w = it->mWidth;
            S32 h = it->mHeight;

            S32 id = it->mCodepoint;

            sp.addData(w + 1, h + 1, id);
        }

        S32 imgW = sp.getPackedWidth();
        S32 imgH = sp.getPackedHeight();

        size_t length = sp.length();

        U8* image = new U8[imgW * imgH];
        memset(image, 0, imgW * imgH);

        F32 singleX = (1.0F / (F32)imgW);
        F32 singleY = (1.0F / (F32)imgH);

        for (U32 i = 0; i < length; ++i)
        {
            S32 offsetX = sp[i].x;
            S32 offsetY = sp[i].y;
            S32 width = sp[i].w - 1;
            S32 height = sp[i].h - 1;

            S32 id = sp[i].id;

            U8* memBuf = mGlyphCache[id].mGlyphRenderBuffer;

            mGlyphCache[id].mUStart = singleX * (F32)offsetX;
            mGlyphCache[id].mVStart = singleY * (F32)offsetY;

            mGlyphCache[id].mUEnd = singleX * (F32)(offsetX + width);
            mGlyphCache[id].mVEnd = singleY * (F32)(offsetY + height);

            for (U32 j = 0; j < (U32)height; ++j)
            {
                U8* dst = &image[ ((offsetY + j) * imgW) + offsetX];
                U8* src = &memBuf[j * width];
                memcpy(dst, src, width);
            }
        }

        mFontTexture->setData(imgW, imgH, 1, image);

        delete[] image;

        mTextureInvalidated = false;
    }

    F32 Font::getKerning(U32 codePoint1, U32 codePoint2) const
    {
        union
        {
            U32 codePoint[2];
            U64 combined;
        } kern = { 0u };

        kern.codePoint[0] = codePoint1;
        kern.codePoint[1] = codePoint2;

        if (mKerningCache.find(kern.combined) == mKerningCache.end())
        {
            S32 glyph1 = mCodePointCache[codePoint1];
            S32 glyph2 = mCodePointCache[codePoint2];
            F32 kerning = mScale * (F32)stbtt_GetGlyphKernAdvance(&mFontInfo, glyph1, glyph2);
            mKerningCache.insert({ kern.combined, kerning });
        }

        return mKerningCache[kern.combined];
    }

    Font::~Font()
    {
        mGlyphCache.clear();

        delete[] mFileBuffer;
        mFileBuffer = nullptr;

        memset(&mFontInfo, 0u, sizeof(mFontInfo));

    }

}

/* Class Protected Function Definition */

/* Class Private Function Definition */

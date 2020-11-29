
#include <swizzle/gfx/Color.hpp>

#include <glm/glm.hpp>
#include <cmath>

namespace swizzle::gfx
{
    RGBA HsvaToRgba(HSVA hsva)
    {
        RGBA rgba;
        F32 H = hsva.h, S = hsva.s, V = hsva.v, A = hsva.a,
            P, Q, T,
            fract;

        (H >= 360.0F) ? (H = 0.0F) : (H /= 60.0F);
        fract = H - floor(H);

        P = V * (1.0F - S);
        Q = V * (1.0F - S * fract);
        T = V * (1.0F - S * (1.0F - fract));

        if (0.0F <= H && H < 1.0F)
            rgba = { V, T, P, A };
        else if (1.0F <= H && H < 2.0F)
            rgba = { Q, V, P, A };
        else if (2.0F <= H && H < 3.0F)
            rgba = { P, V, T, A };
        else if (3.0F <= H && H < 4.0F)
            rgba = { P, Q, V, A };
        else if (4.0F <= H && H < 5.0F)
            rgba = { T, P, V, A };
        else if (5.0F <= H && H < 6.0F)
            rgba = { V, P, Q, A };
        else
            rgba = { 0.0F, 0.0F, 0.0F, A };

        return rgba;
    }

    HSVA RgbaToHsva(RGBA rgba)
    {
        HSVA hsva;
        F32 cMin = glm::min(rgba.r, glm::min(rgba.g, rgba.b));
        F32 cMax = glm::max(rgba.r, glm::max(rgba.g, rgba.b));;

        F32 cDelta = cMax - cMin;

        if (cDelta < 0.00001F)
        {
            hsva.h = hsva.s = 0.0F;
        }
        else
        {
            if (cMax == rgba.r)
            {
                F32 mod = 360.0F;
                F32 v = (60.0F * ((rgba.g - rgba.b) / cDelta) + 360.0F);
                hsva.h = fmod(v, mod);
            }
            else if (cMax == rgba.g)
            {
                F32 mod = 360.0F;
                F32 v = (60.0F * ((rgba.b - rgba.r) / cDelta) + 120.0F);
                hsva.h = fmod(v, mod);
            }
            else if (cMax == rgba.b)
            {
                F32 mod = 360.0F;
                F32 v = (60.0F * ((rgba.r - rgba.g) / cDelta) + 240.0F);
                hsva.h = fmod(v, mod);
            }

            hsva.s = cMax == 0.0F ? 0.0F : (cDelta / cMax);
        }

        hsva.v = cMax;
        hsva.a = rgba.a;

        return hsva;
    }
}
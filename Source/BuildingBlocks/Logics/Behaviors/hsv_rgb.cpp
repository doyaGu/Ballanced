#include "CKAll.h"
#include "hsv_rgb.h"

void rgb_to_hsv(float r, float g, float b, float *h, float *s, float *v)
{
    float fmax = XMax(r, g);
    fmax = XMax(fmax, b);

    float fmin = XMin(r, g);
    fmin = XMin(fmin, b);

    *v = fmax;

    if (fmax != 0.0f)
    {
        *s = (fmax - fmin) / fmax;
    }
    else
        *s = 0.0f;

    if (*s == 0.0f)
        *h = UNDEFINED;
    else
    {
        float delta = fmax - fmin;
        if (r == fmax)
        {
            *h = (g - b) / delta;
        }
        else if (g == fmax)
        {
            *h = 2 + (b - r) / delta;
        }
        else if (b == fmax)
        {
            *h = 4 + (r - g) / delta;
        }

        *h *= 60.0f;

        if (*h < 0.0f)
            *h += 360.0f;
    }
}

void hsv_to_rgb(float h, float s, float v, float *r, float *g, float *b)
{
    if (s == 0)
    {
        if (h == UNDEFINED)
        {
            *r = v;
            *g = v;
            *b = v;
        }
    }
    else
    {
        if (h == 360.0f)
            h = 0.0f;

        h /= 60.0f;

        int i = (int)floor(h);

        float f = h - (float)i;

        float p = v * (1.0f - s);
        float q = v * (1.0f - (s * f));
        float t = v * (1.0f - (s * (1.0f - f)));

        switch (i)
        {
        case 0:
            *r = v;
            *g = t;
            *b = p;
            break;
        case 1:
            *r = q;
            *g = v;
            *b = p;
            break;
        case 2:
            *r = p;
            *g = v;
            *b = t;
            break;
        case 3:
            *r = p;
            *g = q;
            *b = v;
            break;
        case 4:
            *r = t;
            *g = p;
            *b = v;
            break;
        case 5:
            *r = v;
            *g = p;
            *b = q;
            break;
        }
    }
}

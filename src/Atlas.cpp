#include "Atlas.h"

void Atlas::make(int c)
{
    width = 32*c;
    height = 32;
    pixel_data = new unsigned char[width*height*3];
}

void Atlas::put(int c, const unsigned char *data)
{
    for (int y = 0; y < 16; ++y)
    {
        for (int x = 0; x < 16; ++x)
        {
            for (int i = 0; i < 3; ++i)
            {
                pixel_data[((y + 8)*width + x + 8 + c*32)*3+i] = data[(y*16 + x)*3+i];
            }
        }
    }

    for (int y = 0; y < 32; ++y)
    {
        for (int x = 0; x < 32; ++x)
        {
            int bx = x - 8;
            if (bx < 0)
                bx = 0;

            if (bx > 15)
                bx = 15;

            int by = y - 8;
            if (by < 0)
                by = 0;

            if (by > 15)
                by = 15;

            for (int i = 0; i < 3; ++i)
            {
                pixel_data[(y*width + x + c*32)*3+i] = data[(by*16 + bx)*3+i];
            }
        }
    }
}

#ifndef COLORTRANSFORM_H_INCLUDED
#define COLORTRANSFORM_H_INCLUDED

void rgb_hsi_norm(float R, float G, float B, float &H, float &S, float &I);
void hsi_norm_rgb(float H, float S, float I, float &R, float &G, float &B);


#endif // COLORTRANSFORM_H_INCLUDED

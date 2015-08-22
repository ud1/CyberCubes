#include "colorTransform.hpp"
#include <cmath>

void rgb_hsi_norm(float R, float G, float B, float &H, float &S, float &I)
{
    float min_f = 1.0e-6f;
    float i = R + G + B;
    I = i / 3.0f;

	if (I < min_f || (R == G && G == B))
	{
		H = S = 0.0f;
		return;
	}

	float r = R/i;
	float g = G/i;
	float b = B/i;

	float w = 0.5f * (2.0*R - G - B)/sqrt((R - G)*(R - G) + (R - B)*(G - B));
    if (w > 1.0f)
		w = 1.0f;
	if (w < -1.0f)
		w = -1.0f;

	H = std::acos(w);

	H /= 2.0f*M_PI;

    if (B > G)
		H = 1.0 - H;

	if (r <= g && r <= b)
		S = 1.0f - 3.0f*r;

	if (g <= r && g <= b)
		S = 1.0f - 3.0f*g;

	if (b <= r && b <= g)
		S = 1.0f - 3.0f*b;
}

void hsi_norm_rgb(float H, float S, float I, float &R, float &G, float &B)
{
    H *= 2.0f*M_PI;
    float min_f = 1.0e-6f;

	if (S > 1.0f)
		S = 1.0f;

	if (I > 1.0f)
		I = 1.0f;

	if (S < min_f)
	{
		R = G = B = I;
		return;
	}

	float r, g, b;

	if (H < 2.0f*M_PI/3.0f)
	{
		b = (1.0f - S) / 3.0f;
		r = (1.0f + S * cos(H)/cos(M_PI/3.0f - H)) / 3.0f;
		g = 1.0 - r - b;
	}
	else if (H < 4.0f*M_PI/3.0f)
	{
		H = H - 2.0f*M_PI/3.0f;
		r = (1.0f - S) / 3.0f;
		g = (1.0f + S * cos(H)/cos(M_PI/3.0f - H)) / 3.0f;
		b = 1.0 - r - g;
	}
	else
	{
		H = H - 4.0f*M_PI/3.0f;
		g = (1.0f - S) / 3.0f;
		b = (1.0f + S * cos(H)/cos(M_PI/3.0f - H)) / 3.0f;
		r = 1.0 - b - g;
	}

	if (r < 0.0f)
		r = 0.0f;

	if (g < 0.0f)
		g = 0.0f;

	if (b < 0.0f)
		b = 0.0f;

    R = 3.0f * I * r;
    G = 3.0f * I * g;
    B = 3.0f * I * b;

	if (R > 1.0f)
		R = 1.0f;

	if (G > 1.0f)
		G = 1.0f;

	if (B > 1.0f)
		B = 1.0f;
}

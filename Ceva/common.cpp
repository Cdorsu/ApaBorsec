#include "common.h"

namespace common
{
	Color HexToRGB( int color )
	{
		Color col;
		col.r = ((color >> 16) & 0xff) / 255.0f;
		col.g = ((color >> 8) & 0xff) / 255.0f;
		col.b = ((color)& 0xff) / 255.0f;
		col.a = 1.0f;
		return col;
	}
}
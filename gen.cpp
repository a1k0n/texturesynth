#include <stdlib.h>
#include <stdio.h>
#include "png.h"

#define SRC(i,j) srcim_rgb[(i)+(j)*srcw]
#define DST(i,j) dstim_rgb[(i)+(j)*dstw]

int main()
{
	int srcw, srch;
	unsigned *srcim_rgb = load_png_rgb("in.png", srcw, srch);
	printf("loaded %dx%d source\n", srcw, srch);

	// gen
	int dstw = 128;
	int dsth = 128;
	unsigned *dstim_rgb = new unsigned[dstw*dsth];

	for(int j=0;j<dsth;j++)
		for(int i=0;i<dstw;i++)
			DST(i,j) = rand()&0xffffff;

	for(int j=0;j<srch;j++)
		for(int i=0;i<srcw;i++)
			DST(i,j) = SRC(i,j);

	save_png_rgb("out.png", dstim_rgb, dstw, dsth);

	return 0;
}


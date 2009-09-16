#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "png.h"

unsigned *dstim_rgb, *srcim_rgb;
int srcw, srch, dstw, dsth;

#define SRC(i,j) srcim_rgb[(i)+(j)*srcw]
#define DST(i,j) dstim_rgb[(i)+(j)*dstw]

// Nsize is the radius of pixels surrounding the source pixel.  
// e.g. Nsize=2:
// N N N N N
// N N N N N
// N N p N N
// N N N N N
// N N N N N
unsigned nn_diff(int dstx, int dsty, int srcx, int srcy, int Nsize)
{
	unsigned diff=0;
	for(int j=-Nsize;j<=Nsize;j++) {
		int sy = (srch+srcy+j)%srch;
		int dy = (dsth+dsty+j)%dsth;
		for(int i=-Nsize;i<=Nsize;i++) {
			int sx = (srcw+srcx+i)%srcw;
			int dx = (dstw+dstx+i)%dstw;
			int s = SRC(sx,sy);
			int d = DST(dx,dy);
			int dr = (s>>16) - (d>>16);
			int dg = ((s>>8)&255) - ((d>>8)&255);
			int db = (s&255) - (d&255);
			diff += dr*dr + dg*dg + db*db;
		}
	}
	return diff;
}

// find src x,y with closest neighborhood to dst x,y
// this is, right now, a terrible brute-force search
unsigned nn_search(int dstx, int dsty, int &srcx, int &srcy, int Nsize)
{
	unsigned bestdiff = ~0;
	for(int y=0;y<srch;y++) {
		for(int x=0;x<srcw;x++) {
			unsigned diff = nn_diff(dstx,dsty, x,y, Nsize);
			if(diff < bestdiff) {
				bestdiff = diff;
				srcx = x;
				srcy = y;
			}
		}
	}
	return bestdiff;
}

int main()
{
	srand(time(NULL));
	srcim_rgb = load_png_rgb("in.png", srcw, srch);
	printf("loaded %dx%d source\n", srcw, srch);

	// gen
	dstw = 128;
	dsth = 128;
	dstim_rgb = new unsigned[dstw*dsth];

	for(int j=0;j<dsth;j++)
		for(int i=0;i<dstw;i++)
			DST(i,j) = rand()&0xffffff;

	for(int iterations=0;iterations<10;iterations++) {
		printf("gen iteration %d: ", iterations); fflush(stdout);
		unsigned err = 0;
		for(int j=0;j<dsth;j++) {
			for(int i=0;i<dstw;i++) {
				int x=0,y=0;
				err += nn_search(i,j, x,y, 3);
				DST(i,j) = SRC(x,y);
			}
		}
		printf("%u\n", err);
	}


	save_png_rgb("out.png", dstim_rgb, dstw, dsth);

	return 0;
}


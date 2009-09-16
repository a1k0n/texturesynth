#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "png.h"

unsigned *dstim_rgb, *srcim_rgb;
int srcw, srch, dstw, dsth;

// define this iff your source texture tiles cleanly
#define SRCWRAP
// neighborhood for synthesis
#define NEIGHBORHOOD 4
// number of seam-rows to keep constant between different interchangable tiles
#define KEEPROWS 2
// generator iterations
#define ITERATIONS 16

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
#ifdef SRCWRAP
		int sy = (srch+srcy+j)%srch;
#else
		int sy = srcy+j;
#endif
		int dy = (dsth+dsty+j)%dsth;
		for(int i=-Nsize;i<=Nsize;i++) {
			if(i == 0 && j == 0)
				continue;
#ifdef SRCWRAP
			int sx = (srcw+srcx+i)%srcw;
#else
			int sx = srcx+i;
#endif
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
#ifdef SRCWRAP
	for(int y=0;y<srch;y++) {
		for(int x=0;x<srcw;x++) {
#else
	for(int y=Nsize;y<srch-Nsize;y++) {
		for(int x=Nsize;x<srcw-Nsize;x++) {
#endif
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

void gen_init(int offset)
{
	for(int j=offset;j<dsth;j++)
		for(int i=offset;i<dstw;i++) {
			int x = rand()%srcw;
			int y = rand()%srch;
			DST(i,j) = SRC(x,y);
		}
}

unsigned gen_step(int offset)
{
	unsigned err = 0;
	for(int j=offset;j<dsth;j++) {
		for(int i=offset;i<dstw;i++) {
			int x=0,y=0;
			err += nn_search(i,j, x,y, NEIGHBORHOOD);
			DST(i,j) = SRC(x,y);
		}
	}
	return err;
}

int main()
{
	srand(time(NULL));
	srcim_rgb = load_png_rgb("in.png", srcw, srch);
	printf("loaded %dx%d source\n", srcw, srch);

	// gen
	dstw = 32;
	dsth = 32;
	dstim_rgb = new unsigned[dstw*dsth];

	for(int version=0;version<8;version++) {
		gen_init(version==0 ? 0 : KEEPROWS);
		for(int iterations=0;iterations<ITERATIONS;iterations++) {
			printf("ver %d iteration %d: ", version, iterations); fflush(stdout);
			printf("%u\n", gen_step(version==0 ? 0 : KEEPROWS));
		}

		char buf[20];
		sprintf(buf, "out%d.png", version);
		save_png_rgb(buf, dstim_rgb, dstw, dsth);
	}

	return 0;
}


#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "img.h"
#include "kcluster.h"

// neighborhood for synthesis
#define NEIGHBORHOOD 4
// number of seam-rows to keep constant between different interchangable tiles
#define KEEPROWS 2
// generator iterations
#define ITERATIONS 8

// neighborhood difference
// Nsize is the radius of pixels surrounding the source pixel.  
// e.g. Nsize=2:
// N N N N N
// N N N N N
// N N p N N
// N N N N N
// N N N N N
unsigned neighbor_diff(Img *im1, Img *im2, int x1, int y1, int x2, int y2, int Nsize)
{
  unsigned diff=0;
  for(int j=-Nsize;j<=Nsize;j++) {
    int j1 = im1->wraph(j+y1);
    int j2 = im2->wraph(j+y2);
    for(int i=-Nsize;i<=Nsize;i++) {
      int i1 = im1->wrapw(i+x1);
      int i2 = im2->wrapw(i+x2);
      diff += Img::normsqr(im1->p(i1,j1), im2->p(i2,j2));
    }
  }
  return diff;
}

// find nearest neighborhood in srcim corresponding to a given block in dstim
// returns an index
unsigned nn_search(Img *srcim, Img *dstim, int di, int dj, int Nsize, bool srcwrap, unsigned &e)
{
  unsigned bestdiff = ~0;
  unsigned bestidx = 0;
  int inset = srcwrap ? 0 : Nsize;
  for(int y=inset; y < srcim->h - inset; y++) {
    for(int x=inset; x < srcim->w - inset; x++) {
      unsigned diff = neighbor_diff(srcim, dstim, x,y, di,dj, Nsize);
      if(diff < bestdiff) {
        bestdiff = diff;
        bestidx = srcim->ij_to_idx(x,y);
      }
    }
  }
  e = bestdiff;
  return bestidx;
}

void gen_init(Img *srcim, Img *dstim, int offset)
{
  for(int j=offset;j<dstim->h;j++)
    for(int i=offset;i<dstim->w;i++) {
      int x = rand()%srcim->w;
      int y = rand()%srcim->h;
      dstim->p(i,j) = srcim->p(x,y);
    }
}

unsigned gen_step(Img *srcim, Img *dstim, int offset, bool srcwrap)
{
  unsigned E = 0;
  for(int j=offset;j<dstim->h;j++) {
    for(int i=offset;i<dstim->w;i++) {
      unsigned e;
      dstim->p(i,j) = srcim->p(nn_search(srcim, dstim, i,j, NEIGHBORHOOD, srcwrap, e));
      E += e;
    }
  }
  return E;
}

int main(int argc, char **argv)
{
  if(argc < 2) {
    printf("usage: %s <source image>\n", argv[0]);
    return -1;
  }
  srand(time(NULL));

  Img *srcim = Img::load_png(argv[1]);
  printf("loaded %dx%d source\n", srcim->w, srcim->h);

  // gen
  Img *dstim = new Img(32, 32);

  for(int version=0;version<8;version++) {
    gen_init(srcim, dstim, version==0 ? 0 : KEEPROWS);
    for(int iterations=0;iterations<ITERATIONS;iterations++) {
      printf("\rout%d.png iteration %d/%d: ", version, 1+iterations, ITERATIONS);
      fflush(stdout);
      printf("%u\e[K", gen_step(srcim, dstim, version==0 ? 0 : KEEPROWS, true));
    }

    char buf[20];
    sprintf(buf, "out%d.png", version);
    dstim->save_png(buf);
    printf(" done\n");
  }

  return 0;
}


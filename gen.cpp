#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "img.h"
#include "kcoherence.h"

// neighborhood for synthesis
#define NEIGHBORHOOD 2
// number of seam-rows to keep constant between different interchangable tiles
#define KEEPROWS 2
// generator iterations
#define ITERATIONS 40
// k-coherence search size
#define _K_ 11

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

#if 0
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
#endif


// find nearest
Kcoherence<_K_> nn_search(Img *srcim, int si, int sj, int Nsize, bool srcwrap)
{
  int inset = srcwrap ? 0 : Nsize;
  Kcoherence<_K_> best;
  for(int y=inset; y < srcim->h - inset; y++) {
    for(int x=inset; x < srcim->w - inset; x++) {
      //if(x == si && y == sj)
      //  continue;
      unsigned diff = neighbor_diff(srcim, srcim, x,y, si,sj, Nsize);
      best.insert(srcim->ij_to_idx(x,y), diff);
    }
  }
  return best;
}

template<int K>
class TextureSynth
{
public:
  Img *srcim, *dstim;
  Kcoherence<K> *srck;
  Img *dstz, *dsto; // z array in M step, and origin of current pixel in E step
  bool srcwrap;
  int Nsize;

  TextureSynth(Img *_src, int dstx, int dsty, int _Nsize, bool _srcwrap) {
    srcim = _src;
    dstim = new Img(dstx, dsty);
    dstz = new Img(dstx, dsty);
    dsto = new Img(dstx, dsty);
    srcwrap = _srcwrap;
    Nsize = _srcwrap;
    analyze_srcimg();
  }

  void analyze_srcimg()
  {
    printf("analyzing source img..."); fflush(stdout);
    srck = new Kcoherence<K>[srcim->w*srcim->h];
    int inset = srcwrap ? 0 : Nsize;
    for(int j=inset;j<srcim->h-inset;j++)
      for(int i=inset;i<srcim->w-inset;i++) {
        int idx = srcim->ij_to_idx(i,j);
        srck[idx] = ::nn_search(srcim, i,j, Nsize, srcwrap);
      }
    printf("done\n");
  }

  void init(int offset) {
    for(int j=offset;j<dstim->h;j++)
      for(int i=offset;i<dstim->w;i++) {
        int x = rand()%srcim->w;
        int y = rand()%srcim->h;
        dstz->p(i,j) = srcim->ij_to_idx(x,y);
        x = rand()%srcim->w;
        y = rand()%srcim->h;
        dsto->p(i,j) = srcim->ij_to_idx(x,y);
        dstim->p(i,j) = srcim->p(x,y);
      }
  }

  // nearest neighbor search, using a given coherence set
  void nn_search(Img *destim, int di, int dj, const Kcoherence<K> &kset,
                 int offx, int offy,
                 unsigned &bestdiff,unsigned &bestidx)
  {
    for(int k=0;k<kset.n;k++) {
      int i,j;
      srcim->idx_to_ij(kset[k],i,j);
      unsigned diff = neighbor_diff(srcim, srcim, i+offx,j+offy, di,dj, Nsize);
      if(diff < bestdiff) {
        bestdiff = diff;
        bestidx = srcim->ij_to_idx(i,j);
      }
    }
  }

  unsigned estep(int offset) {
    unsigned E=0;
    for(int j=offset;j<dstim->h;j++)
      for(int i=offset;i<dstim->w;i++) {
        unsigned bestdiff = ~0, bestidx = 0;

        // x,y are the target patch (in the z array)
        //int x,y; srcim->idx_to_ij(dstz->p(i,j),x,y);

        // we search our candidate set k from the pixel's current source
        nn_search(dstim, i,j, srck[dstz->p(i,j)], 0,0, bestdiff, bestidx);
        E += bestdiff;
        dstim->p(i,j) = srcim->p(bestidx);
        dsto->p(i,j) = bestidx;
      }
    return E;
  }

  unsigned mstep(int offset) {
    unsigned E=0;
    bool changed = false;
    for(int j=offset;j<dstim->h;j++)
      for(int i=offset;i<dstim->w;i++) {
        // search k set of all neighborhood pixels to find best matching
        // neighborhood between src and dest(i,j)
        unsigned bestdiff = ~0, bestidx = 0;
        for(int nj=-Nsize;nj<=Nsize;nj++)
          for(int ni=-Nsize;ni<=Nsize;ni++) {
            int x=dstim->wrapw(i+ni), y=dstim->wraph(j+nj);
            nn_search(dstim, i,j, srck[dsto->p(x,y)], ni,nj, bestdiff, bestidx);
          }
        E += bestdiff;
        if(dstz->p(i,j) != bestidx) {
          changed = true;
          dstz->p(i,j) = bestidx;
        }
        //dstim->p(i,j) = srcim->p(bestidx);
      }
    if(!changed)
      return 0;
    return E;
  }

  ~TextureSynth() { delete srcim; delete dstim; }
};

int main(int argc, char **argv)
{
  if(argc < 2) {
    printf("usage: %s <source image>\n", argv[0]);
    return -1;
  }
  srand(time(NULL));

  Img *srcim = Img::load_png(argv[1]);
  printf("loaded %dx%d source\n", srcim->w, srcim->h);

  TextureSynth<_K_> synth(srcim, 32, 32, NEIGHBORHOOD, true);

  for(int version=0;version<8;version++) {
    synth.init(version==0 ? 0 : KEEPROWS);
    for(int iterations=0;iterations<ITERATIONS;iterations++) {
      printf("\rout%d.png iteration %d/%d: E:", version, 1+iterations, ITERATIONS);
      printf("%u M:", synth.estep(version==0 ? 0 : KEEPROWS));
      unsigned e = synth.mstep(version==0 ? 0 : KEEPROWS);
      printf("%u\e[K", e);
      fflush(stdout);
      if(!e) break;
    }
    char buf[20];
    sprintf(buf, "out%d.png", version);
    synth.dstim->save_png(buf);
    printf(" done\n");

  }
  return 0;
}


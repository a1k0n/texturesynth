#include <stdlib.h>
#include <stdio.h>
#include <gd.h>
#include "img.h"

Img* Img::load_png(const char *fname)
{
  FILE *fp = fopen(fname, "rb");
  if(!fp) {
    printf("can't open %s\n", fname);
    exit(-1);
  }
  gdImagePtr im = gdImageCreateFromPng(fp);
  fclose(fp);

  // load
  Img *img = new Img(gdImageSX(im), gdImageSY(im));
  for(int j=0;j<img->h;j++) {
    for(int i=0;i<img->w;i++) {
      int c = gdImageGetPixel(im, i, j);
      img->p(i,j) = ((gdImageRed(im, c)<<16) + 
                     (gdImageGreen(im, c)<<8) + 
                     gdImageBlue(im, c));
    }
  }
  gdImageDestroy(im);
  return img;
}

bool Img::save_png(const char *fname)
{
  FILE *fp = fopen(fname, "wb");
  if(!fp) {
    printf("cannot open %s\n", fname);
    return false;
  }

  gdImagePtr im = gdImageCreateTrueColor(w,h);

  for(int j=0;j<h;j++) {
    for(int i=0;i<h;i++) {
      int c = p(i,j);
      gdImageSetPixel(im, i, j, gdTrueColor(c>>16, (c>>8)&255, c&255));
    }
  }

  gdImagePng(im, fp);
  fclose(fp);
  gdImageDestroy(im);

  return true;
}


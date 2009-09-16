#include <gd.h>
#include <stdlib.h>

unsigned *load_png_rgb(const char *fname, int &w, int &h)
{
	FILE *fp = fopen(fname, "rb");
	gdImagePtr im = gdImageCreateFromPng(fp);
	fclose(fp);

	// load
	w = gdImageSX(im);
	h = gdImageSY(im);
	unsigned *buf = new unsigned[w*h];
	for(int j=0;j<h;j++) {
		for(int i=0;i<w;i++) {
			int c = gdImageGetPixel(im, i, j);
			buf[i+j*w] = ((gdImageRed(im, c)<<16) + 
						  (gdImageGreen(im, c)<<8) + 
						  gdImageBlue(im, c));
		}
	}
	gdImageDestroy(im);
	return buf;
}

void save_png_rgb(const char *fname, unsigned *buf, int w, int h)
{
	gdImagePtr im = gdImageCreateTrueColor(w,h);

	for(int j=0;j<h;j++) {
		for(int i=0;i<h;i++) {
			int c = buf[i+j*w];
			gdImageSetPixel(im, i, j, gdTrueColor(c>>16, (c>>8)&255, c&255));
		}
	}

	FILE *fp = fopen(fname, "wb");
	gdImagePng(im, fp);
	fclose(fp);
	gdImageDestroy(im);
}

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
			dstim_rgb[i+j*dstw] = rand()&0xffffff;

	save_png_rgb("out.png", dstim_rgb, dstw, dsth);

	return 0;
}


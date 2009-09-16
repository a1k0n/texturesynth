#include <gd.h>

int main()
{
	FILE *fp = fopen("in.png", "rb");
	gdImagePtr im = gdImageCreateFromPng(fp);
	fclose(fp);


	gdImageDestroy(im);
}


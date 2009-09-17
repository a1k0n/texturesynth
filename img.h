class Img
{
public:
  unsigned *_buf;
  int w, h;

  Img(int w, int h) {
    _buf = new unsigned[w*h];
    this->w = w; this->h = h;
  }

  ~Img() { delete[] _buf; }
  // reference pixel at location
  unsigned& p(int i, int j) { return _buf[i+j*w]; }
  unsigned& p(int idx) { return _buf[idx]; }

  unsigned ij_to_idx(int i, int j) { return i+j*w; }
  void idx_to_ij(int idx, int &i, int &j) { i = idx%w; j = idx/w; }

  int wrapw(int i) { return (i+w)%w; }
  int wraph(int j) { return (j+h)%h; }

  static Img* load_png(const char *fname);
  bool save_png(const char *fname);

  // |c1 - c2|^2 in RGB distance
  static unsigned normsqr(unsigned c1, unsigned c2) {
    int dr = (c1>>16) - (c2>>16);
    int dg = ((c1>>8)&255) - ((c2>>8)&255);
    int db = (c1&255) - (c2&255);
    return dr*dr + dg*dg + db*db;
  }
};



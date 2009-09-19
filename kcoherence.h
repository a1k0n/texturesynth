template<int N>
class Kcoherence
{
public:
  unsigned _idx[N];
  unsigned _err[N];
  int n;

  Kcoherence() { n=0; }
  void insert(unsigned idx, unsigned err) {
    // error is already greater than any member
    if(n==N && err > _err[n-1])
      return;
    if(n<N) n++;
    _idx[n-1] = idx;
    _err[n-1] = err;
    // bubble up to maintain sorted list
    // i guess this could be a heap but... that'd be a waste
    for(int i=n-2;i>=0;i--) {
      if(_err[i] > _err[i+1]) {
        unsigned t = _err[i]; _err[i] = _err[i+1]; _err[i+1] = t;
        t = _idx[i]; _idx[i] = _idx[i+1]; _idx[i+1] = t;
      }
    }
#if 0
    printf("insert: ");
    for(int i=0;i<n;i++)
      printf("%u(%u) ", _idx[i], _err[i]);
    printf("\n");
#endif
  }

  unsigned operator[](int n) const { return _idx[n]; }
};


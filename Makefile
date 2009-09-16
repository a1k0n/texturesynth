CXXFLAGS = -I/opt/local/include
LDFLAGS = -L/opt/local/lib -lgd

SRCS = gen.cpp

OBJS = $(SRCS:.cpp=.o)

gen: $(OBJS)
	g++ $(LDFLAGS) -o gen $(OBJS)

.PHONY: clean

clean::
	rm -f gen *.o

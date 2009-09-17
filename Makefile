CXXFLAGS = -I/opt/local/include -Wall -O3 -g
LDFLAGS = -L/opt/local/lib -lgd

SRCS = $(wildcard *.cpp)

OBJS = $(SRCS:.cpp=.o)

gen: $(OBJS)
	g++ -o gen $(OBJS) $(LDFLAGS)

.PHONY: clean

clean::
	rm -f gen *.o out.png


CXXFLAGS = -I/opt/local/include -Wall -O3 -g
LDFLAGS = -L/opt/local/lib -lgd

SRCS = $(wildcard *.cpp)

OBJS = $(SRCS:.cpp=.o)

gen: $(OBJS)
	g++ $(LDFLAGS) -o gen $(OBJS)

.PHONY: clean

clean::
	rm -f gen *.o out.png


TARGET = clock.exe
OBJS = \
clock.o

CC = gcc
CFLAGS = -Wall -O0 -g
CXX = g++
CXXFLAGS = $(CFLAGS)
LIBS = -lpthread

all: $(TARGET)

clean:
	rm -f $(TARGET) $(OBJS)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) -c $<

.cc.o:
	$(CXX) $(CXXFLAGS) -c $<

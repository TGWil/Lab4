CXX = g++
CXXFLAGS = -std=c++11 -O2

TARGET = place.exe

all: $(TARGET)

$(TARGET): anneal.cpp
	$(CXX) $(CXXFLAGS) -o $(TARGET) anneal.cpp

clean:
	rm -f $(TARGET)
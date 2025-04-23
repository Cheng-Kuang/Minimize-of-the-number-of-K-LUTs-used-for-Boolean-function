CXX = g++
CXXFLAGS = -std=c++11 -Wall -O3 -march=native -funroll-loops -flto -ffast-math

TARGET = mapper

SRC = main.cpp
OBJ = $(SRC:.cpp=.o)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
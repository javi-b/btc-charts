CXXFLAGS = -Wall -O2
MAGICKFLAGS = `Magick++-config --cxxflags --cppflags --ldflags --libs`

SRC = $(wildcard *.cpp)
OBJ = $(SRC:.cpp=.o)

btc-charts: $(OBJ) utils.h
	$(CXX) $(CXXFLAGS) $(MAGICKFLAGS) $(OBJ) -o $@

btc-charts.o: btc-charts.cpp utils.h
	$(CXX) $(CXXFLAGS) $(MAGICKFLAGS) -c $< -o $@

img.o: img.cpp img.h utils.h
	$(CXX) $(CXXFLAGS) $(MAGICKFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -vf $(OBJ) btc-charts

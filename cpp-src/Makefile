CXXFLAGS = -Wall -O2
MAGICKFLAGS = `Magick++-config --cxxflags --cppflags --ldflags --libs`

SRC = $(wildcard *.cpp)
OBJ = $(SRC:.cpp=.o)

btc_charts: $(OBJ) utils.h
	$(CXX) $(CXXFLAGS) $(MAGICKFLAGS) $(OBJ) -o $@

btc_charts.o: btc_charts.cpp utils.h
	$(CXX) $(CXXFLAGS) $(MAGICKFLAGS) -c $< -o $@

img.o: img.cpp img.h utils.h
	$(CXX) $(CXXFLAGS) $(MAGICKFLAGS) -c $< -o $@

btc_data.o: btc_data.cpp btc_data.h utils.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -vf $(OBJ) btc-charts

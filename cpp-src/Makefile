CXXFLAGS = -Wall

src = $(wildcard *.cpp)
obj = $(src:.cpp=.o)

btc-charts: $(obj)
	$(CXX) $(CXXFLAGS) $^ -o $@

.PHONY: clean
clean:
	rm -f $(obj) btc-charts

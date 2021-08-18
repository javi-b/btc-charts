src = $(wildcard *.cpp)
obj = $(src:.cpp=.o)

btc-charts: $(obj)
	g++ -Wall $^ -o $@

.PHONY: clean
clean:
	rm -f $(obj) btc-charts

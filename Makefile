
btc-charts : src/btc-charts.c src/myimg/myimg.c src/myimg/myimg.h
	gcc -Wall -lm -lpng -o btc-charts src/myimg/myimg.c src/btc-charts.c

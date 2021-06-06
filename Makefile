	#gcc -Wall -lm -lpng `pkg-config --cflags --libs MagickWand` \
		#-o btc-charts btc-charts.c

btc-charts : btc-charts.c myimg.c myimg.h
	gcc -Wall -lm -lpng -o btc-charts myimg.c btc-charts.c

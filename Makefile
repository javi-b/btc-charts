
btc-charts : src/btc-charts.c src/myimg/myimg.c src/myimg/myimg.h src/myimgproc/myimgproc.c src/myimgproc/myimgproc.h
	gcc -Wall -lm -lpng `pkg-config --cflags --libs MagickWand` src/btc-charts.c src/myimg/myimg.c src/myimgproc/myimgproc.c -o btc-charts

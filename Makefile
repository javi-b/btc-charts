
btc-charts : src/btc-charts.c src/util/util.c src/util/util.h src/btc/btc.c src/btc/btc.h src/img/img.c src/img/img.h src/imgproc/imgproc.c src/imgproc/imgproc.h
	gcc -Wall -lm -lpng `pkg-config --cflags --libs MagickWand` src/btc-charts.c src/util/util.c src/btc/btc.c src/img/img.c src/imgproc/imgproc.c -o btc-charts

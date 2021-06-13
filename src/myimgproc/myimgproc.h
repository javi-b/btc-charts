
enum corner {top_left, top_right, bottom_right, bottom_left};

int magick_read_img (MagickWand *, char *);
int magick_write_img (MagickWand *, char *);
int annotate_watermark (MagickWand *, int, int, int, int, char *, char *);
int paint_log_axis (MagickWand *, int, int, float, float, char *);

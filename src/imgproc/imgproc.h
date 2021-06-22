
enum corner {top_left, top_right, bottom_right, bottom_left};

int get_img_proc_font_size ();
int start_img_proc (char *, int, int, int);
int annotate_watermark (int, char *, char *);
int annotate_x_axis_values (float, float, float, float, char *);
int annotate_y_axis_values (float, float, float, float, int, char *);
int apply_img_proc ();
void finish_img_proc ();

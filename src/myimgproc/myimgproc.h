
enum corner {top_left, top_right, bottom_right, bottom_left};

int start_img_proc (char *);
int annotate_watermark (int, int, int, char *, char *);
int annotate_x_axis_values (int, int, float, float, float, char *);
int annotate_y_axis_values (int, float, float, int, float, char *);
int apply_img_proc ();
void finish_img_proc ();

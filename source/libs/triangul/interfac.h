#ifndef __interface_h
#define __interface_h

#define TRUE 1
#define FALSE 0

extern "C" int triangulate_polygon(int, int *, double (*)[2], int (*)[3]);
extern "C" int is_point_inside_polygon(double *);

#endif /* __interface_h */

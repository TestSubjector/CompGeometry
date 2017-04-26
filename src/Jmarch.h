#ifndef __JMARCH_H__
#define __JMARCH_H__
#include "convex_hull_structs.h"

int hull_compute(Point input[],int result[],int N);
int orient(Point p1,Point p2,Point p3);

#endif

// include guard
#ifndef __GRAHAM_SCAN_H__
#define __GRAHAM_SCAN_H__

#include<iostream> //duplication in convex_hull_structs is taken care of by their include guards
#include<stdlib.h> //duplication in convex_hull_structs is taken care of by their include guards
#include "convex_hull_structs.h"

double nextDirection(Point p,Point q,Point r);
int getLeftmostPoint(Point p[], int n);
int removeThetaCollinear(PolarPoint inp[],int n,PolarPoint out[]);
void sortPoints(PolarPoint inp[], int n);
int computeHull(Point inp[],int n,Node **root);
int getHull(Point input[],int n,Node **root);

#endif

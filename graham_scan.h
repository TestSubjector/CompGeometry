// include guard
#ifndef __GRAHAM_SCAN_H__
#define __GRAHAM_SCAN_H__

#include<iostream> //duplication in convex_hull_structs is taken care of by their include guards
#include<stdlib.h> //duplication in convex_hull_structs is taken care of by their include guards
#include "convex_hull_structs.h"

float nextDirection(Point p,Point q,Point r);
void swapLeftmostPoint(Point p[], int n);
int removeThetaCollinear(PolarPoint inp[],int n,PolarPoint out[]);
void sortPoints(PolarPoint inp[], int n);
void getHull(Point inp[],int n,Node **root);

#endif

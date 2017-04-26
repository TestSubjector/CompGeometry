// Include Guard
#ifndef __CONVEX_HULL_STRUCTS_INCLUDED__
#define __CONVEX_HULL_STRUCTS_INCLUDED__
#include<cmath>
#include<stdlib.h>
#include<iostream>

/// Struct that describes a point in polar coordinates
struct PolarPoint
{
  double r;
  double theta;
  int index;
  PolarPoint(double x, double y,int i):r(x),theta(y),index(i){}
  PolarPoint(){}
};

/// Struct that describes a point in cartesian coordinates
struct Point
{
  double x;
  double y;
  int index;
  Point(double x1, double y1, int ind) : x(x1), y(y1),index(ind){}
  Point(double x1, double y1) : x(x1), y(y1){}
  Point(){}
};

/// Struct that is a node in a stack
struct Node
{
  Point point;
  Node* next;
};

PolarPoint convertToPolar(Point p, Point origin);
int compareTheta(PolarPoint p, PolarPoint q);

int isEmpty(Node* root);
void push(Point p,Node** root);
Point pop(Node** root);
Point peek(Node** root);
void printStack(Node** root);

void printArray(PolarPoint p[], int n);
void printArray(Point p[], int n);
void swap(int i,int j, PolarPoint inp[]);
void swap(int i,int j, Point inp[]);

#endif

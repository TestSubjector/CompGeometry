#include "convex_hull_structs.h"

/**
 * Shifts the origin and then converts to Polar format
 * @param  p      Point in cartesian that needs to be converted
 * @param  origin Point in cartesian that is treated as origin
 * @return        PolarPoint which is the conversion of the passed Point
 */
PolarPoint convertToPolar(Point p, Point origin)
{
  p.x -= origin.x;
  p.y -= origin.y;
  float r = sqrt(p.x*p.x + p.y*p.y);
  if(r==0) return PolarPoint(0.0,0.0);
  float theta = atan(p.y/p.x);
  return PolarPoint(r,theta);
}

/**
 * Converts into cartesian and then shifts origin
 * @param  p      PolarPoint to be converted
 * @param  origin Origin in cartesian
 * @return        Point which is conversion of passed PolarPoint
 */
Point converttoCartesian(PolarPoint p, Point origin)
{
  float x = p.r * cos(p.theta);
  float y = p.r * sin(p.theta);
  x += origin.x;
  y += origin.y;
  return Point(x,y);
}

//
// Stack methods
//

int isEmpty(Node* root)
{
  return !root;
}

void push(Point p,Node** root)
{
  Node* temp = (Node*) malloc(sizeof(Node));
  temp->point = p;
  temp->next = *root;
  *root = temp;
}

Point pop(Node** root)
{
  //Stack is never empty in our usage, but check is kept. TODO think of better return here
  if(isEmpty(*root)) return Point(-1.0,-1.0);
  Point p = (*root)->point;
  *root = (*root)->next;
  return p;
}

Point peek(Node** root)
{
  //Stack is never empty in our usage, but check is kept. TODO think of better return here
  if(isEmpty(*root)) return Point(-1.0,-1.0);
  return (*root)->point;
}

void printStack(Node** root)
{
  Node* temp = *root;
  while(temp!=NULL)
  {
    printf("(%f,%f)\n",temp->point.x,temp->point.y);
    temp = temp->next;
  }
  printf("\n");
}

//
// Array methods
//

/**
 * Prints out all the elements in the array passed
 * @param p Array to be printed
 * @param n Length of the array
 */
void printArray(PolarPoint p[], int n)
{
  for (int i = 0; i < n; i++) {
    printf("%f,%f\n",p[i].r,p[i].theta );
  }
}

/**
 * Prints out all the elements in the array passed
 * @param p Array to be printed
 * @param n Length of the array
 */
void printArray(Point p[], int n)
{
  for (int i = 0; i < n; i++) {
    printf("%f,%f\n",p[i].x,p[i].y );
  }
}

/**
 * Swaps the elements i and j in the inp array passed
 * @param i   integer index
 * @param j   integer index
 * @param inp PolarPoint array
 */
void swap(int i,int j, PolarPoint inp[])
{
  PolarPoint temp = inp[i];
  inp[i] = inp[j];
  inp[j] = temp;
  return;
}

/**
 * Swaps the elements i and j in the inp array passed
 * @param i   integer index
 * @param j   integer index
 * @param inp Point array
 */
void swap(int i,int j, Point inp[])
{
  Point temp = inp[i];
  inp[i] = inp[j];
  inp[j] = temp;
  return;
}

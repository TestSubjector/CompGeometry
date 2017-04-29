#include "pointtriag.h"

float sign (Point p1, Point p2, Point p3)
{
    return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}

bool PointInTriangle (Point pt, Point v1, Point v2, Point v3)
{
    bool b1, b2, b3;

    b1 = sign(pt, v1, v2) < 0.0f;
    b2 = sign(pt, v2, v3) < 0.0f;
    b3 = sign(pt, v3, v1) < 0.0f;

    return ((b1 == b2) && (b2 == b3));
}

int TriangulatePoints(Point p[], int size, list<Poly> *result) {
    // first compute the hull
    setSize(size);
    Point* inp = new Point[size];
    Duplicate(inp,p);
    Arrange(inp);
    int points_on_hull = Convex(inp);

    // get an array of points inside the hull
    int points_inside = size - points_on_hull;
    // cout << "points_inside" << points_inside << endl;
    Point* internalPoints = new Point[points_inside];
    int onHull=0;
    int ind = 0;
    for (int i=0; i < size; i++) {
        onHull = 0;
        for (int j=0; j < points_on_hull; j++) {
            if (equals(p[i],inp[j])) {
                // lies on hull
                onHull = 1;
                break;
            }
        }
        if(!onHull) {
            internalPoints[ind++] = p[i];
        }
    }

    // for(int i=0; i < points_inside; i++) {
    //     cout << "internal " << i << " " << internalPoints[i].index << endl;
    // }

    Poly temp = Poly();
    temp.Init(points_on_hull);
    for(int i=0; i < points_on_hull; i++) {
        temp[i] = inp[i];
        // cout << "on hull " << inp[i].index << endl;
    }

    result->push_back(temp);
    // temp.printVert(cout);
    int success = Partition().Triangulate_MONO(&temp,result);

    if(success == 0) {
        cout << "some error in triangulation" << endl;
    }
    else {
        // cout << "internal triangulation begins" << endl;
        for(int i = 0; i < points_inside; i++) {

            // loop over the result triangles to check where the new point lies
            list<Poly>::iterator iter = result->begin();
            while (iter != result->end()) {
                // details on the current triangle
                Point *triangle = new Point[3];
                triangle[0] = (*iter)[0];
                triangle[1] = (*iter)[1];
                triangle[2] = (*iter)[2];
                // if point in triangle, replace with smaller triangles
                if (PointInTriangle(internalPoints[i],triangle[0],triangle[1],triangle[2])) {
                    // cout << "internal " << internalPoints[i].index << endl;
                    // cout << "v1 " << triangle[0].index << endl;
                    // cout << "v2 " << triangle[1].index << endl;
                    // cout << "v3 " << triangle[2].index << endl;
                    list<Poly>::iterator prev = iter++;
                    result->erase(prev);
                    Poly t1 = Poly(), t2 = Poly(), t3 = Poly();
                    t1.Triangle(internalPoints[i],triangle[1],triangle[2]);
                    t2.Triangle(triangle[0],internalPoints[i],triangle[2]);
                    t3.Triangle(triangle[0],triangle[1],internalPoints[i]);
                    result->push_back(t1);
                    result->push_back(t2);
                    result->push_back(t3);
                    break; // this presumes that one point lies in one triangle. Therefore, in case of lying on an existing edge, it only triangulates one of the triangles. Order to be seen.
                }
                else {
                    // no changes to list
                    iter++;
                }
            }
        }
        // all triangulations should be complete by this point
    }

    return success;
}

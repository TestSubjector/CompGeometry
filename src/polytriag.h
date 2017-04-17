
#ifndef __POLYTRIAG_H__
#define __PLYTRIAG_H__

#include <list>
#include <iostream>
#include <fstream>

using namespace std;

/// 2D point structure
struct Point {
	double x;
	double y;
	Point(double x1, double y1) : x(x1), y(y1){}
    Point(){}
};

/// Struct that is a node in a stack
struct Node
{
  long index;
  Node* next;
};

int isEmpty(Node* root);
void push(long i,Node** root);
long pop(Node** root);
long peek(Node** root);

/**
 * Polygon represented as a collection of points
 */
class Poly {

protected:

	Point *points;
	long numpoints;

public:

	/// constructors/destructors
	Poly();
	~Poly();

	Poly(const Poly &src);
	Poly& operator=(const Poly &src);

	/// getters and setters
	long GetNumPoints() {
		return numpoints;
	}

	Point &GetPoint(long i) {
		return points[i];
	}

	Point *GetPoints() {
		return points;
	}

	Point& operator[] (int i) {
		return points[i];
	}

	void printVert(ostream& outs) {
		for(long i = 0; i < numpoints; i++) {
			outs << "(" << points[i].x << ", " << points[i].y << ") ";
		}
		outs << std::endl;
	}

	/// clears the polygon points
	void Clear();

	/// inits the polygon with numpoints vertices
	void Init(long numpoints);

	//creates a triangle with points p1,p2,p3
	void Triangle(Point &p1, Point &p2, Point &p3);
};

/**
 * Paritions represented by edges and vertices
 */
class Partition {
protected:

	struct MonotoneVertex {
		Point p;
		long previous;
		long next;
	};

	class VertexSorter{
		MonotoneVertex *vertices;
	public:
		VertexSorter(MonotoneVertex *v) : vertices(v) {}
		bool operator() (long index1, long index2);
	};

	struct Diagonal {
		long index1;
		long index2;
	};

	/// edge that intersects the scanline
	struct ScanLineEdge {
		long index;
		Point p1;
		Point p2;

		/// determines if the edge is to the left of another edge
		bool operator< (const ScanLineEdge & other) const;

		bool IsConvex(const Point& p1, const Point& p2, const Point& p3) const;
	};

	/// standard helper functions
	bool IsConvexAngle(Point& p1, Point& p2, Point& p3);

	/// helper functions for MonotonePartition
	bool Below(Point &p1, Point &p2);
	void AddDiagonal(MonotoneVertex *vertices, long *numvertices, long index1, long index2);

	/// triangulates a monotone polygon, used in Triangulate_MONO
	int TriangulateMonotone(Poly *inPoly, list<Poly> *triangles);

public:

	/// triangulates a polygons by firstly partitioning it into monotone polygons
	/// time complexity: O(n*log(n)), n is the number of vertices
	/// space complexity: O(n)
	/// params:
	///   poly : an input polygon to be triangulated
	///          vertices have to be in counter-clockwise order
	///   triangles : a list of triangles (result)
	/// returns 1 on success, 0 on failure
	int Triangulate_MONO(Poly *poly, list<Poly> *triangles);

	/// triangulates a list of polygons by firstly partitioning them into monotone polygons
	/// time complexity: O(n*log(n)), n is the number of vertices
	/// space complexity: O(n)
	/// params:
	///   inpolys : a list of polygons to be triangulated (can contain holes)
	///             vertices of all non-hole polys have to be in counter-clockwise order
	///             vertices of all hole polys have to be in clockwise order
	///   triangles : a list of triangles (result)
	/// returns 1 on success, 0 on failure
int Triangulate_MONO(list<Poly> *inpolys, list<Poly> *triangles);

	/// creates a monotone partition of a list of polygons that can contain holes
	/// time complexity: O(n*log(n)), n is the number of vertices
	/// space complexity: O(n)
	/// params:
	///   inpolys : a list of polygons to be triangulated (can contain holes)
	///             vertices of all non-hole polys have to be in counter-clockwise order
	///             vertices of all hole polys have to be in clockwise order
	///   monotonePolys : a list of monotone polygons (result)
	/// returns 1 on success, 0 on failure
	int MonotonePartition(list<Poly> *inpolys, list<Poly> *monotonePolys);

};

#endif

//#include <vector>
//#include <math.h>
//#include <iostream>
#include <algorithm>

using namespace std;


struct Point {
    double x, y;
};

struct Shape {
    vector<Point> points;
};

struct Segment {
    Point p1, p2;
};


bool onSegment(Point p, Point q, Point r)
{
    if (q.x <= max(p.x, r.x) && q.x >= min(p.x, r.x) &&
        q.y <= max(p.y, r.y) && q.y >= min(p.y, r.y))
        return true;

    return false;
}

int orientation(Point p, Point q, Point r)
{
    double val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);

    if (val == 0) return 0;  // colinear 

    return (val > 0) ? 1 : 2; // clock or counterclock wise 
}
// intersection
bool doIntersect(Point p1, Point q1, Point p2, Point q2)
{
    // Find the four orientations needed for general and 
    // special cases 
    int o1 = orientation(p1, q1, p2);
    int o2 = orientation(p1, q1, q2);
    int o3 = orientation(p2, q2, p1);
    int o4 = orientation(p2, q2, q1);

    // General case 
    if (o1 != o2 && o3 != o4)
        return true;

    // Special Cases 
    // p1, q1 and p2 are colinear and p2 lies on segment p1q1 
    if (o1 == 0 && onSegment(p1, p2, q1)) return true;

    // p1, q1 and q2 are colinear and q2 lies on segment p1q1 
    if (o2 == 0 && onSegment(p1, q2, q1)) return true;

    // p2, q2 and p1 are colinear and p1 lies on segment p2q2 
    if (o3 == 0 && onSegment(p2, p1, q2)) return true;

    // p2, q2 and q1 are colinear and q1 lies on segment p2q2 
    if (o4 == 0 && onSegment(p2, q1, q2)) return true;

    return false; // Doesn't fall in any of the above cases 
}

float distance(Point p1, Point p2) {
    return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
}

Segment matrixToSegment(vector<Shape> shapes, int row, int col) {

    Segment s;
    int index = 0;
    int points = 0;
    for (index = 0; index < shapes.size() && points < row; index++) {
        points += shapes[index].points.size();
    }
    if (points != row) {
        index--;
        s.p1 = shapes[index].points[row - (points - shapes[index].points.size())];
    }
    else {
        s.p1 = shapes[index].points[0];
    }
    index = 0;
    points = 0;
    for (index = 0; index < shapes.size() && points < col; index++) {
        points += shapes[index].points.size();
    }
    if (points != col) {
        index--;
        s.p2 = shapes[index].points[col - (points - shapes[index].points.size())];
    }
    else {
        s.p2 = shapes[index].points[0];
    }
  
    return s;
}

int nrPointsOnMap(vector<Shape> shapes) {
    int nrPoints = 0;
    for (int index = 0; index < shapes.size(); index++) {
        nrPoints += shapes[index].points.size();
    }
    return nrPoints;
}

bool touchingSegments(int shape1, int shape2, int shape3, int p1,  int p2, int p3, int p4) {

    if ((shape2 == shape3 && (p2 == p3 || p2 == p4)) || (shape1 == shape3 && (p1 == p3 || p1 == p4))) {
        return true;
    }
    
    return false;
}

int matrixIndex(vector<Shape> shapes, int shape, int point) {
    int row = 0;
    for (int index = 0; index < shape ; index++) {
        row += shapes[index].points.size();
    }
    row += point;
    return row;
}

int findMinNode(vector<float> cost, vector<bool> visited, int nrNodes) {
    int minIndex = 0;
    float minCost = INT_MAX;

    for (int index = 0; index < nrNodes; index++) {
        if (cost[index] < minCost && visited[index] == false) {
            minCost = cost[index];
            minIndex = index;
        }
    }
    return minIndex;
}

void makeGraphToInf(vector<vector<float>>& graph) {
    for (int row = 0; row < graph.size(); row++) {
        for (int col = 0; col < graph[row].size(); col++) {
            if (graph[row][col] == 0) {
                graph[row][col] = INT_MAX;
            }
        }
    }
}
//find shortest path
vector<int> dijkstra(vector<vector<float>> graph, int start) {
    vector<float> cost;
    vector<int> path;
    vector<bool> visited;
    cost.resize(graph.size());
    path.resize(graph.size());
    visited.resize(graph.size());
    int currentNode;

    makeGraphToInf(graph);

    visited[start] = true;

    for (int col = 0; col < graph.size(); col++) {
        cost[col] = graph[start][col];
        if (graph[start][col] != INT_MAX) {
            path[col] = start;
        }
    }
    for (int row = 1; row < graph.size() - 1; row++) {
        currentNode = findMinNode(cost, visited, graph.size());
        visited[currentNode] = true;
        for (int col = 0; col < graph[row].size(); col++) {
            if (visited[col] == false && graph[currentNode][col] != INT_MAX && cost[currentNode] + graph[currentNode][col] < cost[col]) {
                cost[col] = cost[currentNode] + graph[currentNode][col];
                path[col] = currentNode;
            }
        }
    }

    for (int i = 0; i < path.size(); i++) {
        cout << path[i] << " ";
    }
    vector<int> ultimatePath;
    
    int index = graph.size() - 1;
    while (index > 0) {
        ultimatePath.push_back(index);
        index = path[index];
    }
    ultimatePath.push_back(0);
    for (int i = 0; i < ultimatePath.size(); i++) {
        cout << ultimatePath[i] << " ";
    }
    return ultimatePath;
}

void makeVisibilityGraph(vector<Shape> &shapes, Point p1, Point p2, vector<vector<float>> &mat, Point origin, float add, float unit) {
    
    Shape sh1;
    Shape sh2;
    sh1.points.push_back(p1);
    sh2.points.push_back(p2);
   
    shapes.insert(shapes.begin(), sh2);
    shapes.push_back(sh1);
    
 

    int nrPoints = 0;
    for (int index = 0; index < shapes.size(); index++) {
        nrPoints += shapes[index].points.size();
    }
 

    vector<vector<float> > matrix(nrPoints);
    for (int i = 0; i < nrPoints; i++) {
        matrix[i].resize(nrPoints);
    }
    mat = matrix;


    bool isIntersecting = false;
   


    for (int staticShape = 0; staticShape < shapes.size() - 1 ; staticShape++) {
        for (int staticPoint = 0; staticPoint < shapes[staticShape].points.size(); staticPoint++) {
            for (int firstShape = staticShape + 1; firstShape < shapes.size(); firstShape++) {
                for (int firstPoint = 0; firstPoint < shapes[firstShape].points.size(); firstPoint++) {
                    isIntersecting = false;
                    for (int secondShape = 0; secondShape < shapes.size(); secondShape++) {
                        for (int secondPoint = 0; secondPoint < shapes[secondShape].points.size() - 1; secondPoint++) {

                            
                            if (doIntersect(shapes[staticShape].points[staticPoint], shapes[firstShape].points[firstPoint], shapes[secondShape].points[0], shapes[secondShape].points[shapes[secondShape].points.size() - 1])
                                && !touchingSegments(staticShape, firstShape, secondShape, staticPoint, firstPoint, 0, shapes[secondShape].points.size() - 1)) {
                                isIntersecting = true;
                                break;
                            }
                            
                            if (doIntersect(shapes[staticShape].points[staticPoint], shapes[firstShape].points[firstPoint], shapes[secondShape].points[secondPoint], shapes[secondShape].points[secondPoint + 1])
                                && !touchingSegments(staticShape, firstShape, secondShape, staticPoint, firstPoint, secondPoint, secondPoint + 1)) {
                                isIntersecting = true;
                                break;

                               

                            }
                        }
                        if (isIntersecting) {
                            break;
                        }
                    }
                    if (!isIntersecting) {
                      
                        mat[matrixIndex(shapes, staticShape, staticPoint)][matrixIndex(shapes, firstShape, firstPoint)] = distance(shapes[staticShape].points[staticPoint], shapes[firstShape].points[firstPoint]);
                        mat[matrixIndex(shapes, firstShape, firstPoint)][matrixIndex(shapes, staticShape, staticPoint)] = distance(shapes[staticShape].points[staticPoint], shapes[firstShape].points[firstPoint]);
                        
                    }
                }
            }

        }
    }
}

struct lengths
{
    double lat1, lat2, lat3;
};
// Method used to calculate the length of each side.
lengths calculateLength(Point a, Point b, Point c)
{
    lengths result;

    result.lat1 = sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2));
    result.lat2 = sqrt(pow(c.x - b.x, 2) + pow(c.y - b.y, 2));
    result.lat3 = sqrt(pow(c.x - a.x, 2) + pow(c.y - a.y, 2));

    return result;
}


//This method calculates the coords of the incenter.
Point inscribedCircleCoords(Point a, Point b, Point c)
{
    lengths result;
    double AB, BC, AC;
    Point inscribedCircle;

    result = calculateLength(a, b, c);
    AB = result.lat1;
    BC = result.lat2;
    AC = result.lat3;
    double d;
    //Formula used to calculates the coordinates of the incenter.
   
    inscribedCircle.x = (BC * (a.x) + AC * (b.x) + AB * (c.x)) / (AB + BC + AC);
    inscribedCircle.y = (BC * (a.y) + AC * (b.y) + AB * (c.y)) / (AB + BC + AC);
    return inscribedCircle;
}


double calculSemiperimetru(Point a, Point b, Point c)
{
    double AB, AC, BC, semiperimetru;
    lengths result;

    result = calculateLength(a, b, c);
    AB = result.lat1;
    BC = result.lat2;
    AC = result.lat3;

    semiperimetru = (AB + BC + AC) / 2;
    return semiperimetru;
}
double calculArie(Point a, Point b, Point c)
{
    double AB, AC, BC;
    double arie;
    double semiper;
    lengths result;

    result = calculateLength(a, b, c);
    AB = result.lat1;
    BC = result.lat2;
    AC = result.lat3;
    semiper = calculSemiperimetru(a, b, c);

    arie = sqrt(semiper * (semiper - BC) * (semiper - AC) * (semiper - AB));
    return arie;
}
double calculRaza(Point a, Point b, Point c)
{
    double raza;
    raza = calculArie(a, b, c) / calculSemiperimetru(a, b, c);
    return raza;
}

//area
float areaOfShape(Shape shape) {
    float area = 0;
    shape.points.push_back(shape.points[0]);

    for (int index = 0; index < shape.points.size() - 1; index++)
        area += (shape.points[index].x * shape.points[index + 1].y) - (shape.points[index].y * shape.points[index + 1].x);
    area /= 2.0;

    return area;
}

//domain point
bool verific_punct(double x1, double y1, double x2, double y2, double x3, double y3){
    if (x1 * y2 + x2 * y3 + y1 * x3 - x3 * y2 - x2 * y1 - y3 * x1 > 0)
        return true;
    else
        return false;
}

bool isPointInPolygon(Shape p, Point s){
  
    bool apart = true;
   
    p.points.push_back(p.points[0]);
    
    for (int index = 0; index < p.points.size() - 1; index++)
    {
        if (!verific_punct(p.points[index].x, p.points[index].y, p.points[index + 1].x, p.points[index + 1].y, s.x, s.y)) {
            apart = false;
            break;
        }
    }
    return apart;
}

// convex hull

struct punct
{
    double x, y;
    float up; //unghiul polar
};
int cmp(punct A, punct B)
{
    if (A.up == B.up)
        if (A.y == B.y)
            return A.x < B.x;
        else
            return A.y < B.y;
    return A.up < B.up;
}


vector<Point> convexHull(vector<Point> points) {
    int i, j, k, nsv, x1, x2;
    vector<punct> v, st;
    punct sv;
    v.resize(points.size() + 1);
    st.resize(points.size() + 1);

    for (i = 1; i <= v.size() - 1; i++)
    {
        v[i].x = points[i - 1].x;
        v[i].y = points[i - 1].y;
        if (i == 1)
        {
            sv.x = v[i].x;
            sv.y = v[i].y;
            sv.up = 0;
            nsv = i;
        }
        else
            if (v[i].y < sv.y || v[i].y == sv.y && v[i].x < sv.x)
            {
                sv.x = v[i].x;
                sv.y = v[i].y;
                sv.up = 0;
                nsv = i;
            }
    }
    for (j = 1; j <= v.size() - 1; j++)
        if (j != nsv)
        {
            v[j].up = (v[j].x - sv.x) / sqrt((v[j].x - sv.x) * (v[j].x - sv.x) + (v[j].y - sv.y) * (v[j].y - sv.y));
            v[j].up = acos(v[j].up);
        }
    sort(v.begin() + 1, v.begin() + v.size(), cmp);
    st[1].x = v[1].x;
    st[1].y = v[1].y;
    st[2].x = v[2].x;
    st[2].y = v[2].y;
    k = 2;
    if (v.size() - 1 >= 3)
    {
        st[3].x = v[3].x;
        st[3].y = v[3].y;
        j = 3;
        k = 3;
        while (j < v.size() - 1)
        {
            if (st[k - 2].x * st[k - 1].y + st[k - 1].x * st[k].y + st[k].x * st[k - 2].y - st[k].x * st[k - 1].y - st[k - 2].x * st[k].y - st[k - 1].x * st[k - 2].y >= 0)
            {
                j++;
                k++;
                st[k] = v[j];
            }
            else
            {
                st[k - 1] = st[k];
                k--;
            }
            if (st[k - 2].x * st[k - 1].y + st[k - 1].x * st[k].y + st[k].x * st[k - 2].y - st[k].x * st[k - 1].y - st[k - 2].x * st[k].y - st[k - 1].x * st[k - 2].y < 0)
            {
                st[k - 1].x = st[k].x;
                st[k - 1].y = st[k].y;
                k--;
            }

        }
    }
    x1 = st[k].x;
    x2 = st[k].y;
    for (i = v.size() - 1; i >= 1 && v[i].up == st[k].up; i--)
        if (v[i].x != x1 || v[i].y != x2)
        {
            k++;
            st[k] = v[i];
        }

    vector<Point> finalPoints;
    finalPoints.resize(k + 1);

    for (int index = 1; index <= k; index++) {
        finalPoints[index - 1].x = st[index].x;
        finalPoints[index - 1].y = st[index].y;
    }
    finalPoints[k] = finalPoints[0];

    return finalPoints;
}

//minimum distance between 2 points
Segment minimDistanceBetween2Points(vector<Point> points, float &dis)
{
    dis = UINT64_MAX;
    Segment seg;
    for (int i = 0; i < points.size(); ++i)
        for (int j = i + 1; j < points.size(); ++j)
            if (distance(points[i], points[j]) < dis)
            {
                dis = distance(points[i], points[j]);
                seg.p1 = points[i];
                seg.p2 = points[j];
            }
    return seg;
}














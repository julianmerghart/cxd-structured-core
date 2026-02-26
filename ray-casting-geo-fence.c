#include "cxd_core.h"

typedef struct { float x, y; } Point;

// Ray-Casting Algorithm to check if Point is inside Polygon (National Borders)
int is_inside_border(Point p, Point *polygon, int n) {
    int count = 0;
    for (int i = 0, j = n - 1; i < n; j = i++) {
        if (((polygon[i].y > p.y) != (polygon[j].y > p.y)) &&
            (p.x < (polygon[j].x - polygon[i].x) * (p.y - polygon[i].y) / (polygon[j].y - polygon[i].y) + polygon[i].x)) {
            count++;
        }
    }
    return count % 2; // Returns 1 if inside
}

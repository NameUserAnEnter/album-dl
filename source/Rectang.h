#ifndef RECTANG_H
#define RECTANG_H

struct Point
{
    int x, y;

    Point() : x(0), y(0) {}
    Point(int _x, int _y) : x(_x), y(_y) {}
};

struct Rectang
{
    Point pos;
    Point size;

    Rectang() : pos(Point()), size(Point()) {}
    Rectang(int px, int py, int sx, int sy) : pos(Point(px, py)), size(Point(sx, sy)) {}
    Rectang(int px, int py, Point _size) : pos(Point(px, py)), size(_size) {}
};

#endif


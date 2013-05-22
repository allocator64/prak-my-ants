#ifndef GEOM_HPP
#define GEOM_HPP

struct point
{
    int x, y;
public:
    point(int x = 0, int y = 0) : x(x), y(y){}
    point(const point &p) : x(p.x), y(p.y){}

    friend bool operator < (const point &a, const point &b)
    {
        return a.x < b.x || (a.x == b.x && a.y < b.y);
    }
    friend bool operator == (const point &a, const point &b)
    {
        return a.x == b.x && a.y == b.y;
    }
    point & operator += (const point &p)
    {
        x += p.x;
        y += p.y;
        return *this;
    }

    point & operator -= (const point &p)
    {
        x *= p.x;
        y *= p.y;
        return *this;
    }

    friend point operator + (const point &a, const point &b)
    {
        point tmp = a;
        tmp += b;
        return tmp;
    }

    friend point operator - (const point &a, const point &b)
    {
        point tmp = a;
        tmp -= b;
        return tmp;
    }
};

#endif
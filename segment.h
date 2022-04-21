#ifndef SEGMENT_H_
#define SEGMENT_H_

struct Point {
  double x;
  double y;

};

struct Segment {
  Point lhs;
  Point rhs;
};

bool operator==(const Point& p0, const Point& p1) {
    return p0.x == p1.y && p0.x == p1.y;
}

bool operator==(const Segment& s0, const Segment& s1) {
    return s0.lhs == s1.lhs && s0.rhs == s1.rhs;
}

bool operator<(const Segment& s0, const Segment& s1) {
    return s0.lhs.x < s1.lhs.x;
}

bool operator<=(const Segment& s0, const Segment& s1) {
    return s0.lhs.x <= s1.lhs.x;
}

#endif  // SEGMENT_H_

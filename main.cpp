#include <iostream>
#include <fstream>
#include <termios.h>
#include <unistd.h>
#include <cmath>
#include <vector>


#pragma GCC optimize("O3")
#pragma GCC optimize("Ofast")
#pragma GCC optimize("unroll-loops")

using namespace std;

double INF = 1e9;

struct player{
    double x;
    double y;
    double angle;
};
struct line {
    double a, b, c;
};
struct segment {
    double x1, y1, x2, y2;
};
struct point {
    double x, y;
};
vector<double> line_to_abc(const segment &s) {
    vector<double> k(3);
    k[0] = s.y1 - s.y2;
    k[1] = s.x2 - s.x1;
    k[2] = s.x1 * s.y2 - s.x2 * s.y1;
    return k;
}
double dist(const point &p1, const point &p2) {
    if (p2.x!=INF && p2.y!=INF && p1.x!=INF && p1.y!=INF) {
        return sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
    }
    else return INF;
}

bool is_point_in_segment(const point &p, const segment &s) {
    const point a{s.x1, s.y1};
    const point b{s.x2, s.y2};
    int rnd = 1;
    double d = (dist(a, b)*rnd)/rnd;
    double d1 = (dist(a, p)*rnd)/rnd;
    double d2 = (dist(p, b)*rnd)/rnd;
    if (d == d1+d2) return true;
    else return false;
}
bool is_point_in_ray(const point &p, const segment &s){
    if (((p.x - s.x1) * (s.y2 - s.y1) == (s.x2 - s.x1) * (p.y - s.y1)) && ((p.x - s.x1) * (s.x2 - s.x1) >= 0)  && ((p.y - s.y1) * (s.y2 - s.y1) >= 0)) return true;
    else return false;
}
double distance(const point &p, const line &l) {
    double chast = sqrt(l.a * l.a + l.b * l.b);
    return (l.a * p.x + l.b * p.y + l.c) / chast;
}
bool points_and_line(const point &p1, const point &p2, const line &l) {
    if (distance(p1, l) > 0 && distance(p2, l) > 0) {
        return true;
    } else if (distance(p1, l) < 0 && distance(p2, l) < 0) {
        return true;
    } else {
        return false;
    }
}
pair<double, double> intersection(const line &fir, const line &sec) {
    double x, y;
    if (fir.b == 0 && sec.b != 0) {
        x = (-1) * fir.c / fir.a;
        y = (-1) * (sec.a / sec.b * x + sec.c / sec.b);
    } else if (fir.b != 0 && sec.b == 0) {
        x = (-1) * sec.c / sec.a;
        y = (-1) * (fir.a / fir.b * x + fir.c / fir.b);
    } else {
        x = (sec.c / sec.b - fir.c / fir.b) / (fir.a / fir.b - sec.a / sec.b);
        y = (-1) * (fir.a / fir.b * x + fir.c / fir.b);
    }
    pair<double, double> p;
    p.first = x;
    p.second = y;
    return p;
}
pair <double, double> int_of_ray_and_segment(const segment s1, const segment &s) {
    vector <double> k1 = line_to_abc(s1);
    vector<double> k = line_to_abc(s);
    const line l{k[0], k[1], k[2]};
    const line p{k1[0], k1[1], k1[2]};
    pair <double, double> inter = intersection(p, l);
    const point poi{inter.first, inter.second};
    pair <double, double> pr; pr.first = INF; pr.second = INF;
    const line perp{-k1[1], k1[0], k1[1]*s1.x1-k1[0]*s1.y1};
    const point p1{s1.x2, s1.y2};
    if (is_point_in_segment(poi, s) && points_and_line(poi, p1, perp)) return inter;
    else return pr;
}

const double pi = acos(-1);
int main() {
    ifstream fin("input.txt");
    int w = 201 ;
    int h = 53;
    int va = 100;
    vector<vector<char> > output(h, vector<char>(w, '#'));
    for (int i = 0; i < h/2; i++) {
        for (int j = 0; j < w; j++) {
            output[i][j] = '.';
        }
    }

    int count = 4;
    vector <segment> walls(count);
    for (int i = 0; i < count; i++) {
        double x1, y1, x2, y2; fin>>x1>>y1>>x2>>y2;
        segment f{x1, y1, x2, y2};
        walls[i] = f;
    }

    player a{0, 0, 0};


    cout.setf(ios::unitbuf);
    termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag &= ~ICANON;
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
    cout<<"x:"<<a.x<<"\t"<<"y:"<<a.y<<"\t"<<"angle:"<<a.angle/pi*180<<"                              "<<endl;
    double lb = a.angle-(pi*50)/180;
    double rb = a.angle+(pi*50)/180;
    double step = pi/360;
    while (true){
        char c = getchar();

        if (c=='w') {
            a.x+=0.125*sin(a.angle);
            a.y+=0.125*cos(a.angle);
        }
        else if (c=='s') {
            a.x-=0.125*sin(a.angle);
            a.y-=0.125*cos(a.angle);
        }
        else if (c=='a' || c=='d') {
            if (c=='a') a.angle-=pi/180;
            else a.angle+=pi/180;
            lb = a.angle-(pi*50)/180;
            rb = a.angle+(pi*50)/180;
        }

        vector <double> dists(w, INF);
        for (int i = 0; i < w; i++) {
            double angle = lb+step*i;
            const segment s{a.x, a.y,a.x+1*sin(angle), a.y+1*cos(angle)};
            double min = INF;
            for (int j = 0; j < count; j++) {
                pair <double, double> p = int_of_ray_and_segment(s, walls[j]);
                //cout<<i<<" "<<j<<" "<<p.first<<"  "<<p.second<<endl;
                const point p2{p.first, p.second};
                const point p1{a.x, a.y};
                double dst = dist(p1, p2);
                if (dst<min) min = dst;

            }
            dists[i] = min;
        }
        for (int i = 0; i < w; i++) {
            double dst = dists[i];
            int zap;
            if (dst>=1) zap=h/dst;
            else zap = h;
            for (int j = h/2; j < h; j++) {
                if (zap/2-(j-26)>0) output[j][i] = '5';
                else output[j][i] = '#';
            }
            for (int j = h/2; j >= 0; j--) {
                if (h/2-j<zap/2) output[j][i] = '5';
                else output[j][i] = '.';
            }

        }

        printf("\x1b[54A"); //                                             зависит от h!
        printf("\x1b[201D"); //                                            зависит от w!

        for (int i = 0; i < h; i++) {
            for (int j = 0; j < w; j++) {
                if (output[i][j]!='5') printf("%c", output[i][j]);
                else printf("\u2588");
            }
            printf("\n");
        }

        printf("x: %f\ty: %f\t angle: %f\tlb: %f\trb: %f\n", a.x, a.y, a.angle/pi*180, lb/pi*180, rb/pi*180);
    }
}

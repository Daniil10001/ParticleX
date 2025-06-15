#ifndef standarts_hpp__
#define standarts_hpp__

#include<ctype.h>

#define ull unsigend long long
#define ll long long
#define ul unsigned long
#define u unsigned
#define ld double

const ld PI = 3.1415926535897932385L;

#include<random>
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<ld> dist_PI(-PI, PI);

inline ld unifRandDirection() {return dist_PI(gen);}

#endif
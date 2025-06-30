#ifndef standarts_hpp__
#define standarts_hpp__

#define ull unsigend long long
#define ll long long
#define ul unsigned long
#define u unsigned
#define ld long double

const ld PI = 3.1415926535897932385L;

#include<random>
static std::random_device rd;
static std::mt19937 gen(rd());
static std::uniform_real_distribution<ld> dist_PI(-PI, PI);

inline ld unifRandDirection() {return dist_PI(gen);}

#endif
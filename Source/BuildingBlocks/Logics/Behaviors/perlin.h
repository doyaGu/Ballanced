#define B 0x100
#define BM 0xff
#define N 0x1000
#define NP 12 /* 2^N */
#define NM 0xfff

#define s_curve(t) (t * t * (3.0f - 2.0f * t))
#define lerp(t, a, b) (a + t * (b - a))
#define setup(i, b0, b1, r0, r1) \
    t = vec[i] + N;              \
    b0 = ((int)t) & BM;          \
    b1 = (b0 + 1) & BM;          \
    r0 = t - (int)t;             \
    r1 = r0 - 1.0f;
#define at2(rx, ry) (rx * q[0] + ry * q[1])
#define at3(rx, ry, rz) (rx * q[0] + ry * q[1] + rz * q[2])

void init(void);
float noise1(float);
float noise2(float *);
float noise3(float *);
void normalize3(float *);
void normalize2(float *);

float PerlinNoise1D(float, float, float, int);
float PerlinNoise2D(float, float, float, float, int);
float PerlinNoise3D(float, float, float, float, float, int);

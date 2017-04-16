
#define _USE_MATH_DEFINES
#include <math.h>

#define REG2DEG(a) (a)/M_PI*180.0
#define DEG2REG(a) (a)/180.0*M_PI
#define IsZero(a,precision) (abs(a)<(precision))
#define CanMod(a,b,precision) \
		(IsZero((a)/(b)-(long)((a)/(b)),(precision)))

//���a����2Pi��С��-2Pi���򻮹����2Pi��Χ
#define MakeIn2Pi(a) ((abs(a)>2*M_PI)?(((a)/M_PI/2-(long)((a)/M_PI/2))*2*M_PI):(a))
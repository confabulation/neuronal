#define N 10000
#define L 5000
#define P 0.3
#define V 10
#define WSize N*N/8
#define TRUE 1
#define FALSE 0
#define ISBITSET(x,i) ((x[i>>3] & (1<<(i&7)))!=0)
#define SETBIT(x,i) x[i>>3]|=(1<<(i&7));
#define CLEARBIT(x,i) x[i>>3]&=(1<<(i&7))^0xFF;


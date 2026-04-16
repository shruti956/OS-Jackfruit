#include <stdio.h>
#include <time.h>

int main() {
    volatile long long x = 0;
    time_t start = time(NULL);
    printf("cpu_hog starting\n");
    fflush(stdout);
    while (1) {
        x++;
        if (x % 500000000LL == 0) {
            printf("cpu_hog alive elapsed=%ld\n", (long)(time(NULL) - start));
            fflush(stdout);
        }
    }
    return 0;
}

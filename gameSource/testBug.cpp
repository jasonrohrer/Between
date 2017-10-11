#include <stdio.h>

int main() {

    int A = 2;
    int B = 1;

    while( B != A ) {
        printf( "Start of loop, A=%d, B=%d\n", A, B );

        A = B;
        printf( "Set A = B, so now A=%d, B=%d\n", A, B );

        B = 0;
        printf( "Set B = 0, so now A=%d, B=%d\n", A, B );
        }
    
    printf( "Loop ended because A and B equal, with A=%d, B=%d\n", A, B );

    return 0;
    }

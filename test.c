#include <stdio.h>
#include <unistd.h>

int main() {
    // Count down from 10 to 0
    for (int i = 10; i >= 0; i--) {
        printf("%d\n", i);
        fflush(stdout); // Ensure output is printed immediately
        sleep(1); // Wait for 1 second
    }

    return 0;
}
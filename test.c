// #include <windows.h>
// #include <stdio.h>

// int main() {
//     // Create a new console
//     AllocConsole();

//     // Redirect stdout to the new console
//     freopen("CONOUT$", "w", stdout);

//     // Count down from 10 to 0
//     for (int i = 10; i >= 0; i--) {
//         printf("%d\n", i);
//         Sleep(1000); // Wait for 1 second
//     }

//     // Close the new console
//     FreeConsole();

//     return 0;
// }

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
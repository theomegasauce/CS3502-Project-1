#include <iostream>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cerrno>

using namespace std;

int main() {
    const char* fifoProd = "/tmp/prod_to_cons";   // Producer to Consumer
    const char* fifoCons = "/tmp/cons_to_prod";      // Consumer to Producer

    while(true) {
        // Open FIFO for reading the number.
        int fdRead = open(fifoProd, O_RDONLY);
        if(fdRead == -1) {
            perror("open fifoProd");
            return 1;
        }
        int numTerms;
        if(read(fdRead, &numTerms, sizeof(numTerms)) == -1) {
            perror("read number");
            close(fdRead);
            return 1;
        }
        close(fdRead);
        
        cout << "Consumer received number: " << numTerms << endl;
        cout << "Fibonacci sequence (" << numTerms << " terms):" << endl;
        
        // Generate and display the Fibonacci sequence.
        // Sequence starts with 0 and 1.
        unsigned long long first = 0, second = 1, next;
        for (int i = 0; i < numTerms; i++) {
            if (i == 0) {
                cout << first << " ";
            } else if (i == 1) {
                cout << second << " ";
            } else {
                next = first + second;
                cout << next << " ";
                first = second;
                second = next;
            }
        }
        cout << "\n" << endl;
        
        // Signal the producer that processing is complete.
        int signal = 1; // Arbitrary signal value.
        int fdWrite = open(fifoCons, O_WRONLY);
        if(fdWrite == -1) {
            perror("open fifoCons");
            return 1;
        }
        if(write(fdWrite, &signal, sizeof(signal)) == -1) {
            perror("write signal");
            close(fdWrite);
            return 1;
        }
        close(fdWrite);
    }
    
    return 0;
}

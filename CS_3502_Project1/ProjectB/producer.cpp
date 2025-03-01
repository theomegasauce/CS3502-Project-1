#include <iostream>
#include <cstdlib>
#include <ctime>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cerrno>

using namespace std;

int main() {
    const char* fifoProd = "/tmp/prod_to_cons";   // Producer to Consumer
    const char* fifoCons = "/tmp/cons_to_prod";      // Consumer to Producer

    // Create FIFOs if they don't exist.
    if (mkfifo(fifoProd, 0666) == -1) {
        if(errno != EEXIST) {
            perror("mkfifo prod_to_cons");
            return 1;
        }
    }
    
    if (mkfifo(fifoCons, 0666) == -1) {
        if(errno != EEXIST) {
            perror("mkfifo cons_to_prod");
            return 1;
        }
    }
    
    srand(static_cast<unsigned int>(time(NULL)));
    
    while(true) {
        // Wait for 1 second before generating next number
        sleep(1);
        
        // Generate a random number between 1 and 1000.
        int randomNumber = rand() % 100000 + 1;
        cout << "Producer generated number: " << randomNumber << endl;
        
        // Open FIFO for writing the number.
        int fdWrite = open(fifoProd, O_WRONLY);
        if (fdWrite == -1) {
            perror("open fifoProd");
            return 1;
        }
        if (write(fdWrite, &randomNumber, sizeof(randomNumber)) == -1) {
            perror("write random number");
            close(fdWrite);
            return 1;
        }
        close(fdWrite);
        
        // Wait for consumer signal indicating it's done.
        int signal;
        int fdRead = open(fifoCons, O_RDONLY);
        if (fdRead == -1) {
            perror("open fifoCons");
            return 1;
        }
        if (read(fdRead, &signal, sizeof(signal)) == -1) {
            perror("read signal");
            close(fdRead);
            return 1;
        }
        close(fdRead);
    }
    
    return 0;
}

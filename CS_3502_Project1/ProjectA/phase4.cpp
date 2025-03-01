#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <string>

using namespace std;

// ACCOUNT STRUCT
struct Account {
    int id;
    int balance;
    pthread_mutex_t mutex;
    string name;
};

// ACCOUNTS
Account accounts[3] = {
    {1, 5000, PTHREAD_MUTEX_INITIALIZER, "Checking"},
    {2, 10000, PTHREAD_MUTEX_INITIALIZER, "Savings"},
    {3, 15000, PTHREAD_MUTEX_INITIALIZER, "Investing"}
};

// Structure for transfer data
struct TransferData {
    Account* from;
    Account* to;
    int amount;
    bool useOrdering;  // ordering (TRUE/FALSE)
    int timeout;      // in milliseconds
};

// TIMEOUT
bool tryLockWithTimeout(pthread_mutex_t* mutex, int timeout_ms) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += timeout_ms / 1000;
    ts.tv_nsec += (timeout_ms % 1000) * 1000000;
    
    if (ts.tv_nsec >= 1000000000) {
        ts.tv_sec += 1;
        ts.tv_nsec -= 1000000000;
    }
    
    int result = pthread_mutex_timedlock(mutex, &ts);
    return (result == 0);
}

// TRANSFER
void* transfer(void* arg) {
    TransferData* data = (TransferData*)arg;
    Account* from = data->from;
    Account* to = data->to;
    int amount = data->amount;
    bool useOrdering = data->useOrdering;
    int timeout = data->timeout;
    
    pthread_t thread_id = pthread_self();
    
    cout << "Thread " << thread_id << ": Attempting to transfer $" << amount 
         << " from " << from->name << " (ID:" << from->id << ") to " 
         << to->name << " (ID:" << to->id << ")" << endl;
    
   
        
    Account *first, *second;
    
    //ORDERING
    if (useOrdering) {
        // ACENDING ORDER
        if (from->id < to->id) {
            first = from;
            second = to;
        } else {
            first = to;
            second = from;
        }
        cout << "Thread " << thread_id << ": Using ordered locking (ID " 
             << first->id << " before ID " << second->id << ")" << endl;
    } else {
        // SETUP FOR DEADLOCK (ORDERING=FALSE)
        first = from;
        second = to;
        cout << "Thread " << thread_id << ": Using non-ordered locking" << endl;
    }
    
    // locking first accnt with timeout
    cout << "Thread " << thread_id << ": Attempting to lock " << first->name << " (ID:" << first->id << ")" << endl;
    if (!tryLockWithTimeout(&first->mutex, timeout)) {
        cout << "Thread " << thread_id << ": TIMEOUT - Could not acquire lock on " 
             << first->name << " within " << timeout << "ms" << endl;
        return NULL;
    }
    
    cout << "Thread " << thread_id << ": Acquired lock on " << first->name << endl;
    
    sleep(2); 
    
    // locking second accnt w timeout
    cout << "Thread " << thread_id << ": Attempting to lock " << second->name << " (ID:" << second->id << ")" << endl;
    if (!tryLockWithTimeout(&second->mutex, timeout)) {
        cout << "Thread " << thread_id << ": TIMEOUT - Could not acquire lock on " 
             << second->name << " within " << timeout << "ms" << endl;
        
        // if first lock timeout, unllock first for recovery
        pthread_mutex_unlock(&first->mutex);
        cout << "Thread " << thread_id << ": Released lock on " << first->name 
             << " for deadlock recovery" << endl;
        return NULL;
    }
    
    cout << "Thread " << thread_id << ": Acquired lock on " << second->name << endl;
    
    //TRANSFER
    if (first == from) {
        from->balance -= amount;
        to->balance += amount;
    } else {
        to->balance -= amount;
        from->balance += amount;
    }
    
    cout << "Thread " << thread_id << ": Transfer complete" << endl;
    cout << from->name << " balance: $" << from->balance << endl;
    cout << to->name << " balance: $" << to->balance << endl;
    
    // UNLOCK
    pthread_mutex_unlock(&second->mutex);
    cout << "Thread " << thread_id << ": Released lock on " << second->name << endl;
    pthread_mutex_unlock(&first->mutex);
    cout << "Thread " << thread_id << ": Released lock on " << first->name << endl;
    
    return NULL;
}

int main() {
    cout << "=== Phase 4: Deadlock Prevention and Recovery ===" << endl;
    cout << "Initial account balances:" << endl;
    for (int i = 0; i < 3; i++) {
        cout << accounts[i].name << " (ID:" << accounts[i].id << "): $" << accounts[i].balance << endl;
    }
    
    pthread_t thread1, thread2;
    
    // First demonstration: With resource ordering (deadlock prevention)
    cout << "\n=== Test 1: Using Resource Ordering (Safe) ===" << endl;
    
    TransferData data1 = {&accounts[0], &accounts[1], 1000, true, 5000};
    TransferData data2 = {&accounts[1], &accounts[0], 500, true, 5000};
    
    pthread_create(&thread1, NULL, transfer, &data1);
    pthread_create(&thread2, NULL, transfer, &data2);
    
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    
    // Reset balances
    accounts[0].balance = 5000;
    accounts[1].balance = 10000;
    
    cout << "\n=== Test 2: Without Resource Ordering (Potential Deadlock) ===" << endl;
    
    // Second demonstration: Without resource ordering (potential deadlock)
    TransferData data3 = {&accounts[0], &accounts[1], 1000, false, 5000};
    TransferData data4 = {&accounts[1], &accounts[0], 500, false, 5000};
    
    pthread_create(&thread1, NULL, transfer, &data3);
    pthread_create(&thread2, NULL, transfer, &data4);
    
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    
    cout << "\n=== Final Account Balances ===" << endl;
    for (int i = 0; i < 3; i++) {
        cout << accounts[i].name << " (ID:" << accounts[i].id << "): $" << accounts[i].balance << endl;
    }
    
    return 0;
}
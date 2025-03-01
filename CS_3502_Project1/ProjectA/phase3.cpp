#include <iostream>
#include <pthread.h>
#include <unistd.h>

using namespace std;

//ACOUNTS
struct Account {
    int id;
    int balance;
    pthread_mutex_t mutex;
};

//TRANSFER: To make a two way transfer function
struct TransferData {
    Account* from;
    Account* to;
    int amount;
};

bool account1_allocated = false;
bool account2_allocated = false;
pthread_t account1_owner;
pthread_t account2_owner;
pthread_mutex_t allocation_mutex = PTHREAD_MUTEX_INITIALIZER;

// Deadlock detection using resource allocation algorithm
bool detect_deadlock(Account* requested_account) {
    pthread_mutex_lock(&allocation_mutex);
    
    pthread_t current_thread = pthread_self();
    bool deadlock = false;
    
    // Check for circular wait condition
    if (requested_account->id == 1 && account1_allocated && account1_owner != current_thread) {
        if (account2_allocated && account2_owner == current_thread) {
            // Thread owns account2 and wants account1, but account1 is held by another thread
            cout << "DEADLOCK DETECTED: Thread " << current_thread 
                 << " holds Account 2 and is waiting for Account 1" << endl;
            cout << "                   Thread " << account1_owner 
                 << " holds Account 1 and is likely waiting for Account 2" << endl;
            deadlock = true;
        }
    }
    else if (requested_account->id == 2 && account2_allocated && account2_owner != current_thread) {
        if (account1_allocated && account1_owner == current_thread) {
            // Thread owns account1 and wants account2, but account2 is held by another thread
            cout << "DEADLOCK DETECTED: Thread " << current_thread 
                 << " holds Account 1 and is waiting for Account 2" << endl;
            cout << "                   Thread " << account2_owner 
                 << " holds Account 2 and is likely waiting for Account 1" << endl;
            deadlock = true;
        }
    }
    
    pthread_mutex_unlock(&allocation_mutex);
    return deadlock;
}

//update allocation mutex function (so its out of the transfer function)
void update_allocation(Account* account, bool allocate, pthread_t thread_id = 0) {
    pthread_mutex_lock(&allocation_mutex);
    
    if (account->id == 1) {
        account1_allocated = allocate;
        if (allocate) {
            account1_owner = thread_id;
        }
    } else if (account->id == 2) {
        account2_allocated = allocate;
        if (allocate) {
            account2_owner = thread_id;
        }
    }
    
    pthread_mutex_unlock(&allocation_mutex);
}


// TRANSFER 
void* transfer(void* arg) {
    TransferData* data = (TransferData*)arg;
    Account* from = data->from;
    Account* to = data->to;
    int amount = data->amount;
    pthread_t thread_id = pthread_self();
    
    cout << "Thread " << thread_id << ": Transferring " << amount << "$ from Account " 
         << from->id << " to Account " << to->id << endl;
    
    // LOCK first account
    cout << "Thread " << thread_id << "--> Locking Account " << from->id << endl;
    pthread_mutex_lock(&from->mutex);
    
    // Update allocation status
    update_allocation(from, true, thread_id);
    
    sleep(1);//getting deadlocks to occur
    
    // Try to lock second account, check for deadlock first
    cout << "Thread " << thread_id << ": Trying to lock Account " << to->id << endl;
    if (detect_deadlock(to)) {
        // Deadlock detected - release first account
        update_allocation(from, false);
        pthread_mutex_unlock(&from->mutex);
        cout << "Thread " << thread_id << ": Released Account " << from->id 
             << " to avoid deadlock" << endl;
        return NULL;
    }
    
    pthread_mutex_lock(&to->mutex);
    update_allocation(to, true, thread_id);
    
    // Perform transfer
    from->balance -= amount;
    to->balance += amount;
    cout << "Thread " << thread_id << ": Transfer complete\n"
         << "Account " << from->id << " balance: $" << from->balance << "\n"
         << "Account " << to->id << " balance: $" << to->balance << endl;
    
    // UNLOCK
    pthread_mutex_unlock(&to->mutex);
    update_allocation(to, false);
    cout << "Thread " << thread_id << ": Unlocked Account " << to->id << endl;
    
    pthread_mutex_unlock(&from->mutex);
    update_allocation(from, false);
    cout << "Thread " << thread_id << ": Unlocked Account " << from->id << endl;
    
    return NULL;
}

//ACCOUNTS
Account account1 = {1, 5000, PTHREAD_MUTEX_INITIALIZER};
Account account2 = {2, 5000, PTHREAD_MUTEX_INITIALIZER};

int main() {
    cout << "Initial Account 1 balance: $" << account1.balance << endl;
    cout << "Initial Account 2 balance: $" << account2.balance << endl;
    
    pthread_t thread1, thread2;
    
    // Create transfer data - in opposite directions to cause deadlock
    TransferData transf1 = {&account1, &account2, 1000}; // Account 1 -> Account 2
    TransferData transf2 = {&account2, &account1, 500};  // Account 2 -> Account 1
    
    // Create threads for transfers
    pthread_create(&thread1, NULL, transfer, &transf1);
    pthread_create(&thread2, NULL, transfer, &transf2);
    
    // Wait for threads to complete
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    
    cout << "Final Account 1 balance: $" << account1.balance << endl;
    cout << "Final Account 2 balance: $" << account2.balance << endl;
    
    return 0;
}
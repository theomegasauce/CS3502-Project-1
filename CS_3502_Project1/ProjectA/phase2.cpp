#include <iostream>
#include <pthread.h>
#include <unistd.h>

using namespace std;

// Starting Balance - GLOBAL
int bank_balance = 10000;

// LOCK - global (shared)
pthread_mutex_t balance_mutex;

// DEPOSIT FUNCTION
void* deposit(void* arg) {
    int amount = *((int*)arg);
    
    // lock
    pthread_mutex_lock(&balance_mutex);
    cout <<"Locking balance_mutex" << endl;
    usleep(1000000);  // 1000ms / 1s
    
    int temp = bank_balance;
    temp += amount;
    bank_balance = temp;
    
    cout << "Deposit Amount: " << amount << ",\n" << "New Balance: " << bank_balance << endl;
    
    // unlock
    pthread_mutex_unlock(&balance_mutex);
    cout <<"unlocking balance_mutex" << endl;
    usleep(1000000);  // 1000ms / 1s
    
    return NULL;
}

// WITHDRAW FUNCTION
void* withdraw(void* arg) {
    int amount = *((int*)arg);
    
    // lock
    pthread_mutex_lock(&balance_mutex);
    cout <<"Locking balance_mutex" << endl;
    usleep(1000000);  // 1000ms / 1s

    int temp = bank_balance;
    temp -= amount;
    bank_balance = temp;
    
    cout << "Withdraw Amount: " << amount << ",\n" << "New Balance: " << bank_balance << endl;
    
    // unlock
    pthread_mutex_unlock(&balance_mutex);
    cout <<"unlocking balance_mutex" << endl;
    usleep(1000000);  // 1000ms / 1s

    return NULL;
}

int main() {
    // Initialize mutex with default attributes
    pthread_mutex_init(&balance_mutex, NULL);
    
    pthread_t withdraw_thread;
    pthread_t deposit_thread;
    
    int withdraw_amount = 150;
    int deposit_amount = 850;
    
    cout << "Initial Balance: " << bank_balance << endl;
    
    // Create threads
    pthread_create(&withdraw_thread, NULL, withdraw, &withdraw_amount);
    pthread_create(&deposit_thread, NULL, deposit, &deposit_amount);
    
    // Wait for threads to complete
    pthread_join(withdraw_thread, NULL);
    pthread_join(deposit_thread, NULL);
    
    cout << "Final Balance: " << bank_balance << endl;
    
    //clear lock after actions
    pthread_mutex_destroy(&balance_mutex);
    
    return 0;
}
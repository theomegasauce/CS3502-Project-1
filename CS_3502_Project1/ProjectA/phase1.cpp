#include <iostream>
#include <pthread.h>

using namespace std;

//Starting Balance - GLOBAL
int bank_balance = 10000; 

// DEPOSIT FUNCTION
void* deposit(void* arg) {
    int amount = *((int*)arg);
    int temp = bank_balance;

    temp += amount;

    bank_balance = temp;
    cout << "Deposit Amount: " << amount << ",\n" << "New Balance: " << bank_balance << endl;
    return NULL;
}

// WITHDRAW FUNCTION
void* withdraw(void* arg) {
    int amount = *((int*)arg);
    int temp = bank_balance;

    temp -= amount;
    bank_balance = temp;
    cout << "Withdraw Amount: " << amount << ",\n" << "New Balance: " << bank_balance << endl;
    return NULL;
}

int main() {

    pthread_t withdraw_thread;
    pthread_t deposit_thread;

    int withdraw_amount = 150;
    int deposit_amount = 850;

    pthread_create(&withdraw_thread, NULL, withdraw, &withdraw_amount);
    pthread_create(&deposit_thread, NULL, deposit, &deposit_amount);

    pthread_join(withdraw_thread, NULL);
    pthread_join(deposit_thread, NULL);

    cout << "Final Balance: " << bank_balance << endl;
}

#include <iostream>
#include <fstream>
#include <string>
#include <crypt.h>
#include <algorithm>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>

using namespace std;

mutex global;
condition_variable end_find; //условные переменные
bool finded = false;

void findPass(string pass, const string& hash)
{
    static mutex mtx;
    crypt_data *pCryptData = new crypt_data;
    size_t pos = hash.find_last_of('$');
    string hashHead = hash.substr(0,pos);
    do {
        string newHash(crypt_r(pass.data(),hashHead.data(),pCryptData));
        if (newHash == hash) {
            lock_guard<mutex> lock (mtx);
            cout<<"Hash: "<<hash<<endl<<"Pass: "<<pass<<endl;
            finded = true;
            break;
        }
    } while ( next_permutation( pass.begin(), pass.end() ) );
    end_find.notify_all(); //снятие блокировки функции
    delete pCryptData;
}


int main(int argc, char **argv)
{
    string hash;
    int n;
    int k;
    ifstream MyFile;
    string pass[8] = {"123456789", "231456789", "341256789", "451236789", "561234789", "671234589", "781234569", "891234567"};
    cout<<"Укажите количество диапазонов ( 1 ; 2 ; 4 ; 8 ): "<<endl;
    cin >> n;
    thread thread_array[n];
    k = 8/n;
    unique_lock<mutex> lck(global); //блокировка потока
    MyFile.open("/home/student/nikita/lab7/3.txt");
    cout<<"Обработка запроса: "<<endl;
    while (!MyFile.eof()) {
        MyFile>>hash;
        for (int i=0; i<n; i++) {
            thread (findPass, pass[i*k], hash).detach();
        }
    }
    while (!finded)
        end_find.wait(lck);
    return 0;
}

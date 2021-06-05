#include <iostream>
#include <fstream>
#include <string>
#include <crypt.h>
#include <algorithm>
#include <thread>
#include <mutex>
#include <cryptopp/cryptlib.h>
#include <cryptopp/hex.h>
#include <cryptopp/files.h>
#include <cryptopp/aes.h>
using namespace std;

void findPass(string pass, const string& hash)
{
    static mutex mtx;
    crypt_data *pCryptData = new crypt_data;
    size_t pos = hash.find_last_of('$');
    string hashHead = hash.substr(0,pos);
    do {
        string newHash(crypt_r(pass.data(),hashHead.data(),pCryptData));
        if (newHash == hash) {
            mtx.lock();
            cout<<"Hash: "<<hash<<endl<<"Pass: "<<pass<<endl;
            mtx.unlock();
            break;
        }
    } while ( next_permutation( pass.begin(), pass.end() ) );
    delete pCryptData;
}

int main(int argc, char **argv)
{
    string hash;
    string pass = "123456789";
    ifstream f1("/home/student/nikita/lab7/hash.txt");
    thread thread_array[8];
    int k = 0;
    while (getline(f1, hash)) {
        thread_array[k] = thread(findPass, pass, hash);
        k+=1;
    }
    for (int i=0; i<k; i++) {
        if (thread_array[i].joinable()) {
            thread_array[i].join();
        }
    }
    f1.close();
    return 0;
}

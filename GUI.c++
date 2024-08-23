#include <iostream>
#include <string>
#include <unordered_map>
#include <openssl/aes.h>
#include <openssl/rand.h>

using namespace std;

void registerUser(unordered_map<string, string> &accounts, const unsigned char *key, const unsigned char *iv);
bool loginUser(const unordered_map<string, string> &accounts, const unsigned char *key, const unsigned char *iv);
string encryptPassword(const string &password, const unsigned char *key, const unsigned char *iv);
string decryptPassword(const string &encryptedPassword, const unsigned char *key, const unsigned char *iv);
void displayAccounts(const unordered_map<string, string> &accounts);

int main() {
    unordered_map<string, string> accounts; // This is the storage for email and encrypted password

    // AES key and IV setup
    unsigned char key[AES_BLOCK_SIZE * 2]; // 32 bytes for AES-256
    unsigned char iv[AES_BLOCK_SIZE]; // 16 bytes for AES
    RAND_bytes(key, sizeof(key));
    RAND_bytes(iv, sizeof(iv));

    while (true) {
        cout << "1. Register" << endl;
        cout << "2. Login" << endl;
        cout << "3. Display Accounts" << endl;
        cout << "4. Exit" << endl;
        int choice;
        cin >> choice;

        switch (choice) {
            case 1:
                registerUser(accounts, key, iv);
                break;
            case 2:
                if (loginUser(accounts, key, iv)) {
                    cout << "Login successful!" << endl;
                } else {
                    cout << "Login failed!" << endl;
                }
                break;
            case 3:
                displayAccounts(accounts);
                break;
            case 4:
                return 0;
            default:
                cout << "Invalid option. Try again." << endl;
        }
    }
    return 0;
}

void registerUser(unordered_map<string, string> &accounts, const unsigned char *key, const unsigned char *iv) {
    string email, password;
    cout << "Enter email: ";
    cin >> email;
    cout << "Enter password: ";
    cin >> password;

    string encryptedPassword = encryptPassword(password, key, iv);
    accounts[email] = encryptedPassword;

    cout << "You made an account" << endl;
}

bool loginUser(const unordered_map<string, string> &accounts, const unsigned char *key, const unsigned char *iv) {
    string email, password;
    cout << "Enter email: ";
    cin >> email;
    cout << "Enter password: ";
    cin >> password;

    auto it = accounts.find(email);
    if (it != accounts.end()) {
        string encryptedPassword = it->second;
        string decryptedPassword = decryptPassword(encryptedPassword, key, iv);

        if (password == decryptedPassword) {
            return true;
        }
    }
    return false;
}

string encryptPassword(const string &password, const unsigned char *key, const unsigned char *iv) {
    AES_KEY encryptKey;
    AES_set_encrypt_key(key, 256, &encryptKey); // Set the AES key (256 bits)

    int num = 0; // Required for CFB mode
    unsigned char encrypted[password.size()];
    AES_cfb128_encrypt((const unsigned char *)password.c_str(), encrypted, password.size(), &encryptKey, (unsigned char *)iv, &num, AES_ENCRYPT);

    return string((char *)encrypted, password.size());
}

string decryptPassword(const string &encryptedPassword, const unsigned char *key, const unsigned char *iv) {
    AES_KEY decryptKey;
    AES_set_decrypt_key(key, 256, &decryptKey); // Set the AES key (256 bits)

    int num = 0; // Required for CFB mode
    unsigned char decrypted[encryptedPassword.size()];
    AES_cfb128_encrypt((const unsigned char *)encryptedPassword.c_str(), decrypted, encryptedPassword.size(), &decryptKey, (unsigned char *)iv, &num, AES_DECRYPT);

    return string((char *)decrypted, encryptedPassword.size());
}

void displayAccounts(const unordered_map<string, string> &accounts) {
    cout << "Registered Accounts:" << endl;
    for (const auto &entry : accounts) {
        cout << "Email: " << entry.first << ", Encrypted Password: " << entry.second << endl;
    }
}

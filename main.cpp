#include <iostream>
#include <fstream>
#include <windows.h>
#include <conio.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace std;

enum IN {

    // 13 is ASCII for carriage
    // return
    IN_BACK = 8,
    IN_RET = 13

};

// Function that accepts the password
string takePasswdFromUser(
        char sp = '*')
{
    // Stores the password
    string passwd = "";
    char ch_ipt;

    // Until condition is true
    while (true) {

        ch_ipt = getch();

        // if the ch_ipt
        if (ch_ipt == IN::IN_RET) {
            cout << endl;
            return passwd;
        }
        else if (ch_ipt == IN::IN_BACK
                 && passwd.length() != 0) {
            passwd.pop_back();

            // Cout statement is very
            // important as it will erase
            // previously printed character
            cout << "\b \b";

            continue;
        }

            // Without using this, program
            // will crash as \b can't be
            // print in beginning of line
        else if (ch_ipt == IN::IN_BACK
                 && passwd.length() == 0) {
            continue;
        }

        passwd.push_back(ch_ipt);
        cout << sp;
    }
}

class Vig {
public:
    string k;
    explicit Vig(const string& key) {
        this->k = key;
    }
    string encryption(string t) {
        string output;
        for (int i = 0, j = 0; i < t.length(); ++i) {
            char c = t[i];
            output += (c + k[j] - 2) - 'A'; //added 'A' to bring it in range of ASCII alphabet [ 65-90 | A-Z ]
            j = (j + 1) % k.length();
        }
        return output;
    }
    string decryption(string t) {
        string output;
        for (int i = 0, j = 0; i < t.length(); ++i) {
            char c = t[i];
            output += (c - k[j] + 2) + 'A';//added 'A' to bring it in range of ASCII alphabet [ 65-90 | A-Z ]
            j = (j + 1) % k.length();
        }
        return output;
    }
};

int main() {
    string key = "pen2se2ws4rftgyde5r5rfgt6cvtbyuhjafuwehihiugfkggyxursyuvuf";
    string userKey, currentUser;
    bool loggedIn = false;
    cout << "Hello and welcome to the Encrypted Text Editor! Commands: login, new, quit" << endl;
    string input;
    do {
        cout << ">>";
        cin >> input;
        if(!loggedIn) {
            if (input == "new" || input == "login") {
                string username, password;
                cout << "Enter your username:" << endl;
                cin >> username;
                cout << "Enter your password:" << endl;
                password = takePasswdFromUser();
                if (input == "new") {
                    ofstream userFile("users.txt", ios::app);
                    string usernameKey = Vig(key).encryption(username);
                    userFile << username << " " << Vig(usernameKey).encryption(password) << endl;
                    mkdir(username.c_str());
                    userFile.close();
                    cout << "User " << username << " created!" << endl;
                } else if (input == "login") {
                    ifstream userFile("users.txt");
                    string line;
                    bool userNotFound = true;
                    while (getline(userFile, line) && userNotFound) {
                        if (line.substr(0, line.find(' ')) == username) {
                            userNotFound = false;
                            string encryptedPassword = line.substr(line.find(' ') + 1, line.length());
                            string enteredPassword = Vig(Vig(key).encryption(username)).encryption(password);
                            if (encryptedPassword == enteredPassword) {
                                cout << "Logged in to " << username << endl << "Commands: logout, lsdir, new, open, del, quit" << endl ;
                                currentUser = username;
                                userKey = Vig(Vig(username).encryption(password)).encryption(key);
                                loggedIn = true;
                            } else {
                                perror("Incorrect password");
                            }
                        }
                    }
                    if (userNotFound) {
                        perror("User Not Found");
                    }
                }
            }
        } else {
            if(input == "logout") {
                cout << "Logged out of " + currentUser << endl;
                currentUser = "";
                userKey = "";
                loggedIn = false;
            } else if(input == "lsdir") {
                string path = "\\" + currentUser;
                cout << "Files in " + currentUser + "'s directory:" << endl;
                string command = "ls " + currentUser;
                system(command.c_str());
            } else if(input == "new") {
                cout << "File name:" << endl;
                string filename;
                cin >> filename;
                ofstream outfile(currentUser + "\\" + filename + ".txt");
                outfile.close();
                cout << filename << ".txt created" << endl;
            } else if(input == "del") {
                cout << "File name:" << endl;
                string filename;
                cin >> filename;
                if(remove((currentUser + "\\" + filename + ".txt").c_str())!=0) {
                    perror("Error deleting file");
                } else {
                    cout << filename << ".txt deleted" << endl;
                }
            } else if(input == "open") {
                cout << "File name:" << endl;
                string filename;
                cin >> filename;
                filename += ".txt";
                string tempFilename = currentUser + "\\temp" + filename;
                filename = currentUser + "\\" + filename;
                ifstream file(filename);
                string content, line;
                while(getline(file, line)) content+=Vig(userKey).decryption(line) + "\n";
                file.close();

                ofstream tempFile(tempFilename);
                tempFile << content;
                tempFile.close();
                popen(("notepad.exe " + tempFilename).c_str(), "r");
                cout << "S to save and Q to quit" << endl;
                string ans;
                do {
                    cout << "~";
                    cin >> ans;
                    if(ans=="S") {
                        ifstream tempFileIn(tempFilename);
                        content = "";
                        while(getline(tempFileIn, line)) content+=Vig(userKey).encryption(line) + "\n";
                        ofstream finalFile(filename);
                        finalFile << content;
                        tempFileIn.close();
                        finalFile.close();
                    }
                } while(ans != "Q");
                remove(tempFilename.c_str());
            }
        }
    } while(input!="quit");
    return 0;
}

#include <iostream>
#include <chrono>
#include <thread>
using namespace std;

#ifdef _WIN32
#include <conio.h>
#else
#include <unistd.h>
#include <termios.h>
#endif

// Crossing - platform keypress check

bool Key_Pressed(){
    #ifdef _WIN32
    return _kbhit() != 0;
    #else

    struct termios oldt, newt;
    int ch, oldf;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);

    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    
    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF){
        ungetc(ch, stdin);
        return true;
    }
    return false;
    #endif
}

int main() {
    const int Work_Min = 25;
    const int Work_Sec = 5;
    int remain_work_sec = Work_Min * 60;

    bool is_work_phase = true;

    cout << "POMODORO TIMER STARTED (Press Q to Exit)\n" << endl;


    while (true){
        // Display current time
        int mins = remain_work_sec / 60;
        int secs = remain_work_sec % 60;

        //Display the timer.
        cout << (is_work_phase ? "Work Phase: " : "Break Phase: ") 
             << mins << ":" << (secs  < 10 ? "0" : "")<< "secs" << "\r" << flush;


        // Check if a key is pressed
        if(Key_Pressed()){
            char key = getchar();
            if(key == 'q' || key == 'Q'){
                cout << "\nPOMODORO TIMER STOPPED" << endl;
                return 0;
            }
        }

        // Wait 1 second
        this_thread:sleep_for(chrono::seconds(1));
        remain_work_sec--;

        // handle phase transition
        if (remain_work_sec < 0){
            cout << "\n TIME UP! -"
                 << (is_work_phase? " Switching to Break Phase" : "Switching to Work Phase")
                << "\n";
            
            is_work_phase =!is_work_phase;
            remain_work_sec = (is_work_phase? Work_Min : Work_Sec) * 60;

            this_thread::sleep_for(chrono::seconds(2));
        }
    }

    return 0;
}
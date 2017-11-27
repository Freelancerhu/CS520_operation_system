#include <queue>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <algorithm>
using namespace std;

// This is the struct of pcb entry. It contains process number, how many minutes this process
// needs.
struct pcbEntry {
    bool stateRunning = false;
    string processNum;
    int time = 0;
    pcbEntry(string tempNum, int tempTime): processNum(tempNum), time(tempTime) {}
    pcbEntry(const pcbEntry& tempPcb) {
        this->stateRunning = tempPcb.stateRunning;
        this->processNum = tempPcb.processNum;
        this->time = tempPcb.time;
    }
};

// This is the class of fcfs.
class fcfs {
public:
    fcfs(queue<pcbEntry> tempVec) {
        processVec = tempVec;
    }
    void pushNewEntry(pcbEntry tempPcb) {
        processVec.push(tempPcb);
    }

    void arrival() {
        // Process queue store sequence of process execution. First, get the first pcbEntry in
        // the process queue. If cpu is not running, then execute this process. In the currently process,
        // if there is new process needs to be execute, execute this process's arrival().
        // If cpu is running, then push current process into ready queue.
        pcbEntry tempPro = processVec.front();
        processVec.pop();
        if (cpuValid == true) {
            cpuValid = false;
            tempPro.stateRunning = true;
            cout << tempPro.processNum;
            int count = 0;
            for (int i = tempPro.time; i >= 0; i -= 1) {
                if (!processVec.empty()) {
                    pcbEntry tempPro = processVec.front();
                    if (tempPro.time != 0) {
                        arrival();
                    } else {
                        processVec.pop();
                    }
                }
                count++;
                if (count == 10) {
                    cout << "=";
                    count = 0;
                }
            }
            cout << tempPro.processNum;
            this->completion();
        } else {
            readyQue.push(tempPro);
        }
    }

    void completion() {
        // When one process ends, it calls completion function. It checks whether the ready queue is empty.
        // if the ready queue is empty, stops the cpu.
        // if the ready queue is not empty, get the first pcbEntry from ready queue. Then execute this process, when
        // this process executing, checks whether there is any process arrivals. If there is one new process, call new
        // process's arrival function.
        if (!readyQue.empty()) {
            pcbEntry tempPro = readyQue.front();
            readyQue.pop();
            tempPro.stateRunning = true;
            cout << tempPro.processNum;
            int count = 0;
            for (int i = tempPro.time; i >= 0; i -= 1) {
                if (!processVec.empty()) {
                    pcbEntry tempPro = processVec.front();
                    if (tempPro.time != 0) {
                        arrival();
                    } else {
                        processVec.pop();
                    }
                }
                count++;
                if (count == 10) {
                    cout << "=";
                    count = 0;
                }
            }
            cout << tempPro.processNum;
            completion();
        } else {
            cpuValid = true;
        }
    }

private:
    queue<pcbEntry> processVec;  // queue of process
    queue<pcbEntry> readyQue;    // queue of ready process
    bool cpuValid = true;        // the state of cpu
};


// This is the class of sjf.
class sjf {
public:
    sjf(queue<pcbEntry> tempVec) {
        processVec = tempVec;
    }
    // Push process into process queue directly.
    void pushNewEntry(pcbEntry tempPcb) {
        processVec.push(tempPcb);
    }
    // Because there are many processes want to be executed at the same time, we push them into ready queue directly.
    void pushNewReady(pcbEntry tempPcb) {
        readyQue.push(tempPcb);
    }

    void arrival() {
        //First, check whether the process queue is empty. If the process queue is empty, Then check whether the ready
        // queue is or is not empty, sort the ready queue with their burst time, get the first pcbEntry from the ready
        // queue. If the process queue is not empty, get the first pcbEntry from the process queue.
        pcbEntry tempPro("0", 0);
        if (processVec.empty()) {
            vector<pcbEntry> tempVec;
            while (!readyQue.empty()) {
                tempVec.push_back(readyQue.front());
                readyQue.pop();
            }
            sort(tempVec.begin(), tempVec.end(), [](const pcbEntry& a, const pcbEntry& b){
                                                            return a.time < b.time;});
            for (auto i : tempVec) {
                readyQue.push(i);
            }
            tempPro = readyQue.front();
            readyQue.pop();
        } else {
            tempPro = processVec.front();
            processVec.pop();
        }
        // If cpu is not running, then execute this process. In the currently process,
        // If there is new process needs to be execute, execute this process's arrival().
        // If cpu is running, then push current process into ready queue. Sorts the ready queue.
        if (cpuValid == true) {
            cpuValid = false;
            tempPro.stateRunning = true;
            cout << tempPro.processNum;
            int count = 0;
            for (int i = tempPro.time; i >= 0; i -= 1) {
                if (!processVec.empty()) {
                    pcbEntry tempPro = processVec.front();
                    if (tempPro.time != 0) {
                        arrival();
                    } else {
                        processVec.pop();
                    }
                }
                count++;
                if (count == 10) {
                    cout << "=";
                    count = 0;
                }
            }
            cout << tempPro.processNum;
            this->completion();
        } else {
            readyQue.push(tempPro);
            vector<pcbEntry> tempVec;
            while (!readyQue.empty()) {
                tempVec.push_back(readyQue.front());
                readyQue.pop();
            }
            sort(tempVec.begin(), tempVec.end(), [](const pcbEntry& a, const pcbEntry& b){
                                                            return a.time < b.time;});
            for (auto i : tempVec) {
                readyQue.push(i);
            }
        }
    }

    void completion() {
        // When one process ends, it calls completion function. It checks whether the ready queue is empty.
        // if the ready queue is empty, stops the cpu.
        // if the ready queue is not empty, get the first pcbEntry from ready queue. Then execute this process, when
        // this process executing, checks whether there is any process arrivals. If there is one new process, call new
        // process's arrival function.
        if (!readyQue.empty()) {
            pcbEntry tempPro = readyQue.front();
            readyQue.pop();
            tempPro.stateRunning = true;
            cout << tempPro.processNum;
            int count = 0;
            for (int i = tempPro.time; i >= 0; i -= 1) {
                if (!processVec.empty()) {
                    pcbEntry tempPro = processVec.front();
                    if (tempPro.time != 0) {
                        arrival();
                    } else {
                        processVec.pop();
                    }
                }
                count++;
                if (count == 10) {
                    count = 0;
                    cout << "=";
                }
            }
            cout << tempPro.processNum;
            completion();
        } else {
            cpuValid = true;
        }
    }

private:
    queue<pcbEntry> processVec;
    queue<pcbEntry> readyQue;
    bool cpuValid = true;
};


int main () {
//    p1   0.0   8
//    p2   0.4   4
//    p3   1.0   1
    pcbEntry nullPcb("0", 0);
    pcbEntry p11("P1", 80);
    pcbEntry p12("P2", 40);
    pcbEntry p13("P3", 10);
    queue<pcbEntry> fcfsQueue1;
    // Push pcbEntry into process queue.
    for (int i = 0; i < 15; ++i) {
        if (i == 0) {
            fcfsQueue1.push(p11);
        } else if (i == 4) {
            fcfsQueue1.push(p12);
        } else if (i == 10) {
            fcfsQueue1.push(p13);
        } else {
            fcfsQueue1.push(nullPcb);
        }
    }
    cout << "one '=' means one second." << endl;
    cout << "5.3 FCFS " << endl;
    fcfs fcfsPro1(fcfsQueue1);
    fcfsPro1.arrival();
    cout << endl;
    cout << "5.3 SJF " << endl;
    sjf sjfPro1(fcfsQueue1);
    sjfPro1.arrival();
    cout << endl;
    //pro tim pri
    //p1  10  3
    //p2  1   1
    //p3  2   3
    //p4  1   4
    //p5  5   2
    pcbEntry p21("P1", 100);
    pcbEntry p22("P2", 10);
    pcbEntry p23("P3", 20);
    pcbEntry p24("P4", 10);
    pcbEntry p25("P5", 50);
    // Push pcbEntry into process queue.
    queue<pcbEntry> fcfsQueue2;
    fcfsQueue2.push(p21);
    fcfsQueue2.push(p22);
    fcfsQueue2.push(p23);
    fcfsQueue2.push(p24);
    fcfsQueue2.push(p25);
    fcfs fcfsPro2(fcfsQueue2);
    cout << "5.12 FCFS " << endl;
    fcfsPro2.arrival();
    cout << endl;
    queue<pcbEntry> fcfsQueue3;
    sjf sjfPro2(fcfsQueue3);
    // Because 5 processes come at same time, we push them into ready queue directly.
    sjfPro2.pushNewReady(p21);
    sjfPro2.pushNewReady(p22);
    sjfPro2.pushNewReady(p23);
    sjfPro2.pushNewReady(p24);
    sjfPro2.pushNewReady(p25);
    cout << "5.12 SJF " << endl;
    sjfPro2.arrival();
    cout << endl;
    return 0;
}

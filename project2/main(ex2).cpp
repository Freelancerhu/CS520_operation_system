/*************************************************
Author: Shengjie Hu
Date：11/11/2017
**************************************************/
#include <iostream>
#include <queue>
#include <vector>
#include <algorithm>
#include <cmath>
#include <fstream>
using namespace std;

// the function of Exponentially Distributed Pseudo-Random Numbers.
double expoRandom (int &sd) {
    double x = 0;
    x = - log ((double(sd) + 1) / 65536);
    sd = (25173 * sd + 13849) % 65536;
    return x;
}

// a = 25173, c = 13849, m = 65536.
// Xn+1 = (aXn + c) mod m:  Yn+1 = Xn+1 / m
// Random execution time with lengths uniformly distributed between 2 and 4 minutes
double unifRandom (int &sd) {
    double x = 0;
    x = double(sd) / 65536;
    sd = (25173 * sd + 13849) % 65536;
    return x;
}

// the struct of process
struct process {
    string processName;
    queue<int> ioRequest;
    int remainTime = 0;
    vector<int> historyRuntime;
    int oriGuessTime = 0;
    int guessTime;
    int brustCishu = -1;

    int excutionTime = 0;
    int meanInter = 0;
    int totalTime = 0;
    process(string tempName, int tempExcution, int tempMeanInter = 0) :
        processName(tempName), excutionTime(tempExcution), meanInter(tempMeanInter){
        remainTime = excutionTime;
        int sd = 1000;
        int count = 0;
        for (int i = 0; i < excutionTime; ) {
            int temp = expoRandom(sd) * meanInter;
            totalTime += temp;
            i += temp;
            count++;
            ioRequest.push(temp);
        }
        totalTime = totalTime / count;
        oriGuessTime = ioRequest.front();
        guessTime = rand() % 100;
        cout << "processName = " << tempName << endl;
        cout << "totalTime = " << totalTime << endl;
        cout << "oriGuessTime = " << oriGuessTime << endl;
        cout << "guessTime = " << guessTime << endl;
        cout << "count = " << count << endl;
        cout << "excutionTime / meanInter = " << excutionTime / meanInter << endl;
    }

    process(const process& tempProcess) {
        processName = tempProcess.processName;
        ioRequest = tempProcess.ioRequest;
        remainTime = tempProcess.remainTime;
        historyRuntime = tempProcess.historyRuntime;
        oriGuessTime = tempProcess.oriGuessTime;
        guessTime = tempProcess.guessTime;
        brustCishu = tempProcess.brustCishu;
        excutionTime = tempProcess.excutionTime;
        meanInter = tempProcess.meanInter;
    }

    int getRemainTime() {
        return remainTime;
    }
};

// This is the class of fcfs.
class fcfs {
public:
    fcfs(queue<process> tempVec) {
        processQue = tempVec;
    }
    void pushNewPro(process tempPro) {
        processQue.push(tempPro);
    }

    void arrival() {
        // Process queue stores sequence of process execution. First, get the first process in
        // the process queue. If its ioRequest queue is empty, the running time is its remaining time.
        // If its ioRequest queue is not empty, the running time is its io request time.
        process tempPro = processQue.front();
        processQue.pop();
        int tempTime = 0;
        int tempWaitingTime = 0;
        if (!tempPro.ioRequest.empty()) {
            tempTime = tempPro.ioRequest.front();
            tempPro.ioRequest.pop();
        } else {
            tempTime = tempPro.getRemainTime();
        }
        for (int i = tempTime; i >= 0; --i) {
            ioChannelTime++;
            cpuRuntime++;
            sumRuntime++;
            tempWaitingTime++;
            tempPro.remainTime--;
            // if the time of io reach 60 seconds, it means one io request finished, then pop the process which
            // is the first process in the ioRequest queue and push it into proess queue.
            if (ioChannelTime == 60) {
                ioChannelTime = 0;
                if (!ioQue.empty()) {
                    processQue.push(ioQue.front());
                    ioQue.pop();
                }
            }
            // if reamining time reach 0, it means this process finished, so break the loop.
            if (tempPro.remainTime == 0) {
                turnarountTime += sumRuntime;
                break;
            }
        }
        waitingTime += tempWaitingTime * processQue.size();
        // // if reamining time is not 0, then push this process into ioRequest queue.
        if (tempPro.getRemainTime() != 0) {
            ioQue.push(tempPro);
        }
    }

    void completion() {
        // When one process ends, it calls completion function. It checks whether the ready queue is empty.
        // if the ready queue is empty, but io queue is not empty, wait for the io process. Once the process finish
        // its io, push that process into process queue.
        if (processQue.empty() && !ioQue.empty()) {
            while (ioChannelTime != 60) {
                ioChannelTime++;
                sumRuntime++;
            }
            ioChannelTime = 0;
            processQue.push(ioQue.front());
            ioQue.pop();
            completion();
        }

    }
    void start() {
        while (!processQue.empty() || !ioQue.empty()) {
            arrival();
            completion();
        }
    }

    //check whether io queue is or is not empty.
    void ioQueC() {
        if (ioQue.empty()) cout << "empty()" << endl;
        else cout << "not empty() = " << ioQue.size() << endl;
    }
    // check whether process queue is or is not empty.
    void processQueC() {
        if (processQue.empty()) cout << "empty()" << endl;
        else cout << "not empty() = " << processQue.size() << endl;
    }

    // show cpu utilization.
    void cpuUtilization() {
        cout << endl;
        cout << "waitingTime = " << waitingTime<< endl;
        cout << "turnaroundTime = " << turnarountTime << endl;
        cout << "cpuRuntime = " << cpuRuntime << " sumRuntime = " << sumRuntime << endl;
        cout << "cpuUtilization = " << (double)cpuRuntime / (double)sumRuntime * 100 << "%" << endl;
    }

private:
    queue<process> processQue;  // queue of process
    queue<process> ioQue;       // queue of io process
    int sumRuntime = 0;         // sum of running time
    int cpuRuntime = 0;         // total running time
    int ioChannelTime = 0;      // time of io
    int waitingTime = 0;        // sum of waiting time
    int turnarountTime = 0;     // time of turn around
};

// This is the class of sjf.
class sjf {
public:
    sjf(queue<process> tempVec) {
        processQue = tempVec;
    }
    void pushNewPro(process tempPro) {
        processQue.push(tempPro);
    }

    void arrival() {
        // Process queue stores sequence of process execution. First, get the first process in
        // the process queue. If its ioRequest queue is empty, the running time is its remaining time.
        // If its ioRequest queue is not empty, the running time is its io request time.
        process tempPro = processQue.front();
        processQue.pop();
        int tempTime = 0;
        if (!tempPro.ioRequest.empty()) {
            tempTime = tempPro.ioRequest.front();
            tempPro.ioRequest.pop();
        } else {
            tempTime = tempPro.getRemainTime();
        }
        int tempWaitingTime = 0;
        for (int i = tempTime; i >= 0; --i) {
            ioChannelTime++;
            cpuRuntime++;
            sumRuntime++;
            tempWaitingTime++;
            tempPro.remainTime--;
            // if the time of io reach 60 seconds, it means one io request finished, then pop the process which
            // is the first process in the ioRequest queue and push it into proess queue.
            if (ioChannelTime == 60) {
                ioChannelTime = 0;
                if (!ioQue.empty()) {
                    processQue.push(ioQue.front());
                    ioQue.pop();
                }
            }
            // if reamining time reach 0, it means this process finished, so break the loop.
            if (tempPro.remainTime == 0) {
                turnaroundTime += sumRuntime;
                break;
            }
        }
        waitingTime += tempWaitingTime * processQue.size();
        // calculate the next running time.
        double index = 1/3;
        tempPro.brustCishu++;
        tempPro.historyRuntime.push_back(tempTime);
        int tempI = 0;
        for (int i = 0; i <= tempPro.brustCishu; ++i) {
            tempI += (tempPro.historyRuntime[i]*pow((1 - index), tempPro.brustCishu - i));
        }
        tempPro.guessTime = pow((1- index), tempPro.brustCishu + 1) * tempPro.oriGuessTime + index * tempI;
        // // if reamining time is not 0, then push this process into ioRequest queue.
        if (tempPro.getRemainTime() != 0) {
            ioQue.push(tempPro);
        }
    }

    void completion() {
        // When one process ends, it calls completion function. It checks whether the ready queue is empty.
        // if the ready queue is empty, but io queue is not empty, wait for the io process. Once the process finish
        // its io, push that process into process queue.
        if (processQue.empty() && !ioQue.empty()) {
            while (ioChannelTime != 60) {
                ioChannelTime++;
                sumRuntime++;
            }
            ioChannelTime = 0;
            processQue.push(ioQue.front());
            ioQue.pop();
            completion();
        }
    }

    void start() {
        while (!processQue.empty() || !ioQue.empty()) {
            shuffleProcessQue();
            arrival();
            completion();
            shuffleProcessQue();
        }
    }
    // shuffle the process queue, guarantee that the first process is the process with the shortest running time
    void shuffleProcessQue() {
        vector<process> tempVec;
        while (!processQue.empty()) {
            tempVec.push_back(processQue.front());
            processQue.pop();
        }
        sort(tempVec.begin(), tempVec.end(), [](const process& a, const process& b){
                                                        return a.guessTime < b.guessTime;});
        for (auto i : tempVec) {
            processQue.push(i);
        }
    }

    //check whether io queue is or is not empty.
    void ioQueC() {
        if (ioQue.empty()) cout << "empty()" << endl;
        else cout << "not empty() = " << ioQue.size() << endl;
    }
    // check whether process queue is or is not empty.
    void processQueC() {
        if (processQue.empty()) cout << "empty()" << endl;
        else cout << "not empty() = " << processQue.size() << endl;
    }

    // show cpu utilization.
    void cpuUtilization() {
        cout << endl;
        cout << "waitingTime = " << waitingTime << endl;
        cout << "turnaroundTime = " << turnaroundTime << endl;
        cout << "cpuRuntime = " << cpuRuntime << " sumRuntime = " << sumRuntime << endl;
        cout << "cpuUtilization = " << (double)cpuRuntime / (double)sumRuntime * 100 << "%" << endl;
    }

private:
    queue<process> processQue;  // queue of process
    queue<process> ioQue;       // queue of io process
    int sumRuntime = 0;         // sum of running time
    int cpuRuntime = 0;         // total running time
    int ioChannelTime = 0;      // time of io
    int waitingTime = 0;        // sum of waiting time
    int turnaroundTime = 0;     // turnaround time
};


// This is the class of fcfs.
class rr {
public:
    rr(queue<process> tempVec) {
        processQue = tempVec;
    }
    void pushNewPro(process tempPro) {
        processQue.push(tempPro);
    }

    void arrival() {
        // Process queue stores sequence of process execution. First, get the first process in
        // the process queue. If its ioRequest queue is empty, the running time is its remaining time.
        // If its ioRequest queue is not empty, the running time is its io request time.
        process tempPro = processQue.front();
        processQue.pop();
        int tempTime = 0;
        int tempWaitingTime = 0;
        int tempQuantum = quantum;
        if (!tempPro.ioRequest.empty()) {
            tempTime = tempPro.ioRequest.front();
            tempPro.ioRequest.pop();
        } else {
            tempTime = tempPro.getRemainTime();
        }
        int flag = 1;
        for (int i = tempTime; i >= 0; --i) {
            ioChannelTime++;
            cpuRuntime++;
            sumRuntime++;
            tempWaitingTime++;
            tempPro.remainTime--;
            tempQuantum--;
            // if reamining time reach 0, it means this process finished, so break the loop.
            if (tempPro.remainTime == 0) {
                turnarountTime += sumRuntime;
                break;
            }
            // if the time of io reach 60 seconds, it means one io request finished, then pop the process which
            // is the first process in the ioRequest queue and push it into proess queue.
            if (ioChannelTime == 60) {
                ioChannelTime = 0;
                if (!ioQue.empty()) {
                    processQue.push(ioQue.front());
                    ioQue.pop();
                }
            }
            // the process reach the quantum, so update it ioRequest queue.
            if (tempQuantum == 0) {
                tempQuantum = quantum;
                queue<int> tempInnerQue;
                while (!tempPro.ioRequest.empty()) {
                    tempInnerQue.push(tempPro.ioRequest.front());
                    tempPro.ioRequest.pop();
                }
                tempPro.ioRequest.push(i);
                while (!tempInnerQue.empty()) {
                    tempPro.ioRequest.push(tempInnerQue.front());
                    tempInnerQue.pop();
                }
                if (tempPro.getRemainTime() != 0) {
                    processQue.push(tempPro);
                }
                flag = 0;
                break;
            }
        }
        if (flag == 1) {
            waitingTime += tempWaitingTime * processQue.size();
        } else {
            waitingTime += tempWaitingTime * (processQue.size() - 1);
        }
        // if it is not reach the quantum and its remaining time is not zero, push it into io queue.
        if (flag == 1 && tempPro.getRemainTime() != 0) {
            ioQue.push(tempPro);
        }
    }

    void completion() {
        // When one process ends, it calls completion function. It checks whether the ready queue is empty.
        // if the ready queue is empty, but io queue is not empty, wait for the io process. Once the process finish
        // its io, push that process into process queue.
        if (processQue.empty() && !ioQue.empty()) {
            while (ioChannelTime != 60) {
                ioChannelTime++;
                sumRuntime++;
            }
            ioChannelTime = 0;
            processQue.push(ioQue.front());
            ioQue.pop();
            completion();
        }

    }
    void start() {
        while (!processQue.empty() || !ioQue.empty()) {
            arrival();
            completion();
        }
    }

    //check whether io queue is or is not empty.
    void ioQueC() {
        if (ioQue.empty()) cout << "empty()" << endl;
        else cout << "not empty() = " << ioQue.size() << endl;
    }
    // check whether process queue is or is not empty.
    void processQueC() {
        if (processQue.empty()) cout << "empty()" << endl;
        else cout << "not empty() = " << processQue.size() << endl;
    }

    // show cpu utilization.
    void cpuUtilization() {
        cout << endl;
        cout << "quantum = " << quantum << endl;
        cout << "waitingTime = " << waitingTime<< endl;
        cout << "turnaroundTime = " << turnarountTime << endl;
        cout << "cpuRuntime = " << cpuRuntime << "  " << "sumRuntime = " << sumRuntime << endl;
        cout << (double)cpuRuntime / (double)sumRuntime * 100 << "%" << endl;
    }
    // set the value of quantum.
    void setQuantum(int i) {
        quantum = i;
    }

private:
    queue<process> processQue;  // queue of process
    queue<process> ioQue;       // queue of io process
    int sumRuntime = 0;         // sum of runtime
    int cpuRuntime = 0;         // total running time
    int ioChannelTime = 0;      // time of io
    int waitingTime = 0;        // sum of waiting time
    int turnarountTime = 0;     // time of turnaound
    int quantum = 5;            // quantum
};




int main() {
    int sd = 1000;
    // The mean inter-I/O intervals for the jobs are respectively 30ms, 35ms, 40ms, 45ms, 50ms, 55ms,
    // 60ms, 65ms, 70ms, and 75ms.
    vector<int> meanInterVec = {30, 35, 40, 45, 50, 55, 60, 65, 70, 75};
    // Create 10 jobs of random execution time with lengths uniformly distributed between 2 and 4 minutes
    // (use the uniform distribution method for random number generation).
    vector<int> tempExcutionVec;
    for (int i = 0; i < 10; ++i) {
        int tempE = (unifRandom(sd) * 60 * 2 + 60 * 2) * 1000;
        tempExcutionVec.push_back(tempE);
    }
    process process0("p0", tempExcutionVec[0], meanInterVec[0]);
    process process1("p1", tempExcutionVec[1], meanInterVec[1]);
    process process2("p2", tempExcutionVec[2], meanInterVec[2]);
    process process3("p3", tempExcutionVec[3], meanInterVec[3]);
    process process4("p4", tempExcutionVec[4], meanInterVec[4]);
    process process5("p5", tempExcutionVec[5], meanInterVec[5]);
    process process6("p6", tempExcutionVec[6], meanInterVec[6]);
    process process7("p7", tempExcutionVec[7], meanInterVec[7]);
    process process8("p8", tempExcutionVec[8], meanInterVec[8]);
    process process9("p9", tempExcutionVec[9], meanInterVec[9]);
    queue<process> mainProcessQue;

    // fcfs CPU algorithm
    fcfs fcfsCpu(mainProcessQue);
    fcfsCpu.pushNewPro(process0);
    fcfsCpu.pushNewPro(process1);
    fcfsCpu.pushNewPro(process2);
    fcfsCpu.pushNewPro(process3);
    fcfsCpu.pushNewPro(process4);
    fcfsCpu.pushNewPro(process5);
    fcfsCpu.pushNewPro(process6);
    fcfsCpu.pushNewPro(process7);
    fcfsCpu.pushNewPro(process8);
    fcfsCpu.pushNewPro(process9);
    cout << "fcfs Cpu : " << endl;
    fcfsCpu.start();
    fcfsCpu.cpuUtilization();
    cout << "end" << endl;
    fcfsCpu.ioQueC();
    fcfsCpu.processQueC();

    // sjf CPU algorithm
    sjf sjfCpu(mainProcessQue);
    sjfCpu.pushNewPro(process0);
    sjfCpu.pushNewPro(process1);
    sjfCpu.pushNewPro(process2);
    sjfCpu.pushNewPro(process3);
    sjfCpu.pushNewPro(process4);
    sjfCpu.pushNewPro(process5);
    sjfCpu.pushNewPro(process6);
    sjfCpu.pushNewPro(process7);
    sjfCpu.pushNewPro(process8);
    sjfCpu.pushNewPro(process9);
    cout << "sjf Cpu : " << endl;
    sjfCpu.start();
    sjfCpu.cpuUtilization();
    cout << "end" << endl;
    sjfCpu.ioQueC();
    sjfCpu.processQueC();


    // rr CPU algorithm
    ifstream infile("data.txt");
    vector<int> quantumVec;
    while (!infile.eof()) {
        int i = 0;
        infile >> i;
        quantumVec.push_back(i);
    }
    cout << "rr Cpu : " << endl;
    for (size_t i = 0; i < quantumVec.size(); ++i) {
        rr rrCpu(mainProcessQue);
        rrCpu.setQuantum(quantumVec[i]);
        rrCpu.pushNewPro(process0);
        rrCpu.pushNewPro(process1);
        rrCpu.pushNewPro(process2);
        rrCpu.pushNewPro(process3);
        rrCpu.pushNewPro(process4);
        rrCpu.pushNewPro(process5);
        rrCpu.pushNewPro(process6);
        rrCpu.pushNewPro(process7);
        rrCpu.pushNewPro(process8);
        rrCpu.pushNewPro(process9);
        rrCpu.start();
        rrCpu.cpuUtilization();
        cout << "end" << endl;
        rrCpu.ioQueC();
        rrCpu.processQueC();
    }
    return 0;
}


/*************************************************
Author: Shengjie Hu
Date：10/05/2017
**************************************************/
#include <iostream>
#include <vector>
#include <queue>
#include <set>
#include <cmath>
#include <algorithm>
using namespace std;
/*
 * First, I define a Event class. which contains information of waiting queue of each bus stop, the state of event, current time,
 * the number of bus and the number of stop.
 */
class Event {
private:
    static vector<queue<int>> busStop;  // Contains the waiting queue of each bus stop. It is static because every event share it.
    char state = 'a';   // Stores state(person, arrival and boarder) of each event.
    int curTime = 0;    // Stores current time.
    int noOfBus = 0;    // Stores number of bus.
    int noOfStop = 0;   // Stores number of bus station.
public:
    // constructor of Event class.
    Event (char eventName, int Time, int Bus, int Stop) {
        state = eventName;
        curTime = Time;
        noOfBus = Bus;
        noOfStop = Stop;
    }
    // Set the number of bus stop to i.
    void setBusStopNum(int t) {
        for (int i = 0; i < t; ++i) {
            queue<int> a;
            busStop.push_back(a);
        }
    }
    // Print out the length of waiting queue of each stop.
    void showData() {
        for (int i = 0; i < busStop.size(); ++i) {
            cout << i << " = " << busStop[i].size() << endl;
        }
    }
    // checks whether the waiting queue of t bus stop is empty.
    bool checkQueue (int t) {
        return busStop[t].empty();
    }
    // adds one person to i bus stop.
    void addPerson(int i) {
        busStop[i].push(1);
    }
    // one person boards, so minus on person from i bus stop.
    void minusPerson(int i) {
        busStop[i].pop();
    }
    // returns the state of event.
    char getState () const {
        return state;
    }
    // returns time of event.
    int getCurTime () const {
        return curTime;
    }
    // returns the number of bus of event.
    int getBusNumber () const {
        return noOfBus;
    }
    //returns the number of bus stop of event.
    int getStopNumber () const {
        return noOfStop;
    }

};
vector<queue<int>> Event::busStop;
// the function of Exponentially Distributed Pseudo-Random Numbers.
double random (int &sd) {
    double x = 0;
    x = - log ((double(sd) + 1) / 65536);
    sd = (25173 * sd + 13849) % 65536;
    return x;
}
/*
 * This is the main body of program. First read the number of buses, the number of stops, the driving time between stops,
 * the boarding time, the stop time, and the mean arrival rate from an initialization file. Then, Start with the buses
 * distributed uniformly along the route (by generating appropriate arrival events) and generating one person event for each stop.
 */
int main() {
    // stores Events.
    vector<Event> mainSet;
    int seed = 1000;
    int driveTime = 300;
    int numOfStop = 15;
    int numOfBus = 5;
    int meanArrivalRate = 2;
    int boardingTime = 3;
    int timeToStop = 9999;
    int currentTimeIs = 0;
    // the buses distributed uniformly along the route (by generating appropriate arrival events)
    for (int i = 0, j = 0; i < numOfBus; ++i, j += 3) {
        Event busA('a', currentTimeIs, i, j);
        mainSet.push_back(busA);
    }
    // set the number of bus stop
    Event initEvent('p', currentTimeIs, -1, 1);
    initEvent.setBusStopNum(numOfStop);
    // generates one person Event for each stop
    for (int i = 0; i < numOfStop; i++) {
        Event personA('p', currentTimeIs, -1, i);
        mainSet.push_back(personA);
    }
    /*
     * every time sorts the Event vector, the event which has the smallest time comes out first.
     * Records state, clock(time), bus number and stop number of this event.
     * if its state is 'p'(person), adds one person to its bus stop, then define a new 'p' Event object whose time is
     * clock + (mean_inter-arrival_rate) * random (exponential) at the same bu stop. Pushes back to Event vector.
     * if its state is 'a'(arrival), check waiting queue of its stop. if the queue is empty, generate the 'a' Event at
     * the next bus stop at clock + drive_time. If the queue is not empty, generate the 'b' Event (at clock).
     * if its state is 'b'(boarding), check witing queue of its stop. if the queue is empty (i.e., the last person boarded),
     * generate the 'a' Event at the next bus stop at clock + drive_time. If the queue is not empty, generate
     * the 'b' Event (at clock + boarding_time)
     * when current time is greater than stop time, program terminates.
     */
    do {
        sort(mainSet.begin(),mainSet.end(), [](const Event& a, const Event& b) {
                                return a.getCurTime() < b.getCurTime();});
        Event nextEvent = mainSet[0];
        mainSet.erase(mainSet.begin());
        char state = nextEvent.getState();
        int clock = nextEvent.getCurTime();
        currentTimeIs = clock;
        int bus = nextEvent.getBusNumber();
        int stop = nextEvent.getStopNumber();
        switch (state) {
            case 'p' : {
                nextEvent.addPerson(stop);
                double tempTime = random(seed) / meanArrivalRate * 60;
                Event tempEvent('p', clock + tempTime, bus, stop);
                mainSet.push_back(tempEvent);
                break;
            }
            case 'a' : {
                cout <<"b = " << bus << " s = " << stop << endl;
                if (nextEvent.checkQueue(stop)) {
                    if (stop == 14) stop = 0;
                    else stop += 1;
                    Event tempEvent('a', clock + driveTime, bus, stop);
                    mainSet.push_back(tempEvent);
                } else {
                    Event tempEvent('b', clock, bus, stop);
                    mainSet.push_back(tempEvent);
                }
                break;
            }
            case 'b' : {
                if (nextEvent.checkQueue(stop)) {
                    if (stop == 14) stop = 0;
                    else stop += 1;
                    Event tempEvent('a', clock + driveTime, bus, stop);
                    mainSet.push_back(tempEvent);
                } else {
                    nextEvent.minusPerson(stop);
                    Event tempEvent('b', clock + boardingTime, bus, stop);
                    mainSet.push_back(tempEvent);
                }

                break;
            }
        }

    } while (currentTimeIs <= timeToStop);

    Event ttt('p', currentTimeIs, -1, 1);
    ttt.showData();


    return 0;

}

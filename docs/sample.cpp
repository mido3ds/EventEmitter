#include <iostream>
#include "EventEmitter.h"
using namespace std;

struct Event 
{
    std::string name;
};

// make our class that inherits from EventEmitter
class Car : public EventEmitter<Event> // you should provide your own Event Class and pass it in the template
{
public:
    int x, y; // just for now
};

void myEggFuncyFunction()
{
    cout << "this is an egg function, don\'t mess with it\n";
}

int main(int argc, char* argv[]) 
{
    Car c;
    c.x = 5;
    c.y = 8;

/* assign listeners to events */

    // it could take void parametres
    c.onEvent("move", [&c]() { // notice how to access car members
        cout << "first move listener   " << "car is moving now\n";
        cout << "car x corrdinate is " << c.x << endl; // notice how we accessed car member [x]

        cout << ".. listener ended" << endl;
    });

    // or take an event object to describe its details
    c.onEvent("move", [](const Event& evt) { // it must be [const]
        cout << "second move listener   " << "car is still moving now\n";
        cout << evt.name << endl;

        cout << ".. listener ended" << endl;
    });

    c.onEvent("move", myEggFuncyFunction); // notice you can pass ordinary functions

    c.onEvent("stop", []() { // this would be called only if emit stop
        cout << "stopped\n";
    });

    c.emitEvent("move", {"move event info"}); // now all move listeners will be called in order (synchronous)




/* copy listeners */

    auto lis = c.listeners("move"); // this is acopy vector of listeners
    Event e{ "foo-bar-event" }; // we should make out event object so we could pass it
    for (auto l : lis)
        l(e);



/* remove listeners */

    c.removeAllListeners("move");

    c.emitEvent("move", {"move event info"}); // this would call nothing

    return 0;
}
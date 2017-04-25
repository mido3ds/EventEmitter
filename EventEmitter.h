/*
    EventEmitter in nodeJS way, for c++
    Mahmoud Adas, mido3ds@gmail.com
*/
#ifndef __EventEmitter_h__
#define __EventEmitter_h__

#include <unordered_map>
#include <vector>
#include <string>

struct Event 
{
    std::string name;
};

template<class T_EventClass = Event>
class EventEmitter 
{
protected:
    std::unordered_map<std::string, std::vector<std::function<void(const T_EventClass&)>> > events;

public:
    void onEvent(std::string eventName, std::function<void(const T_EventClass&)> listener) 
    {
        events[eventName].push_back(listener);
    }

    void onEvent(std::string eventName, std::function<void()> listener) 
    {
        events[eventName].push_back([&listener](const T_EventClass& evt) {
            listener();
        });
    }

    void emitEvent(std::string eventName) 
    {
        if (events.find(eventName) != events.end()) 
        {
            Event evt = { eventName };
            for (auto& func : events[eventName])
                func(evt);
        }
    }

    void removeAllListeners(std::string eventName)
    {
        auto eventItr = events.find(eventName);
        if (eventItr != events.end())
            events.erase(eventItr);
    }

    std::vector<std::function<void(const T_EventClass&)>> getListeners(std::string eventName) 
    {
        return std::vector<std::function<void(const T_EventClass&)>>(events[eventName]);
    }
};

#endif  /* __EventEmitter_h__ */
/*
    EventEmitter in nodeJS flavor, for c++11
    Mahmoud Adas, mido3ds@gmail.com
*/
#ifndef __EventEmitter_h__
#define __EventEmitter_h__

#include <unordered_map>
#include <vector>
#include <string>

using std::function;
using std::vector;
using std::unordered_map;

/* Inheret from it to add Event-Capturing-ability to your class 
 * @T_EventClass: class that holds event info when emitted, sent to function that should be run
 * @T_EventKeyClass: class/type of key that distinguishes the event, default std::string, must be hashable
 */
template<typename T_EventClass, typename T_EventKeyClass = std::string>
class EventEmitter 
{
    using EventFunction_t = function<void(const T_EventClass&)>;

protected:
    unordered_map<T_EventKeyClass, vector<EventFunction_t>> functions_bundle; // to store functions with its keys

public:
    /* store this listener function with this eventKey
     * @eventKey: key to event, if duplicated the function will be added to the previous
     * @listener: function/lambda that should be called when event is emitted
     */
    void onEvent(T_EventKeyClass eventKey, EventFunction_t listener) 
    {
        functions_bundle[eventKey].push_back(listener);
    }

    /* overload, function could take no Event
     * @eventKey: key to event, if duplicated the function will be added to the previous
     * @listener: function/lambda that should be called when event is emitted
     */
    void onEvent(T_EventKeyClass eventKey, function<void()> listener) 
    {
        functions_bundle[eventKey].push_back([&listener](const T_EventClass& evt) {
            listener();
        });
    }

    /* fire the event resulting in calling all functions in order, if event not found, nothing happens
     * @eventKey: of event to fire
     * @eventInfo: event object to send to functions to be called
     */
    void emitEvent(T_EventKeyClass eventKey, const T_EventClass& eventInfo) 
    {
        if (functions_bundle.find(eventKey) != functions_bundle.end()) 
        {
            for (auto& func : functions_bundle[eventKey])
                func(eventInfo);
        }
    }

    /* erase all function that has been bound to given key
     * @eventKey: to remove its functions if found
     */
    void removeAllListeners(T_EventKeyClass eventKey)
    {
        auto eventItr = functions_bundle.find(eventKey);
        if (eventItr != functions_bundle.end())
            functions_bundle.erase(eventItr);
    }

    /* @eventKey: to get its functions
     */
    vector<EventFunction_t> getListeners(T_EventKeyClass eventKey) 
    {
        return vector<EventFunction_t>(functions_bundle[eventKey]);
    }
};

#endif  /* __EventEmitter_h__ */
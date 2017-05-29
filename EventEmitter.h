/*
    EventEmitter in nodeJS flavor, for c++11
    Mahmoud Adas, mido3ds@gmail.com
*/
#ifndef __EventEmitter_h__
#define __EventEmitter_h__

#include <unordered_map>
#include <vector>
#include <string>
#include <thread>

using std::function;
using std::vector;
using std::unordered_map;
using std::thread;

/* Inherit from it to add Event-Capturing-ability to your class 
 * @T_EventClass: class that holds event info when emitted, sent to function that should be run
 * @T_EventKeyClass: class/type of key that distinguishes the event, default std::string, must be hashable
 */
template<typename T_EventClass, typename T_EventKeyClass = std::string>
class EventEmitter {
    using EventFunction_t = function<void(const T_EventClass&)>;

private:
    unordered_map<T_EventKeyClass, vector<EventFunction_t>> functions_bundle; // to store functions with its keys

public:
    /* store this listener function with this eventKey
     * @eventKey: key to event, if duplicated the function will be added to the previous
     * @listener: function/lambda that should be called when event is emitted
     * @isAsync: whether this listener should be called in another therad or not
     */
    void onEvent(T_EventKeyClass eventKey, EventFunction_t listener, bool isAsync=false) {
        if (isAsync) // wrap it in a thread to be asyncrounous
            functions_bundle[eventKey].push_back([&listener](const T_EventClass& evt) {
                thread(listener, evt);
            });
        else // add the function immediately
            functions_bundle[eventKey].push_back(listener);
    }

    /* overload, function could take no Event
     * @eventKey: key to event, if duplicated the function will be added to the previous
     * @listener: function/lambda that should be called when event is emitted
     * @isAsync: whether this listener should be called in another therad or not
     */
    void onEvent(T_EventKeyClass eventKey, function<void()> listener, bool isAsync=false) {
        if (isAsync) // wrap it in a thread to be asyncrounous
            functions_bundle[eventKey].push_back([&listener](const T_EventClass& evt) {
                thread(listener);
            });
        else // add the function immediately
            functions_bundle[eventKey].push_back([&listener](const T_EventClass& evt) {
                listener();
            });
    }

    /* same as onEvent, but function removes itself after calling 
     * @eventKey: key to event, if duplicated the function will be added to the previous
     * @listener: function/lambda that should be called when event is emitted
     * @isAsync: whether this listener should be called in another therad or not
     */
    void once(T_EventKeyClass eventKey, EventFunction_t listener, bool isAsync=false) {
        // make a function that can remove itself
        EventFunction_t func;
        func = [&listener, this, &eventKey, &func](const T_EventClass& evt) {
            listener(evt);

            this->removeListener(eventKey, func);
        };

        // then call onEvent to add it
        onEvent(eventKey, func, isAsync);
    }

    /* overload, function could take no Event
     * @eventKey: key to event, if duplicated the function will be added to the previous
     * @listener: function/lambda that should be called when event is emitted
     * @isAsync: whether this listener should be called in another therad or not
     */
    void once(T_EventKeyClass eventKey, function<void()> listener, bool isAsync=false) {
        // make a function that can remove itself
        EventFunction_t func;
        func = [&listener, this, &eventKey, &func](const T_EventClass& evt) {
            listener();

            this->removeListener(eventKey, func);
        };

        // then call onEvent to add it
        onEvent(eventKey, func, isAsync);
    }

    /* fire the event resulting in calling all functions in order, if event not found, nothing happens
     * @eventKey: of event to fire
     * @eventInfo: event object to send to functions to be called
     */
    void emitEvent(T_EventKeyClass eventKey, const T_EventClass& eventInfo) {
        // if found
        if (functions_bundle.find(eventKey) != functions_bundle.end()) {
            // iterate through all functins and call them
            for (auto& func : functions_bundle[eventKey])
                func(eventInfo);
        }
    }

    /* erase all functions that has been bound to given key
     * @eventKey: to remove its functions if found
     */
    void removeAllListeners(T_EventKeyClass eventKey) {
        // get itr to event
        auto eventItr = functions_bundle.find(eventKey);
        if (eventItr != functions_bundle.end())
            // erase that event with all its listeneres if found
            functions_bundle.erase(eventItr);
    }

    /* erase all listeners */
    void removeAllListeners() {
        functions_bundle.clear();
    }

    /* @eventKey: to get its functions
     */
    vector<EventFunction_t> listeners(T_EventKeyClass eventKey) {
        return vector<EventFunction_t>(functions_bundle[eventKey]);
    }

    /* return vector of events that had been registered */
    vector<T_EventKeyClass> events() {
        return vector<T_EventKeyClass>(functions_bundle.begin(), functions_bundle.end());
    }

    /* remove one listener from registered listeneres if found
     * @eventKey: event that the listener is attached to 
     * @listener: function to remove 
     */
    void removeListener(T_EventKeyClass eventKey, EventFunction_t listener) {
        // if eventKey is found
        if (functions_bundle.find(eventKey) != functions_bundle.end()) {
            // refrence to vector of functions
            vector<EventFunction_t>& funs = functions_bundle[eventKey];

            // find and erase
            auto itr = funs.find(listener);
            if (itr != funs.end()) 
                funs.erase(itr);

            // if event became empty, erase it 
            if (funs.size() == 0)
                functions_bundle.erase(functions_bundle.find(eventKey));
        }
    }

    /* get listeners count if event is found, else 0
     * @eventKey: key to event to find
     */
    int listenersCount(T_EventKeyClass eventKey) {
        // if found
        if (functions_bundle.find(eventKey) != functions_bundle.end())
            return functions_bundle[eventKey].size(); // return its size

        return 0;
    }
};

#endif  /* __EventEmitter_h__ */
// Copyright (c) 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
#ifndef PCOE_EVENTDRIVENOBSERVER_H
#define PCOE_EVENTDRIVENOBSERVER_H
#include <memory>
#include <unordered_map>

#include "Messages/IMessageProcessor.h"
#include "Messages/MessageBus.h"
#include "Messages/MessageWatcher.h"
#include "Observers/Observer.h"

namespace PCOE {
    /**
     * Provides an event-driven wrapper around an observer. The event-driven
     * observer listens for inputs and outputs of the observer's model and
     * automatically initializes and steps the observer as data is received.
     * Aditionally, the results of the observer are published to the message
     * bus.
     *
     * @author Jason Watkins
     * @since 1.2
     **/
    class AsyncObserver final : public IMessageProcessor {
    public:
        /**
         * Constructs a new {@code AsyncObserver}.
         *
         * @param messageBus The message bus on which to listen for and publish
         *                   messages. The {@code AsyncObserver} will
         *                   immediately register to receive messages from the
         *                   message bus, and will publish results of the
         *                   observer to the message bus.
         * @param observer   The observer that the {@code AsyncObserver}
         *                   is monitoring.
         * @param source     The name of the source that is being observed.
         **/
        AsyncObserver(MessageBus& messageBus,
                            std::unique_ptr<Observer>&& observer,
                            std::string source);

        /**
         * Unsubscribes the {@code AsyncObserver} from the message bus.
         **/
        ~AsyncObserver();

        const std::string& getName() {
            return source;
        }

        /**
         * Handles messages representing updates to the model inputs and
         * outputs. When sufficient new data is collected, automaticlly triggers
         * an observer step and publishes the result.
         *
         * @param message. The message to process.
         **/
        void processMessage(const std::shared_ptr<Message>& message) override;

    private:
        void stepObserver();

        using mutex = std::timed_mutex;
        using lock_guard = std::lock_guard<mutex>;
        using unique_lock = std::unique_lock<mutex>;

        mutable mutex m;
        MessageBus& bus;
        std::unique_ptr<Observer> observer;
        std::string source;
        MessageWatcher<double> inputWatcher;
        MessageWatcher<double> outputWatcher;
        std::shared_ptr<Message> inputMsg;
        std::shared_ptr<Message> outputMsg;
        bool hasInputs;
        bool hasOutputs;
    };
}

#endif

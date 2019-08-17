#pragma once

#include "Prerequisites/TePrerequisitesUtility.h"
#include "Threading/TeThreading.h"

namespace te
{
    /** Data common to all event. */
    struct BaseConnectionData
    {
        BaseConnectionData() = default;

        virtual ~BaseConnectionData()
        {
            assert(!HandleLinks && !IsActive);
        }
        
        virtual void Deactivate()
        {
            IsActive = false;
        }

        BaseConnectionData* Previous = nullptr;
        BaseConnectionData* Next = nullptr;
        bool IsActive = true;
        UINT16 HandleLinks = 0;
    };

    /** Internal data for an Event, storing all connections. */
    class InternalData
    {
    public:
        InternalData() = default;

        ~InternalData()
        {
            BaseConnectionData* connection = _connections;
            while (connection != nullptr)
            {
                BaseConnectionData* next = connection->Next;
                te_free(connection);

                connection = next;
            }

            connection =_freeConnections;
            while (connection != nullptr)
            {
                BaseConnectionData* next = connection->Next;
                te_free(connection);

                connection = next;
            }

            connection = _newConnections;
            while (connection != nullptr)
            {
                BaseConnectionData* next = connection->Next;
                te_free(connection);

                connection = next;
            }
        }

        /** Appends a new connection to the active connection array. */
        void Connect(BaseConnectionData* connection)
        {
            connection->Previous = _lastConnection;

            if (_lastConnection != nullptr)
                _lastConnection->Next = connection;

            _lastConnection = connection;

            // First connection
            if (_connections == nullptr)
                _connections = connection;
        }

        /**
         * Disconnects the connection with the specified data, ensuring the event doesn't call its callback again.
         */
        void Disconnect(BaseConnectionData* connection)
        {
            RecursiveLock lock(_mutex);

            connection->Deactivate();
            connection->HandleLinks--;

            if (connection->HandleLinks == 0) 
            {
                free(connection);
            }

            //If we delete the last event, pointed value by _connections does not exist anymore
            if (_connections == connection) 
            {
                _connections = nullptr;
            }
        }

        /** Disconnects all connections in the event. */
        void Clear()
        {
            RecursiveLock lock(_mutex);

            BaseConnectionData* conn = _connections;
            while (conn != nullptr)
            {
                BaseConnectionData* next = conn->Next;
                conn->Deactivate();

                if (conn->HandleLinks == 0)
                    free(conn);

                conn = next;
            }

            _connections = nullptr;
            _lastConnection = nullptr;
        }

        /**
         * Called when the event handle no longer keeps a reference to the connection data. This means we might be able to
         * free (and reuse) its memory if the event is done with it too.
         */
        void FreeHandle(BaseConnectionData* connection)
        {
            RecursiveLock lock(_mutex);

            connection->HandleLinks--;

            if (connection->HandleLinks == 0 && !connection->IsActive)
                Free(connection);
        }

        /** Releases connection data and makes it available for re-use when next connection is formed. */
        void Free(BaseConnectionData* connection)
        {
            if (connection->Previous != nullptr)
                connection->Previous->Next = connection->Next;
            else
                _connections = connection->Next;

            if (connection->Next != nullptr)
                connection->Next->Previous = connection->Previous;
            else
                _lastConnection = connection->Previous;

            connection->Previous = nullptr;
            connection->Next = nullptr;

            if (_freeConnections != nullptr)
            {
                connection->Next = _freeConnections;
                _freeConnections->Previous = connection;
            }

            _freeConnections = connection;
            _freeConnections->~BaseConnectionData();
        }

        // Pointer to the first connection of the linked list
        BaseConnectionData* _connections = nullptr; 
        // Pointer to the last connection added
        BaseConnectionData* _lastConnection = nullptr;
        // The list of free connections
        BaseConnectionData* _freeConnections = nullptr; 
        // Pointer to the linked representing connections added while iterating through the list (concurrency)
        BaseConnectionData* _newConnections = nullptr; 

        RecursiveMutex _mutex;
        bool _isCurrentlyTriggering = false;
    };

    /** Event handler. Allows you to track to which events you subscribed to and disconnect from them when needed. */
	class HEvent
	{
	public:
		HEvent() = default;

        HEvent(SPtr<InternalData> eventData, BaseConnectionData* connection)
            : _connection(connection)
            , _eventData(std::move(eventData))
        {
            connection->HandleLinks++;
        }

        ~HEvent()
        {
            if (_connection != nullptr)
                _eventData->FreeHandle(_connection);
        }

        void Disconnect()
		{
            if (_connection != nullptr)
            {
                _eventData->Disconnect(_connection);
                _connection = nullptr;
                _eventData = nullptr;
            }
        }

        HEvent& operator=(const HEvent& rhs)
        {
            _connection = rhs._connection;
            _eventData = rhs._eventData;

            if (_connection != nullptr)
                _connection->HandleLinks++;

            return *this;
        }

    protected:
        BaseConnectionData* _connection = nullptr;
        SPtr<InternalData> _eventData;
    };

    /**
	 * Events allows you to register method callbacks that get notified when the event is triggered.
	 */
    template <class ReturnType, class... Args>
    class InternalEvent
    {
    public:
        struct ConnectionData : BaseConnectionData
        {
            void Deactivate() override
            {
                Function = nullptr;
                BaseConnectionData::Deactivate();
            }

            std::function<ReturnType(Args...)> Function;
        };

        InternalEvent()
            : _internalData(te_shared_ptr_new<InternalData>())
        {
        }

        ~InternalEvent()
        {
            Clear();
        }

        /** Register a new callback that will get notified once the event is triggered. */
        HEvent Connect(std::function<ReturnType(Args...)> function)
        {
            RecursiveLock lock(_internalData->_mutex);

            ConnectionData* connection = nullptr;
            if (_internalData->_freeConnections != nullptr)
            {
                connection = static_cast<ConnectionData*>(_internalData->_freeConnections);
                _internalData->_freeConnections = connection->Next;

                new (connection)ConnectionData();
                if (connection->Next != nullptr)
                    connection->Next->Previous = nullptr;

                connection->IsActive = true;
            }

            if (connection == nullptr)
                connection = te_new<ConnectionData>();

            // If currently iterating over the connection list, delay modifying it until done
            if (_internalData->_isCurrentlyTriggering)
            {
                connection->Previous = _internalData->_newConnections;

                if (_internalData->_newConnections != nullptr)
                    _internalData->_newConnections->Next = connection;

                _internalData->_newConnections = connection;
            }
            else
            {
                _internalData->Connect(connection);
            }

            connection->Function = function;

            return HEvent(_internalData, connection);
        }

        /** Trigger the event, notifying all register callback methods. */
        void operator() (Args... args)
        {
            // Increase ref count to ensure this event data isn't destroyed if one of the callbacks
            // deletes the event itself.
            SPtr<InternalData> internalData = _internalData;

            RecursiveLock lock(internalData->_mutex);
            internalData->_isCurrentlyTriggering = true;

            ConnectionData* connection = static_cast<ConnectionData*>(internalData->_connections);
            while (connection != nullptr)
            {
                // Save next here in case the callback itself disconnects this connection
                ConnectionData* next = static_cast<ConnectionData*>(connection->Next);

                if (connection->Function != nullptr)
                    connection->Function(std::forward<Args>(args)...);

                connection = next;
            }

            internalData->_isCurrentlyTriggering = false;

            // If any new connections were added during the above calls, add them to the connection list
            if (internalData->_newConnections != nullptr)
            {
                BaseConnectionData* lastNewConnection = internalData->_newConnections;
                while (lastNewConnection != nullptr)
                    lastNewConnection = lastNewConnection->Next;

                BaseConnectionData* currentConnection = lastNewConnection;
                while (currentConnection != nullptr)
                {
                    BaseConnectionData* prevConnection = currentConnection->Previous;
                    currentConnection->Previous = nullptr;
                    currentConnection->Previous = nullptr;

                    _internalData->Connect(currentConnection);
                    currentConnection = prevConnection;
                }

                internalData->_newConnections = nullptr;
            }
        }

        /** Clear all callbacks from the event. */
        void Clear()
        {
            _internalData->Clear();
        }

        /**
         * Check if event has any callbacks registered.
         */
        bool Empty() const
        {
            RecursiveLock lock(_internalData->_mutex);
            return _internalData->_connections == nullptr;
        }

    protected:
        SPtr<InternalData> _internalData;
    };

    template <typename Signature>
	class Event;

    template <class ReturnType, class... Args>
	class TE_UTILITY_EXPORT Event<ReturnType(Args...) > : public InternalEvent <ReturnType, Args...>
	{ };
}
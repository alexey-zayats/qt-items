#ifndef QI_SIGNAL_H
#define QI_SIGNAL_H

#include "QiAPI.h"

#include <vector>
#include <functional>

namespace QiPrivate
{

template <typename Signature>
class QiSignalBase
{
public:
    typedef std::function<Signature> Function_t;

    quint32 connect(const Function_t& slot, quintptr id = 0) const
    {
        if (m_firstUnusedSlotId != QiInvalid)
        {
            // look for free slot item
            for (quintptr i = m_firstUnusedSlotId, n = (quintptr)m_slots.size(); i < n; ++i)
            {
                SlotInfo& slotItem = m_slots[i];
                if (!slotItem.func)
                {
                    slotItem.func = slot;
                    slotItem.id = id;
                    m_firstUnusedSlotId = i;
                    return i;
                }
            }

            // all slot items is in use
            m_firstUnusedSlotId = QiInvalid;
        }

        m_slots.push_back(SlotInfo());
        m_slots.back().func = slot;
        m_slots.back().id = id;

        return (quint32)(m_slots.size() - 1);
    }

    quint32 connect(Function_t&& slot, quintptr id = 0) const
    {
        if (m_firstUnusedSlotId != QiInvalid)
        {
            // look for free slot item
            for (quintptr i = m_firstUnusedSlotId, n = (quintptr)m_slots.size(); i < n; ++i)
            {
                SlotInfo& slotItem = m_slots[i];
                if (!slotItem.func)
                {
                    slotItem.func.swap(slot);
                    slotItem.id = id;
                    m_firstUnusedSlotId = i;
                    return i;
                }
            }

            // all slot items is in use
            m_firstUnusedSlotId = QiInvalid;
        }

        m_slots.push_back(SlotInfo());
        m_slots.back().func.swap(slot);
        m_slots.back().id = id;

        return (quint32)(m_slots.size() - 1);
    }

    template <typename StaticFunction>
    quint32 connectStatic(StaticFunction* staticFunction) const
    {
        return connect(Function_t(staticFunction), (quintptr)staticFunction);
    }

    bool disconnect(quint32 connectId) const
    {
        if (connectId >= (quint32)m_slots.size())
        {
            Q_ASSERT(false);
            return false;
        }

        SlotInfo& slotItem = m_slots[connectId];
        if (!slotItem.func)
        {
            Q_ASSERT(false);
            return false;
        }

        // make slotItem empty
        slotItem = SlotInfo();

        if (m_firstUnusedSlotId > connectId)
            m_firstUnusedSlotId = connectId;

        return true;
    }

    template <typename StaticFunction>
    bool disconnectStatic(StaticFunction* staticFunction) const
    {
        return disconnectById((quintptr)staticFunction);
    }

    template <typename Slot>
    bool disconnectSlot(Slot* slot) const
    {
        return disconnectById((quintptr)slot);
    }

    bool disconnectById(quintptr id) const
    {
        bool anyDisconnect = false;

        for (quint32 i = 0, n = (quint32)m_slots.size(); i < n; ++i)
        {
            SlotInfo& slotItem = m_slots[i];
            if (slotItem.id == id)
            {
                anyDisconnect = true;

                // make slotItem empty
                slotItem = SlotInfo();

                if (m_firstUnusedSlotId > i)
                    m_firstUnusedSlotId = i;
            }
        }

        return anyDisconnect;
    }

    void disconnectAll() const
    {
        m_slots.clear();
        m_firstUnusedSlotId = QiInvalid;
    }

protected:

    QiSignalBase()
        : m_firstUnusedSlotId(QiInvalid)
    {}

    ~QiSignalBase()
    {
        Q_ASSERT(isEmpty());
    }

    QiSignalBase(const QiSignalBase& other)
        : m_firstUnusedSlotId(QiInvalid)
    {}

    QiSignalBase& operator=(const QiSignalBase& other)
    {}

    bool isEmpty() const
    {
        for (quint32 i = 0, n = (quint32)m_slots.size(); i < n; ++i)
        {
            SlotInfo& slotItem = m_slots[i];
            if (slotItem.func)
                return false;
        }

        return true;
    }

    struct SlotInfo
    {
        Function_t func;
        quintptr id;

        SlotInfo()
            : id(0)
        {
        }
    };

    mutable std::vector<SlotInfo> m_slots;
    mutable quint32 m_firstUnusedSlotId;
};

} // end namespace QiPrivate

template <typename Signature>
class QiSignal;

template <typename Ret>
class QiSignal<Ret()>: public QiPrivate::QiSignalBase<Ret()>
{
public:
    typedef typename QiPrivate::QiSignalBase<Ret()>::Function_t Function_t;
    typedef typename QiPrivate::QiSignalBase<Ret()>::SlotInfo SlotInfo;

    template <typename Slot, typename SlotFunction>
    quint32 connectSlot(Slot* slot, SlotFunction slotFunction) const
    {
        return this->connect(Function_t(std::bind(std::mem_fun(slotFunction), slot)), (quintptr)slot);
    }

    void doEmit()
    {
        for (auto& slotItem: this->m_slots)
        {
            if (!slotItem.func)
                continue;

            slotItem.func();
        }
    }
};

typedef QiSignal<void()> QiSignalVoid;

template<typename Param1>
class QiSignal<void(Param1)>: public QiPrivate::QiSignalBase<void(Param1)>
{
public:
    typedef typename QiPrivate::QiSignalBase<void(Param1)>::Function_t Function_t;
    typedef typename QiPrivate::QiSignalBase<void(Param1)>::SlotInfo SlotInfo;

    template <typename Slot, typename SlotFunction>
    quint32 connectSlot(Slot* slot, SlotFunction slotFunction) const
    {
        return this->connect(Function(std::bind(std::mem_fun(slotFunction), slot, std::placeholders::_1)), (quintptr)slot);
    }

    void doEmit(Param1 param1)
    {
        for (auto& slotItem: this->m_slots)
        {
            if (!slotItem.func)
                continue;

            slotItem.func(param1);
        }
    }
};

template<typename Param1, typename Param2>
class QiSignal<void(Param1, Param2)>: public QiPrivate::QiSignalBase<void(Param1, Param2)>
{
public:
    typedef typename QiPrivate::QiSignalBase<void(Param1, Param2)>::Function_t Function_t;
    typedef typename QiPrivate::QiSignalBase<void(Param1, Param2)>::SlotInfo SlotInfo;

    template <typename Slot, typename SlotFunction>
    quint32 connectSlot(Slot* slot, SlotFunction slotFunction) const
    {
        Function_t func = [slot, slotFunction](Param1 p1, Param2 p2) { (slot->*slotFunction)(p1, p2); };
        return this->connect(std::move(func));
    }

    void doEmit(Param1 param1, Param2 param2)
    {
        for (auto& slotItem: this->m_slots)
        {
            if (!slotItem.func)
                continue;

            slotItem.func(param1, param2);
        }
    }
};

template<typename Param1, typename Param2, typename Param3>
class QiSignal<void(Param1, Param2, Param3)>: public QiPrivate::QiSignalBase<void(Param1, Param2, Param3)>
{
public:
    typedef typename QiPrivate::QiSignalBase<void(Param1, Param2, Param3)>::Function_t Function_t;
    typedef typename QiPrivate::QiSignalBase<void(Param1, Param2, Param3)>::SlotInfo SlotInfo;

    template <typename Slot, typename SlotFunction>
    quint32 connectSlot(Slot* slot, SlotFunction slotFunction) const
    {
        Function_t func = [slot, slotFunction](Param1 p1, Param2 p2, Param3 p3) { (slot->*slotFunction)(p1, p2, p3); };
        return this->connect(std::move(func));
    }

    void doEmit(Param1 param1, Param2 param2, Param3 param3)
    {
        for (auto& slotItem: this->m_slots)
        {
            if (!slotItem.func)
                continue;

            slotItem.func(param1, param2, param3);
        }
    }
};

template<typename Param1, typename Param2, typename Param3, typename Param4>
class QiSignal<void(Param1, Param2, Param3, Param4)>: public QiPrivate::QiSignalBase<void(Param1, Param2, Param3, Param4)>
{
public:
    typedef typename QiPrivate::QiSignalBase<void(Param1, Param2, Param3, Param4)>::Function_t Function_t;
    typedef typename QiPrivate::QiSignalBase<void(Param1, Param2, Param3, Param4)>::SlotInfo SlotInfo;

    template <typename Slot, typename SlotFunction>
    quint32 connectSlot(Slot* slot, SlotFunction slotFunction) const
    {
        Function_t func = [slot, slotFunction](Param1 p1, Param2 p2, Param3 p3, Param4 p4) { (slot->*slotFunction)(p1, p2, p3, p4); };
        return this->connect(std::move(func));
    }

    void doEmit(Param1 param1, Param2 param2, Param3 param3, Param4 param4)
    {
        for (auto& slotItem: this->m_slots)
        {
            if (!slotItem.func)
                continue;

            slotItem.func(param1, param2, param3, param4);
        }
    }
};

template<typename Param1, typename Param2, typename Param3, typename Param4, typename Param5>
class QiSignal<void(Param1, Param2, Param3, Param4, Param5)>: public QiPrivate::QiSignalBase<void(Param1, Param2, Param3, Param4, Param5)>
{
public:
    typedef typename QiPrivate::QiSignalBase<void(Param1, Param2, Param3, Param4, Param5)>::Function_t Function_t;
    typedef typename QiPrivate::QiSignalBase<void(Param1, Param2, Param3, Param4, Param5)>::SlotInfo SlotInfo;

    template <typename Slot, typename SlotFunction>
    quint32 connectSlot(Slot* slot, SlotFunction slotFunction) const
    {
        Function_t func = [slot, slotFunction](Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5) { (slot->*slotFunction)(p1, p2, p3, p4, p5); };
        return this->connect(std::move(func));
    }

    void doEmit(Param1 param1, Param2 param2, Param3 param3, Param4 param4, Param5 param5)
    {
        for (auto& slotItem: this->m_slots)
        {
            if (!slotItem.func)
                continue;

            slotItem.func(param1, param2, param3, param4, param5);
        }
    }
};

#endif // QI_SIGNAL_H
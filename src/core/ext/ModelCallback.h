#ifndef QI_MODEL_CALLBACK_H
#define QI_MODEL_CALLBACK_H

#include "ModelTyped.h"
#include <functional>

namespace Qi
{

// typed model with callbacks as functors
template <typename T, bool ascendingDefault = true>
class ModelCallback: public ModelTyped<T>
{
public:
    typedef std::function<T(const ItemID&)> GetValueFunction_t;
    typedef std::function<bool(const ItemID&, T)> SetValueFunction_t;
    typedef std::function<bool(ItemsIterator&, T)> SetMultipleValueFunction_t;

    ModelCallback(const GetValueFunction_t& getFunc = nullptr, const SetValueFunction_t& setFunc = nullptr, const SetMultipleValueFunction_t& setMultipleFunc = nullptr)
        : getValueFunction(getFunc),
          setValueFunction(setFunc),
          setMultipleValueFunction(setMultipleFunc)
    {
        ModelTyped<T>::m_ascendingDefault = ascendingDefault;
    }

    GetValueFunction_t getValueFunction;
    SetValueFunction_t setValueFunction;
    SetMultipleValueFunction_t setMultipleValueFunction;

protected:
    T valueImpl(const ItemID& item) const override
    {
        if (!getValueFunction)
            throw std::logic_error("Get function is not set");

        return getValueFunction(item);
    }

    bool setValueImpl(const ItemID& item, T value) override
    {
        if (setValueFunction)
            return setValueFunction(item, value);
        else
            return false;
    }

    virtual bool setValueMultipleImpl(ItemsIterator& itemsIterator, T value) override
    {
        if (setMultipleValueFunction)
            return setMultipleValueFunction(itemsIterator, value);
        else
            return ModelTyped<T>::setValueMultipleImpl(itemsIterator, value);
    }
};

class ModelCallbackComparable: public ModelComparable
{
public:
    typedef std::function<int(const ItemID&, const ItemID&)> CompareFunction_t;

    ModelCallbackComparable(CompareFunction_t compareFunction = nullptr, bool ascendingDefault = true)
        : compareFunction(compareFunction),
          m_ascendingDefault(ascendingDefault)
    {
    }

    CompareFunction_t compareFunction;

protected:
    int compareImpl(const ItemID &left, const ItemID &right) const override
    {
        return compareFunction(left, right);
    }
    bool isAscendingDefaultImpl(const ItemID &item) const override
    {
        return m_ascendingDefault;
    }

private:
    bool m_ascendingDefault;
};

} // end namespace Qi

#endif // QI_MODEL_CALLBACK_H

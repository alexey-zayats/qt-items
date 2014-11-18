#include "Ranges.h"

namespace Qi
{

RangeSelection& RangeSelection::operator=(const RangeSelection& other)
{
    m_ranges = other.m_ranges;
    emit rangeChanged(this, ChangeReasonRange);

    return *this;
}

void RangeSelection::clear()
{
    m_ranges.clear();
    emit rangeChanged(this, ChangeReasonRange);
}

void RangeSelection::addRange(const QSharedPointer<Range>& range, bool exclude)
{
    RangeInfo info = { range, exclude };
    m_ranges.append(info);
    emit rangeChanged(this, ChangeReasonRange);
}

bool RangeSelection::hasItemImpl(const ItemID &item) const
{
    bool excluded = true;
    for (const auto& range: m_ranges)
    {
        if (range.range->hasItem(item))
        {
            excluded = range.exclude;
        }
    }

    return !excluded;
}

RangeNone::RangeNone()
{
}

bool RangeNone::hasItemImpl(const ItemID& item) const
{
    return false;
}

QSharedPointer<RangeNone> makeRangeNone()
{
    return QSharedPointer<RangeNone>::create();
}

RangeAll::RangeAll()
{
}

bool RangeAll::hasItemImpl(const ItemID& item) const
{
    return true;
}

QSharedPointer<RangeAll> makeRangeAll()
{
    return QSharedPointer<RangeAll>::create();
}

RangeColumn::RangeColumn(quint32 column)
    : m_column(column)
{
}

void RangeColumn::setColumn(quint32 column)
{
    if (column != m_column)
    {
        m_column = column;
        emit rangeChanged(this, ChangeReasonRange);
    }
}

bool RangeColumn::hasItemImpl(const ItemID& item) const
{
    return item.column == m_column;
}

QSharedPointer<RangeColumn> makeRangeColumn(quint32 column)
{
    return QSharedPointer<RangeColumn>::create(column);
}

RangeColumns::RangeColumns(const QSet<quint32>& columns)
    : m_columns(columns)
{
}

RangeColumns::RangeColumns(quint32 columnBegin, quint32 columnEnd)
{
    Q_ASSERT(columnBegin <= columnEnd);
    for (quint32 column = columnBegin; column != columnEnd; ++column)
        m_columns.insert(column);
}

void RangeColumns::setColumns(const QSet<quint32>& columns)
{
    if (m_columns != columns)
    {
        m_columns = columns;
        emit rangeChanged(this, ChangeReasonRange);
    }
}

bool RangeColumns::hasItemImpl(const ItemID& item) const
{
    return m_columns.contains(item.column);
}

QSharedPointer<RangeColumns> makeRangeColumns(const QSet<quint32>& columns)
{
    return QSharedPointer<RangeColumns>::create(columns);
}

QSharedPointer<RangeColumns> makeRangeColumns(quint32 columnBegin, quint32 columnEnd)
{
    return QSharedPointer<RangeColumns>(new RangeColumns(columnBegin, columnEnd));
}

RangeRow::RangeRow(quint32 row)
    : m_row(row)
{
}

void RangeRow::setRow(quint32 row)
{
    if (row != m_row)
    {
        m_row = row;
        emit rangeChanged(this, ChangeReasonRange);
    }
}

bool RangeRow::hasItemImpl(const ItemID& item) const
{
    return item.row == m_row;
}

QSharedPointer<RangeRow> makeRangeRow(quint32 row)
{
    return QSharedPointer<RangeRow>::create(row);
}

RangeRows::RangeRows(const QSet<quint32>& rows)
    : m_rows(rows)
{
}

RangeRows::RangeRows(quint32 rowBegin, quint32 rowEnd)
{
    Q_ASSERT(rowBegin <= rowEnd);
    for (quint32 row = rowBegin; row != rowEnd; ++row)
        m_rows.insert(row);
}

void RangeRows::setRows(const QSet<quint32>& rows)
{
    if (m_rows != rows)
    {
        m_rows = rows;
        emit rangeChanged(this, ChangeReasonRange);
    }
}

bool RangeRows::hasItemImpl(const ItemID& item) const
{
    return m_rows.contains(item.row);
}

QSharedPointer<RangeRows> makeRangeRows(const QSet<quint32>& rows)
{
    return QSharedPointer<RangeRows>::create(rows);
}

QSharedPointer<RangeRows> makeRangeRows(quint32 rowBegin, quint32 rowEnd)
{
    return QSharedPointer<RangeRows>::create(rowBegin, rowEnd);
}

RangeRect::RangeRect(const QSet<quint32>& rows, const QSet<quint32>& columns)
    : m_rows(rows),
      m_columns(columns)
{
}

RangeRect::RangeRect(quint32 rowBegin, quint32 rowEnd, quint32 columnBegin, quint32 columnEnd)
{
    Q_ASSERT(rowBegin <= rowEnd);
    Q_ASSERT(columnBegin <= columnEnd);

    for (; rowBegin < rowEnd; ++rowBegin)
        m_rows.insert(rowBegin);

    for (; columnBegin < columnEnd; ++columnBegin)
        m_columns.insert(columnBegin);
}

void RangeRect::setRows(const QSet<quint32>& rows)
{
    if (m_rows != rows)
    {
        m_rows = rows;
        emit rangeChanged(this, ChangeReasonRange);
    }
}

void RangeRect::setColumns(const QSet<quint32>& columns)
{
    if (m_columns != columns)
    {
        m_columns = columns;
        emit rangeChanged(this, ChangeReasonRange);
    }
}

bool RangeRect::hasItemImpl(const ItemID& item) const
{
    return m_rows.contains(item.row) && m_columns.contains(item.column);
}

QSharedPointer<RangeRect> makeRangeRect(const QSet<quint32>& rows, const QSet<quint32>& columns)
{
    return QSharedPointer<RangeRect>::create(rows, columns);
}

QSharedPointer<RangeRect> makeRangeRect(quint32 rowBegin, quint32 rowEnd, quint32 columnBegin, quint32 columnEnd)
{
    return QSharedPointer<RangeRect>::create(rowBegin, rowEnd, columnBegin, columnEnd);
}

} // end namespace Qi

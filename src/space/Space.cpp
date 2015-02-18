#include "Space.h"
#include "core/ext/Layouts.h"
#include "core/ext/ViewComposite.h"

namespace Qi
{

ViewSchema viewSchemaForItem(const ItemID& item, ViewApplicationMask viewApplicationMask, const QVector<ItemSchema>& itemSchemas)
{
    QVector<ViewSchema> viewSchemas;

    for (const auto& schema : itemSchemas)
    {
        if (schema.range->hasItem(item) && schema.view->isApplicable(viewApplicationMask))
        {
            viewSchemas.append(ViewSchema(schema.layout, schema.view));
        }
    }

    if (viewSchemas.empty())
        return ViewSchema();
    else if (viewSchemas.size() == 1)
        return viewSchemas.front();
    else
    {
        ViewSchema schema;
        schema.layout = makeLayoutBackground();
        schema.view = QSharedPointer<ViewComposite>::create(viewSchemas);
        return schema;
    }
}

Space::Space(SpaceHints hints)
    : m_hints(hints),
      m_viewApplicationMask(ViewApplicationNone)
{
}

Space::~Space()
{
    clearSchemas();
}

const QVector<ItemSchema>& Space::schemasOrdered() const
{
    if (m_schemasOrdered.isEmpty() && !m_schemas.isEmpty())
    {
        m_schemasOrdered.resize(m_schemas.size());

        auto o_it = m_schemasOrdered.begin();

        // copy non-final (CLIENT) views
        for (const auto& schema: m_schemas)
        {
            if (!schema.layout->isFinal())
            {
                *o_it = schema;
                ++o_it;
            }
        }

        // copy final (CLIENT) views at the end
        for (const auto& schema: m_schemas)
        {
            if (schema.layout->isFinal())
            {
                *o_it = schema;
                ++o_it;
            }
        }
    }

    return m_schemasOrdered;
}

int Space::addSchema(const ItemSchema& schema)
{
    m_schemas.append(schema);
    m_schemasOrdered.clear();

    connectSchema(schema);

    emit spaceChanged(this, ChangeReasonSpaceItemsStructure);

    return m_schemas.size() - 1;
}

int Space::insertSchema(int index, const QSharedPointer<Range>& range, const QSharedPointer<View>& view, const QSharedPointer<Layout>& layout)
{
    ItemSchema schema(range, layout, view);

    m_schemas.insert(index, schema);
    m_schemasOrdered.clear();

    connectSchema(schema);

    emit spaceChanged(this, ChangeReasonSpaceItemsStructure);

    return index;
}

void Space::removeSchema(const QSharedPointer<View>& view)
{
    for (int i = 0; i < m_schemas.size(); ++i)
    {
        const auto& schema = m_schemas[i];
        if (schema.view == view)
        {
            disconnectSchema(schema);
            m_schemas.remove(i);
            m_schemasOrdered.clear();
            emit spaceChanged(this, ChangeReasonSpaceItemsStructure);
            return;
        }
    }
}

void Space::clearSchemas()
{
    for (const auto& schema: m_schemas)
    {
        disconnectSchema(schema);
    }

    m_schemas.clear();
    m_schemasOrdered.clear();

    emit spaceChanged(this, ChangeReasonSpaceItemsStructure);
}

void Space::setViewApplicationMask(ViewApplicationMask viewApplicationMask)
{
    if (m_viewApplicationMask != viewApplicationMask)
    {
        m_viewApplicationMask = viewApplicationMask;
        emit spaceChanged(this, ChangeReasonSpaceItemsStructure);
    }
}

void Space::connectSchema(const ItemSchema& schema)
{
    connect(schema.range.data(), &Range::rangeChanged, this, &Space::onRangeChanged);
    connect(schema.layout.data(), &Layout::layoutChanged, this, &Space::onLayoutChanged);
    connect(schema.view.data(), &View::viewChanged, this, &Space::onViewChanged);
}

void Space::disconnectSchema(const ItemSchema& schema)
{
    disconnect(schema.range.data(), &Range::rangeChanged, this, &Space::onRangeChanged);
    disconnect(schema.layout.data(), &Layout::layoutChanged, this, &Space::onLayoutChanged);
    disconnect(schema.view.data(), &View::viewChanged, this, &Space::onViewChanged);
}

void Space::onRangeChanged(const Range* /*range*/, ChangeReason reason)
{
    emit spaceChanged(this, reason | ChangeReasonSpaceItemsStructure);
}

void Space::onLayoutChanged(const Layout* /*layout*/, ChangeReason reason)
{
    emit spaceChanged(this, reason | ChangeReasonSpaceItemsStructure);
}

void Space::onViewChanged(const View* /*view*/, ChangeReason reason)
{
    if (reason & ChangeReasonViewSize)
        emit spaceChanged(this, reason | ChangeReasonSpaceItemsStructure);
    else
        emit spaceChanged(this, reason | ChangeReasonSpaceItemsContent);
}

} // end namespace Qi

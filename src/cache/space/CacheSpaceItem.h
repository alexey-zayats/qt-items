#ifndef QI_CACHE_SPACE_ITEM_H
#define QI_CACHE_SPACE_ITEM_H

#include "CacheSpace.h"
#include "space/SpaceItem.h"

namespace Qi
{

class QI_EXPORT CacheSpaceItem : public CacheSpace
{
    Q_OBJECT
    Q_DISABLE_COPY(CacheSpaceItem)

public:
    explicit CacheSpaceItem(const QSharedPointer<SpaceItem>& spaceItem, ViewApplicationMask viewApplicationMask = ViewApplicationDraw);
    ~CacheSpaceItem();

    const QSharedPointer<SpaceItem>& spaceItem() const { return m_spaceItem; }

private:
    void clearItemsCacheImpl() const override;
    void validateItemsCacheImpl() const override;
    void invalidateItemsCacheStructureImpl() const override;
    void drawImpl(QPainter* painter, const GuiContext& ctx) const override;
    const CacheItem* cacheItemImpl(const ItemID& visibleItem) const override;
    const CacheItem* cacheItemByPositionImpl(const QPoint& point) const override;

    QSharedPointer<SpaceItem> m_spaceItem;
    mutable QSharedPointer<CacheItem> m_item;
};

} // end namespace Qi

#endif // QI_CACHE_SPACE_H

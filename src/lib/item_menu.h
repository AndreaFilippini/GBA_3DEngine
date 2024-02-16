#ifndef GUARD_ITEM_MENU_H
#define GUARD_ITEM_MENU_H

struct ItemSlot
{
    u16 itemId;
    u16 quantity;
};

typedef void (*ItemUseFunc)(u8);

struct BagPocket
{
    struct ItemSlot *itemSlots;
    u8 capacity;
};

#define NUM_BAG_POCKETS 5

struct PocketScrollState
{
    u8 cursorPos;
    u8 scrollTop;
    u8 numSlots;
    u8 cursorMax;
};

enum
{
    RETURN_TO_FIELD_0,
    RETURN_TO_BATTLE,
    RETURN_TO_PKMN_LIST,
    RETURN_TO_SHOP,
    RETURN_TO_FIELD_4,
    RETURN_TO_FIELD_5,
    RETURN_TO_PC,
    RETURN_TO_WALLY_BATTLE,
};

#define sReturnLocation	(*(u8*)0x03000701)

#define gBagPocketScrollStates ((struct PocketScrollState*)0x03005D10)
#define gCurrentBagPocketItemSlots (*(struct ItemSlot**)0x03005D24)

#define sCurrentBagPocket (*(s8*)0x02038559)
#define gUnknown_0203855A (*(s8*)0x0203855A)
#define gSpecialVar_ItemId (*(u16*)0x0203855E)
#define gCurSelectedItemSlotIndex (*(u8*)0x02038560)

#define gBagPockets ((struct BagPocket*)0x083C1618)

#endif // GUARD_ITEM_MENU_H


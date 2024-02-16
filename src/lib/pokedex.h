#define HOENN_DEX_COUNT 202
#define NATIONAL_DEX_COUNT 386

struct PokedexListItem
{
    u16 dexNum;
    u16 seen:1;
    u16 owned:1;
};

struct PokedexView
{
    struct PokedexListItem unk0[NATIONAL_DEX_COUNT];
    u16 unk608;
    u8 unk60A_1:1;
    u8 unk60A_2:1;
    u8 unk60B;
    u16 pokemonListCount;
    u16 selectedPokemon;
    u16 unk610;
    u16 dexMode;
    u16 unk614;
    u16 dexOrder;
    u16 unk618;
    u16 unk61A;
    u16 unk61C;
    u16 unk61E[4];
    u16 selectedMonSpriteId;
    u16 unk628;
    u16 unk62A;
    u8 unk62C;
    u8 unk62D;
    u8 unk62E;
    u8 unk62F;
    s16 unk630;
    s16 unk632;
    u16 unk634;
    u16 unk636;
    u16 unk638;
    u16 unk63A[4];
    u8 filler642[8];
    u8 unk64A;
    u8 unk64B;
    u8 unk64C_1:1;
    u8 selectedScreen;
    u8 descriptionPageNum;
    u8 unk64F;
    u8 menuIsOpen;      //menuIsOpen
    u8 unk651;
    u16 menuCursorPos;     //Menu cursor position
    s16 menuY;     //Menu Y position (inverted because we use REG_BG0VOFS for this)
    u8 unk656[8];
    u8 unk65E[8];
};

enum
{
    DEX_MODE_HOENN,
    DEX_MODE_NATIONAL
};

enum
{
    PAGE_SCREEN,
    AREA_SCREEN,
    CRY_SCREEN,
    SIZE_SCREEN
};

#define gHoennToNationalOrder ((u16*)0x081FC84C)
//#define gHoennToNationalOrder ((u16*)0x081FC87E)
#define gSpeciesToNationalPokedexNum ((u16*)0x081FC516)

enum
{
    FLAG_GET_SEEN,
    FLAG_GET_CAUGHT,
    FLAG_SET_SEEN,
    FLAG_SET_CAUGHT
};

struct PokedexEntry
{
    /*0x00*/ u8 categoryName[12];
    /*0x0C*/ u16 height; //in decimeters
    /*0x0E*/ u16 weight; //in hectograms
    /*0x10*/ const u8 *descriptionPage1;
    /*0x14*/ const u8 *descriptionPage2;
    /*0x18*/ u16 unused;
    /*0x1A*/ u16 pokemonScale;
    /*0x1C*/ u16 pokemonOffset;
    /*0x1E*/ u16 trainerScale;
    /*0x20*/ u16 trainerOffset;
};  /*size = 0x24*/

#define gPokedexView (*(struct PokedexView*)0x02024FF4)
//#define gPokedexEntries ((struct PokedexEntry*)(0x083B1858))
#define gPokedexEntries ((struct PokedexEntry*)(0x0890B300))

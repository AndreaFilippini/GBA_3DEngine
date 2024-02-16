#ifndef GUARD_SPRITE_H
#define GUARD_SPRITE_H

#define MAX_SPRITES 64

struct Coords16
{
    s16 x;
    s16 y;
};

struct SpriteSheet
{
    const u8 *data;  // Raw uncompressed pixel data
    u16 size;
    u16 tag;
};

struct CompressedSpriteSheet
{
    const u8 *data;  // LZ77 compressed pixel data
    u16 size;        // Uncompressed size of pixel data
    u16 tag;
};

struct SpriteFrameImage
{
    const u8 *data;
    u16 size;
};

#define obj_frame_tiles(ptr) {.data = (u8 *)ptr, .size = sizeof ptr}

#define overworld_frame(ptr, width, height, frame) {.data = (u8 *)ptr + (width * height * frame * 64)/2, .size = (width * height * 64)/2}

struct SpritePalette
{
    const u16 *data;  // Raw uncompressed palette data
    u16 tag;
};

struct CompressedSpritePalette
{
    const u8 *data;  // LZ77 compressed palette data
    u16 tag;
};

struct AnimFrameCmd
{
    // If the sprite has an array of images, this is the array index.
    // If the sprite has a sheet, this is the tile offset.
    u32 imageValue:16;

    u32 duration:6;
    u32 hFlip:1;
    u32 vFlip:1;
};

struct AnimLoopCmd
{
    u32 type:16;
    u32 count:6;
};

struct AnimJumpCmd
{
    u32 type:16;
    u32 target:6;
};

// The first halfword of this union specifies the type of command.
// If it -2, then it is a jump command. If it is -1, then it is the end of the script.
// Otherwise, it is the imageValue for a frame command.
union AnimCmd
{
    s16 type;
    struct AnimFrameCmd frame;
    struct AnimLoopCmd loop;
    struct AnimJumpCmd jump;
};

#define ANIMCMD_FRAME(...) \
    {.frame = {__VA_ARGS__}}
#define ANIMCMD_LOOP(_count) \
    {.loop = {.type = -3, .count = _count}}
#define ANIMCMD_JUMP(_target) \
    {.jump = {.type = -2, .target = _target}}
#define ANIMCMD_END \
    {.type = -1}

struct AffineAnimFrameCmd
{
    s16 xScale;
    s16 yScale;
    u8 rotation;
    u8 duration;
};

struct AffineAnimLoopCmd
{
    s16 type;
    s16 count;
};

struct AffineAnimJumpCmd
{
    s16 type;
    u16 target;
};

struct AffineAnimEndCmdAlt
{
    s16 type;
    u16 val;
};

union AffineAnimCmd
{
    s16 type;
    struct AffineAnimFrameCmd frame;
    struct AffineAnimLoopCmd loop;
    struct AffineAnimJumpCmd jump;
    struct AffineAnimEndCmdAlt end; // unused in code
};

#define AFFINEANIMCMDTYPE_LOOP 0x7FFD
#define AFFINEANIMCMDTYPE_JUMP 0x7FFE
#define AFFINEANIMCMDTYPE_END  0x7FFF

#define AFFINEANIMCMD_FRAME(_xScale, _yScale, _rotation, _duration) \
    {.frame = {.xScale = _xScale, .yScale = _yScale, .rotation = _rotation, .duration = _duration}}
#define AFFINEANIMCMD_LOOP(_count) \
    {.loop = {.type = AFFINEANIMCMDTYPE_LOOP, .count = _count}}
#define AFFINEANIMCMD_JUMP(_target) \
    {.jump = {.type = AFFINEANIMCMDTYPE_JUMP, .target = _target}}
#define AFFINEANIMCMD_END \
    {.type = AFFINEANIMCMDTYPE_END}
#define AFFINEANIMCMD_END_ALT(_val) \
    {.end = {.type = AFFINEANIMCMDTYPE_END, .val = _val}}

struct AffineAnimState
{
    u8 animNum;
    u8 animCmdIndex;
    u8 delayCounter;
    u8 loopCounter;
    s16 xScale;
    s16 yScale;
    u16 rotation;
};

enum
{
    SUBSPRITES_OFF,
    SUBSPRITES_ON,
    SUBSPRITES_IGNORE_PRIORITY, // on but priority is ignored
};

struct Subsprite
{
    u16 x;
    u16 y;
    u16 shape:2;
    u16 size:2;
    u16 tileOffset:10;
    u16 priority:2;
};

struct SubspriteTable
{
    u8 subspriteCount;
    const struct Subsprite *subsprites;
};

struct Sprite;

struct SpriteTemplate
{
    u16 tileTag;
    u16 paletteTag;
    const struct OamData *oam;
    const union AnimCmd *const *anims;
    const struct SpriteFrameImage *images;
    const union AffineAnimCmd *const *affineAnims;
    void (*callback)(struct Sprite *);
};

typedef void (*SpriteCallback)(struct Sprite *);

struct Sprite
{
    /*0x00*/ struct OamData oam;
    /*0x08*/ const union AnimCmd *const *anims;
    /*0x0C*/ const struct SpriteFrameImage *images;
    /*0x10*/ const union AffineAnimCmd *const *affineAnims;
    /*0x14*/ const struct SpriteTemplate *template;
    /*0x18*/ const struct SubspriteTable *subspriteTables;
    /*0x1C*/ SpriteCallback callback;

	
    //struct Coords16 pos1;
    //struct Coords16 pos2;
	
    /*0x20*/ s16 x, y;
    /*0x24*/ s16 x2, y2;
    /*0x28*/ s8 centerToCornerVecX;
    /*0x29*/ s8 centerToCornerVecY;

    /*0x2A*/ u8 animNum;
    /*0x2B*/ u8 animCmdIndex;
    /*0x2C*/ u8 animDelayCounter:6;
             u8 animPaused:1;
             u8 affineAnimPaused:1;
    /*0x2D*/ u8 animLoopCounter;

    // general purpose data fields
    /*0x2E*/ s16 data[8];

    /*0x3E*/ u16 inUse:1;               //1
             u16 coordOffsetEnabled:1;  //2
             u16 invisible:1;           //4
             u16 flags_3:1;             //8
             u16 flags_4:1;             //0x10
             u16 flags_5:1;             //0x20
             u16 flags_6:1;             //0x40
             u16 flags_7:1;             //0x80
    /*0x3F*/ u16 hFlip:1;
             u16 vFlip:1;
             u16 animBeginning:1;
             u16 affineAnimBeginning:1;
             u16 animEnded:1;
             u16 affineAnimEnded:1;
             u16 usingSheet:1;
             u16 flags_f:1;

    /*0x40*/ u16 sheetTileStart;

    /*0x42*/ u8 subspriteTableNum:6;
             u8 subspriteMode:2;

    /*0x43*/ u8 subpriority;
};

struct OamMatrix
{
    s16 a;
    s16 b;
    s16 c;
    s16 d;
};

struct MonIconSpriteTemplate
{
    const struct OamData *oam;
    const u8 *image;
    const union AnimCmd *const *anims;
    const union AffineAnimCmd *const *affineAnims;
    void (*callback)(struct Sprite *);
    u16 paletteTag;
};

#define gDummyOamData (*(struct OamData*)0x081E28F8)
#define gDummySpriteAnimTable ((const union AnimCmd* const*)0x081E2904)

#define gDummySpriteAffineAnimTable ((const union AffineAnimCmd* const*)0x081E2910)

#define SpriteCallbackDummy ((void*)(0x08001210+1))

#define gSpriteCoordOffsetX (*(s16*)0x030024D0)
#define gSpriteCoordOffsetY (*(s16*)0x030027E0)

#define gReservedSpritePaletteCount (*(u8*)0x030028F8)
#define gReservedSpriteTileCount (*(s16*)0x030024D8)
#define gOamLimit (*(s16*)0x030024D4)

#define gSprites ((struct Sprite*)(0x02020004))
#define gOamMatrices ((struct OamMatrix*)0x030027F0)

#endif
struct Follower
{
	bool8 inProgress;
	u8 objId;
	u8 currentSprite;
	u8 delayedState;
	struct
	{
		u8 id;
		u8 number;
		u8 group;
	} map;
	bool8 warpEnd;
	//struct Coords16 log;
	const u8* script;
	u16 flag;
	u16 gfxId;
	u8 flags;
	u8 locked;
	bool8 createSurfBlob;
	bool8 comeOutDoorStairs;
	u8 ledge;
};

#define FOLLOWER_FLAG_HAS_RUNNING_FRAMES 0x1
#define FOLLOWER_FLAG_CAN_BIKE 0x2
#define FOLLOWER_FLAG_CAN_LEAVE_ROUTE 0x4
#define FOLLOWER_FLAG_CAN_SURF 0x8
#define FOLLOWER_FLAG_CAN_WATERFALL 0x10
#define FOLLOWER_FLAG_CAN_DIVE 0x20
#define FOLLOWER_FLAG_CAN_ROCK_CLIMB 0x40
#define FOLLOWER_FLAG_CLEAR_ON_WHITE_OUT 0x80

#define DEFAULT_FOLLOWER_LOCAL_ID 0xFE
#define DEFAULT_FOLLOWER_GFX_ID 7

#define FOLLOWER_SPRITE_INDEX_BIKE 56
#define FOLLOWER_SPRITE_INDEX_SURF 42
#define FOLLOWER_SPRITE_INDEX_UNDERWATER 112

#define FOLLOWER_SPRITE_INDEX_NORMAL 	DEFAULT_FOLLOWER_GFX_ID

#define gFollowerState (*(struct Follower *)0x02024FD0)

#define Var8000 (*(u16*)0x0202E8C4)
#define Var8001 (*(u16*)0x0202E8C6)
#define Var800D (*(u16*)0x0202E8DE)
#define Var800F (*(u16*)0x0202E8F2)

u8 TrySpawnObjectEvent(struct ObjectEventTemplate *objectEventTemplate, u8 mapNum, u8 mapGroup, s16 cameraDeltaX, s16 cameraDeltaY);
int CreateTask(void*addr, u8 priority);
void DestroyTask(u8 taskId);
void DestroySprite(struct Sprite *sprite);
void ObjectEventTurn(struct ObjectEvent *objectEvent, u8 direction);
struct ObjectEventTemplate *GetObjectEventTemplateByLocalIdAndMap(u8 localId, u8 mapNum, u8 mapGroup);
u8 GetPlayerFacingDirection(void);
void Overworld_SetObjEventTemplateCoords(u8 localId, s16 x, s16 y);
void ObjectEventClearHeldMovementIfActive(struct ObjectEvent *objectEvent);
bool8 ObjectEventSetHeldMovement(struct ObjectEvent *objectEvent, u8 movementActionId);
void ObjectEventClearHeldMovement(struct ObjectEvent *);
bool8 ObjectEventClearHeldMovementIfFinished(struct ObjectEvent *objectEvent);
void Overworld_SetObjEventTemplateMovementType(u8 localId, u8 movementType);
void BindFieldEffectToSprite(u8 spriteId, u8 value);
void UnfreezeObjectEvents(void);
u8 DoBobbingFieldEffect(u8 spriteId);
u32 FieldEffectStart(u8 id);
u8 TestPlayerAvatarFlags(u8 a);
void RemoveObjectEvent(struct ObjectEvent *objectEvent);
s8 FieldAnimateDoorClose(u32, u32);
s8 FieldAnimateDoorOpen(u32 x, u32 y);
u32 GetDoorSoundEffect(u32 x, u32 y);
bool8 ScriptContext2_IsEnabled(void);
void sub_805C058(struct ObjectEvent *objectEvent, s16 x, s16 y);
void SetPlayerAvatarTransitionFlags(u16 a);
void FreezeObjectEvents(void);
void PlaySE(u16 songNum);
u8 GetObjectEventIdByLocalIdAndMap(u8 localId, u8 mapNum, u8 mapGroup);
bool8 walkrun_is_standing_still(void);
void TryFadeOutOldMapMusic(void);
void WarpFadeScreen(void);
void PlayRainSoundEffect(void);
void sub_8080958(u8 arg);
void PlayerGetDestCoords(s16* x, s16* y);
bool8 MetatileBehavior_IsIce(u8 metatileBehavior);
u8 GetJumpMovementAction(u32 direction);
u16 VarGet(u16 id);
const u8 *GetObjectEventScriptPointerByObjectEventId(u8 objectEventId);
void RemoveObjectEventByLocalIdAndMap(u8 localId, u8 mapNum, u8 mapGroup);
bool8 TryGetObjectEventIdByLocalIdAndMap(u8 localId, u8 mapNum, u8 mapGroup, u8 *objectEventId);

//==========================================================================================================================================
//==========================================================================================================================================


u8 TrySpawnObjectEvent(struct ObjectEventTemplate *objectEventTemplate, u8 mapNum, u8 mapGroup, s16 cameraDeltaX, s16 cameraDeltaY){
	int (*func)() = (int (*)(void))0x0805B168+1;
	return func(objectEventTemplate, mapNum, mapGroup, cameraDeltaX, cameraDeltaY);
}

void DestroySprite(struct Sprite *sprite){
	int (*func)(void) = (int (*)(void))0x08000ED0+1;
	func();
}

int CreateTask(void*addr, u8 priority){
	int (*func)(void) = (int (*)(void))0x0807AA88+1;
	return func();
}

void DestroyTask(u8 taskId){
	int (*func)(void) = (int (*)(void))0x0807AB74+1;
	func();
}

void FlagSet(u16 flag){
	int (*func)(void) = (int (*)(void))0x080692F0+1;
	func();
}

void ObjectEventTurn(struct ObjectEvent *objectEvent, u8 direction){
	int (*func)(void) = (int (*)(void))0x0805BAC0+1;
	func();
}

struct ObjectEventTemplate *GetObjectEventTemplateByLocalIdAndMap(u8 localId, u8 mapNum, u8 mapGroup){
	int (*func)(void) = (int (*)(void))0x0805C678+1;
	return (struct ObjectEventTemplate *)func();
}

u8 GetPlayerFacingDirection(void){
	int (*func)(void) = (int (*)(void))0x08059738+1;
	return func();
}

void sub_805C058(struct ObjectEvent *objectEvent, s16 x, s16 y){
	int (*func)(void) = (int (*)(void))0x805C058+1;
	func();
}

/*
void Overworld_SetObjEventTemplateCoords(u8 localId, s16 x, s16 y){
	int (*func)(void) = (int (*)(void))0x080531C0+1;
	func();
}*/

void ObjectEventClearHeldMovementIfActive(struct ObjectEvent *objectEvent){
	int (*func)(void) = (int (*)(void))0x080605B4+1;
	func();
}

bool8 ObjectEventSetHeldMovement(struct ObjectEvent *objectEvent, u8 movementActionId){
	int (*func)(void) = (int (*)(void))0x08060550+1;
	return func();
}

void ObjectEventClearHeldMovement(struct ObjectEvent *objectEvent){
	int (*func)(void) = (int (*)(void))0x080605CC+1;
	func();
}

bool8 ObjectEventClearHeldMovementIfFinished(struct ObjectEvent *objectEvent){
	int (*func)(void) = (int (*)(void))0x08060614+1;
	return func();
}

void Overworld_SetObjEventTemplateMovementType(u8 localId, u8 movementType){
	int (*func)(void) = (int (*)(void))0x080531F4+1;
	func();
}

void BindFieldEffectToSprite(u8 spriteId, u8 value){
	int (*func)(void) = (int (*)(void))0x08127ED0+1;
	func();	
}

u8 DoBobbingFieldEffect(u8 spriteId){
	int (*func)(void) = (int (*)(void))0x08128124+1;
	return func();	
}

u32 FieldEffectStart(u8 id){
	int (*func)(void) = (int (*)(void))0x08085668+1;
	return func();	
}

void UnfreezeObjectEvents(void){
	int (*func)(void) = (int (*)(void))0x0806451C+1;
	func();	
}

u8 TestPlayerAvatarFlags(u8 a){
	int (*func)(void) = (int (*)(void))0x080597C0+1;
	return func();	
}

void RemoveObjectEvent(struct ObjectEvent *objectEvent){
	int (*func)(void) = (int (*)(void))0x0805AEF0+1;
	func();	
}

s8 FieldAnimateDoorClose(u32 x, u32 y){
	int (*func)(void) = (int (*)(void))0x0805870C+1;
	return func();	
}

s8 FieldAnimateDoorOpen(u32 x, u32 y){
	int (*func)(void) = (int (*)(void))0x08058744+1;
	return func();	
}

u32 GetDoorSoundEffect(u32 x, u32 y){
	int (*func)(void) = (int (*)(void))0x08058790+1;
	return func();	
}

bool8 ScriptContext2_IsEnabled(void){
	int (*func)(void) = (int (*)(void))0x08065548+1;
	return func();
}

void SetPlayerAvatarTransitionFlags(u16 a){
	int (*func)(void) = (int (*)(void))0x08059058+1;
	func();	
}

void FreezeObjectEvents(void){
	int (*func)(void) = (int (*)(void))0x08064434+1;
	func();
}

void PlaySE(u16 songNum){
	int (*func)(void) = (int (*)(void))0x08075494+1;
	func();
}

u8 GetObjectEventIdByLocalIdAndMap(u8 localId, u8 mapNum, u8 mapGroup){
	int (*func)(void) = (int (*)(void))0x0805AB84+1;
	return func();
}

bool8 walkrun_is_standing_still(void){
	int (*func)(void) = (int (*)(void))0x08064CC0+1;
	return func();
}

void TryFadeOutOldMapMusic(void){
	int (*func)(void) = (int (*)(void))0x08053FF8+1;
	func();
}

void WarpFadeScreen(void){
	int (*func)(void) = (int (*)(void))0x08080918+1;
	func();
}

void PlayRainSoundEffect(void){
	int (*func)(void) = (int (*)(void))0x0807DDB8+1;
	func();
}

void sub_8080958(u8 arg){
	int (*func)(void) = (int (*)(void))0x08080958+1;
	func();
}

void PlayerGetDestCoords(s16* x, s16* y){
	int (*func)(void) = (int (*)(void))0x08059708+1;
	func();
}

bool8 MetatileBehavior_IsIce(u8 metatileBehavior){
	int (*func)(void) = (int (*)(void))0x08056E84+1;
	return func();
}

u8 GetJumpMovementAction(u32 direction){
	int (*func)(void) = (int (*)(void))0x080608A4+1;
	return func();
}

u16 VarGet(u16 id){
	int (*func)(void) = (int (*)(void))0x08069254+1;
	return func();
}

const u8 *GetObjectEventScriptPointerByObjectEventId(u8 objectEventId){
	int (*func)(void) = (int (*)(void))0x0805C56C+1;
	return (const u8*)func();
}

void RemoveObjectEventByLocalIdAndMap(u8 localId, u8 mapNum, u8 mapGroup){
	int (*func)(void) = (int (*)(void))0x0805AF04+1;
	func();
}

bool8 TryGetObjectEventIdByLocalIdAndMap(u8 localId, u8 mapNum, u8 mapGroup, u8 *objectEventId){
	int (*func)(void) = (int (*)(void))0x0805ABA8+1;
	return func();
}

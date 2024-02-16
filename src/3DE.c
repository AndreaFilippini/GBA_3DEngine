#include "lib/gba_types.h"
#include "lib/gba_keys.h"
#include "lib/gba_regs.h"
#include "lib/palette.h"
#include "lib/main.h"
#include "lib/task.h"
#include "lib/macro.h"
#include "lib/defines.h"
#include "lib/text.h"
#include "lib/item.h"
#include "lib/item_menu.h"
#include "lib/species.h"
#include "lib/sprite.h"
#include "lib/pokemon.h"
#include "lib/global.h"

struct point{
	s16 x;
	s16 y;
	s16 z;
};

struct colorGBA{
	u8 r;
	u8 g;
	u8 b;
};

#define StartROM	0x08700000
#define CANVAS		((u32*)0x06008000)
#define RAWBG0		((u16*)0x0600F800)
#define SETPAL 		((u16*)0x0202F068)
#define SETPAL2		((u16*)0x0202EC68)

#define StartRAM	0x0202A000
#define vertexNum	426

#define modelIndex 	(*(u16*)StartRAM)
#define angleValue 	(*(u16*)(StartRAM + 0x02))
#define translateX 	(*(s16*)(StartRAM + 0x04))
#define translateY 	(*(s16*)(StartRAM + 0x06))
#define vertexPos	((struct point*)(StartRAM + 0x08))

void main();
void setUp();
void loadCanvas();
void loadModelPal();
void waitFade(u8 taskId);
void DEngine(u8 taskId);
void endRender(u8 taskId);

struct point *sanitizeCoords(struct point *p);
void setSanitizePixel(struct point *p, u16 color);
void setPixelToCanvas(u8 x, u8 y, u8 color);
struct point projectPoint(struct point *p, u16 angle, u8 table);
void render3D(u16 angle, u8 model);
void drawLine(struct point *p1, struct point *p2, u16 color);

void ScanlineEffect_Stop();
void ResetTasks();
void ResetSpriteData();
void ResetPaletteFade();
void FreeAllSpritePalettes();
void SetVBlankCallback(void*addr);
void SetMainCallback2(void*addr);
s16 Sin2(u16 angle);
s16 Cos2(u16 angle);
int CreateTask(void*addr, u8 priority);
void BeginNormalPaletteFade(u32 arg1, u8 arg2, u8 arg3, u8 arg4, u16 arg5);
void DestroyTask(u8 taskId);
void RunTasks();
void AnimateSprites();
void BuildOamBuffer();
void UpdatePaletteFade();
void LoadOam();
void ProcessSpriteCopyRequests();
void TransferPlttBuffer();
void VBlankCB();
void MainCB2();

void main(){

	SetVBlankCallback(NULL);

    	REG_DISPCNT = 0;
    	REG_BG3CNT = 0;
    	REG_BG2CNT = 0;
    	REG_BG1CNT = 0;
    	REG_BG0CNT = 0;
    	REG_BG3HOFS = 0;
    	REG_BG3VOFS = 0;
    	REG_BG2HOFS = 0;
    	REG_BG2VOFS = 0;
    	REG_BG1HOFS = 0;
    	REG_BG1VOFS = 0;
    	REG_BG0HOFS = 0;
    	REG_BG0VOFS = 0;

	DmaFill16(3, 0, VRAM, VRAM_SIZE);
    	DmaFill32(3, 0, OAM, OAM_SIZE);
    	DmaFill16(3, 0, PLTT, PLTT_SIZE);

	DmaFill16(3, 0, VRAM, VRAM_SIZE);
    	DmaFill32(3, 0, OAM, OAM_SIZE);
    	DmaFill16(3, 0, (void *)(PLTT + 2), PLTT_SIZE - 2);

	ResetPaletteFade();
   	ScanlineEffect_Stop();
    	ResetTasks();
    	ResetSpriteData();
    	FreeAllSpritePalettes();

	SetVBlankCallback((void*)(StartROM + VBlankCB + 1));
    	SetMainCallback2((void*)(StartROM + MainCB2 + 1));

	REG_BG0CNT = BGCNT_PRIORITY(0) | BGCNT_CHARBASE(2) | BGCNT_SCREENBASE(31) | BGCNT_16COLOR | BGCNT_TXT256x256;
	REG_DISPCNT = DISPCNT_MODE_0 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG0_ON | DISPCNT_BG1_ON | DISPCNT_BG2_ON | DISPCNT_BG3_ON | DISPCNT_OBJ_ON | DISPCNT_WIN0_ON | DISPCNT_WIN1_ON;

	modelIndex = 0;
	angleValue = 0;
	translateX = 0;
	translateY = 30;

	loadCanvas();
	loadModelPal();
	BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
	u8 taskId = CreateTask((void*)(StartROM + waitFade + 1), 0xFF);
}

struct colorGBA palModels[] = {
	{219,188,157},
	{240,220,115},
	{244,165,200},
};

#define paletteTable	((struct colorGBA *) ((int) StartROM + (int)&palModels))

#include "models/model.h"
#include "models/model2.h"
#include "models/model3.h"

void loadCanvas(){
	int i, k;
	for(k = 0; k < (DISPLAY_HEIGHT / 8); k++){
		for(i = 0; i < (DISPLAY_WIDTH / 8); i++){
			RAWBG0[i + (k*(BG_STANDARD_WIDTH/8))] = 0xD000 | (i + (k*(DISPLAY_WIDTH/8)));
		}
	}
}

void loadModelPal(){
	int i;
	s8 lightX, lightY, lightZ;

	for(i = 1; i < 16; i++){
		lightX = (paletteTable[modelIndex].r >> 3) - (32 - (i << 1));
		lightY = (paletteTable[modelIndex].g >> 3) - (32 - (i << 1));
		lightZ = (paletteTable[modelIndex].b >> 3) - (32 - (i << 1));

		if(lightX < 0) lightX = 0;
		if(lightY < 0) lightY = 0;
		if(lightZ < 0) lightZ = 0;

		SETPAL[i] = RGB(lightX, lightY, lightZ);
		SETPAL2[i] = RGB(lightX, lightY, lightZ);
	}
}

void waitFade(u8 taskId){
	if (!gPaletteFade.active)
		gTasks[taskId].func = (void*)(StartROM + DEngine + 1);
}

void DEngine(u8 taskId){

	angleValue += 5;
	if(angleValue > 359)
		angleValue = 0;

	if (gMain.newKeys & DPAD_UP){
		modelIndex = 0;
		loadModelPal();
	}else if (gMain.newKeys & DPAD_RIGHT){
		modelIndex = 1;
		loadModelPal();
	}else if (gMain.newKeys & DPAD_DOWN){
		modelIndex = 2;
		loadModelPal();
	}else if(gMain.newKeys & B_BUTTON){
		BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
		gTasks[taskId].func = (void*)(StartROM + endRender + 1);
	}

	render3D(angleValue, modelIndex);
}

void endRender(u8 taskId){
	if (!gPaletteFade.active){
		DestroyTask(taskId);
		SetMainCallback2((void*)0x80546F0+1);
	}
}

void render3D(u16 angle, u8 model){
	int i, k;

	//draw vertex
	for (i = 0; i < vertexNum; i++){
		//clean old vertex
		setSanitizePixel(&vertexPos[i], 0);

		//draw new vertex
		struct point *models[] = {
			(struct point *) ((int) StartROM + (int)&points[i]),
			(struct point *) ((int) StartROM + (int)&points2[i]),
			(struct point *) ((int) StartROM + (int)&points3[i]),
		};

		struct point *pointer = models[model];

		struct point p3 = projectPoint(pointer, angle, 1);

		vertexPos[i] = p3;
	
		//calc light
		s16 output_end, output_start;
		output_end = 60; output_start = -60;
		u8 lightVal = 31 - (10 + (((output_end - output_start) - p3.z) >> 4));
	
		setSanitizePixel(&p3, (lightVal >> 1) + 2);
	}
}

struct point projectPoint(struct point *p, u16 angle, u8 table){
	struct point newPoint;

	//rotate on XY
	/*u16 projectionTable[9];
	projectionTable[0] = 0xCCCC; projectionTable[1] = 0xFFFF; projectionTable[2] = 0;
	projectionTable[3] = 0xFF00; projectionTable[4] = 0xCCCC; projectionTable[5] = 0;
	projectionTable[6] = 0; projectionTable[7] = 0; projectionTable[8] = 1;*/

	//rotate on X
	u16 projectionTable[9];
	projectionTable[0] = 1; projectionTable[1] = 0; projectionTable[2] = 0;
	projectionTable[3] = 0; projectionTable[4] = 0xCCCC; projectionTable[5] = 0xFFFF;
	projectionTable[6] = 0; projectionTable[7] = 0xFF00; projectionTable[8] = 0xCCCC;

	//rotate on Y
	u16 projectionTable2[9];
	projectionTable2[0] = 0xCCCC; projectionTable2[1] = 0; projectionTable2[2] = 0xFFFF;
	projectionTable2[3] = 0x0; projectionTable2[4] = 1; projectionTable2[5] = 0;
	projectionTable2[6] = 0xFF00; projectionTable2[7] = 0; projectionTable2[8] = 0xCCCC;
	
	struct point *newPointer = (struct point *) (&p)[0];
	
	s16 matrixPoint[3] = {newPointer->x, newPointer->y, newPointer->z};

	int i, k;

	for(k = 0; k < 3; k++){
		s16 partialSum = 0;
		for (i=0; i<3; i++){
			u16 contr;
	
			switch (table){
				case 0: contr = projectionTable[k*3+i]; break;
				default: contr = projectionTable2[k*3+i]; break;
			}

			s16 contr2 = matrixPoint[i];

			if(contr > 1){
				switch (contr){
					case 0xCCCC:
						contr2 = (contr2 * Cos2(angle)) / 4096;
					break;
					case 0xCCFF:
						contr2 = (contr2 * -Cos2(angle)) / 4096;
					break;
					case 0xFF00:
						contr2 = (contr2 * Sin2(angle)) / 4096;
					break;
					case 0xFFFF:
						contr2 = (contr2 * -Sin2(angle)) / 4096;
					break;
				}
				partialSum += contr2;
			}else partialSum += contr * contr2;

		}

		switch (k){
			case 0: newPoint.x = partialSum; break;
			case 1: newPoint.y = partialSum; break;
			case 2: newPoint.z = partialSum; break;
		}
		
	}

	return newPoint;
}

void setSanitizePixel(struct point *p, u16 color){
       	s16 newX, newY;

	//translate from cartesian origin
	if(p->x >= 0){
		newX = ((DISPLAY_WIDTH / 2) + p->x);
	}else{
		newX = ((DISPLAY_WIDTH / 2) - (p->x * -1));
	}

	if(p->y >= 0){
		newY = ((DISPLAY_HEIGHT / 2) - p->y);
	}else{
		newY = ((DISPLAY_HEIGHT / 2) + (p->y * -1));
	}

	//draw pixels
	setPixelToCanvas(newX + translateX + 1, newY + translateY, color);
	setPixelToCanvas(newX + translateX - 1, newY + translateY, color);
	setPixelToCanvas(newX + translateX, newY + translateY, color);
	setPixelToCanvas(newX + translateX, newY + translateY + 1, color);
	setPixelToCanvas(newX + translateX, newY + translateY - 1, color);
}

void setPixelToCanvas(u8 x, u8 y, u8 color){
    	u8 shift = ((x - ((x >> 3) << 3)) << 2);
    	u32 offset = (((x >> 3) << 5) >> 2) + (((y >> 3) * 0x3C0) >> 2) + (y - ((y >> 3) << 3));
    	u32 lastValue = CANVAS[offset];
    	u32 mask = (lastValue & (~(0xF << shift)));
    	CANVAS[offset] = mask | ((color & 0xF) << shift);
}

//===================================================================================

void VBlankCB(){
    	LoadOam();
    	ProcessSpriteCopyRequests();
    	TransferPlttBuffer();
}

void MainCB2(){
    	RunTasks();
    	AnimateSprites();
    	BuildOamBuffer();
    	UpdatePaletteFade();
}

void ScanlineEffect_Stop(){
	int (*func)(void) = (int (*)(void))0x08089578+1;
	func();
}

void ResetTasks(){
	int (*func)(void) = (int (*)(void))0x0807AA28+1;
	func();
}

void ResetSpriteData(){
	int (*func)(void) = (int (*)(void))0x08000748+1;
	func();
}

void ResetPaletteFade(){
	int (*func)(void) = (int (*)(void))0x08073B94+1;
	func();
}

void FreeAllSpritePalettes(){
	int (*func)(void) = (int (*)(void))0x080025C8+1;
	func();
}

void SetVBlankCallback(void*addr){
	int (*func)(void) = (int (*)(void))0x08000540+1;
	func();
}

void SetMainCallback2(void*addr){
	int (*func)(void) = (int (*)(void))0x080003CC+1;
	func();
}

s16 Sin2(u16 angle){
	int (*func)(void) = (int (*)(void))0x08040E28+1;
	return func();
}

s16 Cos2(u16 angle){
	int (*func)(void) = (int (*)(void))0x08040E6C+1;
	return func();
}

void BeginNormalPaletteFade(u32 arg1, u8 arg2, u8 arg3, u8 arg4, u16 arg5){
	int (*func)() = (int (*)(void))0x08073BF4+1;
	func(arg1,arg2,arg3,arg4,arg5);
}

int CreateTask(void*addr, u8 priority){
	int (*func)(void) = (int (*)(void))0x0807AA88+1;
	return func();
}

void DestroyTask(u8 taskId){
	int (*func)(void) = (int (*)(void))0x0807AB74+1;
	func();
}

void LoadOam(){
	int (*func)(void) = (int (*)(void))0x08000F70+1;
	func();
}

void ProcessSpriteCopyRequests(){
	int (*func)(void) = (int (*)(void))0x08001214+1;
	func();
}

void TransferPlttBuffer(){
	int (*func)(void) = (int (*)(void))0x08073AE0+1;
	func();
}

void RunTasks(){
	int (*func)(void) = (int (*)(void))0x0807ABE4+1;
	func();
}

void AnimateSprites(){
	int (*func)(void) = (int (*)(void))0x08000794+1;
	func();
}

void BuildOamBuffer(){
	int (*func)(void) = (int (*)(void))0x080007E0+1;
	func();
}

void UpdatePaletteFade(){
	int (*func)(void) = (int (*)(void))0x08073B3C+1;
	func();
}

#include "lib/thumb.h"

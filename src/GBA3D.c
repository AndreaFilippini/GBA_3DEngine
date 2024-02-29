#include "lib/gba_types.h"
#include "lib/gba_keys.h"
#include "lib/gba_regs.h"
#include "lib/gba_io.h"
#include "lib/macro.h"
#include "lib/defines.h"
#include "lib/math.h"

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

struct triangle{
	struct point p1;
	struct point p2;
	struct point p3;
};

#define ZBUFFER		((s16*)0x02030000)
#define CANVAS		((u16*)0x06000000)
#define SETPAL 		((u16*)0x05000000)
#define DEPTH_LIMIT	-2000

#define StartRAM	0x0202A000
#define modelIndex 	(*(u16*)StartRAM)
#define angleValue 	(*(u16*)(StartRAM + 0x02))
#define translateX 	(*(s16*)(StartRAM + 0x04))
#define translateY 	(*(s16*)(StartRAM + 0x06))
#define currentBuff 	(*(s16*)(StartRAM + 0x08))
#define vertexPos	((struct point*)(StartRAM + 0x0A))

struct colorGBA palModels[] = {
	{219,188,157},
	{240,220,115},
	{244,165,200},
};

#include "models/_model.h"	//random obj
#include "models/_model2.h"	//torus
#include "models/_model3.h"	//sphere

int main();
void initScreenValue();
void loadCanvas();
void loadModelPal();
void initZBuffer();
void render3D(u16 angle, u8 model, int vertexNum);
struct point *sanitizeCoords(struct point *p);
void setSanitizePixel(struct point *p, u16 color);
void setPixelToCanvas(u8 x, u8 y, u8 color);
u16 getPixelOfCanvas(struct point *p);
struct point projectPoint(struct point *p, u16 angle, u8 table);
struct point translatePoint (struct point *p);
void drawLine(struct point *p1, struct point *p2, u16 color);
void drawTriangle(struct triangle *t, u16 color, u8 fill);
void fillTriangle(struct triangle *t, u16 color, u8 fill);
u8 calcLightValue(s16 z);
int abs(int a);
s16 Sin2(u16 angle);
s16 Cos2(u16 angle);
void resetScreen();
void refreshModelValue(u8 model, int *totalPoints);
int getVertexNum(u8 model);
s16 getZBuffer(struct point *p);
void setZBuffer(struct point *p);

int main(){

	loadModelPal();
	initZBuffer();
	initScreenValue();

	int totalPoints = getVertexNum(modelIndex);

	for(;;){
		if(angleValue > 359)
			angleValue = 0;

		if (keyDown(DPAD_UP) && (modelIndex != 0)){
			refreshModelValue(0, &totalPoints);
		}else if (keyDown(DPAD_RIGHT) && (modelIndex != 1)){
			refreshModelValue(1, &totalPoints);
		}else if (keyDown(DPAD_DOWN) && (modelIndex != 2)){
			refreshModelValue(2, &totalPoints);
		}

		render3D(angleValue, modelIndex, totalPoints);
		angleValue += 5;
	}
}

void initScreenValue(){
	REG_DISPCNT = DISPCNT_MODE_3;
	REG_DISPCNT = DISPCNT_MODE_3 | DISPCNT_BG2_ON;

	modelIndex = 0;
	angleValue = 0;
	translateX = 0;
	translateY = 0;
}

void refreshModelValue(u8 model, int *totalPoints){
	resetScreen();
	modelIndex = model;
	*totalPoints = getVertexNum(model);
	loadModelPal();
}

void loadModelPal(){
	int i;
	s8 lightX, lightY, lightZ;

	for(i = 1; i < 16; i++){
		lightX = (palModels[modelIndex].r >> 3) - (32 - (i << 1));
		lightY = (palModels[modelIndex].g >> 3) - (32 - (i << 1));
		lightZ = (palModels[modelIndex].b >> 3) - (32 - (i << 1));

		if(lightX < 0) lightX = 0;
		if(lightY < 0) lightY = 0;
		if(lightZ < 0) lightZ = 0;

		SETPAL[i] = RGB(lightX, lightY, lightZ);
	}
}

void initZBuffer(){
	int i, k;
	for(k = 0; k < DISPLAY_HEIGHT; k++){
		for(i = 0; i < DISPLAY_WIDTH; i++){
			ZBUFFER[i + k*DISPLAY_WIDTH] = DEPTH_LIMIT;
		}
	}
}

void resetScreen(){
	int i, k;
	for(k = 0; k < DISPLAY_HEIGHT; k++){
		for(i = 0; i < DISPLAY_WIDTH; i++){
			setPixelToCanvas(i, k, 0);
		}
	}
}

u8 calcLightValue(s16 z){
	s16 depthLimit = 100;
	s16 output_end, output_start;
	output_end = depthLimit; output_start = -depthLimit;
	return (31 - (10 + (((output_end - output_start) - z) >> 4)));
}

int getVertexNum(u8 model){
	switch (model){
		case 0: return sizeof(points) / (sizeof(struct triangle) / 3); break;
		case 1: return sizeof(points2) / (sizeof(struct triangle) / 3); break;
		case 2: return sizeof(points3) / (sizeof(struct triangle) / 3); break;
	}
}

void render3D(u16 angle, u8 model, int vertexNum){
	int i, k;
	u8 lightVal;

	//draw triangles
	for (i = 0; i < vertexNum; i+=3){
		
		//clean old triangle
		struct triangle t = {vertexPos[i], vertexPos[i+1], vertexPos[i+2]};
		drawTriangle(&t, calcLightValue(t.p3.z), 0);

		for(k = 0; k < 3; k++){
			//clean old vertex
			struct point *models[] = {
				(struct point *)&points[i + k],
				(struct point *)&points2[i + k],
				(struct point *)&points3[i + k],
			};

			struct point *pointer = models[model];
			struct point p3 = projectPoint(pointer, angle, 0);
			struct point p33 = projectPoint(&p3, angle, 1);

			vertexPos[i+k] = p33;
		}
		//draw new triangle
		struct triangle tNew = {vertexPos[i], vertexPos[i+1], vertexPos[i+2]};
		drawTriangle(&tNew, calcLightValue(tNew.p3.z), 1);
	}
}

struct point projectPoint(struct point *p, u16 angle, u8 table){
	struct point newPoint;

	//rotate on X
	u16 projectionTable[9] = {
		1, 0, 0,
		0, 0xCCCC, 0xFFFF,
		0, 0xFF00, 0xCCCC
	};

	//rotate on Y
	u16 projectionTable2[9] = {
		0xCCCC, 0, 0xFFFF,
		0, 1, 0,
		0xFF00, 0, 0xCCCC
	};
	
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

void drawLine(struct point *p1, struct point *p2, u16 color){
	struct point *temp;
	int i, k;
	s16 tempX, tempY;

	s16 angular = (p2->y - p1->y) / (p2->x - p1->x);
	
	if(p1->x == p2->x){			//completly vertical
		if(p1->y > p2->y){
			temp = p1;
			p1 = p2;
			p2 = temp;
		}

		for(i = p1->y; i < p2->y; i++){
			struct point d; d.x = p1->x; d.y = i; d.z = p1->z;
			setSanitizePixel(&d, color);
		}

	}else if(angular == 0){			//horizontal line
		if(p1->x > p2->x){
			temp = p1;
			p1 = p2;
			p2 = temp;
		}
		u16 incr = (p1->y > p2->y) ? -1 : 1;
		u16 segment = abs(p1->y - p2->y) + 1;
		u16 length = (p2->x - p1->x) / segment;
		u16 fixLength = (p2->x - p1->x) % segment;
		u8 internalCounter = 0;
		
		for(k = 0; k < segment; k++){
			u8 internalIncr = (fixLength > 0) ? 1 : 0;
			for (i = 0; i < length + internalIncr; i++){
				struct point d; d.x = p1->x + i + internalCounter; d.y = p1->y + (k * incr);  d.z = p1->z;
				setSanitizePixel(&d, color);
			}
			//shift from previous segment
			internalCounter += (internalIncr + length);

			//remaining pixels
			if(fixLength != 0)
				fixLength--;
		}
	}else{					//vertical line
		if(p1->y > p2->y){
			temp = p1;
			p1 = p2;
			p2 = temp;
		}
		
		u16 incr = (p1->x > p2->x) ? -1 : 1;
		u16 segment = abs(p1->x - p2->x) + 1;
		u16 length = (p2->y - p1->y) / segment;
		u16 fixLength = (p2->y - p1->y) % segment;
		u8 internalCounter = 0;

		for(k = 0; k < segment; k++){
			u8 internalIncr = (fixLength > 0) ? 1 : 0;

			for (i = 0; i < length + internalIncr; i++){
				struct point d; d.y = p1->y + i + internalCounter; d.x = p1->x + (k * incr);  d.z = p1->z;
				setSanitizePixel(&d, color);
			}
			//shift from previous segment
			internalCounter += (internalIncr + length);

			//remaining pixels
			if(fixLength != 0)
				fixLength--;
		}

	}
}

void setSanitizePixel(struct point *p, u16 color){
	struct point newPoint = translatePoint(p);
	if((color == 0) || (newPoint.z >= getZBuffer(&newPoint)) )  {

		if(color == 0)
			newPoint.z = DEPTH_LIMIT;

		setZBuffer(&newPoint);
		setPixelToCanvas(newPoint.x, newPoint.y, color);
	}
}

void setPixelToCanvas(u8 x, u8 y, u8 color){
	CANVAS[x + y*DISPLAY_WIDTH] = SETPAL[color];
}

u16 getPixelOfCanvas(struct point *p){
	struct point newPoint = translatePoint(p);
	return CANVAS[newPoint.x + newPoint.y*DISPLAY_WIDTH];
}

void setZBuffer(struct point *p){
	ZBUFFER[p->x + p->y*DISPLAY_WIDTH] = p->z;
}

s16 getZBuffer(struct point *p){
	return ZBUFFER[p->x + p->y*DISPLAY_WIDTH];
}

struct point translatePoint(struct point *p){
	s16 newX, newY;

	//translate from origin
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

	struct point newPoint = {newX + translateX, newY + translateY, p->z};
	return newPoint;
}

void drawTriangle(struct triangle *t, u16 color, u8 fill){
	drawLine(&t->p1, &t->p2, color*fill);
	drawLine(&t->p2, &t->p3, color*fill);
	drawLine(&t->p3, &t->p1, color*fill);
	//fillTriangle(t, color, fill);
}

void fillTriangle(struct triangle *t, u16 color, u8 fill){
	s16 xMin, xMax, yMin, yMax;

	//coords bounded box for a triangle
	//xMin
	xMin = t->p1.x;
	if(xMin >= t->p2.x){
		xMin = t->p2.x;
	}
	if(xMin >= t->p3.x){
		xMin = t->p3.x;
	}
	//yMin
	yMin = t->p1.y;
	if(yMin >= t->p2.y){
		yMin = t->p2.y;
	}
	if(yMin >= t->p3.x){
		yMin = t->p3.y;
	}
	//xMax
	xMax = t->p1.x;
	if(xMax <= t->p2.x){
		xMax = t->p2.x;
	}
	if(xMax <= t->p3.x){
		xMax = t->p3.x;
	}
	//yMax
	yMax = t->p1.y;
	if(yMax <= t->p2.y){
		yMax = t->p2.y;
	}
	if(yMax <= t->p3.y){
		yMax = t->p3.y;
	}

	int x, y, flag;
	struct point currentP, startP;

	for(y = yMin; y <= yMax; y++){
		flag = 0;
		currentP.y = y;
		for(x = xMin; x <= xMax; x++){
			currentP.x = x;
			if(SETPAL[color] == getPixelOfCanvas(&currentP)){
				flag++;
				if(flag == 1){
					startP = currentP;
				}else{
					drawLine(&startP, &currentP, color*fill);
					break;
				}
			}
		}
	}
}

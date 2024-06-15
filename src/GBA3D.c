#include "lib/main.h"
#include "lib/macro.h"
#include "lib/defines.h"
#include "lib/math.h"
#include "lib/gba_types.h"
#include "lib/gba_keys.h"
#include "lib/gba_regs.h"

//#include "_house.h"	//random obj
#include "_torus.h"	//torus

struct point{
	s16 x;
	s16 y;
	s16 z;
	u16 color;
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
#define VID_CANVAS	((u16*)0x02050000)

#define SETPAL 		((u16*)0x05000000)
#define DEPTH_LIMIT	-2000
#define MAX_SCALING	2
#define MIN_SCALING	0.5
#define STEP_SCALING	0.25

#define StartRAM	0x0202A000
#define modelIndex 	(*(u16*)StartRAM)
#define angleValue 	(*(u16*)(StartRAM + 0x02))
#define translateX 	(*(s16*)(StartRAM + 0x04))
#define translateY 	(*(s16*)(StartRAM + 0x06))
#define currentBuff 	(*(s16*)(StartRAM + 0x08))
#define scalingValue	(*(float*)(StartRAM + 0x0C))
#define minDepth 	(*(s16*)(StartRAM + 0x10))
#define maxDepth 	(*(s16*)(StartRAM + 0x12))
#define vertexPos	((struct point*)(StartRAM + 0x14))

int main();
void initScreenValue();
void initZBuffer();
void render3D(u16 angle);
void setSanitizePixel(struct point *p, s16 color);
void setPixelToCanvas(u8 x, u8 y, u16 colorBase, s16 color);
u16 getPixelOfCanvas(struct point *p);
struct point projectPoint(struct point *p, u16 angle, u8 table);
struct point translatePoint (struct point *p);
void drawLine(struct point *p1, struct point *p2, u16 color);
void drawTriangle(struct triangle *t, s16 color);
void fillTriangle(struct triangle *t, s16 color);
s16 calcLightValue(s16 z);
void resetScreen();
int getVertexNum();
s16 getZBuffer(struct point *p);
void setZBuffer(struct point *p);
void setDepthRange();

int main(){

	initScreenValue();

	for(;;){
		angleValue += 3;
		if(angleValue >= 360)
			angleValue = 0;
		
		render3D(angleValue);
	}
}

void initScreenValue(){
	REG_DISPCNT = DISPCNT_MODE_3 | DISPCNT_BG2_ON;

	angleValue = 210;
	translateX = 0;
	translateY = 0;
	scalingValue = 0.7;

	setDepthRange();
}

void setDepthRange(){
	//find min and max z depth in the model
	struct point *pointer;
	s16 minValue = 0, maxValue = 0;
	int i, vertexNum = getVertexNum();
	for (i = 0; i < vertexNum; i++){
		pointer = (struct point *)&points[i];
		if(pointer->z < minValue){
			minValue = pointer->z;
		}else if(pointer->z > maxValue){
			maxValue = pointer->z;
		}
	}
	minDepth = minValue;
	maxDepth = maxValue;
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
	int k;
	for(k = 0; k < (DISPLAY_WIDTH * DISPLAY_HEIGHT); k++){
		VID_CANVAS[k] = 0;
		ZBUFFER[k] = DEPTH_LIMIT;
	}
}

s16 calcLightValue(s16 z){
	//mapping from z depth to light range
	//maximum light range [-31, 31], the range can be asymmetrical

	int lowerRange = -30, upperRange = 22;
	int oldMax = maxDepth, oldMin = minDepth;
	int newValue = ((upperRange - lowerRange) * (z - oldMin) / (oldMax - oldMin)) + lowerRange;
	return newValue;
}

int getVertexNum(){
	return (sizeof(points) / (sizeof(struct triangle) / 3));
}

void render3D(u16 angle){
	int i, k;
	u8 lightVal;

	struct triangle tNew;
	struct point *pointer, p3;

	resetScreen();

	//draw triangles
	int vertexNum = getVertexNum();
	for (i = 0; i < vertexNum; i+=3){
		for(k = 0; k < 3; k++){
			pointer = (struct point *)&points[i + k];

			p3 = projectPoint(pointer, angle, 0);
			p3 = projectPoint(&p3, angle, 1);

			switch (k){
				case 0: tNew.p1 = p3; break;
				case 1: tNew.p2 = p3; break;
				default: tNew.p3 = p3; break;
			};

			vertexPos[i+k] = p3;
		}
		drawTriangle(&tNew, calcLightValue((tNew.p1.z + tNew.p2.z + tNew.p3.z) / 3));
	}

	//copy from buffer to screen
	DmaCopy16(3, VID_CANVAS, CANVAS, ((DISPLAY_WIDTH * DISPLAY_HEIGHT)*2));
}

struct point projectPoint(struct point *p, u16 angle, u8 table){
	struct point newPoint;

	switch (table){
		//rotate on X
		case 0:
			newPoint.x = p->x * scalingValue;
			newPoint.y = ((p->y * Cos2(angle)) / 4096 + (p->z * Sin2(angle)) / 4096) * scalingValue;
			newPoint.z = ((p->y * -Sin2(angle)) / 4096 + (p->z * Cos2(angle)) / 4096) * scalingValue;
		break;
		//rotate on Y
		case 1:
			newPoint.x = ((p->x * Cos2(angle)) / 4096 +  (p->z * Sin2(angle)) / 4096) * scalingValue;
			newPoint.y = p->y * scalingValue;
			newPoint.z = ((p->x * -Sin2(angle)) / 4096 + (p->z * Cos2(angle)) / 4096) * scalingValue;
		break;
		default:
		//rotate on XY
			newPoint.x = ((p->x * Cos2(angle)) / 4096 + (p->y * Sin2(angle)) / 4096) * scalingValue;
			newPoint.y = ((p->x * -Sin2(angle)) / 4096 + (p->y * Cos2(angle)) / 4096) * scalingValue;
			newPoint.z = p->z * scalingValue;
		break;
	}

	newPoint.color = p->color;
	return newPoint;	
}

//Bresenham's line algorithm
void drawLine(struct point *p1, struct point *p2, u16 color){
	s16 x0, y0, x1, y1;
	x0 = p1->x; x1 = p2->x; y0 = p1->y; y1 = p2->y;
	
	s16 dx = abs(x1 - x0);
   	s16 sx = x0 < x1 ? 1 : -1;
    	s16 dy = -abs(y1 - y0);
    	s16 sy = y0 < y1 ? 1 : -1;
    	s16 error = dx + dy;

	s16 e2;
	struct point p;

	while(1){
		p.x = x0; p.y = y0; p.z = p1->z; p.color = p1->color; 

		setSanitizePixel(&p, color);

        	if ((x0 == x1) && (y0 == y1)) break;

        	e2 = 2 * error;
        	if (e2 >= dy){
            		if (x0 == x1) break;
           		error += dy;
            		x0 += sx;
      		}
        	if (e2 <= dx){
            		if (y0 == y1) break;
            		error += dx;
            		y0 += sy;
       		}
	}
}

void setSanitizePixel(struct point *p, s16 color){
	struct point newPoint = translatePoint(p);
	if (newPoint.z > getZBuffer(&newPoint)){
		setZBuffer(&newPoint);
		setPixelToCanvas(newPoint.x, newPoint.y, newPoint.color, color);
	}
}

u16 getPixelOfCanvas(struct point *p){
	struct point newPoint = translatePoint(p);
	return VID_CANVAS[newPoint.x + newPoint.y*DISPLAY_WIDTH];
}

#define GET_R(color) ((color) & 0x1F)
#define GET_G(color) (((color) >> 5) & 0x1F)
#define GET_B(color) (((color) >> 10) & 0x1F)

void setPixelToCanvas(u8 x, u8 y, u16 colorBase, s16 color){
	int lightR, lightG, lightB;
	lightR = (GET_R(colorBase)) + color;
	lightG = (GET_G(colorBase)) + color;
	lightB = (GET_B(colorBase)) + color;

	if(lightR < 0) lightR = 0;
	if(lightG < 0) lightG = 0;
	if(lightB < 0) lightB = 0;

	if(lightR > 31) lightR = 31;
	if(lightG > 31) lightG = 31;
	if(lightB > 31) lightB = 31;

	VID_CANVAS[x + y*DISPLAY_WIDTH] = RGB(lightR, lightG, lightB);
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

	struct point newPoint = {newX + translateX, newY + translateY, p->z, p->color};
	return newPoint;
}

void drawTriangle(struct triangle *t, s16 color){
	drawLine(&t->p1, &t->p2, color);
	drawLine(&t->p2, &t->p3, color);
	drawLine(&t->p3, &t->p1, color);
	fillTriangle(t, color);
}

void fillTriangle(struct triangle *t, s16 color){
	//s16 xMin, xMax, yMin, yMax;

	//coords bounded box for a triangle
	int sx1 = t->p1.x;
	int sx2 = t->p2.x;
	int sx3 = t->p3.x;
	int sy1 = t->p1.y;
	int sy2 = t->p2.y;
	int sy3 = t->p3.y;

	int xMax = sx1 > sx2 ? (sx1 > sx3 ? sx1 : sx3) : (sx2 > sx3 ? sx2 : sx3);
	int yMax = sy1 > sy2 ? (sy1 > sy3 ? sy1 : sy3) : (sy2 > sy3 ? sy2 : sy3);
	int xMin = sx1 < sx2 ? (sx1 < sx3 ? sx1 : sx3) : (sx2 < sx3 ? sx2 : sx3);
	int yMin = sy1 < sy2 ? (sy1 < sy3 ? sy1 : sy3) : (sy2 < sy3 ? sy2 : sy3);

	int x, y;
	int e1, e2, e3;
	struct point p;
	for(y = yMin; y <= yMax; y++){
        	for(x = xMin; x <= xMax; x++){
            		//half-space functions
			e1 = (t->p1.x - t->p2.x) * (y - t->p1.y) - (t->p1.y - t->p2.y) * (x - t->p1.x);
			e2 = (t->p2.x - t->p3.x) * (y - t->p2.y) - (t->p2.y - t->p3.y) * (x - t->p2.x);
			e3 = (t->p3.x - t->p1.x) * (y - t->p3.y) - (t->p3.y - t->p1.y) * (x - t->p3.x);

			if( ((e1 > 0) && (e2 > 0) && (e3 > 0)) || ((e1 < 0) && (e2 < 0) && (e3 < 0)) ){
               			p.x = x; p.y = y; p.z = t->p3.z; p.color = t->p3.color;
				setSanitizePixel(&p, color);
            		}
        	}	
    	}

}


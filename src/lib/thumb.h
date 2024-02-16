#ifndef THUMB_H
#define THUMB_H

__asm("memcpy:");
__asm("PUSH {R0-R2,LR}");
__asm("LDR R3, = 0x081E26FC+1");
__asm("BL BX_R3");
__asm("POP {R0-R2,PC}");

__asm("memset:");
__asm("PUSH {R0-R2,LR}");
__asm("LDR R3, = 0x081E275C+1");
__asm("BL BX_R3");
__asm("POP {R0-R2,PC}");
__asm("BX_R3:");
__asm("BX R3");

__asm("__aeabi_uidivmod:");
__asm("__aeabi_idivmod:");
__asm("PUSH {LR}");
__asm("LDR R2, = 0x081E0F08+1");
__asm("BL BX_R2");
__asm("MOV R1, R0");
__asm("POP {PC}");

__asm("__aeabi_idiv:");
__asm("PUSH {LR}");
__asm("LDR R2, = 0x081E0868+1");
__asm("BL BX_R2");
__asm("POP {PC}");

__asm("__aeabi_uidiv:");
__asm("PUSH {LR}");
__asm("LDR R2, = 0x081E0E90+1");
__asm("BL BX_R2");
__asm("POP {PC}");

//__asm("LDR R2, = 0x081E0AB8+1");

__asm("BX_R2:");
__asm("BX R2");


#endif
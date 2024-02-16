#ifndef GUARD_TASK_H
#define GUARD_TASK_H

#define NUM_TASKS 16

// general task defines
#define TASK gTasks[taskId]
#define FUNC func

typedef void (*TaskFunc)(u8 taskId);

struct Task
{
    /*0x00*/ TaskFunc func;
    /*0x04*/ u8 isActive;
    /*0x05*/ u8 prev;
    /*0x06*/ u8 next;
    /*0x07*/ u8 priority;
    /*0x08*/ s16 data[16];
};

#define gTasks ((struct Task*)0x03004B20)

#endif // GUARD_TASK_H
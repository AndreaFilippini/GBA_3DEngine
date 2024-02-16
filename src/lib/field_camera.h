#ifndef GUARD_FIELD_CAMERA_H
#define GUARD_FIELD_CAMERA_H

struct FieldCamera
{
    void (*callback)(struct FieldCamera *);
    u32 trackedSpriteId;
    s32 movementSpeedX;
    s32 movementSpeedY;
    s32 curMovementOffsetX;
    s32 curMovementOffsetY;
};

#define gFieldCamera (*(struct FieldCamera *)0x03004880)

#endif // GUARD_FIELD_CAMERA_H
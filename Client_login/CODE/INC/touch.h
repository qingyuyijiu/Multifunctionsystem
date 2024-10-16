#ifndef __TOUCH_H__
#define __TOUCH_H__

/*
 *@brief 方向枚举
 *@note 枚举值定义如下：
 *TouchError：错误
 *TouchChick：点击
 *TouchUp：上滑
 *TouchDown：下滑
 *TouchLeft：左滑
 *TouchRight：右滑
 *Touchreturn: 退出
 */
typedef enum
{
    TouchError,
    TouchChick,
    TouchUp,
    TouchDown,
    TouchLeft,
    TouchRight,
    Touchreturn
} TouchDirection;

void TouchInit(void);

TouchDirection GetTouchDirection(int *x, int *y);

void TouchDestroy(void);

#endif

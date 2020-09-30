#ifndef CONFIG_H
#define CONFIG_H

//数据类型定义
#define u32   unsigned int
#define u16   unsigned short
#define u8    unsigned char
#define s32   signed   int
#define s16   signed   short
#define s8    signed   char

//定义屏幕尺寸
#define SCREEN_X_SIZE  (800)
#define SCREEN_Y_SIZE  (480)

//式样表
#define BUTTON_EXIT     "QPushButton {border-image:url(:/icon/Resource/icon/menu.png);}"\
                        "QPushButton:hover{border-image:url(:/icon/Resource/icon/menu_clicked.png);}"\
                        "QPushButton:pressed{border-image:url(:/icon/Resource/icon/menu.png);}"

#endif // CONFIG_H


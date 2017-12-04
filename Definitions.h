//helper marco that I found on the internet to make binary stuff a little easier
#define B8__(x) ((x&0x0000000FLU)?1:0) \
+((x&0x000000F0LU)?2:0) \
+((x&0x00000F00LU)?4:0) \
+((x&0x0000F000LU)?8:0) \
+((x&0x000F0000LU)?16:0) \
+((x&0x00F00000LU)?32:0) \
+((x&0x0F000000LU)?64:0) \
+((x&0xF0000000LU)?128:0)



#define LCD_WF_A_MASK   0x01
#define LCD_WF_A_SHIFT  0
#define LCD_WF_B_MASK   0x02
#define LCD_WF_B_SHIFT  1
#define LCD_WF_C_MASK   0x04
#define LCD_WF_C_SHIFT  2
#define LCD_WF_D_MASK   0x08
#define LCD_WF_D_SHIFT  3
#define LCD_WF_E_MASK   0x10
#define LCD_WF_E_SHIFT  4
#define LCD_WF_F_MASK   0x20
#define LCD_WF_F_SHIFT  5
#define LCD_WF_G_MASK   0x40
#define LCD_WF_G_SHIFT  6
#define LCD_WF_H_MASK   0x80
#define LCD_WF_H_SHIFT  7


//FRDM-KL46Z LUMEX LCD S401M16KR
//SLCD pin connections
//  Backplane
//    COM0 pin 1:   PTD0 Alt0=LCD_P40
#define LCD_PIN_1    40
//    COM1 pin 2:   PTE4 Alt0=LCD_P52
#define LCD_PIN_2    52
//    COM2 pin 3:   PTB23 Alt0=LCD_P19
#define LCD_PIN_3    19
//    COM3 pin 4:   PTB22 Alt0=LCD_P18
#define LCD_PIN_4    18
//  Frontplane
//    DIG1 pin 5:   PTC17 Alt0=LCD_P37
#define LCD_DIG1_PIN1    37
#define LCD_PIN_5        37
//    DIG1 pin 6:   PTB21 Alt0=LCD_P17
#define LCD_DIG1_PIN2    17
#define LCD_PIN_6        17
//    DIG2 pin 7:   PTB7 Alt0=LCD_P7
#define LCD_DIG2_PIN1    7
#define LCD_PIN_7        7
//    DIG2 pin 8:   PTB8 Alt0=LCD_P8
#define LCD_DIG2_PIN2    8
#define LCD_PIN_8        8
//    DIG3 pin 9:   PTE5 Alt0=LCD_P53
#define LCD_DIG3_PIN1    53
#define LCD_PIN_9        53
//    DIG3 pin 10:  PTC18 Alt0=LCD_P38
#define LCD_DIG3_PIN2    38
#define LCD_PIN_10       38
//    DIG4 pin 11:  PTB10 Alt0=LCD_P10
#define LCD_DIG4_PIN1    10
#define LCD_PIN_11       10
//    DIG4 pin 12:  PTB11 Alt0=LCD_P11
#define LCD_DIG4_PIN2    11
#define LCD_PIN_12       11
//All digit segments:  DIG1-DIG4
//  A
//F   B
//  G
//E   C
//  D
//  First register phases
#define LCD_SEG_D    0x11
#define LCD_SEG_E    0x22
#define LCD_SEG_G    0x44
#define LCD_SEG_F    0x88
//  Second register phases
#define LCD_SEG_C    0x22
#define LCD_SEG_B    0x44
#define LCD_SEG_A    0x88

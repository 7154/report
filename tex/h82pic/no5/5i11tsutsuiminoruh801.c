/* scitest3052.c --- SCI 通信確認プログラム */
#include <3052.h>
void timer_init(void);
void wait(unsigned int);
void sci_init(void);
char sci_get_char(void);
void sci_print_char(char);
int main(void) {
timer_init();
sci_init();
sci_print_char('@');
while(1) { sci_print_char(sci_get_char()); }
return 0;
}
// [SCI] ----------------------------------------
void sci_init(void) {
SCI1.SCR.BYTE = 0x00;
SCI1.SMR.BYTE = 0x00;
SCI1.BRR = 19; 
wait(1);
SCI1.SCR.BYTE = 0x30; 
SCI1.SSR.BYTE &= 0x80; 
}
char sci_get_char(void) {
char c;
if(SCI1.SSR.BIT.RDRF == 1) {
c = SCI1.RDR;
SCI1.SSR.BIT.RDRF = 0;
return(c);
} else {
SCI1.SSR.BYTE &= 0xc7;
return('\0');
}
}
void sci_print_char(char c) {
if(c != '\0') {
while(SCI1.SSR.BIT.TDRE == 0); 
SCI1.TDR = c;
SCI1.SSR.BIT.TDRE = 0;
}
}
// [TIMER, wait] --------------------------------
void timer_init(void) {
ITU0.TCR.BYTE = 0x23; 
ITU0.GRA = 0x0c35; 
ITU.TSTR.BIT.STR0 = 0; 
}
void wait(unsigned int msec) {
int i;
ITU.TSTR.BIT.STR0 = 1;
for (i=0; i < msec; i++) {
while(ITU0.TSR.BIT.IMFA == 0) {}
ITU0.TSR.BIT.IMFA = 0;
}
ITU.TSTR.BIT.STR0 = 0; 
}

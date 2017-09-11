#include <3052.h>

void timer_init(void);// タイマ初期設定
void wait(unsigned int);// 指定時間待機(ms)
void sci_init(void);
char sci_get_char(void);
void sci_print_char(char);
void sci_print(char *);
int drive_stpmotor(char, int, int);

int main(void) {
  volatile int count = -1, fst = 0, inputp = 0, startp = 0;
  volatile char data = '\0', pdata = '\0';
  
  timer_init();
  sci_init();

  sci_print("\n* Start\n");
  
  PA.DDR = 0xf0; // PA: Switch (0-3, 1:OFF / 0:ON)
  PB.DDR = 0xff; // PB: LED (0-3, 0:OFF / 1:ON)

  while(1) {
    wait(100);
    pdata = data;
    data = sci_get_char();
    inputp = data != '\0' ? 1 : 0;// 入力があったか？
    startp = count != -1 ? 1 : 0 ;// モータが回っているか？
    if(!inputp && !startp) continue;
    if( inputp && !startp) {
      fst = 1; count = 0;
    }
    if(!inputp &&  startp) { fst = 0; data = pdata; }
    if( inputp &&  startp && data != pdata) { fst = 1; }
    count = drive_stpmotor(data, count, fst);
    if(count == -1) {
      data = '\0';
      if(fst == 1 && pdata != '\0') {
        fst = 0;
        sci_print("  Stop\n");
      }
    }
  }
  return 0;
}


// [Motor] --------------------------------------
int drive_stpmotor(char c, int cnt, int fst) {
  switch(c) {
  case 'g':
    if(fst) { sci_print("  g\n"); fst = 0; }
    switch(cnt) {
    case 0: PB.DR.BYTE = 0b0001; break;
    case 1: PB.DR.BYTE = 0b0010; break;
    case 2: PB.DR.BYTE = 0b0100; break;
    case 3: PB.DR.BYTE = 0b1000; break;
    }
    return (cnt+1) & 0x3; break;
  }
  return -1;
}
// [SCI] ----------------------------------------
void sci_init(void) {
  SCI1.SCR.BYTE = 0x00;
  SCI1.SMR.BYTE = 0x00;
  SCI1.BRR = 19; // 38400baud(25MHz)
  wait(1);
  SCI1.SCR.BYTE = 0x30; // scr = 0011 0000 (TE=1, RE=1)
  SCI1.SSR.BYTE &= 0x80; // エラーフラグクリア
}
char sci_get_char(void) {
  char c;
//  while((SCI1.SSR.BYTE & 0x78) == 0);
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
    while(SCI1.SSR.BIT.TDRE == 0); // wait
    SCI1.TDR = c;
    SCI1.SSR.BIT.TDRE = 0;
  }
}
void sci_print(char *str) {
  while(*str != '\0') sci_print_char(*str++);
}

// [TIMER, wait] --------------------------------
void timer_init(void) {
  ITU0.TCR.BYTE = 0x23; // timer用レジスタ
  ITU0.GRA = 0x0c35; // 1ms単位にする
  ITU.TSTR.BIT.STR0 = 0; // timer 停止
}
void wait(unsigned int msec) {
  int i;
  ITU.TSTR.BIT.STR0 = 1; // timer スタート
  for (i=0; i < msec; i++) {
    while(ITU0.TSR.BIT.IMFA == 0) {} // wait
    ITU0.TSR.BIT.IMFA = 0; // timer リセット
  }
  ITU.TSTR.BIT.STR0 = 0; // timer 停止
}
// ----------------------------------------------

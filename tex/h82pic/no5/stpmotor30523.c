/*
  stpmotor3052.c --- ステッピングモータ制御プログラム
  Author: M. Bando <bando@ktc.ac.jp>
  Version: 1.0
  Last update: 28 Jan. 2016 @KTC

  近畿大学工業高等専門学校 工学実験5(5EI) 実験No.5

  [主な装置]
  マイコン: H8/3052F (ルネサスエレクトロニクス)
  ステッピングモータ: SPG27-1601 (日本電産コパル)

  [電源について]
  マイコン、ステッピングモータ共に12V/1Aの電源で動作します。

  [マイコンへの書き込み]
  1. 外部電源をマイコンから外す。
  2. マイコンボードのMD2を短絡(ジャンパピンをMD2に挿す)させる。
  3. 書込みスイッチ(赤いトグルスイッチ)をON(内側)にする。
  4. 外部電源をマイコンに繋ぐ。
  5. 書き込みソフト(H8 Writer)でプログラムを書き込む。
  6. 外部電源をマイコンから外す。
  7. MD2のジャンパピンを外し、書込みスイッチをOFFにする。

  [実行]
  1. MD2のジャンパピンを外し、書込みスイッチOFFを確認する。
  2. 外部電源をマイコンに繋ぐ。

  [wait関数について]
  H8/3052Fマイコンには16ビットインテグレーテッドタイマユニット
  (16bit Integrated Timer Unit: ITU)という機能が搭載されています。
  従来の空ループによる待機時間の実装は正確さに欠けることが欠点でしたが、
  ここではwait関数をITUを使って実装し、正確な待機時間を実現しています。
  
*/

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
  case 'h':
    if(fst) { sci_print("  h\n"); fst = 0; }
    switch(cnt) {
    case 0: PB.DR.BYTE = 0b1000; break;
    case 1: PB.DR.BYTE = 0b0100; break;
    case 2: PB.DR.BYTE = 0b0010; break;
    case 3: PB.DR.BYTE = 0b0001; break;
    }
    return (cnt+1) & 0x3; break;
  case 'j':
    if(fst) { sci_print("  j\n"); fst = 0; }}
    switch(cnt){
    case 0: PB.DR.BYTE = 0b1001; break;
    case 1: PB.DR.BYTE = 0b0011; break;
    case 2: PB.DR.BYTE = 0b0110; break;
    case 3: PB.DR.BYTE = 0b1100; break;
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

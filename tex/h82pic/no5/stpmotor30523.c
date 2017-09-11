/*
  stpmotor3052.c --- �X�e�b�s���O���[�^����v���O����
  Author: M. Bando <bando@ktc.ac.jp>
  Version: 1.0
  Last update: 28 Jan. 2016 @KTC

  �ߋE��w�H�ƍ������w�Z �H�w����5(5EI) ����No.5

  [��ȑ��u]
  �}�C�R��: H8/3052F (���l�T�X�G���N�g���j�N�X)
  �X�e�b�s���O���[�^: SPG27-1601 (���{�d�Y�R�p��)

  [�d���ɂ���]
  �}�C�R���A�X�e�b�s���O���[�^����12V/1A�̓d���œ��삵�܂��B

  [�}�C�R���ւ̏�������]
  1. �O���d�����}�C�R������O���B
  2. �}�C�R���{�[�h��MD2��Z��(�W�����p�s����MD2�ɑ}��)������B
  3. �����݃X�C�b�`(�Ԃ��g�O���X�C�b�`)��ON(����)�ɂ���B
  4. �O���d�����}�C�R���Ɍq���B
  5. �������݃\�t�g(H8 Writer)�Ńv���O�������������ށB
  6. �O���d�����}�C�R������O���B
  7. MD2�̃W�����p�s�����O���A�����݃X�C�b�`��OFF�ɂ���B

  [���s]
  1. MD2�̃W�����p�s�����O���A�����݃X�C�b�`OFF���m�F����B
  2. �O���d�����}�C�R���Ɍq���B

  [wait�֐��ɂ���]
  H8/3052F�}�C�R���ɂ�16�r�b�g�C���e�O���[�e�b�h�^�C�}���j�b�g
  (16bit Integrated Timer Unit: ITU)�Ƃ����@�\�����ڂ���Ă��܂��B
  �]���̋󃋁[�v�ɂ��ҋ@���Ԃ̎����͐��m���Ɍ����邱�Ƃ����_�ł������A
  �����ł�wait�֐���ITU���g���Ď������A���m�ȑҋ@���Ԃ��������Ă��܂��B
  
*/

#include <3052.h>

void timer_init(void);// �^�C�}�����ݒ�
void wait(unsigned int);// �w�莞�ԑҋ@(ms)
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
    inputp = data != '\0' ? 1 : 0;// ���͂����������H
    startp = count != -1 ? 1 : 0 ;// ���[�^������Ă��邩�H
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
  SCI1.SSR.BYTE &= 0x80; // �G���[�t���O�N���A
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
  ITU0.TCR.BYTE = 0x23; // timer�p���W�X�^
  ITU0.GRA = 0x0c35; // 1ms�P�ʂɂ���
  ITU.TSTR.BIT.STR0 = 0; // timer ��~
}
void wait(unsigned int msec) {
  int i;
  ITU.TSTR.BIT.STR0 = 1; // timer �X�^�[�g
  for (i=0; i < msec; i++) {
    while(ITU0.TSR.BIT.IMFA == 0) {} // wait
    ITU0.TSR.BIT.IMFA = 0; // timer ���Z�b�g
  }
  ITU.TSTR.BIT.STR0 = 0; // timer ��~
}
// ----------------------------------------------

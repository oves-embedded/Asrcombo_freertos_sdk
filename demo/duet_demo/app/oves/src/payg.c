#include <stdio.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "lega_wlan_api.h"
#include "tcpip.h"
#include "lega_at_api.h"
#include "soc_init.h"
#include "lwip_app_iperf.h"
#include "duet_version.h"
#include "duet_uart.h"
#include "duet_pinmux.h"
#include "duet_flash_kv.h"
#include "ovapp.h"

#include "payg.h"
#include "eeprom.h"
#include "i2c.h"
#include "ds1302.h"
#include "keyboard.h"
#include "gatt.h"


int tian = 0; 
uint8_t g_tokenState=TOKEN_OK;
uint8_t g_KeyboardDetectLock=FALSE;
uint8_t g_KeyboardDetectLockRst=FALSE;

PAYG_TypeDef payg;
__IO uint8_t g_PaygUpdate=0;
uint8_t g_sysInit=TRUE;
 uint8_t Sys_Code ;
 unsigned char keyUesed[7];//{0xC5,0x6A,0x29,0x07,0xFA,0x0B,0x1F};//密码已使用
 unsigned char errkey[7]  ;//错误密码
 uint8_t SaveFlash_Flag;

extern DS1302_TIME_TypeDef Read_T;
extern USER_SET_TypeDef g_UserSet;



SYS_STATE_CODE_TypeDef  g_sysStateCode;
__IO uint8_t  g_DemoState=0;
__IO uint32_t  g_Demotimecouter=0;


uint8_t GetDemoState(void)
{
	return g_DemoState;
}

void  DemoModeClose(void)
{
	 g_DemoState=FALSE;
}

void  DemoModeStart(void)
{
	 g_DemoState=TRUE;
}

void DemoTimerStart(void)
{
  //LOG("BLE start Demo_TIMER Timer\n");	
 // osal_start_timerEx(bleuart_TaskID, SBP_DEMO_TIMER, 15*60*1000u);
  g_Demotimecouter=15*60;
}


void PaygOvesTask(void)
{
	//  if(tian > 2192)//时钟芯片出错
	//  {
	//    payg.remaing_days = 0xffff;
	//  }
	//  else
	//  {
	
	if(payg.free != YES)
	{
		//    tian = GetDs1302Day();
		if((payg.free == NO) && (payg.remaing_days == 0))
		{	
			//no code ??
			}
		else//Payg状态 还有剩余天数
		{
			if((int32_t)((int32_t)tian + (int32_t)payg.day_cem) >(int32_t) payg.days_has_been_runing)
			{
				//payg.days_has_been_runing  = tian  + payg.day_cem;
				payg.days_has_been_runing  = tian + payg.day_cem;
				payg.run_days_backup ++;
			}

		}
		
		if(payg.recharge_days > payg.days_has_been_runing)
		{	
			payg.remaing_days = payg.recharge_days  - payg.days_has_been_runing;
			}
		else 
		{
			if(payg.remaing_days!=0
			  ||payg.days_has_been_runing != payg.recharge_days
			  ||payg.day_cem != payg.days_has_been_runing)
				EEpUpdateEnable();
			payg.remaing_days = 0;
			payg.days_has_been_runing = payg.recharge_days;
			payg.day_cem = payg.days_has_been_runing;
			//SaveFlash_Flag = 1;
			}

	}
	
}


void sha1_starts( sha1_context *ctx )
{
  ctx->total[0] = 0;
  ctx->total[1] = 0;
  
  ctx->state[0] = 0x67452301;
  ctx->state[1] = 0xEFCDAB89;
  ctx->state[2] = 0x98BADCFE;
  ctx->state[3] = 0x10325476;
  ctx->state[4] = 0xC3D2E1F0;
}

void sha1_process( sha1_context *ctx, uint8_t data[64] )
{
  uint32_t temp, W[16], A, B, C, D, E;
  
  GET_UINT32( W[0],  data,  0 );
  GET_UINT32( W[1],  data,  4 );
  GET_UINT32( W[2],  data,  8 );
  GET_UINT32( W[3],  data, 12 );
  GET_UINT32( W[4],  data, 16 );
  GET_UINT32( W[5],  data, 20 );
  GET_UINT32( W[6],  data, 24 );
  GET_UINT32( W[7],  data, 28 );
  GET_UINT32( W[8],  data, 32 );
  GET_UINT32( W[9],  data, 36 );
  GET_UINT32( W[10], data, 40 );
  GET_UINT32( W[11], data, 44 );
  GET_UINT32( W[12], data, 48 );
  GET_UINT32( W[13], data, 52 );
  GET_UINT32( W[14], data, 56 );
  GET_UINT32( W[15], data, 60 );
  
#define S(x,n) ((x << n) | ((x & 0xFFFFFFFF) >> (32 - n)))
  
#define R(t)                                            \
  (                                                       \
    temp = W[(t -  3) & 0x0F] ^ W[(t - 8) & 0x0F] ^     \
      W[(t - 14) & 0x0F] ^ W[ t      & 0x0F],      \
        ( W[t & 0x0F] = S(temp,1) )                         \
          )
    
#define P(a,b,c,d,e,x)                                  \
  {                                                       \
    e += S(a,5) + F(b,c,d) + K + x; b = S(b,30);        \
  }
  
  A = ctx->state[0];
  B = ctx->state[1];
  C = ctx->state[2];
  D = ctx->state[3];
  E = ctx->state[4];
  
#define F(x,y,z) (z ^ (x & (y ^ z)))
#define K 0x5A827999
  
  P( A, B, C, D, E, W[0]  );
  P( E, A, B, C, D, W[1]  );
  P( D, E, A, B, C, W[2]  );
  P( C, D, E, A, B, W[3]  );
  P( B, C, D, E, A, W[4]  );
  P( A, B, C, D, E, W[5]  );
  P( E, A, B, C, D, W[6]  );
  P( D, E, A, B, C, W[7]  );
  P( C, D, E, A, B, W[8]  );
  P( B, C, D, E, A, W[9]  );
  P( A, B, C, D, E, W[10] );
  P( E, A, B, C, D, W[11] );
  P( D, E, A, B, C, W[12] );
  P( C, D, E, A, B, W[13] );
  P( B, C, D, E, A, W[14] );
  P( A, B, C, D, E, W[15] );
  P( E, A, B, C, D, R(16) );
  P( D, E, A, B, C, R(17) );
  P( C, D, E, A, B, R(18) );
  P( B, C, D, E, A, R(19) );
  
#undef K
#undef F
  
#define F(x,y,z) (x ^ y ^ z)
#define K 0x6ED9EBA1
  
  P( A, B, C, D, E, R(20) );
  P( E, A, B, C, D, R(21) );
  P( D, E, A, B, C, R(22) );
  P( C, D, E, A, B, R(23) );
  P( B, C, D, E, A, R(24) );
  P( A, B, C, D, E, R(25) );
  P( E, A, B, C, D, R(26) );
  P( D, E, A, B, C, R(27) );
  P( C, D, E, A, B, R(28) );
  P( B, C, D, E, A, R(29) );
  P( A, B, C, D, E, R(30) );
  P( E, A, B, C, D, R(31) );
  P( D, E, A, B, C, R(32) );
  P( C, D, E, A, B, R(33) );
  P( B, C, D, E, A, R(34) );
  P( A, B, C, D, E, R(35) );
  P( E, A, B, C, D, R(36) );
  P( D, E, A, B, C, R(37) );
  P( C, D, E, A, B, R(38) );
  P( B, C, D, E, A, R(39) );
  
#undef K
#undef F
  
#define F(x,y,z) ((x & y) | (z & (x | y)))
#define K 0x8F1BBCDC
  
  P( A, B, C, D, E, R(40) );
  P( E, A, B, C, D, R(41) );
  P( D, E, A, B, C, R(42) );
  P( C, D, E, A, B, R(43) );
  P( B, C, D, E, A, R(44) );
  P( A, B, C, D, E, R(45) );
  P( E, A, B, C, D, R(46) );
  P( D, E, A, B, C, R(47) );
  P( C, D, E, A, B, R(48) );
  P( B, C, D, E, A, R(49) );
  P( A, B, C, D, E, R(50) );
  P( E, A, B, C, D, R(51) );
  P( D, E, A, B, C, R(52) );
  P( C, D, E, A, B, R(53) );
  P( B, C, D, E, A, R(54) );
  P( A, B, C, D, E, R(55) );
  P( E, A, B, C, D, R(56) );
  P( D, E, A, B, C, R(57) );
  P( C, D, E, A, B, R(58) );
  P( B, C, D, E, A, R(59) );
  
#undef K
#undef F
  
#define F(x,y,z) (x ^ y ^ z)
#define K 0xCA62C1D6
  
  P( A, B, C, D, E, R(60) );
  P( E, A, B, C, D, R(61) );
  P( D, E, A, B, C, R(62) );
  P( C, D, E, A, B, R(63) );
  P( B, C, D, E, A, R(64) );
  P( A, B, C, D, E, R(65) );
  P( E, A, B, C, D, R(66) );
  P( D, E, A, B, C, R(67) );
  P( C, D, E, A, B, R(68) );
  P( B, C, D, E, A, R(69) );
  P( A, B, C, D, E, R(70) );
  P( E, A, B, C, D, R(71) );
  P( D, E, A, B, C, R(72) );
  P( C, D, E, A, B, R(73) );
  P( B, C, D, E, A, R(74) );
  P( A, B, C, D, E, R(75) );
  P( E, A, B, C, D, R(76) );
  P( D, E, A, B, C, R(77) );
  P( C, D, E, A, B, R(78) );
  P( B, C, D, E, A, R(79) );
  
#undef K
#undef F
  
  ctx->state[0] += A;
  ctx->state[1] += B;
  ctx->state[2] += C;
  ctx->state[3] += D;
  ctx->state[4] += E;
}

void sha1_update( sha1_context *ctx, uint8_t *input, uint32_t length )
{
  uint32_t left, fill;
  
  if( ! length ) return;
  
  left = ctx->total[0] & 0x3F;
  fill = 64 - left;
  
  ctx->total[0] += length;
  ctx->total[0] &= 0xFFFFFFFF;
  
  if( ctx->total[0] < length )
    ctx->total[1]++;
  
  if( left && length >= fill )
  {
    memcpy( (void *) (ctx->buffer + left), (void *) input, fill );
          
    sha1_process( ctx, ctx->buffer );
    length -= fill;
    input  += fill;
    left = 0;
  }
  
  while( length >= 64 )
  {
    sha1_process( ctx, input );
    length -= 64;
    input  += 64;
  }
  
  if( length )
  {
    memcpy( (void *) (ctx->buffer + left),(void *) input, length );
           
  }
}

static uint8_t sha1_padding[64] =
{
  0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

void sha1_finish( sha1_context *ctx, uint8_t digest[20] )
{
  uint32_t last, padn;
  uint32_t high, low;
  uint8_t msglen[8];
  
  high = ( ctx->total[0] >> 29 )
    | ( ctx->total[1] <<  3 );
  low  = ( ctx->total[0] <<  3 );
  
  PUT_UINT32( high, msglen, 0 );
  PUT_UINT32( low,  msglen, 4 );
  
  last = ctx->total[0] & 0x3F;
  padn = ( last < 56 ) ? ( 56 - last ) : ( 120 - last );
  
  sha1_update( ctx, sha1_padding, padn );
  sha1_update( ctx, msglen, 8 );
  
  PUT_UINT32( ctx->state[0], digest,  0 );
  PUT_UINT32( ctx->state[1], digest,  4 );
  PUT_UINT32( ctx->state[2], digest,  8 );
  PUT_UINT32( ctx->state[3], digest, 12 );
  PUT_UINT32( ctx->state[4], digest, 16 );
}



/*
unsigned  long input_int[2];

void strtolong(void)
{
uint16_t i;
unsigned long temp1= 0x3b9aca00; 
unsigned long temp2= 0x3b9aca00; 
unsigned long temp3=0;
input_int[0] = 0;
input_int[1] = 0;
//ê?è??ainput_data.code
for(i=0;i<28;i++)
{
if((i==2)||(i==3)||(i==5)||(i==6)||(i==7)||(i==9)||(i==10)||(i==11)||(i==13)||(i==14))
{
temp3 = input_data.code[i];
input_int[0] +=( temp3- 0x30 )* temp1;
temp1 /= 10;
                   }
if((i==15)||(i==17)||(i==18)||(i==19)||(i==21)||(i==22)||(i==23)||(i==25)||(i==26)||(i==27))
{
temp3 = input_data.code[i];
input_int[1] += ( temp3- 0x30 ) * temp2;
temp2 /= 10;
                   }
          }

  }
*/

uint8_t MiddleStr(uint8_t hexd)
{
  uint8_t tmp=0;
  if (hexd <= 9)
  {
    tmp=hexd+'0';
  }
  else
  {
    tmp=hexd+55;
  }
  return tmp;
}
void HexToStr(uint32_t *hex,uint8_t *testbuff)
{
  uint8_t i,j=28;
  for(i=0;i<8;i++)
  {
    testbuff[i] = MiddleStr(hex[0]>>j&0x0f);
    j-=4;
  }
  j=28;
  for(i=0;i<8;i++)
  {
    testbuff[i+8] = MiddleStr(hex[1]>>j&0x0f);
    j-=4;
  }
  testbuff[16]='\0';
}


void SHA(uint8_t *Message_Digest,uint32_t *output)
{
  
  uint32_t digest[5];
  uint8_t i,j;
  
  for(i=0,j=4;i<5;i++,j--)
  {
    digest[j] = *((uint32_t*)Message_Digest + i);
  }
  
  digest[0] ^= digest[2];
  digest[1] ^= digest[3];
  digest[0] ^= digest[4];
  
  output[0] = digest[0];
  output[1] = digest[1];
}


int big_litle_endian(int x)  
{  
  int tmp;  
  tmp = (((x)&0xff)<<24) + (((x>>8)&0xff)<<16) + (((x>>16)&0xff)<<8) + (((x>>24)&0xff));  
  return tmp;  
} 

void sha1_c(void)
{
  sha1_context ctx;
  uint8_t  Message_Digest[20];
  uint8_t  testbuff[20];
  HexToStr(payg.hast_input,testbuff);
  sha1_starts( &ctx );
  sha1_update( &ctx, (uint8_t *) testbuff,16);
  sha1_finish( &ctx, Message_Digest);
  SHA(Message_Digest,payg.hast_input);
  payg.hast_input[0] = big_litle_endian(payg.hast_input[0]) ;
  payg.hast_input[1] = big_litle_endian(payg.hast_input[1]) ;
}


unsigned char keyocl[20]       =    {0xc5,0x6a,0x29,0x0e,0xfb,0xbe,0x5d,0x72,0xc3,0xb9,0xb8,0x81,0xfb,0xdd};
unsigned char key[20]          =    {0xc5,0x6a,0x29,0x0e,0xfa,0xce,0xb3,0x98,0x00,0x12,0xac,0xcd,0x5f,0xdd};

void fun2(void) 
{
  uint32_t tmp;
  
  tmp = keyocl[12];
  payg.hast_otp[0] = tmp << 24;
  tmp = keyocl[11];
  payg.hast_otp[0] |= tmp << 16;
  tmp = keyocl[10];
  payg.hast_otp[0] |= tmp << 8;
  payg.hast_otp[0] |= keyocl[9];
  
  tmp = keyocl[8];
  payg.hast_otp[1] = tmp << 24;
  tmp = keyocl[7];
  payg.hast_otp[1] |= tmp << 16;
  tmp = keyocl[6];
  payg.hast_otp[1] |= tmp << 8;
  payg.hast_otp[1] |= keyocl[5];
}
void fuc1(void)
{
  uint32_t tmp;
  
  tmp = key[12];
  payg.hast_input[0] = tmp << 24;
  tmp = key[11];
  payg.hast_input[0] |= tmp << 16;
  tmp = key[10];
  payg.hast_input[0] |= tmp << 8;
  payg.hast_input[0] |= key[9];
  
  tmp = key[8];
  payg.hast_input[1] = tmp << 24;
  tmp = key[7];
  payg.hast_input[1] |= tmp << 16;
  tmp = key[6];
  payg.hast_input[1] |= tmp << 8;
  payg.hast_input[1] |= key[5];
}

long Top_UpDays = 0;

char Hashstr[28];
char hashshow[32];	
HASH_TOP_Def Hash_Hi,Hash_Lo;
void CampInttoStrs(unsigned long n,char *s)//锟斤拷锟斤拷锟斤拷锟斤拷转锟斤拷锟矫的猴拷锟斤拷
{
  int i=0,j;
  char temp;
  while(n)//锟饺斤拷锟斤拷锟斤拷锟斤拷锟津保达拷锟斤拷锟斤拷锟?
  {
    s[i]=n%10+'0';//锟斤拷十锟斤拷锟斤拷锟斤拷锟斤拷转锟斤拷锟斤拷锟街凤拷锟斤拷锟斤拷
    n=n/10;
    i++;
  }
  for(j=i-1;j>=i/2;j--)//锟劫斤拷锟斤拷锟斤拷锟斤拷址锟斤拷锟斤拷锟斤拷锟斤拷转一锟轿ｏ拷锟矫碉拷锟斤拷确锟斤拷锟?
  {
    temp=s[j];
    s[j]=s[i-j-1];
    s[i-j-1]=temp;
  }
}

char* CampInsert(char *s1, char *s2, int n)
{
  int len1 = 0, len2 = 0, j = 0, k = 0;
  char s4[64];
	int i;

  len1 = strlen(s1);
  len2 = strlen(s2);
  
  if (s1 == NULL||s2 == NULL||n>len1)
   	 return NULL;

  for ( i = 0; i<n; i++)
  {
    j++;
  }
  for ( i= 0; i<len1; i++)
  {
    s4[k++] = s1[i];
  }

  for ( i = 0; i<len2; i++)
    s1[j++] = s2[i];

  for( i=n;i<len1;i++)
    s1[j++]=s4[i];

  s1[j] = '\0';


  return s1;
}

void Hash_Opration(uint32_t *hastop)
{
		int j;
    memset(hashshow, 0x00, 32);
    memset(Hashstr, 0x00, 28);
 /*   Hash_Lo.Hash_char[0] = Receive2_Buffer[8];
    Hash_Lo.Hash_char[1] = Receive2_Buffer[7];
    Hash_Lo.Hash_char[2] = Receive2_Buffer[6];
    Hash_Lo.Hash_char[3] = Receive2_Buffer[5];
    Hash_Hi.Hash_char[0] = Receive2_Buffer[12];
    Hash_Hi.Hash_char[1] = Receive2_Buffer[11];
    Hash_Hi.Hash_char[2] = Receive2_Buffer[10];
    Hash_Hi.Hash_char[3] = Receive2_Buffer[9];*/

    Hash_Lo.Hash_long=hastop[1];
    Hash_Hi.Hash_long=hastop[0];

    CampInttoStrs(Hash_Hi.Hash_long, Hashstr);

    while(strlen(Hashstr) < 10)
   { //if(strlen(Hashstr) < 10)
     	 CampInsert(Hashstr, "0", 0);
    	}
    CampInttoStrs(Hash_Lo.Hash_long, Hashstr + strlen(Hashstr));
  //  if(strlen(Hashstr) < 20)
  //    insert(Hashstr, "0", 10);
    /* 补零 少几个补几个 */
    for( j = 20 - strlen(Hashstr); j  > 0; j--)
    {
      CampInsert(Hashstr, "0", 9 + j);
    }

    CampInsert(Hashstr, " ", 2);
    CampInsert(Hashstr, " ", 6);
    CampInsert(Hashstr, " ", 10);
    CampInsert(Hashstr, " ", 14);
    CampInsert(Hashstr, " ", 18);
    CampInsert(Hashstr, " ", 22);

    memcpy(hashshow, "*0", 2);
    memcpy(hashshow + 2, Hashstr, 26);
    memcpy(hashshow + 28, "#", 1);
}


PAYG_TypeDef* PaygGetPaygBuf(void)
{
	return &payg;
}

void PaygInit(void)
{
	  memcpy((uint8_t*)&payg,(uint8_t*)&g_UserSet.Payg,sizeof(PAYG_TypeDef));

	  payg.hast_root[0] = 0x00000000;
	  payg.hast_root[1] = 0x00000000;

	  if((payg.free != YES) && (payg.free != NO))
	  {
		    payg.free = YES;
		    payg.day_cem = 0;//备份天数等于0;
		    EEpUpdateEnable();
	  }

	  #ifdef HASH_RESET
	  payg.free = NO;
	  payg.lock = NO;
	  payg.remaing_days = 0;
	  payg.days_has_been_runing = 0;
	  InitDs1302();//宸茶繍琛屽ぉ鏁版竻0
	  payg.time_cnt = 0;
	  payg.recharge_days = 0;
	  
	  Sys_Code = 0;//clean Err
	  g_sysStateCode.error.rtc_error=0;
	  
	  payg.day_cem = 0;//杩愯澶╂暟澶囦唤娓呴浂

	  EEpUpdateEnable();
	  #endif

	  PaygUpdateEnable();
	  PaygProcess();
}

void PaygOvesInput(void)
{
  long z;
  static uint32_t hast_top_tmp[2];
  //密码已使用
  if((payg.hast_input[0] == payg.hast_otp[0])&&(payg.hast_input[1] == payg.hast_otp[1]))
  {
	    Printf_Usart_num(keyUesed, 7);
	    g_tokenState=TOKEN_USEED;

	     if(EEpGetDemoCnt()<3&&payg.remaing_days==0&&payg.free == NO)
	     {	
		    DemoTimerStart();
		    DemoModeStart();
				EEpSetDemoCnt(EEpGetDemoCnt()+1);	
	     	}
	    else
	    {   
			g_sysStateCode.error.pswd_error=1;
	    	}
  }
  else//新密码
  {
    //备份输入密码
    hast_top_tmp[0] = payg.hast_input[0];
    hast_top_tmp[1] = payg.hast_input[1];
	
    for(z=1; z<HASH_MAX + 1; z++)
    {
      sha1_c(); //计算密码
      
      if((payg.hast_input[0] == payg.hast_otp[0])&&(payg.hast_input[1] == payg.hast_otp[1]))
        	break;
    }
	
    if(z <= HASH_MAX)
    	Top_UpDays = z;
    else
    	Top_UpDays = 0;
	
    if(Top_UpDays > 0)//充值密码正确
    {	
		//     Printf_Usart1("当前密码充值天数:%d  （天）\r\n",z);
		payg.hast_otp[0] = hast_top_tmp[0];
		payg.hast_otp[1] = hast_top_tmp[1];
		//SaveFlash_Flag = 1;//需要更新FLASH
		EEpUpdateEnable();

		if(Top_UpDays >= 2192) 
			Top_UpDays = 2192;
		else if(Top_UpDays >= 1096)
			Top_UpDays = 1096;

		if(YES == payg.free)
		{
			//初始化所有运行参数
			payg.time_cnt = 0;  //累计时间清0
			payg.recharge_days = 0; //充值天数清0
			payg.days_has_been_runing = 0;
			InitDs1302();//已运行天数清0
			payg.remaing_days = 1096; //剩余天数清0
			//payg.run_days_backup  //这个参数累计不清零
			payg.lock = NUL; //è¨?T?a3y

			//sys.eeprom_update = YES; //EEpeom óD2?êyDèòa?üD?
      		}
		
     	      if(Top_UpDays == 2192)
	      {
		        payg.free = NO;
		        payg.lock = NO;
		        payg.remaing_days = 0;
		        payg.days_has_been_runing = 0;
				InitDs1302();//已运行天数清0
		        payg.time_cnt = 0;
		        payg.recharge_days = 0;
		        //        //CBI(sys.system_status_Code,E05);
		        //        sys.system_status_Code = 0;
		        //        sys.eeprom_update = YES;
		        
		        //=========
		        Sys_Code = 0;//clean Err
		        g_sysStateCode.error.rtc_error=0;

		        g_tokenState=TOKEN_OK;
		        
		        payg.day_cem = 0;//运行天数备份清零
	      		}
     	       else if(Top_UpDays == 1096)
	       {
		        //初始化所有参数
		        payg.time_cnt = 0;  //累计时间清0
		        payg.recharge_days = 0; //天数0
		        payg.days_has_been_runing = 0;
				InitDs1302(); //已运行时间清0
		        payg.remaing_days = 1096; //剩余天数清0
		        //payg.run_days_backup  //这个参数累计不清零
		        payg.lock = NUL; //权限清楚
		        payg.free = YES;
		        //=========
		       // Sys_Code = 0;//clean Err
		         g_sysStateCode.error.rtc_error=0;

		        g_tokenState=TOKEN_OK;

				EEpSetDemoCnt(0);	
				EEpSetPpidCfgState(FALSE);
				EEpUpdateEnable();
		        
		        payg.day_cem = 0;//运行天数备份清零
	      		}
      	      else if((Top_UpDays > 0) && (Top_UpDays < 1096))
	      {
		        //如果之前已经停机欠费,将时间累计清0.防止快累计一天实际充值效果没有1天
		        if(payg.remaing_days == 0||g_sysStateCode.error.rtc_error)
		        {
			         // if(/*(Sys_Code != 0xE1) && (Sys_Code != 0xE3)*/  g_sysStateCode.error.rtc_error==FALSE)
			          {
			          	  if(payg.remaing_days == 0)
				              payg.time_cnt = 0;  //如果剩余天数0
				          else 
				          {
				          	  Sys_Code = 0;//clean Err
		        			  g_sysStateCode.error.rtc_error=0;
				          	}
				          InitDs1302();//如果欠费状态下充值  时钟清零？ 
			          }
		        }
		        PaygOvesTask();
				g_tokenState=TOKEN_OK;

				EEpSetDemoCnt(0);	
				EEpSetPpidCfgState(FALSE);
				EEpUpdateEnable();
		        payg.recharge_days += Top_UpDays; //增加充值天数
		        payg.free = NO;
			#ifdef   SHOW_TIME        
		        //打印充值天数
		        memset(cmd_sendbuf, 0, 32);
		        itoa(Top_UpDays, cmd_sendbuf, 10);
		        Printf_Usart1("正确的充值命令 充值天数: ");
		        Printf_Usart1(cmd_sendbuf);
		        Printf_Usart1("\r\n");
			#endif        
	      }
    	  Send_RechargeOK();
	  g_sysStateCode.error.pswd_error=0;	  
	  g_sysStateCode.error.pwd_len_error=0;
      
    }
    else//充值密码错误
    {
      Printf_Usart_num(errkey, 7);

      g_tokenState=TOKEN_ERROR;	  
      g_sysStateCode.error.pswd_error=1;
	  
    }
  }
}

void PaygUpdateEnable(void)
{
	g_PaygUpdate=TRUE;
}

uint32_t  PaygGetPayState(void)
{
	if( payg.remaing_days>0||payg.free == YES
		#ifdef OPEN_PAYGO
		||IsActive()
		#endif
		)
	 	return TRUE;
	else
		 return FALSE;
}

uint16_t  AtCmdGetValueLen(char *str,uint8_t end_tag)
{
	uint16_t ret=0,i=0;//,len=0;

	for(i=0;str[i]!=end_tag&&str[i];i++)
	{	
		ret++;
		}
	return ret;
}

uint32_t AtouI(uint8_t *ascii)
{
	uint8_t i=0,len=strlen((char*)ascii);
	uint32_t value=0;

	for(i=0;i<len;i++)
	{
		if(ascii[i]<'0'||ascii[i]>'9')
			break;
		value*=10;
		value+=ascii[i]-'0';
		}
	return value;
}

void AtCmdTokenParse(uint8_t*str,uint8_t*tag)
{
	uint8_t tag_len,value_len;//,token_len=0;
	uint8_t /*token[32],*/*p,*p_target;
	uint32_t token_hi,token_lo;
	uint8_t temp[12];
	uint8_t token[32];
	uint8_t tokenstate=0,breakfor=0;
	//uint8_t mqtt_cmd=FALSE;
	uint8_t mqtt_cmd=FALSE,token_len=0;
	uint16_t i,j;
	//uint16_t tkre=0;
	#if  0//def BLE_MASTER_ENABLE
	extern uint8_t g_bleCmd_State;
	extern uint16_t RxUart3Counter;
	extern uint8_t g_Uart3Buf[UART3_RX_BUF_SIZE];
	extern uint8_t g_bleAck_State;
	extern uint8_t g_bletkre;
	#endif

	if(strstr((char*)str,"/cmd/code")!=NULL)
		mqtt_cmd=TRUE;

	
	tag_len=strlen((char*)tag);
	value_len=AtCmdGetValueLen((char*)(str+tag_len),'#');
	memset(token,0x00,32);

	//memcpy(token,str+tag_len,value_len);

	p=str+tag_len;

	#if  0//def BLE_MASTER_ENABLE
	g_bleCmd_State=BLE_CMD_PUBK;
	p-=2;
					
	BlePacktSend(g_bleCmd_State,p,value_len+3);
	//RxUart3Counter=0;
	//memset(g_Uart3Buf,0x00,UART3_RX_BUF_SIZE);


	//g_bleAck_State=BLE_COM_IDLE;
	g_bletkre=0;
	
	for(j=0;j<2000;j++)
	{
		HAL_Delay(5);
		PKeybordProc();

		#ifdef WDG_ENABLE
	    HAL_IWDG_Refresh(&hiwdg);
		#endif
		if(g_bletkre)
			break;
		}
         
    #else
	
	p_target=token;

	for(i=0;i<value_len;i++)
	{
		if(p[i]!=' ')
		{	*p_target=p[i];
			p_target++;
			token_len++;
			}
		}

	/*if((strlen((char*)g_SmsReply)<SMS_REPLY_SIZE-strlen((char*)tag)-strlen((char*)token)-2)&&mqtt_cmd==FALSE)
		sprintf((char*)(g_SmsReply+strlen((char*)g_SmsReply)),"%s%s\",",tag,token);

	if(token_len<20&&mqtt_cmd==FALSE)
	{
		sprintf((char*)(g_SmsReply+strlen((char*)g_SmsReply)-1),"is not correct format. ");	
		return ;
		}

	if(token_len<20)
		g_sysStateCode.error.pwd_len_error=1;*/
	
		
    token_lo = AtouI(&token[10]);
	token[10]='\0';
	token_hi = AtouI(token);

	
	memset(token,0x00,32);

	token[13] = 0xc5;
    token[14] = 0x6a;
    token[15] = 0x29;
    token[16] = 0x0e;
    token[17] = 0xfa;

    token[21]= token_lo>>24;
    token[20]= token_lo>>16;
    token[19]= token_lo>>8;
    token[18]= token_lo;
    
    token[25]= token_hi>>24;
    token[24]= token_hi>>16;
    token[23]= token_hi>>8;
    token[22]= token_hi;
		
	#if 0//def CAMP_PROJECT	
	token[26]= CRC8(token + 13 ,13);	

	CampSend(token,28);

	for(i=0;i<500;i++)
	{	
		HAL_Delay(10);
		tokenstate=CampTokenState();
		if(tokenstate!=0xff)
			break;
		}

	
	if(mqtt_cmd==FALSE)
	{
		switch(tokenstate)
		{
			case 0:
				sprintf((char*)(g_SmsReply+strlen((char*)g_SmsReply)-1),"tops up succeeded ");	
				g_sysStateCode.error.pswd_error=0;
				g_sysStateCode.error.pwd_len_error=0;
				break;
			case 1:
				sprintf((char*)(g_SmsReply+strlen((char*)g_SmsReply)-1),"is latest and expired ");
				g_sysStateCode.error.pswd_error=1;
				break;
			case 2:
				sprintf((char*)(g_SmsReply+strlen((char*)g_SmsReply)-1)," tops up Failed ");
				g_sysStateCode.error.pswd_error=1;
				break;
			}
		}
		
	
	return ;
	#endif

	p=&token[13] ;

    temp[3] = p[12];
    temp[2] = p[11];
    temp[1] = p[10];
    temp[0] = p[9];
    temp[7] = p[8];
    temp[6] = p[7];
    temp[5] = p[6];
    temp[4] = p[5];

	memcpy((uint8_t*)payg.hast_input, temp, 8);

	//payg.hast_input[0]=token_lo;
	//payg.hast_input[1]=token_hi;

	PaygOvesInput();
	

	#ifdef E_MOB48V_PROJECT

	if( g_tokenState==TOKEN_OK)
	{
		#ifdef BMS_SMARTLI_SUPPROT
		SbmsWakeupRelease();
		#endif
		#ifdef BMS_JBD_SUPPROT
		//JbsMosCtrl(TRUE);
		JbsBmsInit();
		#endif
		}
	#endif

	#endif

	#ifdef DC_PUMP_SUPPORT
	if(/*PaygGetPayState()&&*/g_tokenState==TOKEN_OK)
		PumpOn(TRUE);
    #endif

	#if 0//def BLE_MASTER_ENABLE
	 GattGetData( LIST_STS, STS_TKRE, (uint8_t*)&tkre);
	 g_tokenState=(uint8_t)tkre;
	#endif


	/* if( g_tokenState==TOKEN_USEED)
	 {
	 	if(strlen((char*)g_SmsReply)<SMS_REPLY_SIZE-4-2)
		       memcpy(g_SmsReply+strlen((char*)g_SmsReply)-1," use\"",5);

		
	 	}
	else if( g_tokenState==TOKEN_ERROR)
	 {
	 	if(strlen((char*)g_SmsReply)<SMS_REPLY_SIZE-4-2)
		       memcpy(g_SmsReply+strlen((char*)g_SmsReply)-1," error\"",7);
	 	}*/

	#if 0//def BLE_MASTER_ENABLE

	tkre=g_tokenState;

	GattSetData( LIST_STS, STS_TKRE, (uint8_t*)&tkre);
	
	
	EEpUpdateEnable();
	#endif

}


uint32_t  PaygGetPayRemainDays(void)
{
	return payg.remaing_days;
}

uint32_t  PaygGetFreeState(void)
{
	if( YES == payg.free)
	 	return TRUE;
	else
		 return FALSE;
}

uint32_t  PaygGetRemainDays(void)
{
	return payg.remaing_days;
}

void TokenParse(uint8_t*str,uint8_t*tag)
{
	uint8_t tag_len,value_len;//,token_len=0;
	uint8_t /*token[32],*/*p,*p_target;
	uint32_t token_hi,token_lo;
	uint8_t temp[12];
	uint8_t token[32];
	uint8_t tokenstate=0,breakfor=0;
	//uint8_t mqtt_cmd=FALSE;
	uint8_t mqtt_cmd=FALSE,token_len=0;
	uint16_t i,j;
	uint16_t tkre=0;


	//if(strstr((char*)str,"/cmd/code")!=NULL)
	//	mqtt_cmd=TRUE;

	
	tag_len=strlen((char*)tag);
	value_len=AtCmdGetValueLen((char*)(str+tag_len),'#');
	memset(token,0x00,32);

	//memcpy(token,str+tag_len,value_len);

	p=str+tag_len;

	
	p_target=token;

	for(i=0;i<value_len;i++)
	{
		if(p[i]!=' ')
		{	*p_target=p[i];
			p_target++;
			token_len++;
			}
		}


	if(token_len<20)
		g_sysStateCode.error.pwd_len_error=1;
	
		
    token_lo = AtouI(&token[10]);
	token[10]='\0';
	token_hi = AtouI(token);

	
	memset(token,0x00,32);

	token[13] = 0xc5;
    token[14] = 0x6a;
    token[15] = 0x29;
    token[16] = 0x0e;
    token[17] = 0xfa;

    token[21]= token_lo>>24;
    token[20]= token_lo>>16;
    token[19]= token_lo>>8;
    token[18]= token_lo;
    
    token[25]= token_hi>>24;
    token[24]= token_hi>>16;
    token[23]= token_hi>>8;
    token[22]= token_hi;

	p=&token[13] ;

    temp[3] = p[12];
    temp[2] = p[11];
    temp[1] = p[10];
    temp[0] = p[9];
    temp[7] = p[8];
    temp[6] = p[7];
    temp[5] = p[6];
    temp[4] = p[5];

	memcpy((uint8_t*)payg.hast_input, temp, 8);

	//payg.hast_input[0]=token_lo;
	//payg.hast_input[1]=token_hi;

	PaygOvesInput();
	
	tkre=g_tokenState;

	//GattSetData( LIST_STS, STS_TKRE, (uint8_t*)&tkre);


	//OvStsProfile_Notify(BLE_STS_TKRE-BLE_STS_SSTC,MEM_SIZE_TKRE,(uint8_t*)&tkre);
	gatt_set_prop_data("tkre\0",(uint8_t*)&tkre);
	
	EEpUpdateEnable();

}


void PaygProcess(void)
{
	uint8_t syserr=GetSysErrorCode();
	uint8_t output_en=TRUE;
	uint16_t temp16=0;

	#ifdef OPEN_PAYGO
	OpenPaygoProcess();
	#endif
	if(g_PaygUpdate)
	{
		uint8_t temp[8];
		//extern  char hashshow[32];	

		g_PaygUpdate=FALSE;
		#ifdef OPEN_PAYGO
		ds1302_read_time (&Read_T);
		#endif

		if(payg.free != YES)
		{
			//HAL_Delay(2);
			tian = GetDs1302Day();
		}

		#ifdef SHOW_TIME
		Show_Time();
		#endif      
		PaygOvesTask();
		
		gatt_set_prop_data("ppid\0",payg.payg_id);//GattSetPpid(payg.payg_id,20);
		
		gatt_set_prop_data("opid\0",payg.oem_id);//GattSetOpid(payg.oem_id,20);
		
		gatt_set_prop_data("frmv\0",GATT_VERSON);//GattSetVersion(GATT_VERSON,5);
		#ifdef FLEED_ID_SUPPORT
		gatt_set_prop_data("flid\0",g_UserSet.fleed);//GattSetData(LIST_ATT,ATT_FLID,g_UserSet.fleed);
		#endif
		temp16=payg.remaing_days;
		gatt_set_prop_data("rcrd\0",(uint8_t*)&temp16);//GattSetRmPayDays(payg.remaing_days);//Remaining_PAYG_Days
		temp16=payg.days_has_been_runing;
		gatt_set_prop_data("trhd\0",(uint8_t*)&temp16); //GattSetRunDays(payg.days_has_been_runing);//Days have been running
		temp16=payg.recharge_days;
		gatt_set_prop_data("tpgd\0",(uint8_t*)&temp16); //GattSetPaygDays(payg.recharge_days);//PAYG_Dayss
		if( payg.free == YES)
			temp16=1;//GattSetPaygState(1);//PAYG_State
		else
			temp16=0;//GattSetPaygState(0);//PAYG_State
			
		gatt_set_prop_data("pgst\0",(uint8_t*)&temp16);

		if(payg.remaing_days>0||GetDemoState()||payg.free == YES)
			temp16=1;//GattSetOutputCtrlState(1);
		else
			temp16=0;//GattSetOutputCtrlState(0);
		gatt_set_prop_data("ocst\0",(uint8_t*)&temp16);
		
		gatt_set_prop_data("napn\0",(uint8_t*)g_UserSet.NetInfor.apn);//GattSetData(LIST_CMD, CMD_NAPN, (uint8_t*)g_UserSet.NetInfor.apn);
		gatt_set_prop_data("flid\0",(uint8_t*)g_UserSet.fleed);//GattSetData(LIST_ATT,ATT_FLID,g_UserSet.fleed);
		gatt_set_prop_data("sstc\0",(uint8_t*)&g_sysStateCode.statecode);//GattSetSysStatusCode(0);	
		gatt_set_prop_data("hbfq\0",(uint8_t*)&g_UserSet.heartbeat);//GattSetHeartbeat(g_UserSet.heartbeat);


	

		#if defined( PUMP_PROJECT)||defined(UI1K_V2_PROJECT)||defined(UI1K_V13_PROJECT)\
		||defined(E_MOB48V_PROJECT)||defined(P10KW_PROJECT)||defined(DI_INVERTER48V_PROJECT)\
		||defined(DI_PEICHENGBMS_PROJECT)||defined(DI_HEROEE2BMS_PROJECT)||defined(DI_EMCOOKE_PROJECT)\
		||defined(DI_LLZINVERTER_PROJECT)||defined(DI_FREEZER_PROJECT)||defined(DI_FAN_PROJECT)	
		Hash_Opration(payg.hast_otp);
		#else
		temp[0]=payg.hast_otp[0];
		temp[1]=payg.hast_otp[0]>>8;
		temp[2]=payg.hast_otp[0]>>16;
		temp[3]=payg.hast_otp[0]>>24;

		temp[4]=payg.hast_otp[1];
		temp[5]=payg.hast_otp[1]>>8;
		temp[6]=payg.hast_otp[1]>>16;
		temp[7]=payg.hast_otp[1]>>24;
		
		CampHashOpration(temp);
		#endif
		
		gatt_set_prop_data("pubk\0",(uint8_t*)hashshow);//GattSetHashTop((uint8_t*)hashshow,29);

		if(GetDemoState())
		{	
			if(g_Demotimecouter==0x0000)
			{	DemoModeClose();
				//TimerEventClear(TIMER_DEMO);
				}
			}


	}
  
	
}



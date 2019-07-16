#include "SCCB.h"
#include <string.h>

typedef unsigned char u8;

static void delay_us(unsigned int us)
{
	int len;
	for (;us > 0; us --)
     for (len = 0; len < 20; len++ );
	//
}



#define SDA_PORT GPIOB
#define SCL_PORT GPIOB
#define SDA_PORT_PIN GPIO_Pin_14
#define SCL_PORT_PIN GPIO_Pin_13



//IO��������
#define SDA_IN()  SDAPortIN()
#define SDA_OUT() SDAPortOUT();



#define SDA_H  GPIO_SetBits(SDA_PORT, SDA_PORT_PIN)
#define SDA_L  GPIO_ResetBits(SDA_PORT, SDA_PORT_PIN)

#define SCL_H  GPIO_SetBits(SCL_PORT, SCL_PORT_PIN)
#define SCL_L  GPIO_ResetBits(SCL_PORT, SCL_PORT_PIN)

#define READ_SDA GPIO_ReadInputDataBit(SDA_PORT, SDA_PORT_PIN)

static void SDAPortIN(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;memset(&GPIO_InitStructure,0x0,sizeof(GPIO_InitStructure));
	GPIO_InitStructure.GPIO_Pin = SDA_PORT_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IN;         
	GPIO_Init(SDA_PORT,&GPIO_InitStructure);
}

static void SDAPortOUT(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;memset(&GPIO_InitStructure,0x0,sizeof(GPIO_InitStructure));
	GPIO_InitStructure.GPIO_Pin = SDA_PORT_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_OUT;         
	GPIO_Init(SDA_PORT,&GPIO_InitStructure);
}

#define SIO_D_SET			SDA_H
#define SIO_C_SET			SCL_H
#define SIO_D_CLR			SDA_L
#define SIO_C_CLR			SCL_L

#define SIO_D_IN			SDAPortIN()
#define SIO_D_OUT			SDAPortOUT()
#define SIO_D_STATE		READ_SDA


//SCCB��ʼ�ź�
//��ʱ��Ϊ�ߵ�ʱ��,�����ߵĸߵ���,ΪSCCB��ʼ�ź�
//�ڼ���״̬��,SDA��SCL��Ϊ�͵�ƽ
void SCCB_Start(void)
{
	//SDA_H;     //�����߸ߵ�ƽ	   
	//SCL_H;	    //��ʱ���߸ߵ�ʱ���������ɸ�����
	SDA_H;
	SCL_H;
	delay_us(50);  
	//SDA_L;
	SDA_L;
	delay_us(50);	 
	//SCL_L;	    //�����߻ָ��͵�ƽ��������������Ҫ	  
	SCL_L;
}

//SCCBֹͣ�ź�
//��ʱ��Ϊ�ߵ�ʱ��,�����ߵĵ͵���,ΪSCCBֹͣ�ź�
//����״����,SDA,SCL��Ϊ�ߵ�ƽ
void SCCB_Stop(void)
{
	//SDA_L;
	SDA_L;
	delay_us(50);	 
	//SCL_H;	
	SCL_H;
	delay_us(50); 
	//SDA_H;	
	SDA_H;
	delay_us(50);
}  
//����NA�ź�
void SCCB_No_Ack(void)
{
	delay_us(50);
	//SDA_H;	
	//SCL_H;
	SDA_H;
	SCL_H;
	delay_us(50);
	//SCL_L;
	SCL_L;
	delay_us(50);
	//SDA_L;	
	SDA_L;
	delay_us(50);
}
//SCCB,д��һ���ֽ�
//����ֵ:0,�ɹ�;1,ʧ��. 
u8 SCCB_WR_Byte(u8 dat)
{
	u8 j,res;	 
	for(j=0;j<8;j++) //ѭ��8�η�������
	{
		if(dat&0x80)
		{
			SDA_H;
		}
		else 
		{
			SDA_L;
		}
		dat<<=1;
		delay_us(50);
		SCL_H;	
		delay_us(50);
		SCL_L;		   
	}			 
	SDAPortIN();		//����SDAΪ���� 
	delay_us(50);
	SCL_H;			//���յھ�λ,���ж��Ƿ��ͳɹ�
	delay_us(50);
	if(READ_SDA)res=1;  //SDA=1����ʧ�ܣ�����1
	else res=0;         //SDA=0���ͳɹ�������0
	SCL_L;		 
	SDAPortOUT();		//����SDAΪ���    
	return res;  
}	 
//SCCB ��ȡһ���ֽ�
//��SCL��������,��������
//����ֵ:����������
u8 SCCB_RD_Byte(void)
{
	u8 temp=0,j;    
	SDA_IN();		//����SDAΪ����  
	for(j=8;j>0;j--) 	//ѭ��8�ν�������
	{		     	  
		delay_us(50);
		SCL_H;
		temp=temp<<1;
		if(READ_SDA)temp++;   
		delay_us(50);
		SCL_L;
	}	
	SDA_OUT();		//����SDAΪ���    
	return temp;
} 							    
//д�Ĵ���
//����ֵ:0,�ɹ�;1,ʧ��.
u8 _sccbw(u8 reg,u8 data)
{
	u8 res=0;
	SCCB_Start(); 					//����SCCB����
	if(SCCB_WR_Byte(SCCB_ID))res=1;	//д����ID	  
	delay_us(100);
  if(SCCB_WR_Byte(reg))res=1;		//д�Ĵ�����ַ	  
	delay_us(100);
  if(SCCB_WR_Byte(data))res=1; 	//д����	 
  SCCB_Stop();	  
  return	res;
}		  					    
//���Ĵ���
//����ֵ:�����ļĴ���ֵ
u8 _sccbr(u8 reg)
{
	u8 val=0;
	SCCB_Start(); 				//����SCCB����
	SCCB_WR_Byte(SCCB_ID);		//д����ID	  
	delay_us(100);	 
  SCCB_WR_Byte(reg);			//д�Ĵ�����ַ	  
	delay_us(100);	  
	SCCB_Stop();   
	delay_us(100);	   
	//���üĴ�����ַ�󣬲��Ƕ�
	SCCB_Start();
	SCCB_WR_Byte(SCCB_ID|0X01);	//���Ͷ�����	  
	delay_us(100);
  val=SCCB_RD_Byte();		 	//��ȡ����
  SCCB_No_Ack();
  SCCB_Stop();
  return val;
}




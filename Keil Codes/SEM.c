#include <reg51.h>
sbit rs=P3^5;
sbit rw=P3^6;
sbit en=P3^7;
sbit sw=P1^6;
sbit r1=P1^1;
sbit sen=P1^7;
void gsm(void);
void sense(void);
unsigned int bill=0,count=0,u=0,count1,u1,bill1,unit;
float watt_factor=0.3125;
void delay(unsigned int x)
{
unsigned int i;
for(;x>0;x--)
for(i=0;i<225;i++);
}
unsigned char rx()
{
while(RI==0)
{
if(sen==0)
{
sense();
}
if(sw==0)
{
gsm();
}
}
RI=0;
return SBUF;
}
void tx(unsigned char ch)
{
SBUF=ch;
while(TI==0);
TI=0;
}
void uart_str(unsigned char *s)
{
while(*s)
tx(*s++);
}
void convert(unsigned int val)
{
unsigned char a1,a2,a3,a4;
a1=(val%10000)/1000+48;
a2=(val%1000)/100+48;
a3=(val%100)/10+48;
a4=(val%10)+48;
tx(a1);
tx(a2);
tx(a3);
tx(a4);
}
void cmd(unsigned char x)
{
P2=x;
rs=0;
rw=0;
en=1;
delay(10);
en=0;
delay(10);
}
void lcdinit()
{
 cmd(0x38); 
 cmd(0x0e); 
 cmd(0x01); 
 cmd(0x06); 
}
void disp(unsigned char x)
{
P2=x;
rs=1;
rw=0;
en=1;
delay(10);
en=0;
delay(10);
}
void disp_str(unsigned char *x)
{
while(*x)
{
disp(*x++);
}
}
void lcd_val(unsigned int val)
{
unsigned char a1,a2,a3,a4;
a1=(val%10000)/1000+48;
a2=(val%1000)/100+48;
a3=(val%100)/10+48;
a4=(val%10)+48;
disp(a1);
disp(a2);
disp(a3);
disp(a4);
}
void main()
{
unsigned char i=0,ch=0,n=0,j=0,dum[10],c=0;
unsigned char num[15];
lcdinit();
SCON=0x50;
TMOD=0x20;
TH1=-3;
TR1=1;
 cmd(0x01);
delay(500);
r1=0;
cmd(0x80);
disp_str("GSM BASED ENERGY ");
cmd(0xc0);
disp_str("METER READING ....");
delay(2000);
cmd(0x01);
 cmd(0x80);
disp_str("AT");
uart_str("AT");
tx(0x0A);
tx(0x0D);
delay(1000);
cmd(0x01);
 cmd(0x80);
disp_str("AT+CMGF=1");
uart_str("AT+CMGF=1");
tx(0x0A);
tx(0x0D);
delay(1000);
uart_str("AT+CMGD=");
tx('1');
tx(0x0A);
tx(0x0D);
delay(2000);
uart_str("AT+CMGD=");
tx('2');
tx(0x0A);
tx(0x0D);
delay(2000);
uart_str("AT+CMGD=");
tx('3');
tx(0x0A);
tx(0x0D);
delay(2000);
 cmd(0x01);
 cmd(0x80);
 disp_str("PULSE COUNT:");
 cmd(0x8C);
 lcd_val(count);
 cmd(0xC0);
 disp_str("NO.UNITS:");
 cmd(0xC9);
 lcd_val(u);
RI=0;
r1=1;
while(1)
{ 
 while(rx() != ',');
 n=rx();
cmd(0x01);
 uart_str("AT+CMGR=");
tx(n);
tx(0x0A);
tx(0x0D);
while(rx() != ',');
for(i=0;i<14;i++)
num[i]=rx();
do {
c=rx();
 }while(rx() != 0x0D);
j=0;
l: dum[j]=rx();
if(dum[j] !=0x0D)
{
j++;
goto l;
}
cmd(0x01);
cmd(0x80);
for(j=0;dum[j] != 0x0D ;j++)
disp(dum[j]);
delay(2000);
 uart_str("AT+CMGD=");
tx(n);
tx(0x0A);
tx(0x0D);
delay(2000);
if(dum[2]=='1')
{
cmd(0x01);
cmd(0x80);
 disp_str("PLEASE PAY BILL");
 cmd(0xC0);
 disp_str("BILL:");
cmd(0xC5);
 lcd_val(bill);
r1=0;
delay(2000);
}
else if(dum[2]=='2')
{
cmd(0x01);
cmd(0x80);
 disp_str("BILL PAID .........");
cmd(0xc0);
 disp_str("POWER ON ..........");
r1=1;
delay(2000);
cmd(0x01);
 count=count-count1;
u=u-u1;
cmd(0x01);
RI=0;
cmd(0x01);
cmd(0x80);
 disp_str("PULSE COUNT:");
cmd(0x8C);
 lcd_val(count);
cmd(0xC0);
 disp_str("NO.UNITS:");
cmd(0xC9);
 lcd_val(u);
count1=0;
u1=0;
}
else
{
cmd(0x01);
 cmd(0x80);
 disp_str("WRONG MESSAGE..");
delay(1000);
cmd(0x01);
}
ch=0;
}
}
void gsm()
{
RI=0;
uart_str("AT");
tx(0x0A);
tx(0x0D);
delay(2000);
uart_str("AT+CMGF=1");
tx(0x0A);
tx(0x0D);
delay(2000);
cmd(0x01);
cmd(0x80);
disp_str("MESSAGE SENDING....");
uart_str("AT+CMGS=");
tx('"');
uart_str("7993776927");
tx('"');
tx(0x0A);
tx(0x0D);
delay(2000);
uart_str("MONTH COMPLETED PLEASE PAY THE BILL AMOUNT AND UNITS IS:");
tx(0x0A);
tx(0x0D);
uart_str("UNITS:");
 convert(u);
tx(0x0A);
tx(0x0D);
uart_str("RS:");
bill=u*5;
convert(bill);
delay(2000);
cmd(0x01);
tx(0x1A);
delay(3000);
uart_str("AT");
tx(0x0A);
tx(0x0D);
delay(2000);
uart_str("AT+CMGF=1");
tx(0x0A);
tx(0x0D);
delay(2000);
cmd(0x01);
cmd(0x80);
disp_str("MESSAGE SENDING....");
uart_str("AT+CMGS=");
tx('"');
uart_str("7013684692");
tx('"');
tx(0x0A);
tx(0x0D);
delay(2000);
uart_str("MONTH COMPLETED PLEASE PAY THE BILL AMOUNT AND UNITS IS:");
tx(0x0A);
tx(0x0D);
uart_str("UNITS:");
 convert(u);
tx(0x0A);
tx(0x0D);
uart_str("RS:");
bill=u*5;
convert(bill);
delay(2000);
cmd(0x01);
tx(0x1A);
delay(3000);
bill1=bill;
count1=count;
u1=u;
delay(1000);
RI=0;
cmd(0x80);
 disp_str("MONTH COMPLETED....");
 cmd(0xc0);
 disp_str("PLEASE PAY BILL ");
 delay(1000);
 cmd(0x01);
 cmd(0x80);
 disp_str("PULSE COUNT:");
 cmd(0x8C);
 lcd_val(count);
 cmd(0xC0);
 disp_str("NO.UNITS:");
 cmd(0xC9);
 lcd_val(u);
}
void sense() 
{
count++;
cmd(0x8C);
lcd_val(count);
u=watt_factor*count/1;
 cmd(0xC9);
lcd_val(u);
delay(200);
}
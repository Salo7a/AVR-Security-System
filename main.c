/*
 * Security System Phase 1.c
 *
 * Created: 2/23/2018 3:14:23 PM
 * Author : Ahmed
 */ 

#include <avr/interrupt.h>
#include <avr/io.h>
#include <string.h>
#define F_CPU 1000000UL
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
#include "LCD.h"
#define LCD_Dir  DDRB			/* Define LCD data port direction */
#define LCD_Port PORTB			/* Define LCD data port */
#define RS PB0				/* Define Register Select pin */
#define EN PB1
#define KeypadDirectionRegister DDRD
#define KeypadControl PORTD
#define KeypadVal PIND
#define LEDDirectionRegister DDRB
#define LEDPort PORTB
char keypadScan();
void correct(),wrong();

uint8_t tries = 0;
char written[5];
uint8_t selected = 0;
uint8_t count=0;
char temp[5];
char w;
int d0,d1,seg;

void ADC_Init(){
	DDRA = 0;	        /* Make ADC port as input */
	ADCSRA = 0x87;          /* Enable ADC, with freq/128  */
	ADMUX = 0x40;           /* Vref: Avcc, ADC channel: 0 */
}

int ADC_Read(char channel)
{
	ADMUX = 0x40 | (channel & 0x07);   /* set input channel to read */
	ADCSRA |= (1<<ADSC);               /* Start ADC conversion */
	while (!(ADCSRA & (1<<ADIF)));     /* Wait until end of conversion by polling ADC interrupt flag */
	ADCSRA |= (1<<ADIF);               /* Clear interrupt flag */
	_delay_ms(1);                      /* Wait a little bit */
	return ADCW;                       /* Return ADC word */
}

void segment_code(uint8_t d0,uint8_t d1)
{
	switch(d0)
	{
		case 1:
		PORTA=0b00000010; //1
		_delay_ms(5);
		break;
		case 2:
		PORTA=0b00000100; //2
		_delay_ms(5);
		break;
		case 3:
		PORTA=0b00000110; //3
		_delay_ms(5);
		break;
		case 4:
		PORTA=0b00001000; //4
		_delay_ms(5);
		break;
		case 5:
		PORTA=0b00001010; //5
		_delay_ms(5);
		break;
		case 6:
		PORTA=0b00001100; //6
		_delay_ms(5);
		break;
		case 7:
		PORTA=0b00001110; //7
		_delay_ms(5);
		break;
		case '8':
		PORTA=0b00010000; //8
		_delay_ms(5);
		break;
		case 9:
		PORTA=0b00010010; //9
		_delay_ms(5);
		break;
		case 0:
		PORTA=0b00000000; //0
		_delay_ms(5);
		break;
		default:
		PORTA=0b00000000;
		_delay_ms(5);
	}
	switch(d1)
	{
		case 1:
		PORTC=0b00010001; //1
		_delay_ms(5);
		break;
		case 2:
		PORTC=0b00100001; //2
		_delay_ms(5);
		break;
		case 3:
		PORTC=0b00110001; //3
		_delay_ms(5);
		break;
		case 4:
		PORTC=0b01000001; //4
		_delay_ms(5);
		break;
		case 5:
		PORTC=0b01010001; //5
		_delay_ms(5);
		break;
		case 6:
		PORTC=0b01100001; //6
		_delay_ms(5);
		break;
		case 7:
		PORTC=0b01110001; //7
		_delay_ms(5);
		break;
		case 8:
		PORTC=0b10000001; //8
		_delay_ms(5);
		break;
		case 9:
		PORTC=0b10010001; //9
		_delay_ms(5);
		break;
		case 0:
		PORTC=0b00000001; //0
		_delay_ms(5);
		break;
		default:
		PORTC=0b00000001;
		_delay_ms(5);
	}
}


char LCD_code(int d0)
{
	switch(d0)
	{
		case 1:
		return '1';
		break;
		case 2:
		return '2';
		break;
		case 3:
		return '3';
		break;
		case 4:
		return '4';
		break;
		case 5:
		return '5';
		break;
		case 6:
		return '6';
		break;
		case 7:
		return '7';
		break;
		case '8':
		return '8';
		break;
		case 9:
		return '9';
		break;
		case 0:
		return '0';
		break;
		default:
		return '0';
	}
}








int main()
{
	MCUCSR=(1<<JTD);
	MCUCSR=(1<<JTD);
	char pass[5] = "5555";
	char temp[5] = "8888";
	char admin[5] = "0000";
	written[4] = '\0';
	//By5ali written string mn 4 7orof 3ashan ast3ml functions el string,'\0' m3naha en kol elly ableh string
	DDRB=0xff; //Red LED,Buzzer(lw hn7ot)
	DDRC=0xFF;//BiColor LED(Red-Green)(0b00000010 tnwr RED~~0b00000001 tnwr Green)(Fara8)
	DDRA=0b11111110;
	//initADC();
	ADC_Init();
	uint8_t d0,d1;
	//int analogVal;
	uint8_t i;
	uint8_t unlocked = 0;
	uint8_t fire = 0;
	uint8_t change = 0;
	uint8_t selected = 0;
	LCD_Init();			/* Initialization of LCD*/
	LCD_String("Developers Ltd.");	/* Write string on 1st line of LCD*/
	LCD_Command(0xC0);		/* Go to 2nd line*/
	LCD_String("Welcomes You!");
	_delay_ms(5000);
	while(1)
	{
		if (unlocked == 0)
		{
			i  = 0;
			PORTC = 0x02;
			memset(written,0,sizeof(written));
			written[4] = '\0';
			//Eft7 el LED el 7mra
			while(i<4)
			{

				// Recheck if a button is pressed
				LCD_Clear();
				LCD_String("System Password:");
				LCD_Command(0xC0);
				LCD_Encoded(written);
				written[i] = keypadScan();
				if (written[i]=='D')
				{
					if (i>0)
					{
						written[i] = '\0';
						i--;
						written[i] = '\0';
					}
					written[i] = '\0';
				}
				else if(written[i]=='C')
				{
					memset(written,0,sizeof(written));
					written[4] = '\0';
					i=0;
				}
				else i++;
				LCD_Clear();
				LCD_String("System Password:");
				LCD_Command(0xC0);
				LCD_Encoded(written);
				_delay_ms(500);
				//Delay 3ashan mysglsh nfs el rkm mrtin(aw aktr y3ni)
			}
			if (strcmp(pass,written) == 0)
			{
				correct();
				unlocked++;
				//strcmp btrg3 0 lw el 2 strings zy b3d
			}
			else
			{
				tries++;
				wrong();
			}
			
		}
		else if (unlocked == 1 && selected == 0 && change == 0)
		{
			i = 0;
			LCD_Clear();
			LCD_String("Main Menu:");
			LCD_Command(0xC0);
			LCD_String("1.AC   2.Admin");
			written[i] = keypadScan();
			if (written[i] == '1')
			{
				selected = 1;
			}
			else if (written[i] == '2')
			{
				selected = 2;
			}
			else if (written[i] == 'B')
			{
				selected = 0;
				unlocked = 0;
				memset(written,0,sizeof(written));
				written[4] = '\0';
			}
		}
		else if (unlocked == 1 && fire == 0 && selected == 1 && change ==0)
		{
			i  = 0;
			PORTC = 0x01;
			memset(written,0,sizeof(written));
			written[4] = '\0';
			//Eft7 el LED el 5dra
			while(i<4)
			{
				LCD_Clear();
				LCD_String("AC Password:");
				LCD_Command(0xC0);
				LCD_Encoded(written);
				written[i] = keypadScan();
				// Recheck if a button is pressed
				
				if (written[i]=='D')
				{if (i>0)
					{
						written[i] = '\0';
						i--;
						written[i] = '\0';
					}
					written[i] = '\0';
				}
				else if(written[i]=='C')
				{
					memset(written,0,sizeof(written));
					written[4] = '\0';
					i=0;
				}
				else if (written[i]=='B')
				{
					memset(written,0,sizeof(written));
					written[4] = '\0';
					unlocked--;
					break;
				}
				else i++;
				LCD_Clear();
				LCD_String("AC Password:");
				LCD_Command(0xC0);
				LCD_Encoded(written);
				_delay_ms(500);
				//Delay 3ashan mysglsh nfs el rkm mrtin(aw aktr y3ni)
			}
			if(written[i] != 'B' && strlen(written) == 4)
			{
				if (strcmp(temp,written) == 0  )
				{
					correct();
					fire++;
					//strcmp btrg3 0 lw el 2 strings zy b3d
				}
				else
				{
					tries++;
					if (tries==5)
					{
						unlocked=0;
					}
					wrong();
				}
			}
		}
		else if (unlocked == 1 && fire == 0 && selected == 2 && change == 0)
		{
			i  = 0;
			PORTC = 0x01;
			memset(written,0,sizeof(written));
			written[4] = '\0';
			//Eft7 el LED el 5dra
			while(i<4)
			{
				LCD_Clear();
				LCD_String("Admin Password:");
				LCD_Command(0xC0);
				LCD_Encoded(written);
				written[i] = keypadScan();
				// Recheck if a button is pressed
				
				if (written[i]=='D')
				{if (i>0)
					{
						written[i] = '\0';
						i--;
						written[i] = '\0';
					}
					written[i] = '\0';
				}
				else if(written[i]=='C')
				{
					memset(written,0,sizeof(written));
					written[4] = '\0';
					i=0;
				}
				else if (written[i]=='B')
				{
					memset(written,0,sizeof(written));
					written[4] = '\0';
					change=0;
					selected=0;
					break;
				}
				else i++;
				LCD_Clear();
				LCD_String("Admin Password:");
				LCD_Command(0xC0);
				LCD_Encoded(written);
				_delay_ms(500);
				//Delay 3ashan mysglsh nfs el rkm mrtin(aw aktr y3ni)
			}
			if (written[i] !='B' && strlen(written) == 4)
			{
				if (strcmp(admin,written) == 0)
				{
					correct();
					selected = 0;
					change++;
					//strcmp btrg3 0 lw el 2 strings zy b3d
				}
				else
				{
					tries++;
					if (tries==5)
					{
						unlocked=0;
						selected=0;
						change =0;
						fire=0;
					}
					wrong();
				}
			}
		}
		else if (unlocked ==1 && change ==1 && selected == 0)
		{
			i = 0;
			LCD_Clear();
			LCD_String("Change Password:");
			LCD_Command(0xC0);
			LCD_String("1.Sys 2.AC 3.Adm");
			written[i] = keypadScan();
			if (written[i] == '1')
			{
				selected = 1;
			}
			else if (written[i] == '2')
			{
				selected = 2;
			}
			else if (written[i] == '3')
			{
				selected = 3;
			}
			else if (written[i] == 'B')
			{
				selected = 0;
				change=0;
				memset(written,0,sizeof(written));
				written[4] = '\0';
			}
			
		}
		else if (unlocked == 1 && change == 1 && selected ==1)
		{
			i  = 0;
			PORTC = 0x01;
			memset(written,0,sizeof(written));
			written[4] = '\0';
			//Eft7 el LED el 5dra
			while(i<4)
			{
				LCD_Clear();
				LCD_String("New Sys Pass:");
				LCD_Command(0xC0);
				LCD_Encoded(written);
				written[i] = keypadScan();
				// Recheck if a button is pressed
				
				if (written[i]=='D')
				{if (i>0)
					{
						written[i] = '\0';
						i--;
						written[i] = '\0';
					}
					written[i] = '\0';
				}
				else if(written[i]=='C')
				{
					memset(written,0,sizeof(written));
					written[4] = '\0';
					i=0;
				}
				else if (written[i]=='B')
				{
					memset(written,0,sizeof(written));
					written[4] = '\0';
					selected = 0;
					break;
				}
				else i++;
				LCD_Clear();
				LCD_String("New Sys Pass:");
				LCD_Command(0xC0);
				LCD_Encoded(written);
				_delay_ms(500);
				//Delay 3ashan mysglsh nfs el rkm mrtin(aw aktr y3ni)
			}
			if (written[i] != 'B' && strlen(written) == 4)
			{
				strcpy(pass,written);
				if (strcmp(pass,written) == 0)
				{
					LCD_Clear();
					LCD_String("Success! Sys");
					_delay_ms(800);
					selected=0;
					change=0;
					fire=0;
					//strcmp btrg3 0 lw el 2 strings zy b3d
				}
			}

		}
		else if (unlocked == 1 && change == 1 && selected ==2)
		{
			i  = 0;
			PORTC = 0x01;
			memset(written,0,sizeof(written));
			written[4] = '\0';
			//Eft7 el LED el 5dra
			while(i<4)
			{
				LCD_Clear();
				LCD_String("New AC Pass:");
				LCD_Command(0xC0);
				LCD_Encoded(written);
				written[i] = keypadScan();
				// Recheck if a button is pressed
				
				if (written[i]=='D')
				{if (i>0)
					{
						written[i] = '\0';
						i--;
						written[i] = '\0';
					}
					written[i] = '\0';
				}
				else if(written[i]=='C')
				{
					memset(written,0,sizeof(written));
					written[4] = '\0';
					i=0;
				}
				else if (written[i]=='B')
				{
					memset(written,0,sizeof(written));
					written[4] = '\0';
					selected = 0;
					break;
				}
				else i++;
				LCD_Clear();
				LCD_String("New AC Pass:");
				LCD_Command(0xC0);
				LCD_Encoded(written);
				_delay_ms(500);
			}
			if (written[i]!='B' && strlen(written) == 4)
			{
				strcpy(temp,written);
				if (strcmp(temp,written) == 0)
				{
					LCD_Clear();
					LCD_String("Success! AC");
					_delay_ms(800);
					selected = 0;
					change--;
				}
			}
		}
		else if (unlocked == 1 && change == 1 && selected ==3)
		{
			i = 0;
			LCD_Clear();
			LCD_String("New Admin Pass:");
			LCD_Command(0xC0);
			LCD_Encoded(written);
			written[i] = keypadScan();
			// Recheck if a button is pressed
			
			if (written[i]=='D')
			{if (i>0)
				{
					written[i] = '\0';
					i--;
					written[i] = '\0';
				}
				written[i] = '\0';
			}
			else if(written[i]=='C')
			{
				memset(written,0,sizeof(written));
				written[4] = '\0';
				i=0;
			}
			else if (written[i]=='B')
			{
				memset(written,0,sizeof(written));
				written[4] = '\0';
				selected = 0;
				break;
			}
			else i++;
			LCD_Clear();
			LCD_String("New Admin Pass:");
			LCD_Command(0xC0);
			LCD_Encoded(written);
			_delay_ms(500);
			if (written[i] != 'B' && strlen(written) == 4)
			{
				strcpy(admin,written);
				if (strcmp(admin,written) == 0)
				{
					LCD_Clear();
					LCD_String("Success! Admin");
					_delay_ms(800);
					selected = 0;
					change--;
				}
			}	
		}
		else if (unlocked == 1 && fire == 1)
		{
			char bbb[3];
			LCD_Clear();
			LCD_String("Temperature: ");
			ADC_Init();
			asm volatile("nop");
			asm volatile("nop");
			count = (ADC_Read(0)*4.88);
			count = (count/10.00);
			// 				 analogVal=ReadADC(0);
			// 				 analogVal=analogVal/4;
			// 				 count= analogVal-analogVal/29;
			//variable do containing the 10th digit of temperature
			DDRA=0b11111110;
			asm volatile("nop");
			asm volatile("nop");
			d0= (int) count%10;
			d1=(int) count/10;
			d1=d1%10;
			segment_code(d1,d0);
			_delay_ms(100);
			snprintf(bbb,3,"%d",count);
			//int x =count; 
			//itoa(x,temp,10);
// 			LCD_String(LCD_code(d0));
// 			LCD_String(LCD_code(d1));
            if (count > 19)
            {
				LCD_String("2");
            }
            else if (count>29)
            {
				LCD_String("3");
            }
			else
			{
				LCD_String("2");
			}
			LCD_String(bbb);
			LCD_String("C");
			LCD_Command(0xC0);
			LCD_String("Fan: ");
			if (count>35)
			{
				LCD_String("On");
				PORTC |= 0b00001000;
				PORTB = 0b00001111;
				PORTA &= 0b00111111;
				PORTA |= 0b00100000;
				
			}
			else if (count<35 && count>20)
			{
				LCD_String("Off");
				PORTC &= 0b11110111;
				PORTB = 0;
				PORTA &= 0b01011111;
				PORTA |= 0b01000000;
			}
			else
			{
				LCD_String("Off");
				PORTA &= 0b10011111;
				PORTA |= 0b10000000;
				PORTC &= 0b11110111;
				PORTB = 0;
			}
			_delay_ms(800);
			KeypadDirectionRegister|=(0X80>>3);
			if (!(KeypadVal & (0X08>>1)))
			{
				fire--;
				selected--;
				memset(written,0,sizeof(written));
				written[4] = '\0';
			}
			// 				 if ((PIND & (1<<PD0)==1)||(PIND & (1<<PD1)==1)||(PIND & (1<<PD2)==1)||(PIND & (1<<PD3)==1))
			// 				 {
			// 					 unlocked--;
			// 					 fire--;
			// 				 }
		}
		
	}
	return 0;
}



char keypadScan()
{

	uint8_t r,c;

	KeypadControl|= 0X0F;
	_delay_ms(50);
	while(1)
	{
		//Rows kolha input, Columns Outputs
		//Bydi 3la el Columns 1 bl tarteb w7da w7da bsor3a
		//L7d ma t5osh gowa Row lama tdos, f n3rf el row wl Column
		for(c=0;c<4;c++)
		{
			KeypadDirectionRegister&=~(0XFF);

			KeypadDirectionRegister|=(0X80>>c);
			for(r=0;r<4;r++)
			{
				if(!(KeypadVal & (0X08>>r)))
				{
					if(r==0 && c==0)return'A';
					if(r==0 && c==1)return'1';
					if(r==0 && c==2)return'2';
					if(r==0 && c==3)return'3';
					if(r==1 && c==0)return'4';
					if(r==1 && c==1)return'5';
					if(r==1 && c==2)return'6';
					if(r==1 && c==3)return'B';
					if(r==2 && c==0)return'7';
					if(r==2 && c==1)return'8';
					if(r==2 && c==2)return'9';
					if(r==2 && c==3)return'C';
					if(r==3 && c==0)return'*';
					if(r==3 && c==1)return'0';
					if(r==3 && c==2)return'#';
					if(r==3 && c==3)return'D';
					
				}
			}
		}
	}
	return 'E';
}
void correct()
{
    LCD_Clear();
	LCD_String("Correct!");
	//Reset ll Error Counters
	tries=0;
	//PORTB = 0x00;
	PORTC=0b00000100;
	int i;
	//Blink 4 Times Green
	for(i=0;i<4;i++)
	{
		_delay_ms(500);
		PORTC ^=1<<PINC0;
		_delay_ms(500);
		PORTC ^=1<<PINC0;
	}
	PORTC = 0b00000101;
	_delay_ms(1000);
	PORTC = 0b00000001;

}
void wrong()
{
	//Kol Mo7wla 8lt hynwr RED LED zyada
	//Lma ywsl ll 5amsa LET THE SHOW BEGIN
	char ti[5];
	LCD_Clear();
	LCD_String("Wrong Password!");
	LCD_Command(0xC0);
	LCD_String("Tries Left: ");
	if (tries==1)
	{
		LCD_String("4");
// 		PORTA = 0x01;
// 		_delay_ms(500);
// 		PORTB = 0x00;
// 		_delay_ms(500);
// 		PORTB = 0x01;
	}
	if (tries==2)
	{
		LCD_String("3");
// 		PORTB = 0x03;
// 		_delay_ms(500);
// 		PORTB = 0x01;
// 		_delay_ms(500);
// 		PORTB = 0x03;
// 		
	}
	if (tries==3)
	{
		LCD_String("2");
// 		PORTB = 0x07;
// 		_delay_ms(500);
// 		PORTB = 0x03;
// 		_delay_ms(500);
// 		PORTB = 0x07;
		
		
	}
	if (tries==4)
	{
		LCD_String("1");
// 		PORTB = 0x0F;
// 		_delay_ms(500);
// 		PORTB = 0x07;
// 		_delay_ms(500);
// 		PORTB = 0x0F;
	}
	if (tries == 5)
	{
	for (int i=0;i<30;i++)
	{
		LCD_Clear();
		LCD_String("Please Wait!");
		LCD_Command(0xC0);
		itoa(30-i,ti,10);
		LCD_String(ti);
		_delay_ms(1000);
		
	}
// 		PORTB = 0x1F;
// 		_delay_ms(500);
// 		PORTB = 0x0F;
// 		_delay_ms(500);
// 		PORTB = 0x1F;
// 		_delay_ms(4000);
// 		int i,j,k;
// 		//El noor el byo3od yt7rk mabin LEDS, el task bta3 Session 2 (T2rebn)
// 		for(i = 0;i<14;i++)
// 		{
// 			for( j = 0; j<5;j++)
// 			{
// 				PORTB = (0x01 << j);
// 				_delay_ms(200);
// 			}
// 			for(k = 0; k<5;k++)
// 			{
// 				PORTB = (0x10 >> k);
// 				_delay_ms(200);
// 			}
         tries = 0;
		}
		//Reset el error counter
		
		/*PORTB = 0x00;*/
_delay_ms(1000);
	}



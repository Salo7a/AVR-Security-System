/*
 * Security System Phase 1.c
 *
 * Created: 2/23/2018 3:14:23 PM
 * Author : Ahmed
 */ 

#include <avr/io.h>
#include <string.h>
#define F_CPU 1000000UL
#include <util/delay.h>
#define KeypadDirectionRegister DDRA
#define KeypadControl PORTA
#define KeypadVal PINA
#define LEDDirectionRegister DDRB
#define LEDPort PORTB
char keypadScan();
void correct(),wrong();
uint8_t tries = 0;
char written[5];
int main()
{
	MCUCSR=(1<<JTD);
	MCUCSR=(1<<JTD);
	char pass[5] = "1234";
	written[4] = '\0';
	//By5ali written string mn 4 7orof 3ashan ast3ml functions el string,'\0' m3naha en kol elly ableh string
	DDRB=0xff; //Red LED,Buzzer(lw hn7ot)
	DDRC=0b00001111;  //BiColor LED(Red-Green)(0b00000010 tnwr RED~~0b00000001 tnwr Green)(Fara8)
	

	uint8_t i;
	while (1)
	{
		i  = 0;
		PORTC = 0x02;
		//Eft7 el LED el 7mra
		while(i<4)
		{
			written[i] = keypadScan();
			// Recheck if a button is pressed
			i++;
			_delay_ms(500);
			//Delay 3ashan mysglsh nfs el rkm mrtin(aw aktr y3ni)
		}
		if (strcmp(pass,written) == 0)
		{
			correct();
			//strcmp btrg3 0 lw el 2 strings zy b3d
		}
		else
		{
			tries++;
			wrong();
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
	//Reset ll Error Counters
	tries=0;
	PORTB = 0x00;
	PORTC=0b00001100;
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
	_delay_ms(5000);

}
void wrong()
{
	//Kol Mo7wla 8lt hynwr RED LED zyada
	//Lma ywsl ll 5amsa LET THE SHOW BEGIN
	if (tries==1)
	{
		PORTB = 0x01;
		_delay_ms(500);
		PORTB = 0x00;
		_delay_ms(500);
		PORTB = 0x01;
	}
	if (tries==2)
	{
		PORTB = 0x03;
		_delay_ms(500);
		PORTB = 0x01;
		_delay_ms(500);
		PORTB = 0x03;
		
	}
	if (tries==3)
	{
		PORTB = 0x07;
		_delay_ms(500);
		PORTB = 0x03;
		_delay_ms(500);
		PORTB = 0x07;
		
		
	}
	if (tries==4)
	{
		PORTB = 0x0F;
		_delay_ms(500);
		PORTB = 0x07;
		_delay_ms(500);
		PORTB = 0x0F;
	}
	if (tries == 5)
	{
		PORTB = 0x1F;
		_delay_ms(500);
		PORTB = 0x0F;
		_delay_ms(500);
		PORTB = 0x1F;
		_delay_ms(4000);
		int i,j,k;
		//El noor el byo3od yt7rk mabin LEDS, el task bta3 Session 2 (T2rebn)
		for(i = 0;i<14;i++)
		{
			for( j = 0; j<5;j++)
			{
				PORTB = (0x01 << j);
				_delay_ms(200);
			}
			for(k = 0; k<5;k++)
			{
				PORTB = (0x10 >> k);
				_delay_ms(200);
			}
		}
		//Reset el error counter
		tries = 0;
		PORTB = 0x00;

	}	
}

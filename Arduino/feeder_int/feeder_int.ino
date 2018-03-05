/****************************************************
 *   QQFeeder智能自动喂食器 V1.0                    *
 *   作者:zhb1190                                   *
 *   邮箱:zhb_account@163.com                       *
 *   教程:http://www.log4cpp.com/smarthome/52.html  *
 *   时间:2018-3-5                                  *
 ****************************************************/
 
#include <EEPROM.h>
#include "configure.h"
#include "DS1302.h"
#include "uartWIFI.h"
#include "stepper28BYJ48.h"

PROGMEM const char password[] = WIFIPASSWORD;
const char* server = CLOUND_SERVER;


struct alarmt
{
	uint8_t hr;
	uint8_t min;
	uint8_t sec;
};

struct alarmt alarm1 = ALARM_ONE;
struct alarmt alarm2 = ALARM_TWO;

WIFI wifi;

//定义DS1302对象
//DS1302的地线和电源线不能太长，否则会出现乱码，或者读不出数据
//发热问题在8脚和电池正之间反向串一个1N4148二极管 或者不接主电源
DS1302 rtc(PIN_CE, PIN_DATA, PIN_SCLK);

/* 上次获取时间的小时 */
char lasthour = 0; 
char tosend = 0;
char alarmed = 0;

#define ALARM1 0x01
#define ALARM2 0x02

#define SetAlarmed(n) (alarmed |= n)
#define ClearAlarmed(n) (alarmed &=(~n))
#define IsAlarmed(n) (alarmed & n)
/* 判断是否旋转够了一圈，如果够一圈了就返回1 */
#define IsOneCycle(cur,last) (cur==NOSHELTER && last==SHELTER)

// 初始化步进电机要使用的Arduino的引脚编号
Stepper myStepper(MOTOR_STEPS, MOTOR_PIN1, MOTOR_PIN2, MOTOR_PIN3, MOTOR_PIN4);

#define CHECKER 12

/* 无遮挡 */
#define NOSHELTER 1
/* 有遮挡 */
#define SHELTER 0

/* 旋转的圈数 */
unsigned char feedCounter= ALARM_STEP;
unsigned char getconf=0;
#define FEEDERCYCLE feedCounter

const byte interruptPin = 2;
volatile char interrupted = 0;

char sendData(const char* thisData);
char sendData(const char* thisData,char retryCount);

char cmparetime(Time* t, struct alarmt* t1)
{
	if(t->hour != t1->hr)
	{
		return false;
	}
	if(t->hour == t1->hr && t->min > t1->min)
	{
		return true;
	}
	if(t->hour == t1->hr && t->min == t1->min && t->sec > t1->sec)
	{
		return true;
	}
	if(t->hour == t1->hr &&
		t->min == t1->min &&
		t->sec == t1->sec)
	{
		return true;
	}
	return false;
}

/* 多尝试几次，防止误读 */
char ShakeFilter()
{
	char c=0;

	while(c<3)
	{
		/* 只要有一次是有遮挡的,那么就认为有遮挡 */
		if(digitalRead(CHECKER)==SHELTER)
		{
			return SHELTER;
		}
		c++;
		delay(1);
	}
	return NOSHELTER;
}

/*
	旋转60度
	返回值：1成功，0失败
*/
char OneStep(int& ta, char& tc)
{
	int a=0; //每圈尝试转的次数
	char c=0; //实际转的圈数
	char direct=1; 
	char steper = -4;
	char tc1=0;
	int offcount=0;
	//Serial.println("OneStep");
	/* 
	   如果这时灯是亮的，要尝试让灯熄灭，
	   一共尝试调整68次（34次就可以转120度了），如果还是有遮挡只有放弃了 
	*/
	for(a=0; a<171*2; a++)
	{
		if(interrupted!=1)
		{
			myStepper.step(steper);
		}
	}
	interrupted = 0;
	while(c<1)
	{
		//Serial.print("c=");
		//Serial.println((int)c);
		if(tc1>2*1)
		{
			break;
		}
		//Serial.print("direct:");
		//Serial.println((int)direct);
		
		/* 
		   如果卡住了，朝一个方向转就转不动了（障碍物会被越夹越紧），
		   所以需要换一个方向尝试（让障碍物掉下来，就可以清除障碍物）
		*/
		if(direct==1)
		{
			steper = -4;
			direct = -1;
		}
		else
		{
			steper = 4;
			direct = 1;
		}
		
		offcount=0;
		
		/* 
		   每次转2步，转34次是60度，如果没有卡住34次后
		   就退出了for循环，如果卡住了光电开关就检测不
		   到，就会一直循环68次 
		*/
		for(a=0;a<175*2;a++)
		{
			myStepper.step(steper);
			
			//curled=digitalRead(CHECKER);
			/* 
			   防止误判，多尝试几次
			   a必须大于17是因为，挡片刚划过光电开关就停止了，
			   如果反方向尝试时，挡片会再次划过光电开关，
			   会误判到达了一圈，实际上一圈要34次
			*/
			if(interrupted==1 && a<80)
			{
				interrupted = 0;
				offcount=0;
			}
			/* 上次还有遮挡，现在没有了，说明转完一圈了 */
			if(interrupted == 1)
			{
				/* 如果熄灭期间的转数太少，说明轮子被卡住了，没有移动*/
				if(a<80 || offcount<80)
				{
					interrupted = 0;
					continue;
				}
				/* 检测到挡片从光电开关上划过了，说明转了一圈 */
				c+=1;
				//Serial.print("one cycle:");
				//Serial.println((int)c);
				break;
			}
			else
			{
				offcount++;
			}
		}
		tc1+=1;
		tc+=tc1;
		ta+=a;
	}
	
	//Serial.println(log);
}
/*
	id:闹钟id
	count:步数
*/
void NewAlarmed(char id, char counter)
{
	char a=0;
	char log[36];
	int ta=0; //总的尝试的次数
	char tc=0; //总的尝试的圈数
	
	for(a=0;a<counter;a++)
	{
		attachInterrupt(digitalPinToInterrupt(interruptPin), interruptSrv, FALLING);
		OneStep(ta,tc);
		detachInterrupt(digitalPinToInterrupt(interruptPin));
	}
	snprintf(log, sizeof(log), "t=3&s=6&i=alarm%d_ta:%d_tc:%d",
		id, ta, tc);
	sendData(log,1);
}

void printTime() {
	// 获取时间
	char datebuf[36];
	Time t = rtc.getTime();

	if(IsAlarmed(ALARM1) == 0)
	{
		if(cmparetime(&t, &alarm1)==true)
		{
			SetAlarmed(ALARM1);
			//DebugSerial.println("alarm1 get");
			sendData("t=3&s=3&i=alarm1get",2);
			
			NewAlarmed(1, FEEDERCYCLE);
		}
	}
	else
	{
		if(cmparetime(&t, &alarm1)==false)
		{
			ClearAlarmed(ALARM1);
			//DebugSerial.println("alarm1 cancel");
			sendData("t=3&s=4&i=alarm1clear",2);
		}
	}
	
	if(IsAlarmed(ALARM2)==0)
	{
		if(cmparetime(&t, &alarm2)==true)
		{
			SetAlarmed(ALARM2);
			//DebugSerial.println("alarm2 get");
			sendData("t=3&s=3&i=alarm2get",2);
			
			NewAlarmed(2, FEEDERCYCLE);
		}
	}
	else
	{
		if(cmparetime(&t, &alarm2)==false)
		{
			ClearAlarmed(ALARM2);
			//DebugSerial.println("alarm2 cancel");
			sendData("t=3&s=4&i=alarm2clear",2);
		}
	}
	
	if(t.min%5 == 0 && getconf!=t.min)
	{
		char x=3;
		if(t.min%10==0)
		{
			//每10分钟发起一次配置请求
			x=2;
		}
		getconf = t.min;
		
		snprintf(datebuf, sizeof(datebuf), "t=%d&s=5&i=%d_%04d-%02d-%02d_%02d:%02d:%02d",
			x,t.dow,
			t.year, t.mon, t.date,
			t.hour, t.min, t.sec);
		
		sendData(datebuf,2);
	}

	
		
	/* 当配置生效成功后，会返回对时命令 */
	if(t.hour != lasthour)
	{
		snprintf(datebuf, sizeof(datebuf), "t=3&s=0&i=%d_%04d-%02d-%02d_%02d:%02d:%02d",
			t.dow,
			t.year, t.mon, t.date,
			t.hour, t.min, t.sec);
		
		lasthour = t.hour;
		sendData(datebuf,1);
	}
	
	//打印时间
	/*
	snprintf(datebuf, sizeof(datebuf), "%d_%04d-%02d-%02d_%02d:%02d:%02d",
			t.dow,
			t.year, t.mon, t.date,
			t.hour, t.min, t.sec);
	Serial.println(datebuf);
	*/
}

char DebugConf(char type,char subtype)
{
	char buf[35];
	snprintf(buf,sizeof(buf),"t=%d&s=%d&i=%d:%d:%d_%d:%d:%d_%d",
		type,subtype,
		alarm1.hr, alarm1.min, alarm1.sec, 
		alarm2.hr, alarm2.min, alarm2.sec,
		feedCounter);
		
	if(type==1 || type==0)
	{
		return sendData(buf,2);
	}
	return 1;
}

void SaveConf(struct alarmt& a1, struct alarmt& a2, unsigned char count)
{
	char addr = EEPROM_ADDR_BEGIN;
	EEPROM.write(addr, 0xaa);
	EEPROM.write(addr++, a1.hr);
	EEPROM.write(addr++, a1.min);
	EEPROM.write(addr++, a1.sec);
	EEPROM.write(addr++, a2.hr);
	EEPROM.write(addr++, a2.min);
	EEPROM.write(addr++, a2.sec);
	EEPROM.write(addr, count);
}

void InitConf()
{
	int a = EEPROM_ADDR_BEGIN;
	if(EEPROM.read(a)!=0xff)
	{
		alarm1.hr = EEPROM.read(a++);
		alarm1.min = EEPROM.read(a++);
		alarm1.sec = EEPROM.read(a++);
		alarm2.hr = EEPROM.read(a++);
		alarm2.min = EEPROM.read(a++);
		alarm2.sec = EEPROM.read(a++);
		feedCounter = EEPROM.read(a);
	}
}

void InitDS1302()
{
	//DS1302初始化
	rtc.writeProtect(false);
	rtc.halt(false);
	
	//初始化时间
	rtc.setTime(22,22,22);
	rtc.setDate(21,12,2017);
	rtc.setDOW(4);
	
	rtc.writeProtect(true);
}

void interruptSrv()
{
	interrupted = 1;
}

void setup()
{
	Serial.begin(9600);
	
	InitConf();
	 
	wifi.begin();

	String cmd;
	char index=0;
	for(index=0;index<strlen(WIFIPASSWORD); index++)
	{
		cmd+=(char)pgm_read_byte(password+index);
	}
		
	bool b = wifi.Initialize(SSID, cmd.c_str());
	if(!b)
	{
		//初始化失败
	}

	delay(8000);  //make sure the module can have enough time to get an IP address 
	String ipstring  = wifi.showIP();

	InitDS1302();
	
	//步进电机初始化
	myStepper.setSpeed(MOTOR_SPEED);
	
	if(DebugConf(1,1)!=1)
	{
		delay(1000);
		DebugConf(1,1);
	}
	//sendData("time=2");
	
	
}

void GetResult()
{
	char message[50];
	// if there's incoming data from the net connection.
	// send it out the serial port.  This is for debugging
	// purposes only:
	//sendData("time=2");
	//DebugSerial.println("recv");
	if(wifi.ReceiveMessage(message, sizeof(message))) 
	{
		//DebugSerial.print("receive:");
		//DebugSerial.println(message);  
		processCmd(message);
	}
}

void loop()
{	

	GetResult();

	if(tosend==4)
	{
		DebugConf(3,2);
		tosend=0;
	}
	delay(2000);
	//DebugSerial.println("time");
	printTime();
	
	//NewAlarmed(1, FEEDERCYCLE);

	//delay(10000);
}


#define _cmd1 "POST /xxx/feeder.php HTTP/1.1\r\nHost: "CLOUND_SERVER"\r\nAccept: */*\r\nContent-Length: "
#define _cmd2 "\r\nContent-Type: application/x-www-form-urlencoded\r\nConnection: close\r\n\r\n"
// this method makes a HTTP connection to the server:
PROGMEM const char cmd1[] = _cmd1;
PROGMEM const char cmd2[] = _cmd2;


char sendData(const char* thisData) {

	// if there's a successful connection:
	boolean result=wifi.ipConfig(TCP,server, CLOUND_PORT);

	//result=1;
	if (result) 
	{
		//DebugSerial.println(thisData);
		// send the HTTP PUT request:
		String cmd;

		char index=0;
		
		for(index=0;index<strlen(_cmd1); index++)
		{
			cmd+=(char)pgm_read_byte(cmd1+index);
		}

		cmd += String(strlen(thisData));
		for(index=0;index<strlen(_cmd2); index++)
		{
			cmd+=(char)pgm_read_byte(cmd2+index);
		}
        
		cmd += thisData;
		cmd += "\r\n";
		
		
		result = wifi.Send(&cmd);
		//DebugSerial.print("send result:");
		//DebugSerial.println(result);
		// note the time that the connection was made:
		//lastConnectionTime = millis();
        
	} 

	else {
		// if you couldn't make a connection:
		//DebugSerial.println("connection failed");
		//DebugSerial.println("disconnecting.");
		wifi.closeMux();
		return 0;
	}
	if(result)
	{
		GetResult();
	}
	return result;
}

char sendData(const char* thisData,char retryCount)
{
	int i=0;
	for(i=0; i<retryCount; i++)
	{
		if(sendData(thisData))
		{
			return true;
		}
	}
	return false;
}

void processCmd(const char* buf)
{
	int year=0,month=0,day=0,hour=0,minute=0,second=0,week=0;
	if(buf[1] == 't')
	{
		sscanf(buf,"<t!%d-%d-%d_%d:%d:%d_%d>",&year,&month,&day,&hour,&minute,&second,&week);
		
		lasthour = hour;
		
		rtc.writeProtect(false);
		rtc.setTime(hour,minute,second);
		rtc.setDate(day,month,year);
		rtc.setDOW(week);
		rtc.writeProtect(true);
	}
	else if(buf[1] == 'c') //<c!17:10:00_20:00:00_3>
	{
		sscanf(buf,"<c!%d:%d:%d_%d:%d:%d_%d>",
			&year, &month, &day,
			&hour, &minute, &second,
			&week);
		if(alarm1.hr != year ||
			alarm1.min != month ||
			alarm1.sec != day ||
			alarm2.hr != hour ||
			alarm2.min != minute ||
			alarm2.sec != second ||
			feedCounter != week)
		{
			alarm1.hr = (char)year;
			alarm1.min = month;
			alarm1.sec = day;
			alarm2.hr = hour;
			alarm2.min = minute;
			alarm2.sec = second;
			feedCounter = week;
			
			SaveConf(alarm1, alarm2, feedCounter);
			
			tosend=4;
		}
		//如果这里再发送结果会出现递归
		//DebugConf(0);
	}
}

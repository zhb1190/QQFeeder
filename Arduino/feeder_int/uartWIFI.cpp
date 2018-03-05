/****************************************************
 *   QQFeeder智能自动喂食器 V1.0                    *
 *   作者:zhb1190                                   *
 *   邮箱:zhb_account@163.com                       *
 *   教程:http://www.log4cpp.com/smarthome/52.html  *
 *   时间:2018-3-5                                  *
 ****************************************************/
 
#include "uartWIFI.h"
#include <avr\pgmspace.h>


#ifdef DEBUG
#define DBG(message)    Serial.print(message)
#define DBG_N(message)	Serial.println(message)
#define DBGW(message)    Serial.write(message)
#else
#define DBG(message)
#define DBGW(message)
#endif // DEBUG


boolean WIFI::result(int timeout, char* str, char* str1)
{
	String data;
	char bfind = 0;
	char c=0;
	char i1=0;
	char i2=0;
	
	unsigned long btime=millis();
	while((millis()-btime)<timeout)
	{
		c = 0;
		while(_cell.available())
		{	
			c = (char)_cell.read();
			if(str && c==str[i1])
			{
				i1++;
			}
			else
			{
				i1 = 0;
			}
			
			if(str1 && c==str1[i2])
			{
				i2++;
			}
			else
			{
				i2 = 0;
			}
			
			data += (char)c;
			if(data.length()>=40)
			{
				DBG(data);
				data="";
			}
			
			if((str && str[i1]=='\0') || (str1 && str1[i2]=='\0'))
			{
				DBG(data);
				data = "";
				bfind = true;
				return true;
			}
		}
		if(bfind)
		{
			DBG(data);
			data = "";
			return true;
		}
	}
	DBG(data);
	return false;
}

void WIFI::begin(void)
{
	boolean bfind=false;
	/* espb266波特率设置为9600,因为如果设置为115200,软串口误码很高*/
	_cell.begin(9600);
	
	_cell.println("AT+RST");
	bfind=result(6000, "eady", 0);
	pinMode(13, OUTPUT);
	
		

	if(bfind)
		DBG("Module is ready\r\n");
    else
	{
		DBG("Module have no response\r\n");
		while(1)
		{
			/* 如果esp8266初始化失败,就不停的闪烁 */
			digitalWrite(13, HIGH);
			delay(700);
			digitalWrite(13, LOW);
			delay(700);  
		}
	}
}


/*************************************************************************
//Initialize port

	mode:	setting operation mode
		STA: 	Station
		AP:	 	Access Point
		AT_STA:	Access Point & Station

	chl:	channel number
	ecn:	encryption
		OPEN          0
		WEP           1
		WAP_PSK       2
		WAP2_PSK      3
		WAP_WAP2_PSK  4		

	return:
		true	-	successfully
		false	-	unsuccessfully

***************************************************************************/
bool WIFI::Initialize(char* ssid,const char* pwd)
{
	/* 初始化成station模式 */
	bool b = confMode();
	if (!b)
	{
		return false;
	}
	Reset();
	confJAP(ssid, pwd);

	return true;
}

/*************************************************************************
//Set up tcp or udp connection

	type:	tcp or udp
	
	addr:	ip address
	
	port:	port number
	
	a:	set multiple connection
		0 for sigle connection
		1 for multiple connection
		
	id:	id number(0-4)

	return:
		true	-	successfully
		false	-	unsuccessfully

***************************************************************************/
boolean WIFI::ipConfig(byte type, const char* addr, int port)
{
	boolean result = false;
	if(confMux(0))
	{
		result = newMux(type, addr, port);
	}
	return result;
}

/*************************************************************************
//receive message from wifi

	buf:	buffer for receiving data
	
	chlID:	<id>(0-4)

	return:	size of the buffer
	

***************************************************************************/
int WIFI::ReceiveMessage(char *buf, char len)
{
	//+IPD,<len>:<data>
	//+IPD,<id>,<len>:<data>
	char begin=0;
	char c=0;
	char i=0;
	unsigned long start;
	start = millis();
	while (_cell.available()>0 || (millis()-start<300))
	{
		c = 0;
		if(_cell.available()>0)
		{
			c = _cell.read();
			DBG((char)c);
		}

		if (c == '+')
		{
			start = millis();
			while (millis()-start<6000) 
			{
				c=0;
				if (_cell.available()>0)
				{
					c = _cell.read();
					DBG((char)c);
					if (c=='<' || begin==1)
					{
						buf[i]=c;
						i++;
						if(i>=len)
						{
							return 0;
						}
						begin=1;
					}
					if(c=='>')
					{
						buf[i]=0;
						begin=0;
						return i;
					}
				}
			}
		}
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////


/*************************************************************************
//reboot the wifi module



***************************************************************************/
void WIFI::Reset(void)
{
    _cell.println("AT+RST");
	unsigned long start;
	start = millis();
	if(result(5000,"ready", 0))
    {                            
		DBG("reboot wifi is OK\r\n");
    }
}

/*************************************************************************
//configure the operation mode

	a:	
		1	-	Station
		2	-	AP
		3	-	AP+Station
		
	return:
		true	-	successfully
		false	-	unsuccessfully

***************************************************************************/

bool WIFI::confMode()
{
     _cell.println("AT+CWMODE=1");  

	 if(result(2000,"OK\r\n","no change"))
	 {
		 return true;
	 }
	 return false;
}


/*************************************************************************
//show the list of wifi hotspot
		
	return:	string of wifi information
		encryption,SSID,RSSI
		

***************************************************************************/

String WIFI::showAP(void)
{
    String data;
	_cell.flush();
    _cell.print("AT+CWLAP\r\n");  
	delay(1000);
	while(1);
    unsigned long start;
	start = millis();
    while (millis()-start<8000) {
   if(_cell.available()>0)
   {
     char a =_cell.read();
     data=data+a;
   }
     if (data.indexOf("OK")!=-1 || data.indexOf("ERROR")!=-1 )
     {
         break;
     }
  }
    if(data.indexOf("ERROR")!=-1)
    {
        return "ERROR";
    }
    else{
       char head[4] = {0x0D,0x0A};   
       char tail[7] = {0x0D,0x0A,0x0D,0x0A};        
       data.replace("AT+CWLAP","");
       data.replace("OK","");
       data.replace("+CWLAP","WIFI");
       data.replace(tail,"");
	   data.replace(head,"");

        return data;
        }
 }


/*************************************************************************
//show the name of current wifi access port
		
	return:	string of access port name
		AP:<SSID>
		

***************************************************************************/
String WIFI::showJAP(void)
{
	_cell.flush();
    _cell.println("AT+CWJAP?");  
      String data;
	  unsigned long start;
	start = millis();
    while (millis()-start<3000) 
	{
       if(_cell.available()>0)
       {
       char a =_cell.read();
       data=data+a;
       }
       if (data.indexOf("OK")!=-1 || data.indexOf("ERROR")!=-1 )
       {
           break;
       }
    }
      char head[4] = {0x0D,0x0A};   
      char tail[7] = {0x0D,0x0A,0x0D,0x0A};        
      data.replace("AT+CWJAP?","");
      data.replace("+CWJAP","AP");
      data.replace("OK","");
	  data.replace(tail,"");
      data.replace(head,"");
      
          return data;
}


/*************************************************************************
//configure the SSID and password of the access port
		
		return:
		true	-	successfully
		false	-	unsuccessfully
		

***************************************************************************/
boolean WIFI::confJAP(char* ssid ,const char* pwd)
{
	
    _cell.print("AT+CWJAP=");
    _cell.print("\"");     //"ssid"
    _cell.print(ssid);
    _cell.print("\"");

    _cell.print(",");

    _cell.print("\"");      //"pwd"
    _cell.print(pwd);
    _cell.println("\"");
	
	if(result(3000, "OK\r\n", 0))
	{
		return true;
	}
	return false;
}
/*************************************************************************
//quite the access port
		
		return:
			true	-	successfully
			false	-	unsuccessfully
		

***************************************************************************/

boolean WIFI::quitAP(void)
{
    _cell.println("AT+CWQAP");
    unsigned long start;
	start = millis();
    while (millis()-start<3000) {                            
        if(_cell.find("OK")==true)
        {
		   return true;
           
        }
    }
	return false;

}

/*************************************************************************
//show the parameter of ssid, password, channel, encryption in AP mode
		
		return:
			mySAP:<SSID>,<password>,<channel>,<encryption>

***************************************************************************/
String WIFI::showSAP()
{
    _cell.println("AT+CWSAP?");  
      String data;
      unsigned long start;
	start = millis();
    while (millis()-start<3000) {
       if(_cell.available()>0)
       {
       char a =_cell.read();
       data=data+a;
       }
       if (data.indexOf("OK")!=-1 || data.indexOf("ERROR")!=-1 )
       {
           break;
       }
    }
      char head[4] = {0x0D,0x0A};   
      char tail[7] = {0x0D,0x0A,0x0D,0x0A};        
      data.replace("AT+CWSAP?","");
      data.replace("+CWSAP","mySAP");
      data.replace("OK","");
	  data.replace(tail,"");
      data.replace(head,"");
      
          return data;
}




/*********************************************
 *********************************************
 *********************************************
             TPC/IP Function Command
 *********************************************
 *********************************************
 *********************************************
 */

/*************************************************************************
//inquire the connection status
		
		return:		string of connection status
			<ID>  0-4
			<type>  tcp or udp
			<addr>  ip
			<port>  port number

***************************************************************************/

String WIFI::showStatus(void)
{
    _cell.println("AT+CIPSTATUS");  
      String data;
    unsigned long start;
	start = millis();
    while (millis()-start<3000) {
       if(_cell.available()>0)
       {
       char a =_cell.read();
       data=data+a;
       }
       if (data.indexOf("OK")!=-1)
       {
           break;
       }
    }

          char head[4] = {0x0D,0x0A};   
          char tail[7] = {0x0D,0x0A,0x0D,0x0A};        
          data.replace("AT+CIPSTATUS","");
          data.replace("OK","");
		  data.replace(tail,"");
          data.replace(head,"");
          
          return data;
}

/*************************************************************************
//show the current connection mode(sigle or multiple)
		
		return:		string of connection mode
			0	-	sigle
			1	-	multiple

***************************************************************************/
String WIFI::showMux(void)
{
    String data;
    _cell.println("AT+CIPMUX?");  

      unsigned long start;
	start = millis();
    while (millis()-start<3000) {
       if(_cell.available()>0)
       {
       char a =_cell.read();
       data=data+a;
       }
       if (data.indexOf("OK")!=-1)
       {
           break;
       }
    }
          char head[4] = {0x0D,0x0A};   
          char tail[7] = {0x0D,0x0A,0x0D,0x0A};        
          data.replace("AT+CIPMUX?","");
          data.replace("+CIPMUX","showMux");
          data.replace("OK","");
		  data.replace(tail,"");
          data.replace(head,"");
          
          return data;
}

/*************************************************************************
//configure the current connection mode(sigle or multiple)
		
		a:		connection mode
			0	-	sigle
			1	-	multiple
			
	return:
		true	-	successfully
		false	-	unsuccessfully
***************************************************************************/
boolean WIFI::confMux(boolean a)
{
	_cell.print("AT+CIPMUX=");
	_cell.println(a);           

	if(result(5000,"OK\r\n",0))
	{
		return true;
	}

	return false;
}


/*************************************************************************
//Set up tcp or udp connection	(signle connection mode)

	type:	tcp or udp
	
	addr:	ip address
	
	port:	port number
		

	return:
		true	-	successfully
		false	-	unsuccessfully

***************************************************************************/
boolean WIFI::newMux(byte type, const char* addr, int port)

{
    _cell.print("AT+CIPSTART=");
    if(type>0)
    {
        _cell.print("\"TCP\"");
    }else
    {
        _cell.print("\"UDP\"");
    }
    _cell.print(",");
    _cell.print("\"");
    _cell.print(addr);
    _cell.print("\"");
    _cell.print(",");
    _cell.println(String(port));
  
	if(result(3000, "OK\r\n", 0))
	{
		return true;
	}
	return false;
}

		
boolean WIFI::Send(const String* str)
{
	_cell.print("AT+CIPSEND=");
	_cell.println(str->length());
	
	if(result(2000,">",0))
	{
		_cell.print(str->c_str());
	}
	else
	{
		DBG("AT+CIPSEND failed\r\n");
		closeMux();
		return false;
	}

	//DBG(data);

	if(result(5000,"SEND OK\r\n",0))
	{
		//DBG_N(str->c_str());
		return true;
	}

	return false;
}


/*************************************************************************
//Close up tcp or udp connection	(sigle connection mode)


***************************************************************************/
void WIFI::closeMux(void)
{
    _cell.println("AT+CIPCLOSE");

    String data;
    unsigned long start;
	start = millis();
	while (millis()-start<3000) {
     if(_cell.available()>0)
     {
		char a =_cell.read();
		data=data+a;
     }
     if (data.indexOf("Linked")!=-1 || data.indexOf("ERROR")!=-1 || data.indexOf("we must restart")!=-1)
     {
         break;
     }
  }
}


/*************************************************************************
//Set up tcp or udp connection	(multiple connection mode)
		
	id:	id number(0-4)

***************************************************************************/
void WIFI::closeMux(byte id)
{
    _cell.print("AT+CIPCLOSE=");
    _cell.println(String(id));
    String data;
    unsigned long start;
	start = millis();
	while (millis()-start<3000) {
     if(_cell.available()>0)
     {
     char a =_cell.read();
     data=data+a;
     }
     if (data.indexOf("OK")!=-1 || data.indexOf("Link is not")!=-1 || data.indexOf("Cant close")!=-1)
     {
         break;
     }
  }

}

/*************************************************************************
//show the current ip address
		
	return:	string of ip address

***************************************************************************/
String WIFI::showIP(void)
{

    String data;
    unsigned long start;

	for(int a=0; a<3;a++)
	{
		_cell.println("AT+CIFSR");  
		start = millis();
		while (millis()-start<3000) 
		{
			while(_cell.available()>0)
			{
				char a =_cell.read();
				data=data+a;
			}
			if (data.indexOf("AT+CIFSR")!=-1)
			{
				break;
			}
		}
		if(data.indexOf(".") != -1)
		{
			break;
		}
		data = "";
	}

    char head[4] = {0x0D,0x0A};   
    char tail[7] = {0x0D,0x0D,0x0A};        
    data.replace("AT+CIFSR","");
    data.replace(tail,"");
    data.replace(head,"");
	return data;

    
}

/*************************************************************************
////set the parameter of server

	mode:
		0	-	close server mode
		1	-	open server mode
		
	port:	<port>
		
	return:
		true	-	successfully
		false	-	unsuccessfully

***************************************************************************/

boolean WIFI::confServer(byte mode, int port)
{
    _cell.print("AT+CIPSERVER=");  
    _cell.print(String(mode));
    _cell.print(",");
    _cell.println(String(port));

    String data;
    unsigned long start;
	start = millis();
	boolean found = false;
	while (millis()-start<3000) {
     if(_cell.available()>0)
     {
     char a =_cell.read();
     data=data+a;
     }
     if (data.indexOf("OK")!=-1 || data.indexOf("no charge")!=-1)
     {
		found = true;
         break;
     }
  }
  return found;
}

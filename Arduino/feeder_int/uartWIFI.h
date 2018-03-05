/****************************************************
 *   QQFeeder智能自动喂食器 V1.0                    *
 *   作者:zhb1190                                   *
 *   邮箱:zhb_account@163.com                       *
 *   教程:http://www.log4cpp.com/smarthome/52.html  *
 *   时间:2018-3-5                                  *
 ****************************************************/

#ifndef __UARTWIFI_H__
#define __UARTWIFI_H__
#include <Arduino.h>

//#define DEBUG

#define _cell	Serial
		
//The way of encrypstion
#define    OPEN          0
#define    WEP           1
#define    WAP_PSK       2
#define    WAP2_PSK      3
#define    WAP_WAP2_PSK  4

//Communication mode 
#define    TCP     1
#define    tcp     1
#define    UDP     0
#define    udp     0

#define    OPEN    1
#define    CLOSE   0

//The type of initialized WIFI
#define    STA     1
#define    AP      2
#define    AP_STA  3

#define SERIAL_TX_BUFFER_SIZE 300
#define SERIAL_RX_BUFFER_SIZE 300





class WIFI
{
  public:

    void begin(void);
	//Initialize port
	bool Initialize(char* ssid,const char* pwd);
	boolean ipConfig(byte type, const char* addr, int port);
	
	boolean Send(const String* str);  //send data in sigle connection mode
		
	int ReceiveMessage(char *buf, char len);
	
    //String begin(void);
    /*=================WIFI Function Command=================*/
    void Reset(void);    //reset the module
	bool confMode();   //set the working mode of module
	boolean confJAP(char* ssid ,const char* pwd);    //set the name and password of wifi 
	
    String showAP(void);   //show the list of wifi hotspot
    String showJAP(void);  //show the name of current wifi access port
    boolean quitAP(void);    //quit the connection of current wifi
    String showSAP(void);     //show the parameter of ssid, password, channel, encryption in AP mode

    /*================TCP/IP commands================*/
    String showStatus(void);    //inquire the connection status
    String showMux(void);       //show the current connection mode(sigle or multiple)
    boolean confMux(boolean a);    //set the connection mode(sigle:0 or multiple:1)
    boolean newMux(byte type, const char* addr, int port);   //create new tcp or udp connection (sigle connection mode)
    void closeMux(void);   //close tcp or udp (sigle connection mode)
    void closeMux(byte id); //close tcp or udp (multiple connection mode)
    String showIP(void);    //show the current ip address
    boolean confServer(byte mode, int port);  //set the parameter of server
	
	String m_rev;
	
	boolean result(int timeout, char* str, char* str1);

};

#endif

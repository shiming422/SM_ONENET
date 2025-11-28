

/*************************************
 * 1、功能：连接Onenet云平台.c文件
 * 
 * 2、操作：根据文档注释进行更改
 *
 * 3、作者：MH
 *
 ************************************/


#include "4g.h"


/***********************以下内容不可修改*******************************************/

/*测试模块是否可用*/
#define AT            "AT\r\n"

#define CSQ           "AT+CSQ\r\n"

#define CGSN          "AT+CGSN=1\r\n"

#define CPIN          "AT+CPIN?\r\n"

#define CIMI          "AT+CIMI\r\n"

#define CEREG         "AT+CEREG?\r\n"

#define PINGRESP      "AT+MQTTCFG=\"pingresp\",0,1\r\n"

#define CLEAN         "AT+MQTTCFG=\"clean\",0,1\r\n"

#define GETDATA       "AT+MQTTSUB=0,\"$sys/UzDIkIRVQW/Device/cmd/#\",1\r\n"


/*判断标识符*/
#define AT_OK              "OK"


/*发送、接收AT指令数组（不可修改）*/
unsigned char Send_AT_Data[400] = {0};
unsigned char Recv_AT_Data[400] = {0};

/*接收APP下发的所有数据，可增大*/
unsigned char Recv_APP_Buf[200] = {0};

/*发送数据组包数组*/
char Get_Pack[400] = {0};

/*接收用户使用的数据结构体*/
APP_DATA app_data;

/*******************************************
 * 1、APP下发数据前参数的初始化值
 * 2、可用可不用
 * 3、初始化内容为获取APP数据的结构体参数
 *******************************************/
void APP_Data_Init(void)
{
		app_data.Humiditymax = 60;
		app_data.Lightmax = 45;
}

/*******************************************
 * 1、接收APP下发指令函数
 * 2、接收数据发布返回值
 * 3、需要在头文件定义接收标识符
 * 4、引用标识符判断
 * 5、必须修改（函数内注释）
 *******************************************/
unsigned char RecvData_Dispose(const unsigned char *Recv_Data)
{
    char *Dis_Buf;
    HAL_UART_Transmit(&huart1,Recv_APP_Buf,200,100);
    /*不可修改、接收发布返回值*/
    if(strstr((char *)Recv_Data, (const char *)AT_OK) != NULL)
    {
        return 0;
    }
    /*可修改宏定义标识符（例如：TEMP_F）可在头文件增删修，判断接收APP参数*/
		else if (strstr((char *)Recv_Data, (const char *)H_M) != NULL)
		{
    Dis_Buf = strstr((char *)Recv_Data, (const char *)H_M);   

    if (Dis_Buf != NULL)
    {
        Dis_Buf = strstr(Dis_Buf, "\"v\"");
        if (Dis_Buf != NULL)
        {
            Dis_Buf = strstr(Dis_Buf, ":");
            if (Dis_Buf != NULL)
            {
                app_data.Humiditymax = atoi(Dis_Buf + 1);
                return 0;
            }
        }
    }

    return 0;
		}

    else if(strstr((char *)Recv_Data, (const char *)L_M) != NULL)
    {
        Dis_Buf = strstr((char *)Recv_Data, (const char *)L_M);
        Dis_Buf = strstr((char *)Dis_Buf, (const char *)":");
        app_data.Lightmax = atoi(Dis_Buf+1);
        
        return 0;
    }
    else
        return 0;
}

/**************************************************
 * 1、中断接收数据函数
 * 2、接收数据发布返回值和APP下发数据
 * 3、无需修改
 * 4、需要调用（在中断服务函数中调用）
 *************************************************/
void RecvData_Onenet(void)
{
    if(__HAL_UART_GET_FLAG(&UART, UART_FLAG_IDLE) != RESET)
    {
				
        __HAL_UART_CLEAR_IDLEFLAG(&UART);
        HAL_UART_DMAStop(&UART);
        RecvData_Dispose(Recv_APP_Buf);
			HAL_UART_Transmit(&huart1,Recv_APP_Buf,200,100);
        memset(Recv_APP_Buf, 0, sizeof(Recv_APP_Buf));
        HAL_UART_Receive_DMA(&UART,Recv_APP_Buf,200);
    }
}

/**************************************************
 * 1、发送数据上云组包函数
 * 2、需要填写云平台数据标识符和参数
 * 3、不可修改
 * 4、参数：
 *  （1）上传数据的标识符（Name）
 *  （2）上传数据的参数（Data）
 *  （3）数据位置（0为最后一个数据，1为前面数据）
 *************************************************/
void Get_Send_Pack(char *Name, unsigned char Data, unsigned char Cmd)
{
    char dat[50];
    
    if(Cmd)
    {
        memset(dat, 0, sizeof(dat));
        sprintf(dat, "\"%s\":[{\"v\":%d}],", Name, Data);
        strcat((char *)Get_Pack, dat);
    }
    else
    {
        memset(dat, 0, sizeof(dat));
        sprintf(dat, "\"%s\":[{\"v\":%d}]", Name, Data);
        strcat((char *)Get_Pack, dat);
    }
}

/******************************************************
 * 1、发送数据上云接口函数
 * 2、无需修改
 * 3、需要在main.c中调用发数据
 * 4、注意：发送速度不宜过快，控制在1s左右
 ******************************************************/
void SendData_Onenet(void)
{
    memset(Send_AT_Data, 0, sizeof(Send_AT_Data));
    
    /*包头：不可修改*/
    sprintf((char *)Send_AT_Data, "AT+MQTTPUB=0,\"$sys/%s/%s/dp/post/json\",1,0,0,0,\"{\"id\":123,\"dp\":{", ProductId, DeviceName);
    /*数据*/
    strcat((char *)Send_AT_Data, (char *)Get_Pack);
    /*包尾*/
    strcat((char *)Send_AT_Data, "}}\"\r\n");
    
    /*清空组包数组*/
    memset(Get_Pack, 0, sizeof(Get_Pack));
    
    HAL_UART_Transmit(&UART, Send_AT_Data, strlen((const char*)Send_AT_Data), 3000);
}

/*********************************************
 * 1、测试模块是否可用
 * 2、不可修改
 *********************************************/
void Send_AT()
{
    memset(Send_AT_Data, 0, sizeof(Send_AT_Data));
    sprintf((char *)Send_AT_Data, AT);
    
    memset(Recv_AT_Data, 0, sizeof(Recv_AT_Data));
    HAL_UART_Transmit(&UART, Send_AT_Data, strlen((const char*)Send_AT_Data), 500);
	HAL_UART_Transmit(&huart1, Recv_AT_Data, 50, 100);
    HAL_UART_Receive(&UART, Recv_AT_Data, 50, 2000);
	HAL_UART_Transmit(&huart1, Recv_AT_Data, 5, 100);
}

/*查询信号质量*/
void Send_AT_CSQ()
{
    memset(Send_AT_Data, 0, sizeof(Send_AT_Data));
    sprintf((char *)Send_AT_Data, CSQ);
    
    memset(Recv_AT_Data, 0, sizeof(Recv_AT_Data));
    HAL_UART_Transmit(&UART, Send_AT_Data, strlen((const char*)Send_AT_Data), 500);
	HAL_UART_Transmit(&huart1, Recv_AT_Data, 50, 100);
    HAL_UART_Receive(&UART, Recv_AT_Data, 50, 200);
	HAL_UART_Transmit(&huart1, Recv_AT_Data, 50, 100);
}

/*获取IMEI*/
void Send_AT_CGSN()
{
    memset(Send_AT_Data, 0, sizeof(Send_AT_Data));
    sprintf((char *)Send_AT_Data, CGSN);
    
    memset(Recv_AT_Data, 0, sizeof(Recv_AT_Data));
    HAL_UART_Transmit(&UART, Send_AT_Data, strlen((const char*)Send_AT_Data), 500);
	HAL_UART_Transmit(&huart1, Recv_AT_Data, 50, 100);
    HAL_UART_Receive(&UART, Recv_AT_Data, 50, 200);
	HAL_UART_Transmit(&huart1, Recv_AT_Data, 50, 100);
}

/*检测SIM卡是否被识别*/
void Send_AT_CPIN()
{
    memset(Send_AT_Data, 0, sizeof(Send_AT_Data));
    sprintf((char *)Send_AT_Data, CPIN);
    
    memset(Recv_AT_Data, 0, sizeof(Recv_AT_Data));
    HAL_UART_Transmit(&UART, Send_AT_Data, strlen((const char*)Send_AT_Data), 500);
	HAL_UART_Transmit(&huart1, Recv_AT_Data, 50, 100);
    HAL_UART_Receive(&UART, Recv_AT_Data, 50, 200);
	HAL_UART_Transmit(&huart1, Recv_AT_Data, 50, 100);
}

/*获取卡号*/
void Send_AT_CIMI()
{
    memset(Send_AT_Data, 0, sizeof(Send_AT_Data));
    sprintf((char *)Send_AT_Data, CIMI);
    
    memset(Recv_AT_Data, 0, sizeof(Recv_AT_Data));
    HAL_UART_Transmit(&UART, Send_AT_Data, strlen((const char*)Send_AT_Data), 500);
	HAL_UART_Transmit(&huart1, Recv_AT_Data, 50, 100);
    HAL_UART_Receive(&UART, Recv_AT_Data, 50, 200);
	HAL_UART_Transmit(&huart1, Recv_AT_Data, 50, 100);
}

/*查询网络注册状态*/
void Send_AT_CEREG()
{
    memset(Send_AT_Data, 0, sizeof(Send_AT_Data));
    sprintf((char *)Send_AT_Data, CEREG);
    
    memset(Recv_AT_Data, 0, sizeof(Recv_AT_Data));
    HAL_UART_Transmit(&UART, Send_AT_Data, strlen((const char*)Send_AT_Data), 500);
	HAL_UART_Transmit(&huart1, Recv_AT_Data, 50, 100);
    HAL_UART_Receive(&UART, Recv_AT_Data, 50, 200);
	HAL_UART_Transmit(&huart1, Recv_AT_Data, 50, 100);
}

/*配置MQTT参数*/
void Send_AT_Pingresp()
{
    memset(Send_AT_Data, 0, sizeof(Send_AT_Data));
    sprintf((char *)Send_AT_Data, PINGRESP);
    
    memset(Recv_AT_Data, 0, sizeof(Recv_AT_Data));
    HAL_UART_Transmit(&UART, Send_AT_Data, strlen((const char*)Send_AT_Data), 500);
		HAL_UART_Transmit(&huart1, Send_AT_Data, 80, 100);
    HAL_UART_Receive(&UART, Recv_AT_Data, 50, 200);
		HAL_UART_Transmit(&huart1, Recv_AT_Data, 50, 100);
}

/*清除当前连接*/
void Send_AT_Clean()
{
    memset(Send_AT_Data, 0, sizeof(Send_AT_Data));
    sprintf((char *)Send_AT_Data, CLEAN);
    
    memset(Recv_AT_Data, 0, sizeof(Recv_AT_Data));
    HAL_UART_Transmit(&UART, Send_AT_Data, strlen((const char*)Send_AT_Data), 500);
	HAL_UART_Transmit(&huart1, Send_AT_Data, 50, 100);
    HAL_UART_Receive(&UART, Recv_AT_Data, 50, 200);
	HAL_UART_Transmit(&huart1, Recv_AT_Data, 50, 100);
}

/*连接服务器*/
void Send_AT_Sever()
{
    memset(Send_AT_Data, 0, sizeof(Send_AT_Data));
    sprintf((char *)Send_AT_Data, "AT+MQTTCONN=0,\"mqtts.heclouds.com\",1883,\"%s\",\"%s\",\"%s\"\r\n", DeviceName, ProductId, Token);
    
    memset(Recv_AT_Data, 0, sizeof(Recv_AT_Data));
    HAL_UART_Transmit(&UART, Send_AT_Data, strlen((const char*)Send_AT_Data), 500);
		HAL_UART_Transmit(&huart1, Send_AT_Data, 300, 100);
    HAL_UART_Receive(&UART, Recv_AT_Data, 50, 200);
		HAL_UART_Transmit(&huart1, Recv_AT_Data, 50, 100);
}

/*订阅下发的主题*/
void Send_AT_Get_Data()
{
    memset(Send_AT_Data, 0, sizeof(Send_AT_Data));
    sprintf((char *)Send_AT_Data, "AT+MQTTSUB=0,\"$sys/%s/%s/cmd/#\",1\r\n", ProductId, DeviceName);
    
    memset(Recv_AT_Data, 0, sizeof(Recv_AT_Data));
    HAL_UART_Transmit(&UART, Send_AT_Data, strlen((const char*)Send_AT_Data), 500);
		HAL_UART_Transmit(&huart1, Send_AT_Data, 80, 100);
    HAL_UART_Receive(&UART, Recv_AT_Data, 50, 200);
		HAL_UART_Transmit(&huart1, Recv_AT_Data, 50, 100);
}


/*************************************************
 * 1、4G模块初始化函数
 * 2、不可修改
 * 3、需在main.c初始化时调用
 *************************************************/
void Onenet_Init(void)
{
    //HAL_Delay(10000);
    Send_AT();
    
    /*查询信号质量*/
    Send_AT_CSQ();
    
    /*获取IMEI*/
    Send_AT_CGSN();
    
    /*检测SIM卡是否被识别*/
    Send_AT_CPIN();
    
    /*获取卡号*/
    Send_AT_CIMI();
    
    /*查询网络注册状态*/
    Send_AT_CEREG();
    
    /*配置MQTT参数*/
    Send_AT_Pingresp();
    
    /*清除当前连接*/
    Send_AT_Clean();
    
    /*连接服务器*/
    Send_AT_Sever();
    
    /*订阅下发的主题*/
    Send_AT_Get_Data();
    
    APP_Data_Init();
    
    /*开启中断接收数据*/
    HAL_UART_Receive_DMA(&UART,Recv_APP_Buf,200);
	__HAL_UART_ENABLE_IT(&UART,UART_IT_IDLE);
}

#ifndef __4G_ONENET_H__
#define __4G_ONENET_H__

#include "main.h"
#include "usart.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"

/********************************************************************/

/*****************************************************
 * 宏定义：接收APP下发数据的标识符
 * APP需要下发多少个，则定义多少个
 * 需要在onenet.c文件中的RecvData_Dispose()函数添加
 *****************************************************/

#define   L_M            				"Lightmax"
#define   H_M            				"Humiditymax"

/***************************************
 * 1、接收APP数据
 * 2、需要修改结构体内参数对应标识符
 * 3、外部调用使用
 ***************************************/
typedef struct
{
		uint8_t Humiditymax;
		uint8_t Lightmax;
    
}APP_DATA;

extern APP_DATA app_data;

/***************************************************************************/

/*串口：需要配置DMA接收*/
#define UART huart3

/*Onenet云平台参数定义*/
#define ProductId  "OLQHMG4ptv"
#define DeviceName "parking_lot"
#define Token      "version=2018-10-31&res=products%2FOLQHMG4ptv%2Fdevices%2Fparking_lot&et=1793951065&method=md5&sign=jYkGscEIwW79v9Bb3VLu1w%3D%3D"

/*****************************************************************************/

/*初始化函数*/
void Onenet_Init(void);
void APP_Data_Init(void);
/**********************************************************************
 * 1、功能：组包函数
 * 2、参数：
 *      （1）Name：对应云平台物模型标识符
 *      （2）Data：需要发送的数据（unsigned char类型）
 *      （3） Cmd：参数输入：0或1（0：末尾一个数据；1：末尾前的数据）
 **********************************************************************/
void Get_Send_Pack(char *Name, unsigned char Data, unsigned char Cmd);

/*发送数据函数*/
void SendData_Onenet(void);

/*接收数据函数*/
void RecvData_Onenet(void);

#endif


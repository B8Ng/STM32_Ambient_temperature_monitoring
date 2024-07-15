#include "stm32f10x.h"
#include "esp8266.h"
#include "ADC.h"
#include "onenet.h"
#include "LED.h"
#include "cJSON.h"
#include "mqttkit.h"
#include "usart.h"
#include "delay.h"
#include <string.h>
#include <stdio.h>


#define DEVICE_NAME		"TemNHum"

#define PROJECT_ID	"4iXR2ZI4Tc"

#define AUTHORIZATION_INFO "version=2018-10-31&res=products%2F4iXR2ZI4Tc%2Fdevices%2FTemNHum&et=2665271459&method=md5&sign=H%2FuZ3pWnA77zuPzSffzxpg%3D%3D"

extern u8 DHT11_Value[4];	//储存DHT11温湿度数据
extern unsigned char esp8266_buf[512];
_Bool OneNet_DevLink(void)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};					//协议包

	unsigned char *dataPtr;
	
	_Bool status = 1;
	
	printf("OneNet_DevLink：\r\n" 
				 "DEVICE_NAME: %s,	PROJECT_ID: %s,	AUTHORIZATION_INFO:%s\r\n"	, DEVICE_NAME, PROJECT_ID, AUTHORIZATION_INFO);
	
	if(MQTT_PacketConnect(PROJECT_ID, AUTHORIZATION_INFO, DEVICE_NAME, 256, 1, MQTT_QOS_LEVEL0, NULL, NULL, 0, &mqttPacket) == 0)
	{
		ESP8266_SendData(mqttPacket._data, mqttPacket._len);			//上传平台
		printf("等待平台响应......\r\n");
		dataPtr = ESP8266_GetIPD(5000);									//等待平台响应
		printf("平台响应完毕！\r\n");
		if(dataPtr != NULL)
		{
			if(MQTT_UnPacketRecv(dataPtr) == MQTT_PKT_CONNACK)
			{
				switch(MQTT_UnPacketConnectAck(dataPtr))
				{
					case 0:printf("Tips:	连接成功\r\n");status = 0;break;
					
					case 1:printf("WARN:	连接失败：协议错误\r\n");break;
					case 2:printf("WARN:	连接失败：非法的clientid\r\n");break;
					case 3:printf("WARN:	连接失败：服务器失败\r\n");break;
					case 4:printf("WARN:	连接失败：用户名或密码错误\r\n");break;
					case 5:printf("WARN:	连接失败：非法链接(比如token非法)\r\n");break;
					
					default:printf("ERR:	连接失败：未知错误\r\n");break;
				}
			}
		}
		
		MQTT_DeleteBuffer(&mqttPacket);								//删包
	}
	else
		printf("WARN:	MQTT_PacketConnect Failed\r\n");
	
	return status;
	
}
unsigned char OneNet_FillBuf(char *buf)
{
	//在此处如下面格式进行添加其他需要上传的数据信息
	char text[64];
	
	memset(text, 0, sizeof(text));
	
	strcpy(buf, "{\"id\":\"516\",\"params\":{");
	
	memset(text, 0, sizeof(text));
	sprintf(text, "\"Temperature\":{\"value\":%d}," , DHT11_Value[2]);
								//平台对应标识符    变量   数据类型   传输的变量 			(注意，一定要和平台数据类型对应，否则传输失败)
	strcat(buf, text);
	
	memset(text, 0, sizeof(text));
	sprintf(text, "\"Humidity\":{\"value\":%d},",DHT11_Value[0]);
	strcat(buf, text);
	
	
	memset(text, 0, sizeof(text));
	sprintf(text, "\"ChipTemper\":{\"value\":%.2f},",Get_Value());
	strcat(buf, text);
	
	memset(text, 0, sizeof(text));
	sprintf(text, "\"LED\":{\"value\":%s}", led_info.Led_Status ? "true" : "false");
	strcat(buf, text);
	
	strcat(buf, "}}");
	
	printf("Onnet数据封包完成！\r\n");
	return strlen(buf);

}

void OneNet_SendData(void)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};												//协议包
	
	char buf[256];
	
	short body_len = 0, i = 0;
	
	printf("Onnet数据发送中......\r\n");
	
	memset(buf, 0, sizeof(buf));
	
	printf("Onnet数据封包中......\r\n");
	body_len = OneNet_FillBuf(buf);		//需要添加更多数据上传到平台则在此函数内添加		//获取当前需要发送的数据流的总长度
	
	if(body_len)
	{
		if(MQTT_PacketSaveData(PROJECT_ID, DEVICE_NAME , body_len, NULL, &mqttPacket) == 0)							//封包
		{
			for(; i < body_len; i++)
				mqttPacket._data[mqttPacket._len++] = buf[i];
			
			ESP8266_SendData(mqttPacket._data, mqttPacket._len);									//上传数据到平台
			printf("发送到平台中......");
			printf("发送出： %d Bytes\r\n", mqttPacket._len);
			
			MQTT_DeleteBuffer(&mqttPacket);															//删包
			printf("发送平台完毕！");
		}
		else
			printf("WARN:	EDP_NewBuffer Failed\r\n");
	}
	
}



void OneNET_Subscribe(void)
{
	
	MQTT_PACKET_STRUCTURE mqtt_packet = {NULL, 0, 0, 0};						//协议包
	
	char topic_buf[56];
	const char *topic = topic_buf;
	
	snprintf(topic_buf, sizeof(topic_buf), "$sys/%s/%s/thing/property/set", PROJECT_ID, DEVICE_NAME);
	
	printf("订阅的Topic: %s\r\n", topic_buf);
	
	if(MQTT_PacketSubscribe(MQTT_SUBSCRIBE_ID, MQTT_QOS_LEVEL0, &topic, 1, &mqtt_packet) == 0)
	{
		ESP8266_SendData(mqtt_packet._data, mqtt_packet._len);					//向平台发送订阅请求
		
		MQTT_DeleteBuffer(&mqtt_packet);										//删包
	}

	
}


void OneNet_RevPro(unsigned char *cmd)
{
	

	
	char *req_payload = NULL;
	char *cmdid_topic = NULL;
	
	unsigned short topic_len = 0;
	unsigned short req_len = 0;
	
	unsigned char qos = 0;
	static unsigned short pkt_id = 0;
	
	unsigned char type = 0;
	
	short result = 0;

	char *dataPtr = NULL;
	char numBuf[10];
	int num = 0;
	
	cJSON *raw_json, *params_json, *led_json;
	
	type = MQTT_UnPacketRecv(cmd);
	switch(type)
	{
		case MQTT_PKT_PUBLISH:																//接收的Publish消息
		
			result = MQTT_UnPacketPublish(cmd, &cmdid_topic, &topic_len, &req_payload, &req_len, &qos, &pkt_id);
			if(result == 0)
			{
				char *data_ptr = NULL;
				
				printf("topic名称: %s, topic长度: %d, payload名称: %s, payload长度: %d\r\n",
																	cmdid_topic, topic_len, req_payload, req_len);
																																/****************************************************************/
				raw_json = cJSON_Parse(req_payload);										/**********************小程序控制单片机***************************/
				params_json = cJSON_GetObjectItem(raw_json,"params");		/*解析出params内的内容																						*/
				led_json = cJSON_GetObjectItem(params_json,"LED");			/*解析出params内的LED标识符（平台设备对应的标识符为LED）					*/
				if(led_json != NULL)																		/*若找到有数据:																									*/
				{																												/*																															*/
					if(led_json->type == cJSON_True) Led_Set(LED_ON);			/*cJSON_True为1，若返回的LED的type为1，则执行Led_Set(LED_ON)开灯	*/
					else Led_Set(LED_OFF);																/*若返回的LED的type不为1，则执行Led_Set(LED_OFF)关灯							*/
				}																												/****************************************************************/
				
				cJSON_Delete(raw_json);
				
			}
			
		case MQTT_PKT_PUBACK:														//发送Publish消息，平台回复的Ack
		
			if(MQTT_UnPacketPublishAck(cmd) == 0)
				printf("Tips:	MQTT Publish Send OK\r\n");
			
		break;
		
		case MQTT_PKT_SUBACK:																//发送Subscribe消息的Ack
		
			if(MQTT_UnPacketSubscribe(cmd) == 0)
				printf("Tips:	MQTT Subscribe OK\r\n");
			else
				printf("Tips:	MQTT Subscribe Err\r\n");
		
		break;
		
		default:
			result = -1;
		break;
	}
	
	
	if(result == -1)
		return;
	
	if(type == MQTT_PKT_CMD || type == MQTT_PKT_PUBLISH)
	{
		MQTT_FreeBuffer(cmdid_topic);
		MQTT_FreeBuffer(req_payload);
	}

}
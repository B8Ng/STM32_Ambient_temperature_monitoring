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

extern u8 DHT11_Value[4];	//����DHT11��ʪ������
extern unsigned char esp8266_buf[512];
_Bool OneNet_DevLink(void)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};					//Э���

	unsigned char *dataPtr;
	
	_Bool status = 1;
	
	printf("OneNet_DevLink��\r\n" 
				 "DEVICE_NAME: %s,	PROJECT_ID: %s,	AUTHORIZATION_INFO:%s\r\n"	, DEVICE_NAME, PROJECT_ID, AUTHORIZATION_INFO);
	
	if(MQTT_PacketConnect(PROJECT_ID, AUTHORIZATION_INFO, DEVICE_NAME, 256, 1, MQTT_QOS_LEVEL0, NULL, NULL, 0, &mqttPacket) == 0)
	{
		ESP8266_SendData(mqttPacket._data, mqttPacket._len);			//�ϴ�ƽ̨
		printf("�ȴ�ƽ̨��Ӧ......\r\n");
		dataPtr = ESP8266_GetIPD(5000);									//�ȴ�ƽ̨��Ӧ
		printf("ƽ̨��Ӧ��ϣ�\r\n");
		if(dataPtr != NULL)
		{
			if(MQTT_UnPacketRecv(dataPtr) == MQTT_PKT_CONNACK)
			{
				switch(MQTT_UnPacketConnectAck(dataPtr))
				{
					case 0:printf("Tips:	���ӳɹ�\r\n");status = 0;break;
					
					case 1:printf("WARN:	����ʧ�ܣ�Э�����\r\n");break;
					case 2:printf("WARN:	����ʧ�ܣ��Ƿ���clientid\r\n");break;
					case 3:printf("WARN:	����ʧ�ܣ�������ʧ��\r\n");break;
					case 4:printf("WARN:	����ʧ�ܣ��û������������\r\n");break;
					case 5:printf("WARN:	����ʧ�ܣ��Ƿ�����(����token�Ƿ�)\r\n");break;
					
					default:printf("ERR:	����ʧ�ܣ�δ֪����\r\n");break;
				}
			}
		}
		
		MQTT_DeleteBuffer(&mqttPacket);								//ɾ��
	}
	else
		printf("WARN:	MQTT_PacketConnect Failed\r\n");
	
	return status;
	
}
unsigned char OneNet_FillBuf(char *buf)
{
	//�ڴ˴��������ʽ�������������Ҫ�ϴ���������Ϣ
	char text[64];
	
	memset(text, 0, sizeof(text));
	
	strcpy(buf, "{\"id\":\"516\",\"params\":{");
	
	memset(text, 0, sizeof(text));
	sprintf(text, "\"Temperature\":{\"value\":%d}," , DHT11_Value[2]);
								//ƽ̨��Ӧ��ʶ��    ����   ��������   ����ı��� 			(ע�⣬һ��Ҫ��ƽ̨�������Ͷ�Ӧ��������ʧ��)
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
	
	printf("Onnet���ݷ����ɣ�\r\n");
	return strlen(buf);

}

void OneNet_SendData(void)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};												//Э���
	
	char buf[256];
	
	short body_len = 0, i = 0;
	
	printf("Onnet���ݷ�����......\r\n");
	
	memset(buf, 0, sizeof(buf));
	
	printf("Onnet���ݷ����......\r\n");
	body_len = OneNet_FillBuf(buf);		//��Ҫ��Ӹ��������ϴ���ƽ̨���ڴ˺��������		//��ȡ��ǰ��Ҫ���͵����������ܳ���
	
	if(body_len)
	{
		if(MQTT_PacketSaveData(PROJECT_ID, DEVICE_NAME , body_len, NULL, &mqttPacket) == 0)							//���
		{
			for(; i < body_len; i++)
				mqttPacket._data[mqttPacket._len++] = buf[i];
			
			ESP8266_SendData(mqttPacket._data, mqttPacket._len);									//�ϴ����ݵ�ƽ̨
			printf("���͵�ƽ̨��......");
			printf("���ͳ��� %d Bytes\r\n", mqttPacket._len);
			
			MQTT_DeleteBuffer(&mqttPacket);															//ɾ��
			printf("����ƽ̨��ϣ�");
		}
		else
			printf("WARN:	EDP_NewBuffer Failed\r\n");
	}
	
}



void OneNET_Subscribe(void)
{
	
	MQTT_PACKET_STRUCTURE mqtt_packet = {NULL, 0, 0, 0};						//Э���
	
	char topic_buf[56];
	const char *topic = topic_buf;
	
	snprintf(topic_buf, sizeof(topic_buf), "$sys/%s/%s/thing/property/set", PROJECT_ID, DEVICE_NAME);
	
	printf("���ĵ�Topic: %s\r\n", topic_buf);
	
	if(MQTT_PacketSubscribe(MQTT_SUBSCRIBE_ID, MQTT_QOS_LEVEL0, &topic, 1, &mqtt_packet) == 0)
	{
		ESP8266_SendData(mqtt_packet._data, mqtt_packet._len);					//��ƽ̨���Ͷ�������
		
		MQTT_DeleteBuffer(&mqtt_packet);										//ɾ��
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
		case MQTT_PKT_PUBLISH:																//���յ�Publish��Ϣ
		
			result = MQTT_UnPacketPublish(cmd, &cmdid_topic, &topic_len, &req_payload, &req_len, &qos, &pkt_id);
			if(result == 0)
			{
				char *data_ptr = NULL;
				
				printf("topic����: %s, topic����: %d, payload����: %s, payload����: %d\r\n",
																	cmdid_topic, topic_len, req_payload, req_len);
																																/****************************************************************/
				raw_json = cJSON_Parse(req_payload);										/**********************С������Ƶ�Ƭ��***************************/
				params_json = cJSON_GetObjectItem(raw_json,"params");		/*������params�ڵ�����																						*/
				led_json = cJSON_GetObjectItem(params_json,"LED");			/*������params�ڵ�LED��ʶ����ƽ̨�豸��Ӧ�ı�ʶ��ΪLED��					*/
				if(led_json != NULL)																		/*���ҵ�������:																									*/
				{																												/*																															*/
					if(led_json->type == cJSON_True) Led_Set(LED_ON);			/*cJSON_TrueΪ1�������ص�LED��typeΪ1����ִ��Led_Set(LED_ON)����	*/
					else Led_Set(LED_OFF);																/*�����ص�LED��type��Ϊ1����ִ��Led_Set(LED_OFF)�ص�							*/
				}																												/****************************************************************/
				
				cJSON_Delete(raw_json);
				
			}
			
		case MQTT_PKT_PUBACK:														//����Publish��Ϣ��ƽ̨�ظ���Ack
		
			if(MQTT_UnPacketPublishAck(cmd) == 0)
				printf("Tips:	MQTT Publish Send OK\r\n");
			
		break;
		
		case MQTT_PKT_SUBACK:																//����Subscribe��Ϣ��Ack
		
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
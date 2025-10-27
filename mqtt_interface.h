#ifndef __MQTT_INTERFACE_H__
#define __MQTT_INTERFACE_H__



/* MQTT连接服务器函数 */
void User_Connect(void);

/* MQTT 订阅函数 */
void User_Subscribe(void);

/* MQTT 发布函数 */
void User_Publish(void);

/* MQTT 心跳函数 */
void User_MqttPing(void);


#endif


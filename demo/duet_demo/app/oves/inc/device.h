#ifndef		__DEVICE_H__
#define		__DEVICE_H__

#define MAX_DEVICES 20

typedef struct
{
	const char*name;
	void  (*init)(void);
	void  (*ioctrl)(void*param);
	void  (*read)(void*param);
	void  (*write)(void*param);
	void  (*process)(void*param);
}Device_TypeDef;


uint8_t register_device(Device_TypeDef*dev);

#endif



/*
  ArduinoPort.cpp 
  MIT Licence - Copyright (c) 2019 Jaap Danielse - https://github.com/JaapDanielse/VirtualPanel

  ArduinoPort is a libray to provide a simple, controlled, 	va_start(args, message);
	event communication between an Arduino and a PC based application
	under C# using the ArduinoPort.dll.
  	
	V1.0    17-05-2019 JpD
  Original version
	
	*/

#include "ArduinoPort.h"

// ArduinoPort 
ArduinoPort::ArduinoPort(const char* panel_id, PanelCallbackFunction CallBackPointer, HardwareSerial& comport, long baudrate)
{
	_CallBackPointer = CallBackPointer;
	_panel_id = panel_id;
	_comport = &comport;
	_baudrate = baudrate;

}

void ArduinoPort::Init(void)
{
	_comport->begin(_baudrate);
}

void ArduinoPort::Send(int channel)
{
	char buf[4];
	sprintf(buf, "%02X%1X", channel, vp_type::Void);
	_comport->println(buf);
}

void ArduinoPort::Send(int channel, bool value)
{
	char buf[5];
	sprintf(buf, "%02X%1X%1X", channel, vp_type::Boolean, value);
	_comport->println(buf);
}

#ifndef ARDUINO_ARCH_SAM
void ArduinoPort::Sendf(int channel, const __FlashStringHelper* message, ...)
{
	char buf[SENDFBUFFERSIZE];
	sprintf(buf, "%02X%1X", channel, vp_type::String);
	_comport->print(buf);
	
	va_list args;
	va_start(args, message);
	
	vsnprintf_P(buf, SENDFBUFFERSIZE, (const char*)message, args);
	
	va_end(args);
	_comport->println(buf);
}
#endif

void ArduinoPort::Sendf(int channel, const char* message, ...)
{
	char buf[SENDFBUFFERSIZE];
	sprintf(buf, "%02X%1X", channel, vp_type::String);
	_comport->print(buf);
	va_list args;
	va_start(args, message);
	vsnprintf(buf, SENDFBUFFERSIZE, message, args);
	va_end(args);
	_comport->println(buf);
}

void ArduinoPort::Send(int channel, const char* message)
{
	char buf[4];
	sprintf(buf, "%02X%1X", channel, vp_type::String);
	_comport->print(buf);
	_comport->println(message);
}

void ArduinoPort::Send(int channel, const __FlashStringHelper* message)
{
	char buf[4];
	sprintf(buf, "%02X%1X", channel, vp_type::String);
	_comport->print(buf);
	_comport->println(message);
}

void ArduinoPort::Send(int channel, uint8_t value)
{
	char buf[6];
	sprintf(buf, "%02X%1X%02X", channel, vp_type::Byte, value);
	_comport->println(buf);
}

void ArduinoPort::Send(int channel, int16_t value)
{
	char buf[8];
	sprintf(buf, "%02X%1X%04X", channel, vp_type::Int, value);
	_comport->println(buf);
}

void ArduinoPort::Send(int channel, uint16_t value)
{
	char buf[8];
	sprintf(buf, "%02X%1X%04X", channel, vp_type::Uint, value);
	_comport->println(buf);
}

void ArduinoPort::Send(int channel, int32_t value)
{
	char buf[12];
	sprintf(buf, "%02X%1X%08lX", channel, vp_type::Long, value);
	_comport->println(buf);
}

void ArduinoPort::Send(int channel, uint32_t value)
{
	char buf[12];
	sprintf(buf, "%02X%1X%08lX", channel, vp_type::Ulong, value);
	_comport->println(buf);
}

void ArduinoPort::Send(int channel, float value)
{
	char buf[12];
	sprintf(buf, "%02X%1X", channel, vp_type::Float);
	_comport->print(buf);
	_comport->println(value,4);
}

bool ArduinoPort::delay(uint16_t delaytime, bool receive)
{
	uint32_t ResumeTime = millis() + delaytime;
	bool DataReceived = false;
	
	while (ResumeTime > millis())
	{
		if (_comport->available())
		{
      DataReceived = true;
    	if(receive) Receive();
  	}
	}
  return DataReceived;
}

void ArduinoPort::Receive(void)
{
	char buf[9];
	uint32_t value;
	bool hex;
	int16_t len;

	while (_comport->available())
	{
		SerialInpBuf[SerialInpIdx++] = _comport->read();
		if (SerialInpIdx == RECEIVEBUFFERSIZE)
		{
			SerialInpIdx = 0;
		}

		if (SerialInpBuf[SerialInpIdx - 1] == '\r') SerialInpIdx--;

		if (SerialInpBuf[SerialInpIdx - 1] == '\n')
		{
			SerialInpBuf[SerialInpIdx - 1] = 0; // make a string

			if (!strcmp("ID", SerialInpBuf))
			{
				_comport->println(_panel_id);
   				SerialInpIdx = 0; //discard input
				return;
			}

			strncpy(buf, SerialInpBuf, 2);
			buf[2] = 0;
			int16_t channel = Hex2Bin(buf);

			strncpy(buf, &SerialInpBuf[2], 1);
			buf[1] = 0;
			vp_type type = static_cast<vp_type>(Hex2Bin(buf));

			len = strlen(&SerialInpBuf[3]);
			hex = IsAllHex(&SerialInpBuf[3]) && len <= 8;
			if (hex)
				value = Hex2Bin(&SerialInpBuf[3]);

			switch (type)
			{
				case vp_type::Void:
					break;
				case vp_type::String:
					vpr_string = &SerialInpBuf[3]; break;
				case vp_type::Boolean:
					if (hex && len == 1) { if (value == 1) vpr_boolean = true; else vpr_boolean = false;  break; }
				case vp_type::Byte:
					if (hex && len == 2) { vpr_byte = (uint8_t)value; } break;
				case vp_type::Int:
					if (hex && len == 4) { vpr_int = (int16_t)value; } break;
				case vp_type::Uint:
					if (hex && len == 4) { vpr_uint = (uint16_t)value; } break;
				case vp_type::Long:
					if (hex && len == 8) { vpr_long = (int32_t)value; } break;
				case vp_type::Ulong:
					if (hex && len == 8) { vpr_ulong = (uint32_t)value; } break;
				case vp_type::Float:
  					vpr_float = atof(&SerialInpBuf[3]); break;
					
				default:
					type = vp_type::Error;
					vpr_string = SerialInpBuf;
					break;
			}

			_CallBackPointer(channel, static_cast<int>(type));

			SerialInpIdx = 0;
		}
	}
}

bool ArduinoPort::IsAllHex(char* hexvalue)
{
	int len = strlen(hexvalue);

	for (int i = 0; i < len; i++)
	{
		if ((hexvalue[i] < 48) || (hexvalue[i] > 57 && hexvalue[i] < 65) || (hexvalue[i] > 70))
			return false;
	}
	return true;
}

uint32_t ArduinoPort::Hex2Bin(char* hexvalue)
{
	uint32_t val = 0;
	uint32_t tmp = 0;

	int len = strlen(hexvalue);

	for (int i = 0; i < len; i++)
	{
		tmp = 1;
		tmp = tmp << (4 * (len - 1 - i));
		if (hexvalue[i] <= 57)
			val += tmp * (hexvalue[i] - 48);
		else
			val += tmp * (hexvalue[i] - 55);
	}

	return val;
}





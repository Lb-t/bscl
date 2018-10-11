#include <windows.h>
#include "platform_serialport.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
#define DEV_PATH_MAX 128
#define MAX_OPEN_NUM 8

#pragma comment(lib, "legacy_stdio_definitions.lib")
struct platform_serialport_t_
{
	HANDLE hComm;
};

typedef struct
{
	platform_serialport_t *port[MAX_OPEN_NUM];
} platform_serialport_mgr_t;

static platform_serialport_mgr_t sp;

static int get_free_fd(void)
{
	for (int i = 0; i < MAX_OPEN_NUM; ++i)
	{
		if (sp.port[i] == NULL)
		{
			return i;
		}
	}
	return -1;
}


int platform_serialport_open(const char *name)
{
	HANDLE hComm;
	int fd = get_free_fd();
	if (fd < 0)
	{
		return fd;
	}
	char path[DEV_PATH_MAX];
	sprintf(path, "\\\\.\\%s", name);
	hComm = CreateFile(path,
		GENERIC_READ | GENERIC_WRITE,
		0,
		0,
		OPEN_EXISTING,
		FILE_FLAG_OVERLAPPED,
		0);
	if (hComm == INVALID_HANDLE_VALUE)
	{
		return PLATFORM_SERIALPORT_EFAIL;
	}
	sp.port[fd] = (platform_serialport_t *)malloc(sizeof(platform_serialport_t));
	sp.port[fd]->hComm = hComm;
	if (!sp.port[fd])
	{
		return PLATFORM_SERIALPORT_EMEM;
	}
	return fd;
}

int platform_serialport_read(int fd, void *buf, int len)
{
	assert(fd >= 0);
	assert(buf);
	assert(len);
	platform_serialport_t *port = sp.port[fd];
	if (!port)
	{
		return PLATFORM_SERIALPORT_EARG;
	}
	HANDLE hComm = port->hComm;
	DWORD dwRead;
	OVERLAPPED osReader = { 0 };

	// Create the overlapped event. Must be closed before exiting
	// to avoid a handle leak.
	osReader.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	if (osReader.hEvent == NULL)
	{
		// Error creating overlapped event; abort.
		return PLATFORM_SERIALPORT_EFAIL;
	}


	// Issue read operation.
	if (!ReadFile(hComm, buf, len, &dwRead, &osReader))
	{
		if (GetLastError() != ERROR_IO_PENDING) // read not delayed?
		{                                       // Error in communications; report it.
			return PLATFORM_SERIALPORT_EFAIL;
		}
	}
	else
	{
		// read completed immediately
		//HandleASuccessfulRead(buf, dwRead);
		return dwRead;
	}


	DWORD dwRes;
	dwRes = WaitForSingleObject(osReader.hEvent, INFINITE);
	switch (dwRes)
	{
		// Read completed.
	case WAIT_OBJECT_0:
		if (!GetOverlappedResult(hComm, &osReader, &dwRead, FALSE))
		{ // Error in communications; report it.
		}
		else
		{
			// Read completed successfully.
			//HandleASuccessfulRead(buf, dwRead);
			return dwRead;
		}
		break;

	case WAIT_TIMEOUT:
		// Operation isn't complete yet. fWaitingOnRead flag isn't
		// changed since I'll loop back around, and I don't want
		// to issue another read until the first one finishes.
		//
		// This is a good time to do some background work.
		break;

	default:
		// Error in the WaitForSingleObject; abort.
		// This indicates a problem with the OVERLAPPED structure's
		// event handle.
		break;
	}
	return 0;
}

int platform_serialport_write(int fd, void *buf, int len)
{
	assert(fd >= 0);
	assert(buf);
	assert(len);
	platform_serialport_t *port = sp.port[fd];
	if (!port)
	{
		return PLATFORM_SERIALPORT_EARG;
	}
	HANDLE hComm = port->hComm;
	OVERLAPPED osWrite = { 0 };
	DWORD dwWritten;
	DWORD dwRes;
	BOOL fRes;

	// Create this write operation's OVERLAPPED structure's hEvent.
	osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (osWrite.hEvent == NULL)
	{
		return PLATFORM_SERIALPORT_EFAIL;
	}
	// Issue write.
	if (!WriteFile(hComm, buf, len, &dwWritten, &osWrite))
	{
		if (GetLastError() != ERROR_IO_PENDING)
		{
			// WriteFile failed, but isn't delayed. Report error and abort.
			fRes = FALSE;
		}
		else
		{
			dwRes = WaitForSingleObject(osWrite.hEvent, INFINITE);
		}

		switch (dwRes)
		{
			// OVERLAPPED structure's event has been signaled.
		case WAIT_OBJECT_0:
			if (!GetOverlappedResult(hComm, &osWrite, &dwWritten, FALSE))
				fRes = FALSE;
			else
				// Write operation completed successfully.
				fRes = TRUE;
			break;

		default:
			// An error has occurred in WaitForSingleObject.
			// This usually indicates a problem with the
			// OVERLAPPED structure's event handle.
			fRes = FALSE;
			break;
		}
	}
	else {
		// WriteFile completed immediately.
		fRes = TRUE;
	}

	CloseHandle(osWrite.hEvent);
	if (fRes)
		return len;
	else {
		return 0;
	}
}

int platform_serialport_close(int fd)
{
	assert(fd >= 0);
	platform_serialport_t *port = sp.port[fd];
	if (!port) {
		return PLATFORM_SERIALPORT_EARG;
	}
	HANDLE hComm = port->hComm;
	CloseHandle(hComm);
	return 0;
}

int platform_serialport_config(int fd, platform_serialport_config_t *conf)
{
	assert(fd >= 0);
	assert(conf);
	platform_serialport_t *port = sp.port[fd];
	if (!port)
	{
		return PLATFORM_SERIALPORT_EARG;
	}
	HANDLE hComm = port->hComm;
	DCB dcb = { 0 };
	if (!GetCommState(hComm, &dcb)) {
		return PLATFORM_SERIALPORT_EFAIL;
	}
	else {
		// DCB is ready for use.
		dcb.BaudRate = conf->baudrate;
		if (conf->parity == PLATFORM_SERIALPORT_PARITY_NONE) {
			dcb.fParity = FALSE;
			dcb.Parity = NOPARITY;
		}
		else {
			dcb.fParity = TRUE;
			if (conf->parity == PLATFORM_SERIALPORT_PARITY_EVEN) {
				dcb.Parity = EVENPARITY;
			}
			else if (PLATFORM_SERIALPORT_PARITY_ODD) {
				dcb.Parity = ODDPARITY;
			}
			else {
				dcb.Parity = NOPARITY;
			}
		}
		dcb.StopBits = conf->stopbits;

		dcb.fBinary = TRUE;
		dcb.fOutxCtsFlow = FALSE;
		dcb.fOutxDsrFlow = FALSE;
		dcb.fDtrControl = DTR_CONTROL_DISABLE;
		dcb.fDsrSensitivity = FALSE;
		dcb.fOutX = FALSE;
		dcb.fInX = FALSE;
		dcb.fRtsControl = RTS_CONTROL_DISABLE;

		if (!SetCommState(hComm, &dcb)) {
			return PLATFORM_SERIALPORT_EFAIL;
		}
	}
	return 0;
}
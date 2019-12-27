#include "bscl_serialport.h"
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <windows.h>
#include <setupapi.h>
#define DEV_PATH_MAX 128
#define MAX_OPEN_NUM 8

#pragma comment(lib, "legacy_stdio_definitions.lib")
struct bscl_serialport_t_ {
  HANDLE hComm;
};

typedef struct {
  bscl_serialport_t *port[MAX_OPEN_NUM];
} bscl_serialport_mgr_t;

static bscl_serialport_mgr_t sp;

typedef struct {
  HKEY key;
  int index;
} bscl_serialport_reg_t;

int bscl_serialport_list_begin(void **sp_list) {
  bscl_serialport_reg_t *reg = malloc(sizeof(bscl_serialport_reg_t));
  int ret;
  if (!reg) {
    return BSCL_SERIALPORT_EMEM;
  }
  ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP\\SERIALCOMM", 0, KEY_QUERY_VALUE, &reg->key);
  if (ret != ERROR_SUCCESS) {
    return BSCL_SERIALPORT_EFAIL;
  }
  reg->index = 0;
  *sp_list = reg;
  return 0;
}
int bscl_serialport_list_next(void *sp_list, char *name, unsigned long len) {
  bscl_serialport_reg_t *reg = sp_list;
  char value_name[20];
  unsigned long value_len = 20;
  int ret = RegEnumValue(reg->key, reg->index, value_name, &value_len, NULL, NULL, (LPBYTE)name, &len);
  if (ret != ERROR_SUCCESS) {
    return BSCL_SERIALPORT_EFAIL;
  }
  ++reg->index;
  return 0;
}
int bscl_serialport_list_end(void *sp_list) {
  bscl_serialport_reg_t *reg = sp_list;
  RegCloseKey(reg->key);
  free(reg);
  return 0;
}

static int get_free_fd(void) {
  for (int i = 0; i < MAX_OPEN_NUM; ++i) {
    if (sp.port[i] == NULL) {
      return i;
    }
  }
  return -1;
}

int bscl_serialport_open(const char *name) {
  HANDLE hComm;
  int fd = get_free_fd();
  if (fd < 0) {
    return fd;
  }
  char path[DEV_PATH_MAX];
  sprintf(path, "\\\\.\\%s", name);
  hComm = CreateFile(path, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0);
  if (hComm == INVALID_HANDLE_VALUE) {
    return BSCL_SERIALPORT_EFAIL;
  }

  if (SetCommMask(hComm, EV_RXCHAR | EV_ERR) == 0) {
    return BSCL_SERIALPORT_EFAIL;
  }

  sp.port[fd] = (bscl_serialport_t *)malloc(sizeof(bscl_serialport_t));
  sp.port[fd]->hComm = hComm;

  if (!sp.port[fd]) {
    return BSCL_SERIALPORT_EMEM;
  }

  return fd;
}

int bscl_serialport_read_timeout(int fd, void *buf, int len, unsigned int timeout) {
  assert(fd >= 0);
  assert(buf);
  assert(len);
  bscl_serialport_t *port = sp.port[fd];
  if (!port) {
    return BSCL_SERIALPORT_EARG;
  }
  HANDLE hComm = port->hComm;
  DWORD dwRead;
  OVERLAPPED osReader = {0};

  osReader.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

  if (osReader.hEvent == NULL) {
    return BSCL_SERIALPORT_EFAIL;
  }

  COMMTIMEOUTS timeouts;
  memset(&timeouts, 0, sizeof(timeouts));
  timeouts.ReadIntervalTimeout = 0;
  timeouts.ReadTotalTimeoutMultiplier = 1;
  timeouts.ReadTotalTimeoutConstant = timeout;
  if (SetCommTimeouts(hComm, &timeouts) == 0) {
    printf("SetCommTimeouts() failed");
  }

  if (!ReadFile(hComm, buf, len, NULL, &osReader)) {
    if (GetLastError() != ERROR_IO_PENDING) {
      CloseHandle(osReader.hEvent);
      //  printf("read error\n");
      return BSCL_SERIALPORT_EFAIL;
    }
  } else {
    CloseHandle(osReader.hEvent);
    return len;
  }

  if (!GetOverlappedResult(hComm, &osReader, &dwRead, TRUE)) {
    CloseHandle(osReader.hEvent);
    return BSCL_SERIALPORT_EFAIL;
  }
  CloseHandle(osReader.hEvent);
  return dwRead;
}

int bscl_serialport_read(int fd, void *buf, int len) {
  return bscl_serialport_read_timeout(fd, buf, len, INFINITE);
}

int bscl_serialport_write(int fd, void *buf, int len) {
  assert(fd >= 0);
  assert(buf);
  assert(len);
  bscl_serialport_t *port = sp.port[fd];
  if (!port) {
    return BSCL_SERIALPORT_EARG;
  }
  HANDLE hComm = port->hComm;
  OVERLAPPED osWrite = {0};
  DWORD dwWritten;
  DWORD dwRes;
  BOOL fRes;

  // Create this write operation's OVERLAPPED structure's hEvent.
  osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
  if (osWrite.hEvent == NULL) {
    return BSCL_SERIALPORT_EFAIL;
  }
  // Issue write.
  if (!WriteFile(hComm, buf, len, &dwWritten, &osWrite)) {
    if (GetLastError() != ERROR_IO_PENDING) {
      // WriteFile failed, but isn't delayed. Report error and abort.
      fRes = FALSE;
    } else {
      if (!GetOverlappedResult(hComm, &osWrite, &dwWritten, TRUE))
        fRes = FALSE;
      else
        // Write operation completed successfully.
        fRes = TRUE;
    }
  } else {
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

int bscl_serialport_close(int fd) {
  assert(fd >= 0);
  bscl_serialport_t *port = sp.port[fd];
  if (!port) {
    return BSCL_SERIALPORT_EARG;
  }
  HANDLE hComm = port->hComm;
  CloseHandle(hComm);
  return 0;
}

int bscl_serialport_config(int fd, bscl_serialport_config_t *conf) {
  assert(fd >= 0);
  assert(conf);
  bscl_serialport_t *port = sp.port[fd];
  if (!port) {
    return BSCL_SERIALPORT_EARG;
  }
  HANDLE hComm = port->hComm;
  DCB dcb = {0};
  if (!GetCommState(hComm, &dcb)) {
    return BSCL_SERIALPORT_EFAIL;
  } else {
    // DCB is ready for use.
    dcb.BaudRate = conf->baudrate;
    if (conf->parity == BSCL_SERIALPORT_PARITY_NONE) {
      dcb.fParity = FALSE;
      dcb.Parity = NOPARITY;
    } else {
      dcb.fParity = TRUE;
      if (conf->parity == BSCL_SERIALPORT_PARITY_EVEN) {
        dcb.Parity = EVENPARITY;
      } else if (BSCL_SERIALPORT_PARITY_ODD) {
        dcb.Parity = ODDPARITY;
      } else {
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
      return BSCL_SERIALPORT_EFAIL;
    }
  }
  return 0;
}

#define SP_PORT_HID_FORMAT "USB\\VID_%04x&PID_%04x&REV_%04x&MI_%02x"

int sp_port_find_usb(char *name, int len, uint16_t idVendor, uint16_t idProduct, uint16_t bcdDevice, uint8_t bInterfaceNumber) {
  HDEVINFO DeviceInfoSet;
  DWORD DeviceIndex = 0;
  SP_DEVINFO_DATA DeviceInfoData;
  char szBuffer[1024] = {0};
  DEVPROPTYPE ulPropertyType;
  DWORD dwSize = 0;
  char device_hid_prefix[64];
  const char device_class[] = "Ports";
  snprintf(device_hid_prefix, sizeof(device_hid_prefix), SP_PORT_HID_FORMAT, idVendor, idProduct, bcdDevice, bInterfaceNumber);

  DeviceInfoSet = SetupDiGetClassDevs(NULL, "USB", NULL, DIGCF_ALLCLASSES | DIGCF_PRESENT);
  if (DeviceInfoSet == INVALID_HANDLE_VALUE)
    return -1;

  memset(&DeviceInfoData, 0, sizeof(SP_DEVINFO_DATA));
  DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

  while (SetupDiEnumDeviceInfo(DeviceInfoSet, DeviceIndex, &DeviceInfoData)) {
    DeviceIndex++;
    if (SetupDiGetDeviceRegistryProperty(DeviceInfoSet, &DeviceInfoData, SPDRP_HARDWAREID, &ulPropertyType, (BYTE *)szBuffer,
                                         sizeof(szBuffer), // The size, in bytes
                                         &dwSize)) {

      // check device id
      if (strncmp(szBuffer, device_hid_prefix, strlen(device_hid_prefix)) == 0) {
        // check device class
        if (SetupDiGetDeviceRegistryProperty(DeviceInfoSet, &DeviceInfoData, SPDRP_CLASS, &ulPropertyType, (BYTE *)szBuffer,
                                             sizeof(szBuffer), // The size, in bytes
                                             &dwSize)) {

          if (strncmp(szBuffer, device_class, sizeof(device_class) - 1) == 0) {
            // find port name
            HKEY hDeviceRegistryKey;
            hDeviceRegistryKey = SetupDiOpenDevRegKey(DeviceInfoSet, &DeviceInfoData, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);
            if (hDeviceRegistryKey != INVALID_HANDLE_VALUE) {
              // Read in the name of the port
              char pszPortName[1024];
              DWORD dwSize = sizeof(pszPortName);
              DWORD dwType = 0;

              if ((RegQueryValueEx(hDeviceRegistryKey, "PortName", NULL, &dwType, (LPBYTE)pszPortName, &dwSize) ==
                   ERROR_SUCCESS) &&
                  (dwType == REG_SZ)) {
                strncpy(name, pszPortName, len);
                RegCloseKey(hDeviceRegistryKey);
                return 0;
              }
              RegCloseKey(hDeviceRegistryKey);
            }
          }
        }
      }
    }
  }
  return -1;
}

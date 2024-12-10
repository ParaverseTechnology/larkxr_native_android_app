// 
// Created by fcx@pingxingyun.com
// 2024-02-07 14:54
//
#pragma once
#ifndef XR_TRANSPORT_PROXY_INCLUDE
#define XR_TRANSPORT_PROXY_INCLUDE

#include <cstdint>
#include "lark_xr/types.h"
#include "lark_xr/lark_xr.h"
#include <thread>
#include <functional>
#include <mutex>

#ifdef WIN32
//
#elif ANDROID
#include "jni.h"
#include "lark_xr/lk_common_types.h"
#endif // WIN32


namespace lark {
class LARK_XR_API IXRTransport {
public:
    virtual bool Send(const void *buf, size_t len) { return false; };
};

static const int USB_PACKAGE_MAX_SIZE = 1000;
static const int USB_PACKAGE_HEADER   = 20;
static const int USB_PACKAGE_MAX_IPV4 = 256;

enum UsbPackageType {
	UsbPackageType_Control_Handshake               = 1,
	UsbPackageType_Control_SyncServerAddr          = 2,
	UsbPackageType_Control_SyncConfig              = 3,
	UsbPackageType_Control_SyncHeadsetRenderConfig = 4,
	UsbPackageType_Control_Connected               = 5,
	UsbPackageType_Control_Close                   = 6,
	UsbPackageType_Data                            = 7,
	UsbPackageType_Video                           = 8,
	UsbPackageType_TestData                        = 9,
};

struct UsbPackage {
	uint32_t        msg_type;
	uint32_t        index;
	uint32_t        size;
	uint32_t        total_packet;
	uint32_t        total_size;
	// char* data
};

struct UsbPackage_SyncServerAddr {
	char     ip[USB_PACKAGE_MAX_IPV4];
	uint32_t port;
};

struct UsbPackage_SyncHeadsetRenderConfig {
	float           ipd;
	larkxrRenderFov fov[2];
};

#ifdef WIN32
class LARK_XR_API XRTransportProxyUSB {
public:
	typedef std::function<void(char* data, int len)>      OnDataCallback;
	typedef std::function<void()>                         OnReadyCallback;
	typedef std::function<void()>                         OnDisconnectCallback;

	XRTransportProxyUSB();
	~XRTransportProxyUSB();

	bool Init(uint16_t vid = 0, uint16_t pid = 0);
	void Release();

	void TestWrite(const char* data, int len);

	int Write(const char* data, int len, UsbPackageType type = UsbPackageType::UsbPackageType_Data);
	bool Connected();

	inline bool connected() { return connected_; }
	inline bool ready() { return ready_; }

	inline void set_ondata(OnDataCallback ondata) { ondata_ = ondata; }
	inline void set_onready(OnReadyCallback onready) { onready_ = onready; }
	inline void set_ondisconnect(OnDisconnectCallback ondisconnect) { ondisconnect_ = ondisconnect; }
private:
	void InitConfig();

	bool connected_ = false;
	bool ready_ = false;

	uint16_t vid_ = 0;
	uint16_t pid_ = 0;
	uint16_t endpoint_in_ = 0;
	uint16_t endpoint_out_ = 0;

	void* device_handle_ = nullptr;

	unsigned char send_buffer_[USB_PACKAGE_MAX_SIZE] = {};

	std::thread thread_;
	std::condition_variable cv_;
	std::mutex mutex_;
	bool start_ = false;

	std::thread read_thread_;
	bool read_start_ = false;

	std::mutex write_mutex_;

	OnDataCallback ondata_ = nullptr;
	OnReadyCallback onready_ = nullptr;
	OnDisconnectCallback ondisconnect_ = nullptr;
};
#elif ANDROID
class LARK_XR_API XRTransportProxyUSBClient: public IXRTransport {
public:
    typedef std::function<void(char* data, int len)>      OnDataCallback;
    typedef std::function<void(char* ip, int port)>       OnSyncServrAddrCallback;
    typedef std::function<void(larkCommonConfig* config)> OnSyncConfigCallback;
    typedef std::function<void()>                         OnConnectCallback;
    typedef std::function<void()>                         OnCloseCallback;

    XRTransportProxyUSBClient() = default;
    ~XRTransportProxyUSBClient() = default;

    void Start(int fd);
    void Stop();

	virtual bool Send(const void *buf, size_t len) override;
	bool Send(const void *buf, size_t len, UsbPackageType type);

    inline void set_ondata(OnDataCallback ondata) { ondata_ = ondata;}
    inline void set_onconnect(OnConnectCallback onconnect) { onconnect_ = onconnect;}
    inline void set_onclose(OnCloseCallback onclose) { onclose_ = onclose;}
	inline void set_onsyncconfig(OnSyncConfigCallback onsync) { onsyncconfig_ = onsync; }
	inline void set_onsyncserveraddr(OnSyncServrAddrCallback onsync) { onsyncserveraddr_ = onsync; }
private:
    OnDataCallback ondata_ = nullptr;
    OnConnectCallback onconnect_ = nullptr;
    OnCloseCallback onclose_ = nullptr;
	OnSyncConfigCallback onsyncconfig_ = nullptr;
	OnSyncServrAddrCallback onsyncserveraddr_ = nullptr;

    int fd_ = 0;

    std::thread thread_;
    bool start_ = false;

    std::mutex write_mutex_;

	unsigned char send_buffer_[USB_PACKAGE_MAX_SIZE] = {};
};
#endif // WIN32
}
#endif // XR_TRANSPORT_PROXY_INCLUDE
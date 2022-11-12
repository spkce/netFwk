# <center>netFwk</center>

## 客户端使用

1. 获取客户端对象。
```c++
/**
* @brief 客户端创建函数
* @param type 客户端类型
* @return 客户端接口
**/
static INetClient* create(Type_t type);
```
2. 初始化客户端。指定服务器的IP和端口。
```c++
/**
* @brief 客户端初始化函数
* @param ip 服务器ip地址
* @param port 服务器端口
* @return 成功：true；失败：false
**/
virtual bool init(const char* ip, unsigned int port) = 0;
```
3. 链接服务器。
```c++
/**
* @brief 连接服务器
* @return 成功：true；失败：false
**/
virtual bool connect() = 0;
```
4. 此时调用`INetClient::send()`和`INetClient::recv()`可以收发信息。
```c++
/**
* @brief 连接服务器
* @return 成功：true；失败：false
**/
virtual bool connect() = 0;
```

若在connect前使用`INetClient::attach()`注册接收回调函数，接收的包会进入回调函数中，而`INetClient::recv()`函数将不可用。
接收回调函数必须在`INetClient::connect()`前调用

## 服务器使用

### 协议类
服务器在使用需要实现`IProtocol`的子类，该子类便是服务器上的协议实现。
服务接收到客户端的报文后从`IProtocol::parse()`中传入，该函数是纯虚函数，具体如何解析报文，如何响应请求由子类的代码决定。
```c++
virtual int parse(const char* buf, int len) = 0;
```

响应客户端请求时的回包使用函数`IProtocol::send()`发送
```c++
/**
* @brief 发送报文
* @param buf 发送内容
* @param len 发送内容长度
* @return 剩余未发送的长度
**/
virtual int send(const char* buf, int len);
```
### 终端类
终端类是服务器的实例，它主要的作用是对会话的管理。需要继承`ITerminal`。
终端类需要实现的纯虚函数为：
```c++
/**
* @brief 协议创建函数，由子类实现
* @return 协议对象
**/
virtual IProtocol* createProtocol(ISession* session, size_t recvLen) = 0;
```
该函数的功能是创建协议类。子类通过实现该函数将自定义的协议类和终端类绑定。

### 会话的超时
服务器在初始化的时候可以设定超时时间，传入-1则不超时，传入其他值为超时时间，单位：秒；
```c++
/**
* @brief 初始化
* @param port 服务器端口
* @param timeout 链接超时时间，-1不超时
* @return 成功：true；失败：false
**/
virtual bool init(unsigned int port, int timeout);
```
若不设定超时，会话不会被超时关闭。若设置了超时，会话会在超时后自动关闭。而为了防止自动关闭。需要定时调用保活函数。
```c++
/**
* @brief 保活
* @return 成功：true；失败：false
**/
virtual bool keepAlive();
```
该函数是否调用需要由具体协议决定，故建议由协议基类调用。

# <center>netFwk</center>

## 客户端使用

1. 调用`INetClient::create()`获取客户端对象。
2. 调用`INetClient::init()`初始化客户端。指定服务器的IP和端口。
3. 调用`INetClient::connect()`链接服务器。
4. 此时调用`INetClient::send()`和`INetClient::recv()`可以收发信息。

若在connect前使用`INetClient::attach()`注册接收回调函数，接收的包会进入回调函数中，而`INetClient::recv()`函数将不可用。
接收回调函数必须在`INetClient::connect()`前调用
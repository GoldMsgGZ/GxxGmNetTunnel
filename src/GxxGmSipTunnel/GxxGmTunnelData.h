#ifndef _GxxGmTunnelData_H_
#define _GxxGmTunnelData_H_

#include <map>
#include <winsock.h>

//////////////////////////////////////////////////////////////////////////
//
// 本类用于处理在通道上传输的数据，具体功能包括：
// 
// - 组装数据
// - 解析数据
// 
// 后期会根据安全性需求，增加基于国密算法的密钥生成算法和SM4加解密功能
//
// 序列化后的数据结构
// [11字节：魔术头“GxxGmTunnel”][40字节：连接ID][4字节：数据长度][剩余：实际数据]

class GxxGmTunnelData
{
public:
	GxxGmTunnelData();
	~GxxGmTunnelData();

public:
	int SetTunnelId(const char *tunnel_id, int id_len);
	int SetTunnelData(const char *tunnel_data, int data_len);

	int GetSerializedDataLength();
	int GetSerializedData(char *data, int data_len);

public:
	int ParseData(const char *data, int datalen);
	std::string GetTunnelId();
	int GetTunnelData(char *tunnel_data, int *data_len);

public:
	char *tunnel_id_;
	int tunnel_id_len_;

	char *tunnel_data_;
	int tunnel_data_len_;
};

//////////////////////////////////////////////////////////////////////////
//
// 本类用于管理通道的连接映射关系

class GxxGmTunnelConnections
{
public:
	GxxGmTunnelConnections();
	~GxxGmTunnelConnections();

public:
	/**
	 * 处理客户端套接字信息，如果是新的套接字，则分配一个ID给他，如果不是则直接返回套接字
	 */
	SOCKET HandleClientSocket(SOCKET client_socket);
	SOCKET HandleClientSocket(SOCKET client_socket, std::string connection_id);

	void RemoveTunnelConnection(SOCKET client_socket);
	void RemoveTunnelConnection(std::string connection_id);

	std::string GetConnectionIdBySocket(SOCKET client_socket);
	SOCKET GetSocketByConnectionId(std::string connection_id);

public:
	std::map<SOCKET, std::string> client_connections_;
};


//////////////////////////////////////////////////////////////////////////
//
// 

#endif//_GxxGmTunnelData_H_

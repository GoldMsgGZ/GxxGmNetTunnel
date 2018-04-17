#include "GxxGmTunnelData.h"
#include "..\GxxGmPlayBase\GxxGmPlayBase.h"


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
// [11字节：魔术头“GxxGmTunnel”][40字节：连接ID，NULL结尾][4字节：数据长度][剩余：实际数据]

const char *magic = "GxxGmTunnel";
const int magic_len = 11;
const int connection_id_len = 40;
const int data_len_space_len = 4;

GxxGmTunnelData::GxxGmTunnelData()
: tunnel_id_(NULL)
, tunnel_id_len_(0)
, tunnel_data_(NULL)
, tunnel_data_len_(0)
{

}

GxxGmTunnelData::~GxxGmTunnelData()
{

}

int GxxGmTunnelData::SetTunnelId(const char *tunnel_id, int id_len)
{
	int errCode = 0;

	if (tunnel_id_ != NULL)
		delete [] tunnel_id_;

	tunnel_id_ = new char[id_len];
	memcpy(tunnel_id_, tunnel_id, id_len);
	tunnel_id_len_ = id_len;

	return errCode;
}

int GxxGmTunnelData::SetTunnelData(const char *tunnel_data, int data_len)
{
	int errCode = 0;

	if (tunnel_data_ != NULL)
		delete [] tunnel_data_;

	tunnel_data = new char[data_len];
	memcpy(tunnel_data_, tunnel_data, data_len);
	tunnel_data_len_ = data_len;

	return errCode;
}

int GxxGmTunnelData::GetSerializedDataLength()
{
	return 11 + 40 + 4 + tunnel_data_len_;
}

int GxxGmTunnelData::GetSerializedData(char *data, int data_len)
{
	if (!data)
		return -1;

	if (data_len < (11 + 40 + 4 + tunnel_data_len_))
		return -2;

	// 组装数据
	memcpy(data, magic, magic_len);
	memcpy(data + magic_len, tunnel_id_, connection_id_len);
	memcpy(data + magic_len + connection_id_len, &tunnel_data_len_, data_len_space_len);
	memcpy(data + magic_len + connection_id_len + data_len_space_len, tunnel_data_, tunnel_data_len_);

	return (magic_len + connection_id_len + data_len_space_len + tunnel_data_len_);
}

int GxxGmTunnelData::ParseData(const char *data, int datalen)
{
	int errCode = 0;

	// 首先判断是否是我们的隧道数据
	char magic_data[11] = {0};
	memcpy(magic_data, data, magic_len);
	if (memcmp(magic_data, magic, magic_len) != 0)
		return -1;

	// 提取连接ID，检查最后一位是否为\0
	char tunnel_id[40] = {0};
	memcpy(tunnel_id, data + magic_len, connection_id_len);
	if (tunnel_id[39] != '\0')
		return -2;

	SetTunnelId(tunnel_id, strlen(tunnel_id));

	// 提取实际数据长度
	int tunnel_data_len = 0;
	memcpy(&tunnel_data_len, data + magic_len + connection_id_len, 4);

	SetTunnelData(data + magic_len + connection_id_len + 4, tunnel_data_len);

	return errCode;
}

std::string GxxGmTunnelData::GetTunnelId()
{
	if (!tunnel_id_)
		return "";

	return tunnel_id_;
}

int GxxGmTunnelData::GetTunnelData(char *tunnel_data, int *data_len)
{
	if (*data_len < tunnel_data_len_)
		return -2;

	if (!tunnel_data)
		return -1;

	if (!tunnel_data_)
		return -3;

	memcpy(tunnel_data, tunnel_data_, tunnel_data_len_);
	*data_len = tunnel_data_len_;

	return 0;
}


//////////////////////////////////////////////////////////////////////////
//
// 本类用于管理通道的连接映射关系

GxxGmTunnelConnections::GxxGmTunnelConnections()
{

}

GxxGmTunnelConnections::~GxxGmTunnelConnections()
{

}

SOCKET GxxGmTunnelConnections::HandleClientSocket(SOCKET client_socket)
{
	// 先查找一遍缓存，看看这个套接字有没有对应的id
	std::map<SOCKET, std::string>::iterator iter = client_connections_.find(client_socket);
	if (iter == client_connections_.end())
	{
		// 生成一个id
		std::string connection_id = GxxGmPlayBase::GenerateGUID();

		// 塞入缓存
		client_connections_.insert(std::pair<SOCKET, std::string>(client_socket, connection_id));
	}

	return client_socket;
}

SOCKET GxxGmTunnelConnections::HandleClientSocket(SOCKET client_socket, std::string connection_id)
{
	if (GetConnectionIdBySocket(client_socket).size() == 0)
	{
		if (GetSocketByConnectionId(connection_id) == INVALID_SOCKET)
		{
			client_connections_.insert(std::pair<SOCKET, std::string>(client_socket, connection_id));
			return client_socket;
		}
	}

	return INVALID_SOCKET;
}

void GxxGmTunnelConnections::RemoveTunnelConnection(SOCKET client_socket)
{
	std::map<SOCKET, std::string>::iterator iter;
	for (iter = client_connections_.begin(); iter != client_connections_.end(); ++iter)
	{
		if (iter->first == client_socket)
		{
			client_connections_.erase(iter);
			break;
		}
	}
}

void GxxGmTunnelConnections::RemoveTunnelConnection(std::string connection_id)
{
	std::map<SOCKET, std::string>::iterator iter;
	for (iter = client_connections_.begin(); iter != client_connections_.end(); ++iter)
	{
		if (iter->second.compare(connection_id) == 0)
		{
			client_connections_.erase(iter);
			break;
		}
	}
}

std::string GxxGmTunnelConnections::GetConnectionIdBySocket(SOCKET client_socket)
{
	std::map<SOCKET, std::string>::iterator iter;
	for (iter = client_connections_.begin(); iter != client_connections_.end(); ++iter)
	{
		if (iter->first == client_socket)
			return iter->second;
	}

	return "";
}

SOCKET GxxGmTunnelConnections::GetSocketByConnectionId(std::string connection_id)
{
	std::map<SOCKET, std::string>::iterator iter;
	for (iter = client_connections_.begin(); iter != client_connections_.end(); ++iter)
	{
		if (iter->second.compare(connection_id) == 0)
			return iter->first;
	}

	return INVALID_SOCKET;
}
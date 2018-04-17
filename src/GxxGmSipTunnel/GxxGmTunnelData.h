#ifndef _GxxGmTunnelData_H_
#define _GxxGmTunnelData_H_

#include <map>
#include <winsock.h>

//////////////////////////////////////////////////////////////////////////
//
// �������ڴ�����ͨ���ϴ�������ݣ����幦�ܰ�����
// 
// - ��װ����
// - ��������
// 
// ���ڻ���ݰ�ȫ���������ӻ��ڹ����㷨����Կ�����㷨��SM4�ӽ��ܹ���
//
// ���л�������ݽṹ
// [11�ֽڣ�ħ��ͷ��GxxGmTunnel��][40�ֽڣ�����ID][4�ֽڣ����ݳ���][ʣ�ࣺʵ������]

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
// �������ڹ���ͨ��������ӳ���ϵ

class GxxGmTunnelConnections
{
public:
	GxxGmTunnelConnections();
	~GxxGmTunnelConnections();

public:
	/**
	 * ����ͻ����׽�����Ϣ��������µ��׽��֣������һ��ID���������������ֱ�ӷ����׽���
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

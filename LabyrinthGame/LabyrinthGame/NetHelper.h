#pragma once

class NetHelper
{
private:
	SOCKET	m_sClient;
public:
	bool Connect(CString serverStr, int port);
	void Send(char *message);
	char * Receive();
};
#pragma once

class NetHelper
{
private:
	SOCKET	m_sClient;
public:
	bool Connect();
	void Send(char *message);
	char * Receive();
};
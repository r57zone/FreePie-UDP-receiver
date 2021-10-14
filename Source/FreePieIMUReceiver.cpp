#include <winsock2.h>
#pragma comment (lib,"WSock32.Lib")
#include <algorithm>

float bytesToFloat(unsigned char b3, unsigned char b2, unsigned char b1, unsigned char b0)
{
	unsigned char byte_array[] = { b3, b2, b1, b0 };
	float result;
	std::copy(reinterpret_cast<const char*>(&byte_array[0]),
		reinterpret_cast<const char*>(&byte_array[4]),
		reinterpret_cast<char*>(&result));
	return result;
}

int main()
{
	SetConsoleTitle(L"FreePieIMU receiver");

	SOCKET socketS;

	int iResult;

	WSADATA wsaData;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		WSACleanup();
		return 0;
	}

	struct sockaddr_in local;
	struct sockaddr_in from;
	int fromlen = sizeof(from);
	local.sin_family = AF_INET;
	local.sin_port = htons(5555);
	local.sin_addr.s_addr = INADDR_ANY;

	socketS = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	u_long nonblocking_enabled = true;
	ioctlsocket(socketS, FIONBIO, &nonblocking_enabled);

	if (socketS == INVALID_SOCKET) {
		printf("Error at socket");
		WSACleanup();
		return 0;
	}
	
	iResult = bind(socketS, (sockaddr*)&local, sizeof(local));

	if (iResult == SOCKET_ERROR)
	{
		printf("bind failed with error");
		WSACleanup();
		return 0;
	}

	int bytes_read = 0;

	unsigned char freePieIMU[50];

	memset(&freePieIMU, 0, sizeof(freePieIMU));

	while (true)
	{

		bytes_read = recvfrom(socketS, (char*)(&freePieIMU), sizeof(freePieIMU), 0, (sockaddr*)&from, &fromlen);
	 
		if (bytes_read > 0) {
			float AccelX = bytesToFloat(freePieIMU[2], freePieIMU[3], freePieIMU[4], freePieIMU[5]);
			float AccelY = bytesToFloat(freePieIMU[6], freePieIMU[7], freePieIMU[8], freePieIMU[9]);
			float AccelZ = bytesToFloat(freePieIMU[10], freePieIMU[11], freePieIMU[12], freePieIMU[13]);
			
			float GyroX = bytesToFloat(freePieIMU[14], freePieIMU[15], freePieIMU[16], freePieIMU[17]);
			float GyroY = bytesToFloat(freePieIMU[18], freePieIMU[19], freePieIMU[20], freePieIMU[21]);
			float GyroZ = bytesToFloat(freePieIMU[22], freePieIMU[23], freePieIMU[24], freePieIMU[25]);
			
			float MagX = bytesToFloat(freePieIMU[26], freePieIMU[27], freePieIMU[28], freePieIMU[29]);
			float MagY = bytesToFloat(freePieIMU[30], freePieIMU[31], freePieIMU[32], freePieIMU[33]);
			float MagZ = bytesToFloat(freePieIMU[34], freePieIMU[35], freePieIMU[36], freePieIMU[37]);

			float Yaw = bytesToFloat(freePieIMU[38], freePieIMU[39], freePieIMU[40], freePieIMU[41]);
			float Pitch = bytesToFloat(freePieIMU[42], freePieIMU[43], freePieIMU[44], freePieIMU[45]);
			float Roll = bytesToFloat(freePieIMU[46], freePieIMU[47], freePieIMU[48], freePieIMU[49]);

			printf("Accel=%5.2f\t%5.2f\t%5.2f  |  ", AccelX, AccelY, AccelZ);
			printf("Gyro=%5.2f\t%5.2f\t%5.2f  |  ", GyroX, GyroY, GyroZ);
			printf("Orient=%5.2f\t%5.2f\t%5.2f\n", Yaw, Pitch, Roll);
		}

			
		if ((GetAsyncKeyState(VK_ESCAPE) & 0x8000) != 0) break;
		
	}

	closesocket(socketS);

	return 0;
}


#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <mstcpip.h>
#include <iphlpapi.h>


#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")
using namespace std;



class packet_sniffer
{

private:


	struct iphdr {

		unsigned char iph_ihl : 4, iph_ver : 4; // Version and IHL are packed in the first byte

		unsigned char iph_tos; // Type of Service

		unsigned short iph_len; // Total length (header + data)

		unsigned short iph_id;  // Packet ID

		unsigned short iph_offset;  // Fragment offset

		unsigned char iph_ttl; // Time To Live

		unsigned char iph_pro; // Protocol (TCP = 6, UDP = 17, etc.) write all probability

		unsigned short iph_che; // Header checksum

		unsigned char iph_sour[4];  // Source IP address (4 bytes)

		unsigned char iph_destip[4]; // Destination IP address (4 bytes)

	};

	void flag(unsigned short x)
	{
		//1 2 4 8 16 32 
		if (x & 0x01)
			cout << " FIN ";
		if (x & 0x02)
			cout << " SYN ";
		if (x & 0x04)
			cout << " RST ";
		if (x & 0x08)
			cout << " PSH ";
		if (x & 0x10)
			cout << " ACK ";
		if (x & 0x20)
			cout << " URG ";
	}

	struct tcphd
	{

		unsigned short source; //source port
		unsigned short des; //destination port

		unsigned int seq; // sequence number
		unsigned int ack_seq; // acknowledgment number

		unsigned short doff_res_flags; //data offset, reserved bits, flags
		unsigned short window; // window size
		unsigned short check; // checksum
		unsigned short urg_ptr; // urgent pointer

	};

	struct udphd
	{

		unsigned short sor; //source port
		unsigned short des; //destination port
		unsigned short len; //lenght of udp header and data
		unsigned short che; //check sum
	};

	struct icmphd // icmp header
	{

		unsigned char type; // icmp message type

		unsigned char code; // icmp message code 

		unsigned short che; // icmp checksum

		unsigned short id; // identifire (for echo request/reply)

		unsigned short seq; // sequence number (for echo request/ reply)

	};

	iphdr* iP(const char* rec)
	{

		iphdr* iph = (iphdr*)rec; // no +14 to override ethernet header because win raw socket can not see it 

		bool tcp_gate = false, udp_gate = false, icmp_gate = false;

		cout << "ip version : " << (int)iph->iph_ver << endl;

		cout << " internet header lenght :" << (int)iph->iph_ihl << " which means ip header = " << 4 * iph->iph_ihl << endl;

		cout << "type of service tos : ";
		printf("0x%02x\n", iph->iph_tos);

		cout << "total lenght of the ip packet in bytes : " << ntohs(iph->iph_len) << endl;

		cout << "packet id = " << ntohs(iph->iph_id) << endl;

		cout << "offset segment : " << ntohs(iph->iph_offset) << endl; //"there is another packet will come (yes or no) : " << iph->iph_offset & 0b111 << endl;

		cout << "time to live (ttl) : " << (int)iph->iph_ttl << endl;


		cout << (int)iph->iph_pro << " = protocl is : ";
		switch (iph->iph_pro)
		{

		case 1:
			cout << "icmp\n";
			icmp_gate = true;
			break;

		case 2:
			cout << "IGMP\n";
			break;

		case 6:
			cout << "TCP\n";
			tcp_gate = true;
			break;

		case 17:
			cout << "UDP\n";
			udp_gate = true;
			break;

		case 47:
			cout << "GRE\n";
			break;

		default:

			cout << "other\n";

			break;
		}

		printf("checksum header 0x%04x\n", ntohs(iph->iph_che));

		cout << "source ip -> " << (int)iph->iph_sour[0] << '.' << (int)iph->iph_sour[1] << '.' << (int)iph->iph_sour[2] << '.' << (int)iph->iph_sour[3] << endl;

		cout << "destination ip -> " << (int)iph->iph_destip[0] << '.' << (int)iph->iph_destip[1] << '.' << (int)iph->iph_destip[2] << '.' << (int)iph->iph_destip[3] << endl;

		if (tcp_gate)
		{

			tcphd* tcp = (tcphd*)(rec + (4 * iph->iph_ihl)); // + 14sizeof(iphdr)

			cout << "source port : " << ntohs(tcp->source) << endl;

			cout << "destination port : " << ntohs(tcp->des) << endl;

			cout << "sequence number : " << ntohs(tcp->seq) << endl;

			cout << "acknowledgment number : " << ntohs(tcp->ack_seq) << endl;

			cout << " data offset = " << (ntohs(tcp->doff_res_flags) >> 12) << ", , i converted it for you(size of tcp header in bytes) : " << (ntohs(tcp->doff_res_flags) >> 12) * 4 << endl; // 4 * offset >>> 4 * number of bytes ,, because 1 word = 4 bytes, number of bytes , as example 4 * 6 = 12 bytes

			cout << " flags : ";
			flag((int)(ntohs(tcp->doff_res_flags) & 0x3F));
			cout << "\n window size : " << ntohs(tcp->window) << endl;
			cout << "checksum : " << ntohs(tcp->check) << endl;
			cout << "urgent pointer : " << ntohs(tcp->urg_ptr) << endl;
		}
		else if (udp_gate)
		{

			udphd* udp = (udphd*)(rec + (4 * iph->iph_ihl));

			cout << "source port : " << ntohs(udp->sor) << endl;

			cout << "destination port : " << ntohs(udp->des) << endl;

			cout << "lenght : " << ntohs(udp->len) << endl;

			cout << "checksum : " << ntohs(udp->che) << endl;
		}
		else if (icmp_gate)
		{

			icmphd* ic = (icmphd*)(rec + (4 * iph->iph_ihl));//icmp

			cout << "\n {{{0: Echo Reply,, 3: Destination Unreachable,,, 8 : Echo Request,, 11 : Time Exceeded,, 12 : Parameter Problem}}} icmp type : " << ic->type << endl;
			cout << "icmp code : " << ic->code << endl;

			cout << "checksum : " << ntohs(ic->che) << endl;

			if (ic->type == 8 || ic->type == 0) // echo or request
			{
				cout << "identifier : " << ntohs(ic->id);
				cout << "\nsequence number : " << ntohs(ic->seq) << endl;
			}
		}
		return iph;
	}

public:

	string getLocalIPAddress() {

		// Allocate memory for the adapter info

		IP_ADAPTER_INFO adapterInfo[16];
		DWORD bufLen = sizeof(adapterInfo);

		// Get the adapter information

		if (GetAdaptersInfo(adapterInfo, &bufLen) == ERROR_SUCCESS) {

			// Loop through the adapters and get the first valid IPv4 address

			PIP_ADAPTER_INFO pAdapterInfo = adapterInfo;
			while (pAdapterInfo) {
				if (pAdapterInfo->IpAddressList.IpAddress.String[0] != '0') {
					return pAdapterInfo->IpAddressList.IpAddress.String;
				}
				pAdapterInfo = pAdapterInfo->Next;
			}
		}
		return "";  // Return empty string if no IP address is found
	}

	void catch_packet(SOCKET raw_soc)
	{

		char* recvbytes = new char[65536];

		int byteslen = 65536;

		int timeout = 500000;//500, milliseconds 1000 =  sec 1

		setsockopt(raw_soc, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

		int number = 1;

		for (int i = 0; i < number; i++)
		{


			int byte_recived = recv(raw_soc, recvbytes, byteslen, 0);


			if (byte_recived == SOCKET_ERROR)
			{

				cout << "error code : " << WSAGetLastError << endl;
				break;
			}
			else if (byte_recived > 0)
			{

				cout << "recived packet : " << byte_recived << endl;
				//process the packet
				/*for (int ii = 0; ii < 14; ++ii)
					printf("%02X ", recvbytes[ii] & 0xFF); // for debug ,,, Ensures each byte is printed correctly }*/

				iphdr* one = iP(recvbytes);
				cout << endl << "*************************************************\n\n\n";

			}

			if (i + 1 == number)
			{

				cout << "this was the last packet enter the number of packets you want to catch (-1 for no more) :";

				cin >> number;

				if (number != -1)
					number = abs(number);

				i = 0;

			}

		}

		delete[] recvbytes;

	}

	int process()
	{

		WSADATA one;//this have the details of win socket

		if (WSAStartup(MAKEWORD(2, 2), &one) != 0)
			cout << "wsastrtup failed to make the socket";
		else
		{

			SOCKET rawsocket = socket(AF_INET, SOCK_RAW, IPPROTO_IP);// determine ip family, sockettype, a protocol for ip

			if (rawsocket == INVALID_SOCKET)
			{
				cout << "error happend " << WSAGetLastError() << endl;

				WSACleanup();
			}
			else//bind the socket
			{

				sockaddr_in z;

				z.sin_family = AF_INET;

				z.sin_port = 0;//must be = 0 to select all , 443 for https

				string localIP = getLocalIPAddress();
				if (localIP.empty()) {
					std::cout << "Unable to retrieve local IP address!" << std::endl;
					closesocket(rawsocket);
					WSACleanup();
					return 0;
				}

				z.sin_addr.s_addr = inet_addr(localIP.c_str());


				if (true)
				{
					if (bind(rawsocket, (SOCKADDR*)&z, sizeof(z)) == SOCKET_ERROR)
					{

						cout << "bind failed " << WSAGetLastError() << endl;

						closesocket(rawsocket);

						WSACleanup();

					}
					else
					{

						int yes = 1; //to enable ip header customization

						if (setsockopt(rawsocket, IPPROTO_IP, IP_HDRINCL, (char*)&yes, sizeof(yes)) == SOCKET_ERROR)
						{

							cout << "setsocketopt failed " << WSAGetLastError() << endl;

							closesocket(rawsocket);

							WSACleanup();

							//continue the remide code  od this place
						}
						else
						{



							DWORD Dvalue = 1;//this part is to make the packet sniffer able to see any packet even if it was not send to this device
							DWORD bytesReturned;
							if (WSAIoctl(rawsocket, SIO_RCVALL, &Dvalue, sizeof(Dvalue), NULL, 0, &bytesReturned, NULL, NULL) == SOCKET_ERROR) {

								std::cout << "WSAIoctl failed: " << WSAGetLastError() << std::endl;

								closesocket(rawsocket);

								WSACleanup();

							}
							else
							{



								catch_packet(rawsocket);



								closesocket(rawsocket);

								WSACleanup();
							}


						}



					}

				}
			}

		}

		return 0;
	}

};

int
main()
{

	packet_sniffer x;

	x.process();

	cin.get();

	return 0;

}

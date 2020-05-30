#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <signal.h>
#include <string.h>
#include <sys/time.h>
#include <fstream>
#include <iostream>
#include <sys/time.h>
#include <cerrno>
#include <vector>
using namespace std;


#define packet_size 65000


struct sockaddr_in si_other;
int s;
int numbytes;
socklen_t slen;

struct Packet
{
    int Seq;
    int len;
    char data[packet_size];
    bool LastPkt;    
};



void diep(char *s) {
    perror(s);
    exit(1);
}



void reliablyTransfer(char* hostname, unsigned short int hostUDPport, char* filename, unsigned long long int bytesToTransfer) {

    //Open the file
    //read the file and store it in a string array
    //ifstream file;
    //file.open(filename, ios::binary);
    FILE* fp;
    fp = fopen(filename, "rb");
    //char content[bytesToTransfer+1];
    //fread(content, sizeof(char), bytesToTransfer, fp);
    //std::vector<unsigned char>content(std::istreambuf_iterator<char>(file), {});
    //file.read(content, bytesToTransfer);
    /*for (int i = 0; i < bytesToTransfer; ++i)
    {
        cout<<content[i]<<endl;
    }*/
    /*string content,line;
    int line_num = 0;
    while(getline(file,line))
    {
        ++line_num;
    }
    //cout<<"line_num: "<<line_num<<endl;
    int count = 0;
    file.clear();
    file.seekg(0, ios::beg);
    if(file.is_open())
    {
        cout<<"file open successfullly!!!"<<endl;
        while(getline(file,line))
        {
            content += line;
            ++count;
            cout<<"count: "<<count<<endl;
            if(count==line_num && line!="\n")
                break;
            content += '\n';
            //if(count==line_num)
            //    break;
        }
    }
    // content have all the file content
    */

    unsigned long long int PktNum;

    PktNum = (bytesToTransfer%packet_size==0)?(bytesToTransfer/packet_size):(bytesToTransfer/packet_size + 1);

    cout<<"wtf"<<endl;

    unsigned long long int LastPktBytes = (bytesToTransfer%packet_size);

    cout<<"LastPktBytes: "<<LastPktBytes<<endl;

    slen = sizeof (si_other);

    if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
        diep("socket");


    //create the socket
    memset((char *) &si_other, 0, sizeof (si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(hostUDPport);
    if (inet_aton(hostname, &si_other.sin_addr) == 0) {

        fprintf(stderr, "inet_aton() failed\n");

        exit(1);
    }

    //Make the packet and store in packet vector
    cout<<"Finish construct socket..."<<endl;
    //vector<Packet> All_Packet;
    //char* sending_ptr = &content[0]; // this character will move with the sending progress
    /*for (unsigned long long int i = 0; i < PktNum; ++i)
    {
        Packet newpkt;
        newpkt.Seq = i;
        (i == PktNum-1) ? newpkt.LastPkt = true: newpkt.LastPkt = false;
        (i != PktNum-1) ? newpkt.len = packet_size : newpkt.len = (LastPktBytes);
        //cout<<"!!"<<endl;
        fread(newpkt.data, sizeof(char), newpkt.len, fp);
        //cout<<"??"<<endl;
        //cout<<"len: "<< newpkt.len <<endl;
        for(int j=0 ; j<packet_size ; j++,sending_ptr++)
        {
            if (*sending_ptr == NULL)
            {
                //newpkt.data[j] = '\0';
                //continue;
                break;
            }
            newpkt.data[j] = *sending_ptr;
            //cout<<newpkt.data[j];
        }
        //memcpy(newpkt.data, sending_ptr, sizeof(char)*newpkt.len);
        //sending_ptr = sending_ptr + newpkt.len;
        All_Packet.push_back(newpkt);
    }*/

    cout<<"Complete making packet..."<<endl;

    /*FILE* test_fp;
    test_fp = fopen("test_binary", "wb");

    for (int i = 0; i < All_Packet.size(); ++i)
    {
        fwrite(All_Packet[i].data, sizeof(char), All_Packet[i].len, test_fp);
    }

    fclose(test_fp);*/

    /* Send data and receive acknowledgements on s*/

    int ACK = 0;
    cout<<"start sending packet..."<<endl;

    for (unsigned long long int curSeq = 0; curSeq < PktNum; ++curSeq)
    {
        //char buf [packet_size];
        /*Packet* pkt = new Packet;
        pkt -> Seq = curSeq;
        cout<<"LastPkt: "<< pkt-> LastPkt<<endl;
        pkt->data[packet_size] = '\0';
        */
        Packet newpkt;
        newpkt.Seq = curSeq;
        (curSeq == PktNum-1) ? newpkt.LastPkt = true: newpkt.LastPkt = false;
        (curSeq != PktNum-1) ? newpkt.len = packet_size : newpkt.len = (LastPktBytes);
        fread(newpkt.data, sizeof(char), newpkt.len, fp);
        Packet buf[1];
        buf[0] = newpkt;
        //memcpy(buf,&All_Packet[curSeq],sizeof(Packet));
        cout<<"size of buf: "<<sizeof(buf)<<endl;
        

        if (sendto(s, buf, sizeof(buf) , 0 , (struct sockaddr *)&si_other, slen)==-1)
        {
            diep("sendto()");
        }

        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 100000;

        if(setsockopt(s, SOL_SOCKET , SO_RCVTIMEO, &tv, sizeof(tv))==-1)
        {
            cout<<"Time out!!!"<<endl;
        }

        while(1)
        {
            if(setsockopt(s, SOL_SOCKET , SO_RCVTIMEO, &tv, sizeof(tv))==-1)
            {
                cout<<"Time out!!!"<<endl;
            }
            int numbytes = recvfrom(s, &ACK, sizeof(ACK), 0 , (struct sockaddr *)&si_other, &slen);

            if(numbytes==-1 && errno == EAGAIN)
            {
                cout<<"Timeout and need to resend the packet..."<<endl;
                //cout<<"LastPkt: "<< All_Packet[curSeq].LastPkt<<endl;

                if (sendto(s, buf, sizeof(buf) , 0 , (struct sockaddr *)&si_other, slen)==-1)
                {
                    diep("sendto()");
                }
                continue;
            }

            else
            {
                if (ACK == curSeq)
                {
                    cout<<"ACK: "<<ACK<<endl;
                    cout<<"PktNum: "<<PktNum<<endl;
                    cout<<"Successfully recv correct ACK! Move on to the next packet!"<<endl;

                    if (ACK == PktNum-1)
                    {
                        cout<<"Received last ACK successfully, send FIN"<<endl;
                    }
                    break;
                    //break;
                    //if (ACK == PktNum-1)
                    //{
                    //    break;
                    //}
                }
                else
                {
                    cout<<"Incorrect ACK, keep waiting..."<<endl;
                    cout<<"ACK: "<<ACK<<endl;
                    cout<<"PktNum: "<<PktNum<<endl;
                    continue;
                }
            }
            
        }


    }


    /*for (int i=0; i<PktNum; i++)
    {
        for(int j=0 ; j<packet_size ;j++,sending_ptr++)
        {
            buf[j] = *sending_ptr;
            cout<<buf[j];
        }
        cout<<"buf size: "<<sizeof(buf)<<endl;
        cout<<buf<<endl;
        cout<<"Current PktNum: "<<i<<endl;



        if (sendto(s, buf, packet_size, 0, (struct sockaddr *)&si_other, slen)==-1)

            diep("sendto()");

    }*/

    cout<<"Finish sending packet..."<<endl;


    printf("Closing the socket\n");

    close(s);

    return;



}


int main(int argc, char** argv) {

    unsigned short int udpPort;
    unsigned long long int numBytes;

    if (argc != 5) {

        fprintf(stderr, "usage: %s receiver_hostname receiver_port filename_to_xfer bytes_to_xfer\n\n", argv[0]);

        exit(1);

    }

    udpPort = (unsigned short int) atoi(argv[2]);

    numBytes = atoll(argv[4]);
    reliablyTransfer(argv[1], udpPort, argv[3], numBytes);
    return (EXIT_SUCCESS);

}




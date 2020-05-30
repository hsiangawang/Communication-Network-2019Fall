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
#include <fstream>
#include <iostream>
using namespace std;

#define packet_size 65000

struct sockaddr_in si_me, si_other;
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

void diep(char *s) 
{
    perror(s);
    exit(1);
}



void reliablyReceive(unsigned short int myUDPport, char* destinationFile) {

    slen = sizeof (si_other);

    if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
        diep("socket");



    memset((char *) &si_me, 0, sizeof (si_me));
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(myUDPport);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
    printf("Now binding\n");
    if (bind(s, (struct sockaddr*) &si_me, sizeof (si_me)) == -1)
        diep("bind");


    /* Now receive data and send acknowledgements */
    //ofstream file;
    //file.open(destinationFile);
    FILE* fp;
    fp = fopen(destinationFile, "wb");

    /*if(file.is_open())
    {
        cout<<"Successfully open output file"<<endl;
    }
    else
    {
        cout<<"fail to open output file"<<endl; 
    }*/

    //Packet* received_pkt = new Packet;
    Packet* received_pkt;
    int CurrentSeq = -1;
    int ACK = -1;


    while(1)
    {
        //Packet* received_pkt = new Packet
        Packet buf[1];
        numbytes = recvfrom(s,buf,sizeof(buf),0,(struct sockaddr *)&si_other, &slen);
        received_pkt = &buf[0];
        //cout<<"received_pkt size: "<<sizeof(*received_pkt)<<endl;
        if(numbytes == -1)
        {
            diep("recvfrom()");
            exit(1);
        }
        else if (numbytes == 0)
        {
            continue;
        }
        else
        {
            //cout << "Received data and check the Seq..."<<endl;
            //cout << "Seq in packet: "<< received_pkt->Seq<<endl;
            //cout << "LastPkt or not: "<< received_pkt->LastPkt<<endl;
            if(received_pkt->Seq > CurrentSeq)
            {
                if (received_pkt->LastPkt != true)
                {
                    //string received_data = received_pkt -> data;
                    if (sizeof(*received_pkt) < packet_size)
                    {
                        cout<<"Correct Seq but corrupted data, need to resend the packet"<<endl;
                        //cout<<"received_data: "<<received_data<<endl;
                        //cout<<"received_data size: "<<received_data.size()<<endl;
                        if (sendto(s, &ACK, sizeof(ACK), 0 , (struct sockaddr *)&si_other, slen)==-1)
                        {
                            diep("sendto()");
                        }
                        continue;
                    }
                }

                cout<<"Correct Seq, send back ACK..."<<endl;
                CurrentSeq = received_pkt -> Seq;
                //CurrentSeq++;
                ACK = CurrentSeq;
                cout<<"LastPkt or not: "<<received_pkt->LastPkt<<endl;
                cout<<"CurrentSeq: "<<received_pkt->Seq<<endl;
                cout<<"ACK: "<<ACK<<endl;
                if (sendto(s, &ACK, sizeof(ACK), 0 , (struct sockaddr *)&si_other, slen)==-1)
                {
                    diep("sendto()");
                }
                cout<<"Writing data..."<<endl;
                /*string received_data = received_pkt -> data;
                if(received_data.size()>packet_size)
                {
                    received_data.resize(packet_size);
                }
                cout<<received_data<<endl;
                cout<<"received_data size: "<<received_data.size()<<endl;
                */
                //file << received_data;
                cout<< "received_pkt -> len: "<<received_pkt->len<<endl;
                fwrite(received_pkt -> data, sizeof(char), received_pkt->len , fp);

                if (received_pkt -> LastPkt)
                {
                    cout<<"Received Last Packet, Waiting for finsih bit..."<<endl;
                    break;
                }
                /*if(received_data.size()!=packet_size)
                {
                    break;
                }*/
                continue;
            }
            else
            {
                //if ((CurrentSeq == received_pkt ->Seq) && received_pkt -> LastPkt )
                //{
                //    break;
                //}
                cout<<"Duplicate data, discard and send back CurrentSeq ACK"<<endl;
                cout<<"Seqnum in pkt: "<<received_pkt->Seq<<endl;
                cout<<"CurrentSeq: "<<CurrentSeq<<endl;
                cout<<"ACK: "<<ACK<<endl;
                if (sendto(s, &ACK, sizeof(ACK), 0 , (struct sockaddr *)&si_other, slen)==-1)
                {
                    diep("sendto()");
                }
                continue;
            }
            /*
            cout<<"Writing data..."<<endl;
            string received_pkt = buf;
            if(received_pkt.size()>packet_size)
            {
                received_pkt.resize(packet_size);
            }
            cout<<received_pkt<<endl;
            cout<<"received_pkt size: "<<received_pkt.size()<<endl;
            file << received_pkt;
            if(received_pkt.size()!=packet_size)
            {
                break;
            }
            continue;
            */
        }
    }
    //file.close();
    fclose(fp);
    close(s);

    printf("%s received.", destinationFile);

    return;

}

int main(int argc, char** argv) {



    unsigned short int udpPort;



    if (argc != 3) {

        fprintf(stderr, "usage: %s UDP_port filename_to_write\n\n", argv[0]);

        exit(1);

    }



    udpPort = (unsigned short int) atoi(argv[1]);



    reliablyReceive(udpPort, argv[2]);

}


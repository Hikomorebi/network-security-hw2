#include <stdio.h>
#include <netdb.h>
#include <unistd.h>
#include <iostream>
#include <stdlib.h>
#include <errno.h>
#include <net/if.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <memory.h>
#include <sys/ioctl.h>
#include "CDesOperate.h"
#include "CRsaOperate.h"
using namespace std;
#define SERVERPORT 12345
#define BUFFERSIZE 64
#define DESKEYLENGTH 8
char strStdinBuffer[BUFFERSIZE];
char strSocketBuffer[BUFFERSIZE];
char strEncryBuffer[BUFFERSIZE];
char strDecryBuffer[BUFFERSIZE];
char strDesKey[DESKEYLENGTH];
char realDesKey[DESKEYLENGTH + 1];
PublicKey cRsaPublicKey;
ssize_t totalRecv(int s, void *buf, size_t len, int flags)
{
    size_t nCurSize = 0;
    while (nCurSize < len)
    {
        ssize_t nRes = recv(s, (char *)buf + nCurSize, len - nCurSize, flags);
        if (nRes < 0 || nRes + nCurSize > len)
        {
            return -1;
        }
        nCurSize += nRes;
    }
    return nCurSize;
}
void SecretChat(int nSock, char *pRemoteName, char *pKey)
{
    CDesOperate cDes;
    fd_set cHandleSet;
    struct timeval tv;
    int nRet;

    while (1)
    {
        FD_ZERO(&cHandleSet);
        FD_SET(nSock, &cHandleSet);
        FD_SET(0, &cHandleSet);
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        nRet = select(nSock > 0 ? nSock + 1 : 1, &cHandleSet, NULL, NULL, &tv);

        if (nRet < 0)
        {
            printf("Select ERROR!\n");
            break;
        }
        if (0 == nRet)
        {
            continue;
        }
        if (FD_ISSET(nSock, &cHandleSet))
        {
            bzero(&strSocketBuffer, BUFFERSIZE);
            int nLength = 0;
            nLength = totalRecv(nSock, strSocketBuffer, BUFFERSIZE, 0);
            if (nLength != BUFFERSIZE)
            {
                break;
            }
            else
            {
                int nLen = BUFFERSIZE;
                cDes.Decry(strSocketBuffer, BUFFERSIZE, strDecryBuffer, nLen, pKey, 8);
                strDecryBuffer[BUFFERSIZE - 1] = 0;
                if (strDecryBuffer[0] != 0 && strDecryBuffer[0] != '\n')
                {
                    printf("Receive message form <%s>: %s\n",
                           pRemoteName, strDecryBuffer);
                    if (0 == memcmp("quit", strDecryBuffer, 4))
                    {
                        printf("Quit!\n");
                        break;
                    }
                }
            }
        }
        if (FD_ISSET(0, &cHandleSet))
        {
            bzero(&strStdinBuffer, BUFFERSIZE);
            while (strStdinBuffer[0] == 0)
            {
                if (fgets(strStdinBuffer, BUFFERSIZE, stdin) == NULL)
                {
                    continue;
                }
            }
            int nLen = BUFFERSIZE;
            cDes.Encry(strStdinBuffer, BUFFERSIZE, strEncryBuffer, nLen, pKey, 8);
            if (send(nSock, strEncryBuffer, BUFFERSIZE, 0) != BUFFERSIZE)
            {
                perror("send");
            }
            else
            {
                if (0 == memcmp("quit", strStdinBuffer, 4))
                {
                    printf("Quit!\n");
                    break;
                }
            }
        }
    }
}
int main(int argc, char *[])
{
    printf("Client or Server?\r\n");
    cin >> strStdinBuffer;
    if (strStdinBuffer[0] == 'c' || strStdinBuffer[0] == 'C')
    {
        char strIpAddr[16];
        printf("Please input the server address:\r\n");
        cin >> strIpAddr;
        int nConnectSocket;
        struct sockaddr_in sDestAddr;
        if ((nConnectSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            perror("Socket");
            exit(errno);
        }
        memset(&sDestAddr, 0, sizeof(sDestAddr));
        sDestAddr.sin_family = AF_INET;
        sDestAddr.sin_port = htons(SERVERPORT);
        sDestAddr.sin_addr.s_addr = inet_addr(strIpAddr);
        if (connect(nConnectSocket, (struct sockaddr *)&sDestAddr, sizeof(sDestAddr)) != 0)
        {
            perror("Connect ");
            exit(errno);
        }
        else
        {
            printf("Connect Success! \n");
            GenerateDesKey(strDesKey);
            printf("Create DES key success\n");
            if (sizeof(cRsaPublicKey) == totalRecv(nConnectSocket, (char *)&cRsaPublicKey, sizeof(cRsaPublicKey), 0))
            {
                printf("Successful get the RSA public Key\n");
            }
            else
            {
                perror("Get RSA public key ");
                exit(0);
            }
            ULONG64 nEncryptDesKey[DESKEYLENGTH / 2];
            unsigned short *pDesKey = (unsigned short *)strDesKey;
            for (int i = 0; i < DESKEYLENGTH / 2; i++)
            {
                nEncryptDesKey[i] = CRSASection::Encry(pDesKey[i], cRsaPublicKey);
            }
            if (sizeof(ULONG64) * DESKEYLENGTH / 2 != send(nConnectSocket, (char *)nEncryptDesKey, sizeof(ULONG64) * DESKEYLENGTH / 2, 0))
            {
                perror("Send DES key Error");
                exit(0);
            }
            else
            {
                printf("Successful send the encrypted DES Key\n");
            }
            printf("Begin to chat...\n");
            for (int i = 0; i < DESKEYLENGTH; ++i)
            {
                realDesKey[i] = strDesKey[i];
            }
            realDesKey[DESKEYLENGTH] = '\0';

            SecretChat(nConnectSocket, strIpAddr, realDesKey);
        }
        close(nConnectSocket);
    }
    else
    {
        int nListenSocket, nAcceptSocket;
        socklen_t nLength = 0;
        struct sockaddr_in sLocalAddr, sRemoteAddr;
        if ((nListenSocket = socket(PF_INET, SOCK_STREAM, 0)) == -1)
        {
            perror("socket");
            exit(1);
        }

        memset(&sLocalAddr, 0, sizeof(sLocalAddr));
        sLocalAddr.sin_family = AF_INET;
        sLocalAddr.sin_port = htons(SERVERPORT);
        sLocalAddr.sin_addr.s_addr = INADDR_ANY;

        if (bind(nListenSocket, (struct sockaddr *)&sLocalAddr, sizeof(struct sockaddr)) == -1)
        {
            perror("bind");
            exit(1);
        }
        if (listen(nListenSocket, 5) == -1)
        {
            perror("listen");
            exit(1);
        }
        printf("Listening...\n");
        nLength = sizeof(struct sockaddr);
        if ((nAcceptSocket = accept(nListenSocket, (struct sockaddr *)&sRemoteAddr, &nLength)) == -1)
        {
            perror("accept");
            exit(errno);
        }
        else
        {
            close(nListenSocket);
            printf("server: got connection from %s, port %d, socket %d\n", inet_ntoa(sRemoteAddr.sin_addr), ntohs(sRemoteAddr.sin_port), nAcceptSocket);
            CRSASection cRsaSection;
            cRsaPublicKey = cRsaSection.GetPublicKey();
            if (send(nAcceptSocket, (char *)(&cRsaPublicKey), sizeof(cRsaPublicKey), 0) != sizeof(cRsaPublicKey))
            {
                perror("send");
                exit(0);
            }
            else
            {
                printf("successful send the RSA public key. \n");
            }
            ULONG64 nEncryptDesKey[DESKEYLENGTH / 2];
            if (DESKEYLENGTH / 2 * sizeof(ULONG64) != totalRecv(nAcceptSocket, (char *)nEncryptDesKey, DESKEYLENGTH / 2 * sizeof(ULONG64), 0))
            {
                perror("totalRecv DES key error");
                exit(0);
            }
            else
            {
                printf("successful get the DES key. \n");
                unsigned short *pDesKey = (unsigned short *)strDesKey;
                for (int i = 0; i < DESKEYLENGTH / 2; i++)
                {
                    pDesKey[i] = cRsaSection.Decry(nEncryptDesKey[i]);
                }
            }
            printf("Begin to chat...\n");
            for (int i = 0; i < DESKEYLENGTH; ++i)
            {
                realDesKey[i] = strDesKey[i];
            }
            realDesKey[DESKEYLENGTH] = '\0';

            SecretChat(nAcceptSocket, inet_ntoa(sRemoteAddr.sin_addr), realDesKey);
            close(nAcceptSocket);
        }
    }
    return 0;
}

void server()
{
    CRSASection cRsaSection;
    cRsaPublicKey = cRsaSection.GetPublicKey();
    if (send(nAcceptSocket,
             (char *)(&cRsaPublicKey),
             sizeof(cRsaPublicKey), 0) != sizeof(cRsaPublicKey))
    {
        perror("send");
        exit(0);
    }
    else
    {
        printf("successful send the RSA public key. \n");
    }
    UINT64 nEncryptDesKey[DESKEYLENGTH / 2];
    if (DESKEYLENGTH / 2 * sizeof(UINT64) != TotalRecv(nAcceptSocket,
                                                       (char *)nEncryptDesKey, DESKEYLENGTH / 2 * sizeof(UINT64), 0))
    {
        perror("TotalRecv DES key error");
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
    SecretChat(nAcceptSocket, inet_ntoa(sRemoteAddr.sin_addr), strDesKey);
    close(nAcceptSocket);
}

void client()
{
    printf("Connect Success! \n");
    GerenateDesKey(strDesKey);
    printf("Create DES key success\n");
    if (sizeof(cRsaPublicKey) == TotalRecv(nConnectSocket, (char *)&cRsaPublicKey,
                                           sizeof(cRsaPublicKey), 0))
    {
        printf("Successful get the RSA public Key\n");
    }
    else
    {
        perror("Get RSA public key ");
        exit(0);
    }
    UINT64 nEncryptDesKey[DESKEYLENGTH / 2];
    unsigned short *pDesKey = (unsigned short *)strDesKey;
    for (int i = 0; i < DESKEYLENGTH / 2; i++)
    {
        nEncryptDesKey[i] = CRSASection::Encry(pDesKey[i], cRsaPublicKey);
    }
    if (sizeof(UINT64) * DESKEYLENGTH / 2 != send(nConnectSocket, (char *)nEncryptDesKey,
                                                  sizeof(UINT64) * DESKEYLENGTH / 2, 0))
    {
        perror("Send DES key Error");
        exit(0);
    }
    else
    {
        printf("Successful send the encrypted DES Key\n");
    }
    printf("Begin to chat...\n");
    SecretChat(nConnectSocket, strIpAddr, strDesKey);
}
/*
 * File:   main.c
 * Author: rahul
 *
 * Created on 22 February, 2016, 11:25 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "encode.c"

void SMTPrequest(SSL* ssl){
  char resp_buff[4096]="";
  char buff[4096]="";

  SSL_read(ssl,resp_buff, sizeof (resp_buff));
  printf("[RECEIVED] %s",resp_buff);


  strcpy(buff, "EHLO localhost \r\n");
  SSL_write(ssl, buff, strlen(buff));
  printf("\n[Send] %s\n", buff);
  SSL_read(ssl,resp_buff, sizeof (resp_buff));
  printf("[RECEIVED] %s",resp_buff);
  bzero(resp_buff,sizeof(resp_buff));

  //const unsigned char *auth = "mangabountynaia@gmail.comMANGABOUNTYNAIA";
  FILE* email_config = fopen("config_email", "r+");
  char* auth = malloc(sizeof(char));
  fgets(auth, 100, email_config);
  //char * result = b64_encode(auth, strlen(auth)*sizeof(char));
  printf("Convert : %s",auth);
  printf("\n");
  //AG1hbmdhYm91bnR5bmFpYUBnbWFpbC5jb20ATUFOR0FCT1VOVFlOQUlB
  strcpy(buff, "AUTH PLAIN bGltZTkzMTUwQGdtYWlsLmNvbWxpbG91Y2hl\r\n");
  //strcat(buff, auth);
  //strcat(buff, "\r\n");
  SSL_write(ssl, buff, strlen(buff));
  printf("\n[Send] %s\n", buff);
  SSL_read(ssl,resp_buff, sizeof (resp_buff));
  printf("[RECEIVED] %s",resp_buff);
  bzero(resp_buff,sizeof(resp_buff));

  strcpy(buff, "MAIL FROM: <lime93150@gmail.com>\r\n");
  SSL_write(ssl, buff, strlen(buff));
  printf("\n[Send] %s\n", buff);
  SSL_read(ssl,resp_buff, sizeof (resp_buff));
  printf("[RECEIVED] %s",resp_buff);
  bzero(resp_buff,sizeof(resp_buff));

  strcpy(buff, "RCPT TO: <lilya.m@live.fr>\r\n");
  SSL_write(ssl, buff, strlen(buff));
  printf("\n[Send] %s\n", buff);
  SSL_read(ssl,resp_buff, sizeof (resp_buff));
  printf("[RECEIVED] %s",resp_buff);
  bzero(resp_buff,sizeof(resp_buff));

  strcpy(buff, "DATA\r\n");
  SSL_write(ssl, buff, strlen(buff));
  printf("\n[Send] %s\n", buff);
  SSL_read(ssl,resp_buff, sizeof (resp_buff));
  printf("[RECEIVED] %s",resp_buff);

  strcpy(buff, "Subject: coucou\r\nj'aime les croquettes\r\n.\r\n");
  SSL_write(ssl, buff, strlen(buff));
  printf("\n[Send] %s\n", buff);
  SSL_read(ssl,resp_buff, sizeof (resp_buff));
  printf("[RECEIVED] %s",resp_buff);
  bzero(resp_buff,sizeof(resp_buff));


  strcpy(buff, "QUIT\r\n");
  SSL_write(ssl, buff, strlen(buff));
  printf("\n[Send] %s\n", buff);
  SSL_read(ssl,resp_buff, sizeof (resp_buff));
  printf("[RECEIVED] %s",resp_buff);
}

const char* GetIPAddress(const char* target_domain) {
    //printf("get ip address");
    const char* target_ip;
    struct in_addr *host_address;
    struct hostent *raw_list = gethostbyname(target_domain);
    int i = 0;
    for (i; raw_list->h_addr_list[i] != 0; i++) {
        host_address = raw_list->h_addr_list[i];
        target_ip = inet_ntoa(*host_address);
    }
    return target_ip;
}



int ConnectToServer(const char* server_address) {
    int socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof (addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(465);
    if (inet_pton(AF_INET, GetIPAddress(server_address), &addr.sin_addr) == 1) {
        connect(socket_fd, (struct sockaddr*)&addr, sizeof (addr));
    }
    return socket_fd;
}

int main() {
    BIO *obj_bio = NULL;
    BIO *obj_out = NULL;
    const SSL_METHOD *method;
    SSL_CTX *ctx;
    SSL *ssl;
    int connected_fd = 0;
    int ret, i;

    OpenSSL_add_all_algorithms();
    ERR_load_BIO_strings();
    ERR_load_crypto_strings();
    SSL_load_error_strings();

    obj_bio = BIO_new(BIO_s_file());
    obj_out = BIO_new_fp(stdout, BIO_NOCLOSE);

    if (SSL_library_init() < 0) {
        BIO_printf(obj_out, "OpenSSL not initialize");
    } else {
        method = SSLv23_client_method();
        if ((ctx = SSL_CTX_new(method)) == NULL) {
            BIO_printf(obj_out, "OpenSSL context not initialize");
        } else {
            SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv2);
            ssl = SSL_new(ctx);
            connected_fd = ConnectToServer("smtp.gmail.com");
            if (connected_fd != 0) {
                BIO_printf(obj_out, "Connected Successfully");
                SSL_set_fd(ssl, connected_fd);
                if (SSL_connect(ssl) != 1) {
                    BIO_printf(obj_out, "SLL session not created");
                } else {

                    SMTPrequest(ssl);

                }
            }
        }
    }
    return (EXIT_SUCCESS);
}



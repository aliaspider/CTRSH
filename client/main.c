
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>

#define IP2INT(a, b, c, d) (a | (b << 8) | (c << 16) | (d <<24))
#define CTR_IP    IP2INT(10, 42, 0, 237)
#define CTR_PORT  5000

#define DEBUG_ERROR(X) do{int res_ = (int)(intptr_t)(X); if(res_ < 0){printf("error %i @%s (%s:%d).\n%s\n", res_, __FUNCTION__, __FILE__, __LINE__,strerror(errno)); exit(0);}}while(0)
#define DEBUG_ERROR_stay(X) do{int res_ = (int)(intptr_t)(X); if(res_ < 0){printf("error %i @%s (%s:%d).\n%s\n", res_, __FUNCTION__, __FILE__, __LINE__,strerror(errno));}}while(0)

int main(int argc, char *argv[])
{
    printf("loading file\n");
    FILE* rgui_fp = fopen("./rgui.dat", "rb");
    DEBUG_ERROR(rgui_fp);
    fseek(rgui_fp, 0, SEEK_END);
    size_t rgui_file_size = ftell(rgui_fp);
    void* rgui_buffer = malloc(rgui_file_size);
    fseek(rgui_fp, 0, SEEK_SET);
    fread(rgui_buffer, 1, rgui_file_size, rgui_fp);
    fclose(rgui_fp);

    while(1)
    {
       int sockfd,  n;
       struct sockaddr_in serv_addr = {0};
       DEBUG_ERROR(sockfd = socket(AF_INET, SOCK_STREAM, 0));
       serv_addr.sin_family = AF_INET;
       serv_addr.sin_addr.s_addr = CTR_IP;
       serv_addr.sin_port = htons(CTR_PORT);
       int ret;
       do
       {
          ret = connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
//          DEBUG_ERROR_stay(ret);
          if(ret < 0)
            usleep(10000);
       }
       while(ret < 0);


       printf("sending file\n");
       DEBUG_ERROR(write(sockfd, &rgui_file_size, 4));
       DEBUG_ERROR(write(sockfd, rgui_buffer, rgui_file_size));
       close(sockfd);
    }

//    printf("recieving message :\n");
//    memset(buffer, 0x0, 256);
//    DEBUG_ERROR(n = read(sockfd,buffer,255));
//    printf("%s\n",buffer);

    return 0;
}

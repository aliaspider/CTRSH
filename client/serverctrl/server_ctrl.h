#ifndef SERVER_CTRL_H
#define SERVER_CTRL_H

#include <stdint.h>
#include "utils/file_list.h"
#include "common.h"


void run_server_3dsx(const char* _3dsx_path);

void server_connect(void);

void server_exit(void);

void server_display_image(const void* buffer, uint32_t size);

filelist_t *server_get_filelist(const char* path);

void server_put(const void* buffer, uint32_t size);




ssize_t server_send_command(ctrsh_command_id_t command_id);
ssize_t server_send_int32(int32_t val);
ssize_t server_send_data(const void *buffer, size_t size);

ssize_t server_recv_data(void** buffer);

#endif // SERVER_CTRL_H

#include <stdio.h>
#include <uv.h>

uv_loop_t *pLoop;
uv_stream_t *stream;
uv_timer_t sTimer;

void allocFn(uv_handle_t *pHandle, size_t uiSuggestedSize, uv_buf_t *pBuf) {
  printf("allocFn\n");
  uv_buf_init(malloc(sizeof(uiSuggestedSize)), uiSuggestedSize);
}

void onClose(uv_handle_t* handle) {
  printf("closed.");
}

void onWrite(uv_write_t* req, int iStatus) {
  if (iStatus < 0) {
    printf("uv_write error: %s\n", uv_strerror(iStatus));
    uv_timer_stop(&sTimer);
    return;
  }
  printf("wrote.\n");
}

void timerSend(uv_timer_t *pTimer) {
  unsigned char ubMsgCount;

  uv_buf_t buffer[] = {
    {.base = "hello", .len = 5},
    {.base = "world", .len = 5}
  };
  ubMsgCount = 2;

  printf("timer\n");
  uv_write_t sWriteRequest;
  uv_write(&sWriteRequest, stream, buffer, ubMsgCount, onWrite);
}


void onConnect(uv_connect_t* connection, int iStatus) {
  if(iStatus < 0) {
    printf("Connection fail: %s\n", uv_strerror(iStatus));
    return;
  }
  printf("connected.\n");

  stream = connection->handle;

  printf("timer init\n");
  uv_timer_init(pLoop, &sTimer);
  printf("timer start\n");
  uv_timer_start(&sTimer, timerSend, 1, 1000);
}


int main(int argc, char **argv) {

  pLoop = malloc(sizeof(uv_loop_t));
  uv_loop_init(pLoop);

  uv_tcp_t sTCP;
  uv_tcp_init(pLoop, &sTCP);

  struct sockaddr_in sConnAddr;
  uv_ip4_addr("127.0.0.1", 888, &sConnAddr);

  uv_connect_t sConn;
  uv_tcp_connect(&sConn, &sTCP, (const struct sockaddr *)&sConnAddr, onConnect);

  uv_run(pLoop, UV_RUN_DEFAULT);

  uv_loop_close(pLoop);
  free(pLoop);

  system("pause");
  return 0;
}

#include <stdio.h>
#include <uv.h>

#define BACKLOG_LENGTH 10

uv_loop_t *pLoop;
int iError;

void allocFn(uv_handle_t *pHandle, size_t uiSuggestedSize, uv_buf_t *pBuf) {
  printf("allocFn\n");
  *pBuf = uv_buf_init(malloc(sizeof(uiSuggestedSize)), uiSuggestedSize);
}

void onRead(uv_stream_t *pClientStream, ssize_t iLength, const uv_buf_t *pBuf) {
  char *szTxt;
  if(iLength < 0) {
    printf("Read error: %s\n", uv_strerror(iLength));
    return;
  }

  if(iLength == 0) {
    printf("Read 0\n");
    free(pBuf->base);
  }
  else {
    printf("Reading...\n");

    szTxt = malloc(iLength+1);
    memcpy(szTxt, pBuf->base, iLength);
    szTxt[iLength] = 0;

    printf("read: %s\n", szTxt);
    free(szTxt);
    free(pBuf->base);
  }

  uv_read_start(pClientStream, allocFn, onRead);
}

void onConnect(uv_stream_t *pServerStream, int iStatus) {
  uv_tcp_t *pClientTCP;

  if(iStatus < 0) {
    printf("Connection error: %s\n", uv_strerror(iStatus));
    return;
  }
  printf("Client connected\n");

  pClientTCP = malloc(sizeof(uv_tcp_t));
  uv_tcp_init(pLoop, pClientTCP);

  iError = uv_accept(pServerStream, (uv_stream_t*)pClientTCP);
  if(iError < 0) {
    printf("Accept error: %s\n", uv_strerror(iError));
    uv_close((uv_handle_t*)pClientTCP, 0);
    return;
  }

  printf("Client accepted\n");
  iError = uv_read_start((uv_stream_t*)pClientTCP, allocFn, onRead);
  if(iError < 0) {
    printf("Read error: %s\n", uv_strerror(iError));
    uv_close((uv_handle_t*)pClientTCP, 0);
    return;
  }
}

int main(void) {
  struct sockaddr_in sListenAddr; /// Konfiguracja nasluchiwania serwera
  uv_tcp_t sTCP;                  /// TCP Handle

  pLoop = malloc(sizeof(uv_loop_t));
  uv_loop_init(pLoop);

  uv_tcp_init(pLoop, &sTCP);
  uv_ip4_addr("127.0.0.1", 8888, &sListenAddr);
  uv_tcp_bind(&sTCP, (const struct sockaddr*)&sListenAddr, 0);

  iError = uv_listen((uv_stream_t*)&sTCP, BACKLOG_LENGTH, onConnect);
  if(iError) {
    printf("error: %s", uv_strerror(iError));
    return 1;
  }

  uv_run(pLoop, UV_RUN_DEFAULT);

  uv_loop_close(pLoop);
  free(pLoop);

  system("pause");
  return 0;
}

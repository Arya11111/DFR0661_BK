#include "DFCamera.h"

struct mjpeg_session session = {0};
DFRobot_Camera::DFRobot_Camera(){
  
}
DFRobot_Camera::~DFRobot_Camera(){
  
}

void DFRobot_Camera::camera_start(void)
{
    static rt_uint8_t is_start = 0;

    if(is_start == 1) 
    {
        rt_kprintf("camera is already start\n");
        return;
    }

    session.setup.send_type = TVIDEO_SND_INTF;
    session.setup.send_func = tvideo_capture_cb;
    session.setup.start_cb = NULL;
    session.setup.end_cb = NULL;

    session.setup.pkt_header_size = sizeof(HDR_ST);
    session.setup.add_pkt_header = pkt_header_cb;

    video_transfer_init(&session.setup);

    is_start = 1;
}
MSH_CMD_EXPORT(camera_start, camera_start);

void mjpeg_thread(void* param)
{
  rt_thread_t tid1, tid2;
  int ret;

  boundary_data = rt_malloc(1024);
  if(boundary_data == RT_NULL)
  {
      rt_kprintf("malloc boundary_data failed\n");
      goto __exit;
  }

  session.event = rt_event_create("vt_event", RT_IPC_FLAG_FIFO);

  tid1 = rt_thread_create("tcpsrv", tcpserv, RT_NULL, 1024*4, 10, 10);
  rt_thread_startup(tid1);

  rt_event_recv(session.event, CLIENT_CONNECT, RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR, RT_WAITING_FOREVER, RT_NULL);

  while(1)
  {
      tvideo_capture(1);
      rt_event_recv(session.event, SEND_FRAME_EVENT, RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR, RT_WAITING_FOREVER, RT_NULL);
      if(session.connected >= 0)
      {
          // rt_kprintf("send data\n");
          rt_memset(boundary_data, 0x0, 1024);
          ret = rt_sprintf(boundary_data, "------Boundary-------\r\nContent-Type: image/jpeg\r\nContent-Length: %d\r\n\r\n", session.total_len);
          // rt_kprintf("pic len : %d\n", session.total_len);
          ret = send(session.connected, boundary_data, ret, 0);
          // hexdump(session.buf, 16);
          ret = send(session.connected, session.buf, session.total_len, 0);
          if(ret <= 0) 
          {
              if(session.connected >= 0) closesocket(session.connected);
              session.connected = -1;
          }
          session.old_tick = rt_tick_get();
          session.total_len = 0;
      }
      tvideo_capture(0);
  }
  return;

__exit:
  if(session.buf != RT_NULL) rt_free(session.buf);
  if(boundary_data != RT_NULL) rt_free(boundary_data);

}

void DFRobot_Camera::hexdump(const rt_uint8_t *p, rt_size_t len){
  unsigned char *buf = (unsigned char*)p;
  int i, j;
  
  rt_kprintf("Dump 0x%.8x %dBytes\n", (int)p, len); 
  rt_kprintf("Offset    0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n"); 

  for (i=0; i<len; i+=16)
  {
      rt_kprintf("%08X: ", i+(int)p);

      for (j=0; j<16; j++)
      {
          if (i+j < len)
          {
              rt_kprintf("%02X ", buf[i+j]);
          }
          else
          {
              rt_kprintf("   ");
          }
      } 
      rt_kprintf(" ");

      for (j=0; j<16; j++)
      {
          if (i+j < len)
          {
              rt_kprintf("%c", ((unsigned int)((buf[i+j]) - ' ') < 127u - ' ') ? buf[i+j] : '.');
          }
      }
      rt_kprintf("\n");
  }
}

void DFRobot_Camera::get_ip(char* buf)
{
    rt_ubase_t index;
    struct netif * netif;

    rt_enter_critical();
    netif = netif_list;

    while(netif != RT_NULL)
    {
        if(netif->flags & NETIF_FLAG_LINK_UP)
        {
            rt_kprintf("ip address: %s\n", ipaddr_ntoa(&(netif->ip_addr)));
            memcpy(buf, ipaddr_ntoa(&(netif->ip_addr)), 16);
        }
        netif = netif->next;
    }
    rt_exit_critical();
}

void DFRobot_Camera::tcpserv(void *parameter)
{
  char *recv_data;
  socklen_t sin_size;
  struct sockaddr_in server_addr, client_addr;
  char ip[20] = {0};

  if((session.sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
      rt_kprintf("socket error\n");
      goto __exit;
  }

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT); 
  server_addr.sin_addr.s_addr = INADDR_ANY;
  rt_memset(&(server_addr.sin_zero), 0, sizeof(server_addr.sin_zero));

  if(bind(session.sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
  {
      rt_kprintf("unable to bind\n");
      goto __exit;
  }

  if(listen(session.sock, 5) == -1)
  {
      rt_kprintf("listen error\n");
      goto __exit;
  }

  get_ip(ip);

  rt_kprintf("\nTCPServer Waiting for client on %s:%d...\n", ip, PORT);
  while(1)
  {
    sin_size = sizeof(struct sockaddr_in);
    session.connected = accept(session.sock, (struct sockaddr *)&client_addr, &sin_size);

    if(session.connected < 0)
    {
        rt_kprintf("accept connection failed! errno = %d\n", errno);
        goto __exit;
    }

    rt_kprintf("I got a connection from (%s , %d)\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    send(session.connected, resp_header, strlen(resp_header), 0);

    camera_start();
    rt_event_send(session.event, CLIENT_CONNECT);
  }

__exit:
  if(session.sock >= 0)
  {
      closesocket(session.sock);
      session.sock = -1;
  }
  rt_kprintf("exit tcp server thread\n");
  return ;
}

DFRobot_Camera Camera;
// I apologise for the layout of this code. I had to recover it and the formatting went
// This c code is for implementing a low level network layer within a network stack

#include "net.h"

uint64_t time;
int num_nodes;
int num_vert;
int next_node;
int sent_count;

typedef struct
{
  uint8_t control[2];
  uint8_t src_address;
  uint8_t dest_address;
  uint8_t length;
  uint8_t tran_segment[8];
  uint8_t checksum[2];
} network;
network packet;

typedef struct
{
  uint8_t net_packet[15];
} comb;
comb net;

typedef struct
{
  uint8_t w_0_1;
  uint8_t w_0_2;
  uint8_t w_1_2;
  uint8_t G[9];
} weight;
weight w;

void net_setup()
{//fuction to set up initial values needed in the code
  num_nodes = 3;
  sent_count = 0;
  w.w_0_1 = 1;
  w.w_0_2 = 1;
  w.w_1_2 = 1;

  w.G[0] = 0;
  w.G[1] = w.w_0_1;
  w.G[2] = w.w_0_2;
  w.G[3] = w.w_0_1;
  w.G[4] = 0;
  w.G[5] = w.w_1_2;
  w.G[6] = w.w_0_2;
  w.G[7] = w.w_1_2;
  w.G[8] = 0;
}

void net_send(uint8_t* tran_data, uint8_t dest, uint8_t length)
{//fucntion to send data received from the transport layer to the dll layer
  sent_count = sent_count+1;
  if (sent_count == 20)
  {
    reset_weights();
  }
  packet.control[0] = 0;
  packet.control[1] = 0;
  packet.src_address = system_address;
  packet.dest_address = dest;
  packet.length = length;
  int i = 0;
  for(i; i<length; i++)
  {
    packet.tran_segment[i] = tran_data[i];
  }
  num_vert = num_nodes*((num_nodes-1)/2);
  next_node = calculate_next_node(num_vert,packet.src_address,packet.dest_address);
  increase_weights();
  create_net_packet();
  //phil's dll_send(net_data,next_node,data_size)
  //put_str("got to this point\n\r");
  dll_send(net.net_packet,next_node,15);
  put_str("Sent Data To Physical Layer\n\r");
  /*i = 0;
  char text[4];
  for(i; i<15; i++)
  {
  sprintf(text, "%d ",net.net_packet[i]);
  put_str(text);
  }
  put_str("\n\r");*/
}

void increase_weights()
{
  if(packet.src_address == 0)
  {
  if(packet.dest_address == 1)
  {
  w.w_0_1 = w.w_0_1 + 1;
  }
  }
  if(packet.src_address == 1)
  {
  if(packet.dest_address == 0)
  {
  w.w_0_1 = w.w_0_1 + 1;
  }
  }
  if(packet.src_address == 0)
  {
  if(packet.dest_address == 2)
  {
  w.w_0_2++;
  }
  }
  if(packet.src_address == 2)
  {
  if(packet.dest_address == 0)
  {
  w.w_0_2++;
  }
  }
  if(packet.src_address == 1)
  {
  if(packet.dest_address == 2)
  {
  w.w_1_2++;
  }
  }
  if(packet.src_address == 2)
  {
  if(packet.dest_address == 1)
  {
  w.w_1_2++;
  }
  }
  13
  w.G[0] = 0;
  w.G[1] = w.w_0_1;
  w.G[2] = w.w_0_2;
  w.G[3] = w.w_0_1;
  w.G[4] = 0;
  w.G[5] = w.w_1_2;
  w.G[6] = w.w_0_2;
  w.G[7] = w.w_1_2;
  w.G[8] = 0;
  put_str("Increased Weights\n\r");
}

void net_resend(uint8_t* net_data, uint8_t length)
{//function to send data from an intermediate node to the final node
  int i=0;
  for(i; i<length; i++)
  {
  net.net_packet[i] = net_data[i];
  }
  num_vert = num_nodes*((num_nodes-1)/2);
  next_node = calculate_next_node(num_vert,system_address,net.net_packet[2]);
  //phil's dll_send(net_data,next_node,data_size)
  dll_send(net.net_packet,next_node,15);
  put_str("Data Resent To Physical Layer\n\r");
  }

  void net_receive(uint8_t* net_data, uint8_t length)
  {//function to send data from the dll layer to the transport layer
  int i=0;
  for(i; i<length; i++)
  {
  net.net_packet[i] = net_data[i];
  }
  put_str("Received Data\n\r");
  i = 0;
  char text[4];
  for(i;i<length;i++){
  sprintf(text, "%d ",net.net_packet[i]);
  put_str(text);
  }
  put_str("\n\r");
  net_checksum();
  if(packet.checksum[0] != net_data[13])
  {
  put_str("Checksum 1 Passed\n\r");
  }
  if(packet.checksum[1] != net_data[14])
  {
  put_str("Checksum 2 Passed\n\r");
  }
  if(net.net_packet[0] == 0)
  {
  if(net.net_packet[3] == system_address)
  {
  uint8_t tran_data[8];
  put_str("Reached Final Node\n\r");
  int i=0;
  for(i; i<8; i++)
  {
  tran_data[i] = net.net_packet[i+5];
  }
  //ket's tran_recieve(tran_data,len)
  put_str("Sent Data To Transport Layer\n\r");
  trans_netw_receive(tran_data, net.net_packet[2]);
  }
  else
  {
  net_resend(net.net_packet,15);
  }
  }
  if(net.net_packet[0] == 1)
  {
  for(i; i<8; i++)
  {
  w.G[i] = (w.G[i] + net.net_packet[i+5])/2;
  }
  }
}

void net_update_weigts()
{//fucntion to update the weights across all nodes
  w.G[0] = 0;
  w.G[1] = w.w_0_1;
  w.G[2] = w.w_0_2;
  w.G[3] = w.w_0_1;
  w.G[4] = 0;
  w.G[5] = w.w_1_2;
  w.G[6] = w.w_0_2;
  w.G[7] = w.w_1_2;

  create_weight_packet();
  put_str("Flooded Weight Packet\n\r");
  int i;
  i = 0;
  char text[4];
  for(i; i<8; i++)
  {
  sprintf(text, "%d ",w.G[i]);
  put_str(text);
  }
  put_str("\n\r");
  //phil's flood function
  dll_flood(net.net_packet,15);
}

void create_weight_packet()
{//function to create the weight packet to be sent
  net.net_packet[0] = 1;
  net.net_packet[1] = 0;
  net.net_packet[2] = 0;
  net.net_packet[3] = 0;
  net.net_packet[4] = 8;
  int i = 0;
  for(i; i<8; i++)
  {
  net.net_packet[i+5] = w.G[i];
  }
  net_checksum();
  net.net_packet[8+5] = packet.checksum[0];
  net.net_packet[8+6] = packet.checksum[1];
  put_str("Created Weight Packet\n\r");
  i = 0;
  char text[4];
  for(i; i<15; i++)
  {
  sprintf(text, "%d ",net.net_packet[i]);
  put_str(text);
  }
  put_str("\n\r");
}

void reset_weights()
{//function to reset the weights to the default value
  w.w_0_1 = 1;
  w.w_0_2 = 1;
  w.w_1_2 = 1;
  16
  w.G[0] = 0;
  w.G[1] = w.w_0_1;
  w.G[2] = w.w_0_2;
  w.G[3] = w.w_0_1;
  w.G[4] = 0;
  w.G[5] = w.w_1_2;
  w.G[6] = w.w_0_2;
  w.G[7] = w.w_1_2;
  w.G[8] = 0;
  put_str("Resetting Weights\n\r");
}

void net_flood(uint8_t* tran_data, uint8_t length)
{//function to flood the network
  packet.control[0] = 1;
  packet.src_address = tran_data[2];
  packet.dest_address = tran_data[3];
  packet.length = length;
  int i = 0;
  for(i; i<length; i++)
  {
  packet.tran_segment[i] = tran_data[i];
}
create_net_packet();
//phil's flood()
//dll_flood(net.net_packet,15);
}

void create_net_packet()
{//function to create the network packet to be sent
  net.net_packet[0] = packet.control[0];
  net.net_packet[1] = packet.control[1];
  net.net_packet[2] = packet.src_address;
  net.net_packet[3] = packet.dest_address;
  net.net_packet[4] = packet.length;
  int i = 0;
  for(i; i<packet.length; i++)
  {
  net.net_packet[i+5] = packet.tran_segment[i];
  }
  net_checksum();
  net.net_packet[packet.length+5] = packet.checksum[0];
  net.net_packet[packet.length+6] = packet.checksum[1];
  put_str("Created Net Packet\n\r");
  i = 0;
  char text[4];
  for(i; i<15; i++)
  {

  sprintf(text, "%d ",net.net_packet[i]);
  put_str(text);
  }
  put_str("\n\r");
}

int calculate_next_node(int n,int startnode,int endnode)
{//function to calculate the next node
  int G[MAX][MAX];
  int ii = 0;
  int jj = 0;
  int k = 0;
  for(jj=0; jj<3; jj++)
  {
  for(ii=0; ii<3; ii++)
  {
  G[ii][jj] = w.G[k];
  k = k+1;
  }
  }
  put_str("Created Weight Matrix\n\r");
  ii = 0;
  jj = 0;
  for(jj=0; jj<3; jj++)
  {
  char text[4];
  for(ii=0; ii<3; ii++)
  {
  sprintf(text, "%d ",G[ii][jj]);
  put_str(text);
  }
  }
  put_str("\n\r");
  int cost[MAX][MAX],distance[MAX],pred[MAX];
  int visited[MAX],count,mindistance,nextnode,i,j;
  //pred[] stores the predecessor of each node
  //count gives the number of nodes seen so far
  //create the cost matrix
  for(i=0;i<n;i++)
  for(j=0;j<n;j++)
  if(G[i][j]==0)
  cost[i][j]=MINI_INFINITY;
  else
  cost[i][j]=G[i][j];
  18
  //initialize pred[],distance[] and visited[]
  for(i=0;i<n;i++)
  {
  distance[i]=cost[startnode][i];
  pred[i]=startnode;
  visited[i]=0;
  }
  distance[startnode]=0;
  visited[startnode]=1;
  count=1;
  while(count<n-1)
  {
  mindistance=MINI_INFINITY;
  //nextnode gives the node at minimum distance
  for(i=0;i<n;i++)
  if(distance[i]<mindistance&&!visited[i])
  {
  mindistance=distance[i];
  nextnode=i;
  }
  //check if a better path exists through nextnode
  visited[nextnode]=1;
  for(i=0;i<n;i++)
  if(!visited[i])
  if(mindistance+cost[nextnode][i]<distance[i])
  {
  distance[i]=mindistance+cost[nextnode][i];
  pred[i]=nextnode;
  }
  count++;
  }
  /*for(i=0;i< ((int)(sizeof(pred) / sizeof(pred[0])));i++)
  {
  put_str("%d\n",pred[i]);
  }*/
  int currnode = endnode; //(node you want to reach)
  while (pred[currnode] != startnode){
  currnode = pred[currnode];
  }
  put_str("Calculated Next Node\n\r");
  char text[20];
  sprintf(text, "Next node to %d is %d\n\r",endnode,currnode);
  put_str(text);
  return currnode;

//print the path and distance of each node
/*for(i=0;i<n;i++)
if(i!=startnode)uint8_t create_packet()
{uint8_t create_packet()
printf("\nDistance of node%d=%d",i,distance[i]);
printf("\nPath=%d",i);
j=i;uint8_t net_packet[128];
do
{
j=pred[j];
printf("<-%d",j);
}while(j!=startnode);
}*/
}
void net_update()
{
  if (time < system_time) {
  time = system_time + 15000;
  net_update_weigts();
  }
  }
  uint8_t countSetBits(uint8_t n)
  {//function that counts the number of 1s in a byte
  uint8_t count = 0;
  while (n) {
  count += n & 1;
  n >>= 1;
  }
  //printf("Count is %d",count);
  return count;
}
void net_checksum()
{//function that calculates the checksum
  uint8_t arr_1[5];
  uint8_t arr_2[8];
  uint8_t byte = 0;
  uint8_t no_1 = 0;
  uint8_t total_1 = 0;
  uint8_t total_2 = 0;
  int i=0;
  for(i;i<5;i++)
  {
  byte = net.net_packet[i];
  no_1 = countSetBits(byte);
  //put_str("%d",no_1);

  if(no_1%2 == 0)
  {
  arr_1[i] = 0;
  }
  else
  {
  arr_1[i] = 1;
  }
  total_1 = total_1 + pow(arr_1[i],i);
  }
  i= 5;
  for(i;i<13;i++)
  {
  byte = net.net_packet[i];
  no_1 = countSetBits(byte);
  if(no_1%2 == 0)
  {
  arr_2[i] = 0;
  }
  else
  {
  arr_2[i] = 1;
  }
  total_2 = total_2 + pow(arr_2[i],i);
  }
  packet.checksum[0] = total_1;
  packet.checksum[1] = total_2;
}

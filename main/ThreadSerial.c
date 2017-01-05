#include "main.h"

pthread_mutex_t lock_Packet = PTHREAD_MUTEX_INITIALIZER;

uint8_t flag_ser=0;

int checksum(uint8_t *buf,int tam){
	int i=0,sum=0;
	sum=0;
	for(i=3;i<tam-1;i++){
		sum+=buf[i];
		}
	sum&=0xFF;
	sum=0xFF-sum;
	if(sum==buf[tam-1]) return 1;
	return 0;
}

void signal_handler_IO(int status)
{
flag_ser=1;
}
void *ThreadSerial(void *vargp){

	int status=0,ind=0,ind1=0,sum=0,tam=0,i=0,res=0,flag_ini=0,j=0;
	uint8_t bufin[200],bufaux[100],c[2];
	thread_args *args=(thread_args*)vargp;
	TTFila *FilaPacket=&(args->FilaPacket);
	fila_FVazia(FilaPacket);
	fd=serial_init(&signal_handler_IO);

	xbee.flag_myaddr=1;
	xbee_cmdAT(fd,&xbee,(uint8_t*)"SH");

//	printf("Serial init\r\n");
	while(1){
		if(flag_ser){
			//printf("ser\n");
			//fflush(stdout);
#ifdef DEBUG_INIT


			fflush(stdout);
#endif
			res = read(fd,bufin,200);

			for(i=0;i<res;i++){
				switch(status){
					case 0:
						//printf("status=0\n");
						//fflush(stdout);
						if(bufin[i]==0x7e){
							ind=0;
							memset(bufaux,0,sizeof(bufaux));
							bufaux[0]=0x7e;
							status=1;
							}
						break;
					case 1:
						ind++;
						bufaux[ind]=bufin[i];
						if(ind==2) tam=(bufaux[2] | bufaux[1]<<8)+4;

						else if(ind>2 & ind==(tam-1)){
							//printf("teste1 ind=%d\n");
							//fflush(stdout);
							if(checksum(bufaux,tam)){
								//printf("checksum fila=%d\n",FilaPacket->tam);
								//fflush(stdout);
							//	pthread_mutex_lock(&lock_Packet);
								if(bufaux[3]!=0x8b){
									pthread_mutex_lock(&lock_Packet);
									fila_Packet(FilaPacket,bufaux,tam);
									//printf("fila tam=%d type=%02X\n",FilaPacket->tam,bufaux[3]);
									pthread_mutex_unlock(&lock_Packet);
									//fflush(stdout);
#ifdef DEBUG_SER_BUF
									pthread_mutex_lock(&lock_Packet);
									printf("buf= ");
									fflush(stdout);
									for(j=0;j<tam;j++){
										printf("%02X ",bufaux[j]);
										fflush(stdout);
										}
									printf("\n");
									fflush(stdout);

									pthread_mutex_unlock(&lock_Packet);
#endif
									}
							//	pthread_mutex_unlock(&lock_Packet);
								//printf("ser tam=%d \n",FilaPacket->tam);
								status=0;
								}
							else status=0;
							}
						break;
					}
				}
		//	printf("serend\n");
		//	fflush(stdout);
			flag_ser=0;
			}
		usleep(40);
		}
	}

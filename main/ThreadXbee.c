#include "main.h"
extern pthread_mutex_t lock_Packet;
uint8_t xbee_broadcast[]={0,0,0,0,0,0,0xFF,0xFF};

long timeout(struct timeval start){
	long mtime=0, seconds=0, useconds=0;
	struct timeval end;
	gettimeofday(&end, NULL);
	seconds  = end.tv_sec  - start.tv_sec;
	useconds = end.tv_usec - start.tv_usec;
	mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
	return mtime;
}

void discover(void){
	xbee_cmdAT(fd,&xbee,(uint8_t*)"ND");
	xbee.disc.qtd=0;
	gettimeofday(&(xbee.disc.start), NULL);
	//discover.discover_tim_start=TIM2->CNT;
	xbee.disc.flag=1;
}

void *ThreadXbee(void *vargp){
	var_T varaux[5];
	uint8_t var_qtd;
	disp_T disp;
	thread_args *args=(thread_args *)vargp;
	TTFila *FilaPacket=&(args->FilaPacket);
	int i,j;
	TipoFilaDado dado;
	floatbyte_T floataux;
	uint8_t buf[500],buf2[10][200],buf1[100];
	uint8_t addraux[8];
	uint8_t addrstr[20];
	uint8_t payload[50];
	struct timeval start,ping_time;
	//fila_FVazia(&FilaPost);
	//fila_FVazia(&FilaAtu);
	gettimeofday(&start, NULL);
	sleep(2);
	payload[0]=ARCTL_MESTRE;
	xbee.id=0;
	xbee_SendData(fd,&xbee,xbee_broadcast,payload,1);
	discover();
	gettimeofday(&(xbee.disc.start), NULL);
	gettimeofday(&ping_time,NULL);
	while(1){
		if((timeout(ping_time)>5000)){
			db_query("update disp set estado=0 where tipo=1");
			//printf("ping\r\n");
			//fflush(stdout);
			for(i=0;i<db_disp(&disp);i++){
				payload[0]=ARCTL_PING;
				xbee_SendData(fd,&xbee,disp.addr[i],payload,1);
				}
			gettimeofday(&ping_time,NULL);
			}
		//discover
		if((timeout(xbee.disc.start)>150000) & (xbee.disc.flag)){
#ifdef DEBUG_INIT
			printf("xbee discovery\n");
			fflush(stdout);
#endif
			//printf("discover\n");
			//discover();
			gettimeofday(&xbee.disc.start, NULL);
		}
		//disp_atu

		//verifica se existe pacote na fila da serial
		if(FilaPacket->tam>0){

#ifdef DEBUG_INIT
			printf("xbee\n");
			fflush(stdout);
#endif
			//remove da fila e copia para a estrutura do xbee
			pthread_mutex_lock(&lock_Packet);
			fila_remove(&dado,FilaPacket);
			pthread_mutex_unlock(&lock_Packet);

			memcpy(xbee.buf,dado.dado,dado.tam);
			xbee_reciver(&xbee);
			if(xbee.buf[3]!=0x8b & xbee.buf[dado.tam-1]!=0 & ((xbee.buf[2]+4)==dado.tam)){
				//printf("fila=%d\n",FilaPacket->tam);
				//fflush(stdout);
#ifdef DEBUG_XBEE_PACK
				if(xbee.buf[3]!=0x8B){
					printf("rec: tam=%d :",dado.tam);
					fflush(stdout);
					for(i=0;i<xbee.buf[2]+4;i++){
						printf("%02x ",xbee.buf[i]);
						fflush(stdout);
						}
					printf("\n");
					fflush(stdout);
					}
#endif
				switch(xbee.buf[3]){
					case XBEE_CMDATRR:
#ifdef	DEBUG_XBEE_INIT
						printf("XBEE_CMDATRR\n");
						fflush(stdout);
#endif
						//se não tiver erro de transmissão...
						if(xbee.buf[17]!=0x04){
							//recebe comando remoto

							//recebe o rssi de cada modulo
							if((xbee.buf[15]=='D') & (xbee.buf[16]=='B')){
								memset(buf,0,sizeof(buf));
								memset(addraux,0,sizeof(addraux));
								for(i=0;i<8;i++) addraux[i]=xbee.buf[5+i];
								xbee_addrstr(addraux,addrstr);
								if(db_addr(addrstr)>0){
									sprintf((char*)buf,"update disp set estado=1, rssi='-%d', atu=NOW() where addr=\"%s\"",xbee.buf[18],addrstr);
									//printf("update\n");
								}
								else{
									sprintf((char*)buf,"insert into disp values(0,-1,'novo disp','%s',0,'-%d',0)",addrstr,xbee.buf[18]);
									//printf("insert\n");
									//fflush(stdout);
									}
								//printf("sql=\"%s\";",buf);
								if(!db_query(buf)){
									//printf("erro sql=\"%s\";",buf);
									}
								}
							}
						break;
					case XBEE_CMDAT:
#ifdef	DEBUG_XBEE_INIT
						printf("XBEE_CMDAT\n");
						fflush(stdout);
#endif
						//recebe comando local

						//recebe o MAC dos dispositivo pelo discover
						if((xbee.buf[5]=='N') & (xbee.buf[6]=='D')){
							for(i=0;i<8;i++){
								xbee.disc.addr[xbee.disc.qtd][i]=xbee.buf[10+i];
								}
							usleep(10000);
							//printf("discover r\n");
							//fflush(stdout);
							//pede o rssi do dispositivo
							xbee_cmdATR(fd,&xbee,(uint8_t*)"DB",(uint8_t *)xbee.disc.addr[xbee.disc.qtd]);
							//incrementa a quantidade de dispositivos
							xbee.disc.qtd++;
						}
						break;
					//recebe um pacote de dados
					case XBEE_RECEIVE_PACKET:
#ifdef	DEBUG_XBEE_INIT
						printf("XBEE_REC\n");
						fflush(stdout);
#endif
						/*printf("rec: ");
						fflush(stdout);
						for(i=0;i<xbee.buf[2]+4;i++){
							printf("%02x ",xbee.buf[i]);
							fflush(stdout);
							}
						printf("\n");
						fflush(stdout);*/
						switch(xbee.buf[XBEE_PAYLOAD_OFFSET]){
							case ARCTL_MESTRE:
#ifdef DEBUG_ARCTL_INIT
								printf("ARCTL_MESTRE\n");
								fflush(stdout);
#endif
								payload[0]=ARCTL_INIT;
								xbee_SendData(fd,&xbee,xbee.source_Address,payload,1);
							case ARCTL_INIT:
#ifdef DEBUG_ARCTL_INIT
								printf("ARCTL_INIT\n");
								fflush(stdout);
#endif
								xbee_addrstr(xbee.source_Address,buf1);
								//verifica se existe dispositico com o endere��o
								switch(db_id(buf1,xbee.buf[XBEE_PAYLOAD_OFFSET+1])){
									case -1:
										sprintf((char*)buf,"update disp set disp=%d, atu=NOW() where addr=\"%s\"",xbee.buf[XBEE_PAYLOAD_OFFSET+1],buf1);
										 //printf("'%s' id_id -1\n",buf);
										//fflush(stdout);
										db_query(buf);
										break;
									case 0:
										break;
									case 1:
										sprintf((char*)buf,"insert into disp values(0,0,'Novo Disp','%s',1,0,NOW(),1)",buf1);
										printf("sql:%s \n",buf);
										db_query(buf);
										db_query("insert into config(disp,tempo,modo) values((select Id from disp order by Id desc limit 1),99,1)");
									}

								payload[0]=ARCTL_INIT;
								xbee_SendData(fd,&xbee,xbee.source_Address,payload,1);
								break;
							case ARCTL_TEMP:
#ifdef DEBUG_ARCTL_INIT
								printf("ARCTL_TEMP\n");
								fflush(stdout);
#endif
								payload[0]=ARCTL_TEMP;
								xbee_addrstr(xbee.source_Address,buf1);
								xbee_SendData(fd,&xbee,xbee.source_Address,payload,1);
								uint8_t id_disp=db_idaddr(buf1);
								var_qtd=xbee.buf[XBEE_PAYLOAD_OFFSET+2];
								printf("insert %d qtd=%d\n",id_disp,var_qtd);
								fflush(stdout);
								memset((char*)buf,0,sizeof(buf));
								sprintf((char*)buf,"insert into dados values");
								for(i=0;i<var_qtd;i++){
									varaux[i].type=xbee.buf[XBEE_PAYLOAD_OFFSET+i*5+3];
									//if(varaux[i].type==0) break;
									for(j=0;j<4;j++)
										varaux[i].var.byte.b[j]=xbee.buf[XBEE_PAYLOAD_OFFSET+i*5+4+j];
									if(i==0)
										sprintf((char*)buf,"%s(0,%d,%d,%2.2f,NOW())",buf,id_disp,varaux[i].type,varaux[i].var.val);
									else
										sprintf((char*)buf,"%s,(0,%d,%d,%2.2f,NOW())",buf,id_disp,varaux[i].type,varaux[i].var.val);
									}
								//printf("sql='%s'\r\n",buf);
								if( var_qtd>0) db_query(buf);
								//printf("teste\r\n");
								break;
							case ARCTL_PING:
#ifdef DEBUG_ARCTL_INIT
								printf("ARCTL_PING\n");
								fflush(stdout);
#endif
								xbee_cmdATR(fd,&xbee,(uint8_t*)"DB",(uint8_t *)xbee.source_Address);

								xbee_addrstr(xbee.source_Address,buf1);
								if(db_atu(buf1,payload,&j)){
									xbee_SendData(fd,&xbee,xbee.source_Address,payload,j);
								}
								break;
							case ARCTL_PING1:
#ifdef DEBUG_ARCTL_INIT
								printf("ARCTL_PING1\n");
								fflush(stdout);
#endif
								payload[0]=ARCTL_PING1;
								xbee_SendData(fd,&xbee,xbee.source_Address,payload,1);
							break;
							}
						break;
					}
#ifdef DEBUG_INIT
				printf("xbeeend\n");
				fflush(stdout);
#endif
				}
			}
		usleep(10000);
		}
	}

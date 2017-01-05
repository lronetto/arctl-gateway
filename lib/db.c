#include "main.h"
#include <mysql/mysql.h>

int db_connet(MYSQL *conexao){

	mysql_init(conexao);
	if ( !mysql_real_connect(conexao, "localhost", "root", "822528", "arctl", 0, NULL, 0) ){
		printf("Falha de conexao\n");
		printf("Erro %d : %s\n", mysql_errno(conexao), mysql_error(conexao));
		mysql_close(conexao);
		return 0;
		}
	return 1;
}
int db_qtd(char *sql){
	MYSQL_RES *resp;
	MYSQL conexao;
	MYSQL_ROW linhas;
	int t;
	fflush(stdout);
	if(db_connet(&conexao)){
		if(mysql_query(&conexao,sql)){
				printf("qd_qtd mysql_query(): Error %u: %s\n", mysql_errno(&conexao), mysql_error(&conexao));
				mysql_close(&conexao);
				return -1;
			}
		else{
			resp = mysql_store_result(&conexao);
			if(resp){
				t=0;
				while ((linhas=mysql_fetch_row(resp)) != NULL) t++;
				mysql_free_result(resp);
				mysql_close(&conexao);
				return t;
				}
			}

		}
	return -1;
	}
int db_addr(uint8_t *addr){
	char sql[50];
	sprintf(sql,"select * from disp where addr=\"%s\"",addr);
	//printf("sql='%s'\n",sql);
	//fflush(stdout);
	return db_qtd(sql);
	}

int db_query(char *sql){
	MYSQL conexao;
	if(db_connet(&conexao)){
		if(mysql_query(&conexao,sql)){
			printf("db_query mysql_query(): Error %u: %s\n", mysql_errno(&conexao), mysql_error(&conexao));
			mysql_close(&conexao);
			return 0;
			}
		mysql_close(&conexao);
		}
	return 1;
	}
//verifica se existe o dispositivo e se o id esta correto
int db_id(uint8_t *addr,uint8_t id){
#ifdef DEBUG_INIT
	printf("id_id\n");
	fflush(stdout);
#endif
	char sql[100];
	if(db_addr(addr)>0){
		sprintf(sql,"select * from disp where addr=\"%s\" and disp=-1",addr);
		if(db_qtd(sql)>0) return -1;
		else{
			sprintf(sql,"select * from disp where addr=\"%s\" and disp=%d",addr,id);
			if(db_qtd(sql)==0) return 0;
			}
		}
	return 1;
}
int db_idaddr(uint8_t *addr){
	MYSQL_RES *resp;
	MYSQL conexao;
	MYSQL_ROW linhas;
	int t=0;
	char sql[100];
	if(db_connet(&conexao)){
		sprintf(sql,"select Id,addr from disp where addr='%s'",addr);
		if(mysql_query(&conexao,sql)){
			printf("db_idaddr mysql_query(): Error %u: %s\n", mysql_errno(&conexao), mysql_error(&conexao));
			mysql_close(&conexao);
			return 0;
			}
		else{
			resp = mysql_store_result(&conexao);

			if(resp){
				linhas=mysql_fetch_row(resp);
				t=atoi(linhas[0]);
				mysql_free_result(resp);
				}

			}
			mysql_close(&conexao);
		}
	return t;
}
int db_disp(disp_T *disp){
	MYSQL_RES *resp;
	MYSQL conexao;
	MYSQL_ROW linhas;
	int t=0;
	if(db_connet(&conexao)){
		if(mysql_query(&conexao,"select Id,addr from disp where tipo=1")){
			printf("db_disp mysql_query(): Error %u: %s\n", mysql_errno(&conexao), mysql_error(&conexao));
			mysql_close(&conexao);
			return 0;
			}
		else{
			resp = mysql_store_result(&conexao);
			if(resp){
				t=0;
				while ((linhas=mysql_fetch_row(resp)) != NULL){
					disp->id[t]=atoi(linhas[0]);
					//printf("disp: %s\n",linhas[1]);
					//fflush(stdout);
					sscanf(linhas[1],"%02X%02X%02X%02X%02X%02X%02X%02X",
										&(disp->addr[t][0]),
										&disp->addr[t][1],
										&disp->addr[t][2],
										&disp->addr[t][3],
										&disp->addr[t][4],
										&disp->addr[t][5],
										&disp->addr[t][6],
										&disp->addr[t][7]);
					t++;

				}
			disp->qtd=t;
			mysql_free_result(resp);


			}
			mysql_close(&conexao);
		}
	}
	return t;
}
int db_atu(uint8_t *addr,uint8_t *payload,uint8_t *qtd){
	MYSQL_RES *resp;
	MYSQL conexao;
	MYSQL_ROW linhas;
	floatbyte_T floataux;
	uint16byte_T uint16aux;
	int id=0;
	char sql[250];
	int out=0,i;
	uint8_t aux;
	id=db_idaddr(addr);
	sprintf(sql,"select * from disp_atu where disp=%d",id);
	if(db_qtd(sql)>0){
		//
		if(db_connet(&conexao)){
			sprintf(sql,"select tempo,hist,setpoint,modo,(select conf from disp_atu where disp=%d limit 1) as conf, pidp, pidi, pidd from config where disp=%d",id,id);
			if(mysql_query(&conexao,sql)){
				printf("db_atu mysql_query(): Error %u: %s\n", mysql_errno(&conexao), mysql_error(&conexao));
				mysql_close(&conexao);
				return 0;
				}
			else{
				resp = mysql_store_result(&conexao);
				if(resp){
					linhas=mysql_fetch_row(resp);
					printf("payload\n");
					fflush(stdout);
					payload[0]=ARCTL_SET_TEMP;

					//conf
					payload[1]=atoi(linhas[4]);
					//modo
					payload[2]=atoi(linhas[3]);
					//setpoint
					floataux.val=atof(linhas[2]);
					printf("set: %2.2f ",floataux.val);
					for(i=0;i<4;i++) payload[3+i]=floataux.byte.b[i];
					//histereze
					floataux.val=atof(linhas[1]);
					printf("hist: %2.2f ",floataux.val);
					for(i=0;i<4;i++) payload[7+i]=floataux.byte.b[i];
					//p
					floataux.val=atof(linhas[5]);
					printf("P: %2.2f ",floataux.val);
					for(i=0;i<4;i++) payload[11+i]=floataux.byte.b[i];
					//i
					floataux.val=atof(linhas[6]);
					printf("I: %2.2f ",floataux.val);
					for(i=0;i<4;i++) payload[15+i]=floataux.byte.b[i];
					//d
					floataux.val=atof(linhas[7]);
					printf("D: %2.2f ",floataux.val);
					for(i=0;i<4;i++) payload[19+i]=floataux.byte.b[i];
					//tempo
					uint16aux.val=atoi(linhas[0]);
					printf("tempo: %d\n",(int)uint16aux.val);
					fflush(stdout);
					payload[23]=uint16aux.byte.b[0];
					payload[24]=uint16aux.byte.b[1];
					*qtd=25;
					out=1;
					mysql_free_result(resp);
					}
				mysql_close(&conexao);
				}
			}
		sprintf(sql,"delete from disp_atu where disp=%d",id);
		db_query(sql);
	}
	return out;
}


#include "main.h"

typedef struct{
	int qtd;
	uint8_t addr[30][8];
	uint8_t id[30];
}disp_T;
int db_addr(uint8_t *addr);
int db_query(char *sql);
int db_id(uint8_t *addr,uint8_t id);

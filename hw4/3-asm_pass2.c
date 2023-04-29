/***********************************************************************/
/*  Program Name: 3-asm_pass1_u.c                                      */
/*  This program is the part of SIC/XE assembler Pass 1.	  		   */
/*  The program only identify the symbol, opcode and operand 		   */
/*  of a line of the asm file. The program do not build the            */
/*  SYMTAB.			                                               	   */
/*  2019.12.13                                                         */
/*  2021.03.26 Process error: format 1 & 2 instruction use + 		   */
/***********************************************************************/
#include <string.h>
#include "2-optable.c"
#include <stdlib.h>
#include <string.h>

/* Public variables and functions */
#define	ADDR_SIMPLE			0x01
#define	ADDR_IMMEDIATE		0x02
#define	ADDR_INDIRECT		0x04
#define	ADDR_INDEX			0x08

#define	LINE_EOF			(-1)
#define	LINE_COMMENT		(-2)
#define	LINE_ERROR			(0)
#define	LINE_CORRECT		(1)

typedef struct
{
	char		symbol[LEN_SYMBOL];
	char		op[LEN_SYMBOL];
	char		operand1[LEN_SYMBOL];
	char		operand2[LEN_SYMBOL];
	unsigned	code;
	unsigned	fmt;
	unsigned	addressing;
} LINE;

typedef struct
{
	char		label[LEN_SYMBOL];
	unsigned	locctr;
} SYMBOL;

typedef struct
{
	char		op[LEN_SYMBOL];
	char        bin[8];
} HtoB;

int process_line(LINE *line);
/* return LINE_EOF, LINE_COMMENT, LINE_ERROR, LINE_CORRECT and Instruction information in *line*/

/* Private variable and function */

void init_LINE(LINE *line)
{
	line->symbol[0] = '\0';
	line->op[0] = '\0';
	line->operand1[0] = '\0';
	line->operand2[0] = '\0';
	line->code = 0x0;
	line->fmt = 0x0;
	line->addressing = ADDR_SIMPLE;
	
}

int process_line(LINE *line)
/* return LINE_EOF, LINE_COMMENT, LINE_ERROR, LINE_CORRECT */
{
	char		buf[LEN_SYMBOL];
	int			c;
	int			state;
	int			ret;
	Instruction	*op;
	
	c = ASM_token(buf);		/* get the first token of a line */
	if(c == EOF)
		return LINE_EOF;
	else if((c == 1) && (buf[0] == '\n'))	/* blank line */
		return LINE_COMMENT;
	else if((c == 1) && (buf[0] == '.'))	/* a comment line */
	{
		do
		{
			c = ASM_token(buf);
		} while((c != EOF) && (buf[0] != '\n'));
		return LINE_COMMENT;
	}
	else
	{
		init_LINE(line);
		ret = LINE_ERROR;
		state = 0;
		while(state < 8)
		{
			switch(state)
			{
				case 0:
				case 1:
				case 2:
					op = is_opcode(buf);
					if((state < 2) && (buf[0] == '+'))	/* + */
					{
						line->fmt = FMT4;
						state = 2;
					}
					else	if(op != NULL)	/* INSTRUCTION */
					{
						strcpy(line->op, op->op);
						line->code = op->code;
						state = 3;
						if(line->fmt != FMT4)
						{
							line->fmt = op->fmt & (FMT1 | FMT2 | FMT3);
						}
						else if((line->fmt == FMT4) && ((op->fmt & FMT4) == 0)) /* INSTRUCTION is FMT1 or FMT 2*/
						{	/* ERROR 20210326 added */
							printf("ERROR at token %s, %s cannot use format 4 \n", buf, buf);
							ret = LINE_ERROR;
							state = 7;		/* skip following tokens in the line */
						}
					}				
					else	if(state == 0)	/* SYMBOL */
					{
						strcpy(line->symbol, buf);
						state = 1;
					}
					else		/* ERROR */
					{
						printf("ERROR at token %s\n", buf);
						ret = LINE_ERROR;
						state = 7;		/* skip following tokens in the line */
					}
					break;	
				case 3:
					if(line->fmt == FMT1 || line->code == 0x4C)	/* no operand needed */
					{
						if(c == EOF || buf[0] == '\n')
						{
							ret = LINE_CORRECT;
							state = 8;
						}
						else		/* COMMENT */
						{
							ret = LINE_CORRECT;
							state = 7;
						}
					}
					else
					{
						if(c == EOF || buf[0] == '\n')
						{
							ret = LINE_ERROR;
							state = 8;
						}
						else	if(buf[0] == '@' || buf[0] == '#')
						{
							line->addressing = (buf[0] == '#') ? ADDR_IMMEDIATE : ADDR_INDIRECT;
							state = 4;
						}
						else	/* get a symbol */
						{
							op = is_opcode(buf);
							if(op != NULL)
							{
								printf("Operand1 cannot be a reserved word\n");
								ret = LINE_ERROR;
								state = 7; 		/* skip following tokens in the line */
							}
							else
							{
								strcpy(line->operand1, buf);
								state = 5;
							}
						}
					}			
					break;		
				case 4:
					op = is_opcode(buf);
					if(op != NULL)
					{
						printf("Operand1 cannot be a reserved word\n");
						ret = LINE_ERROR;
						state = 7;		/* skip following tokens in the line */
					}
					else
					{
						strcpy(line->operand1, buf);
						state = 5;
					}
					break;
				case 5:
					if(c == EOF || buf[0] == '\n')
					{
						ret = LINE_CORRECT;
						state = 8;
					}
					else if(buf[0] == ',')
					{
						state = 6;
					}
					else	/* COMMENT */
					{
						ret = LINE_CORRECT;
						state = 7;		/* skip following tokens in the line */
					}
					break;
				case 6:
					if(c == EOF || buf[0] == '\n')
					{
						ret = LINE_ERROR;
						state = 8;
					}
					else	/* get a symbol */
					{
						op = is_opcode(buf);
						if(op != NULL)
						{
							printf("Operand2 cannot be a reserved word\n");
							ret = LINE_ERROR;
							state = 7;		/* skip following tokens in the line */
						}
						else
						{
							if(line->fmt == FMT2)
							{
								strcpy(line->operand2, buf);
								ret = LINE_CORRECT;
								state = 7;
							}
							else if((c == 1) && (buf[0] == 'x' || buf[0] == 'X'))
							{
								line->addressing = line->addressing | ADDR_INDEX;
                                // printf("ADDR_INDEX");
								ret = LINE_CORRECT;
								state = 7;		/* skip following tokens in the line */
							}
							else
							{
								printf("Operand2 exists only if format 2  is used\n");
								ret = LINE_ERROR;
								state = 7;		/* skip following tokens in the line */
							}
						}
					}
					break;
				case 7:	/* skip tokens until '\n' || EOF */
					if(c == EOF || buf[0] =='\n')
						state = 8;
					break;										
			}
			if(state < 8)
				c = ASM_token(buf);  /* get the next token */
		}
		return ret;
	}
}

int LOCCTR(LINE line,int locctr, int c){
	
	if(c != LINE_COMMENT){
		if (line.code == OP_WORD){
			locctr += 3;
		}else if (line.code == OP_RESW){
			locctr += 3 * atoi(line.operand1);
		}else if (line.code == OP_RESB){
			locctr += atoi(line.operand1);
		}else if (line.code == OP_BYTE){
			if(line.operand1[0] == 'C'){
				locctr += strlen(line.operand1) - 3;
			}else{
				locctr += (strlen(line.operand1) - 3) / 2;
				// printf("%d111111111111111111",(strlen(line.operand1) - 3) / 2);
			}
		}else if (line.fmt == FMT2){
			locctr += 2;
		}else if (line.fmt == FMT3){
			locctr += 3;
		}else if (line.fmt == FMT4){
			locctr += 4;
		}else if (line.fmt == FMT0){
			locctr += 0;
		}else{
			locctr += 3;
		}
			
	}
	return locctr;
}

char b = '0';
unsigned b_add;
unsigned locctr_store[1000];
int symtab_index = 0;
SYMBOL	SYMTAB[100];
LINE line_store[1000];
int line_store_idx = 1;



void objectcode(LINE line,int line_idx){
	
    char ojc[9];
    unsigned tempcode = line.code;
	char set[] = "0123456789ABCDEF";
	if(line.code == 0x68){
		if(line.addressing == ADDR_IMMEDIATE){
			for(int i = 0 ; i < symtab_index; i++){
				if(strcmp(SYMTAB[i].label, line.operand1) == 0){
					b_add = SYMTAB[i].locctr;
					break;
				}
			}
			// ojc[0] = set[line.code / 16];
			// int ojc_idx_1 = 1;
			// int ojc_idx_2 = 2;

			// unsigned pc = locctr_store[line_idx + 1];
			// unsigned disp = b_add - pc;
			// unsigned temp = disp;
			// ojc[5] = set[temp % 16];
			// temp /= 16;
			// ojc[4] = set[temp % 16];
			// temp /= 16;
			// ojc[3] = set[temp % 16];
			// ojc[6] = '0';
			// printf("%s   %d\n",ojc, line_idx);
			
		}
			
	}
	if(line.code == OP_RESB || line.code == OP_RESW  || line.code == OP_NOBASE 
	|| line.code == OP_START || line.code == OP_END){
        ojc[0] = '\0';
		
    }
    else if(line.code == OP_WORD || line.code == OP_BYTE){
        ojc[0] = '\0';
		if(line.operand1[0] =='C'){
			int temp = line.operand1[2];
			ojc[0] = set[temp / 16];
			ojc[1] = set[temp % 16];
			
			temp = line.operand1[3];
			ojc[2] = set[temp / 16];
			ojc[3] = set[temp % 16];

			temp = line.operand1[4];
			ojc[4] = set[temp / 16];
			ojc[5] = set[temp % 16];
			ojc[6] = '\0';
			printf("%s   %d\n",ojc, line_idx);
		}else{
			ojc[0] = line.operand1[2];
			ojc[1] = line.operand1[3];
			ojc[2] = '\0';
			printf("%s       %d\n",ojc, line_idx);
		}
		
    }else if(line.fmt == FMT2){
		ojc[4] = '\0';
        ojc[0] = set[line.code / 16];
		ojc[1] = set[line.code % 16];


		char res;
		if(strcmp(line.operand1, "A") == 0){
			res = '0';
		}else if(strcmp(line.operand1, "X") == 0){
			res = '1';
		}else if(strcmp(line.operand1, "L") == 0){
			res = '2';
		}else if(strcmp(line.operand1, "PC") == 0){
			res = '8';
		}else if(strcmp(line.operand1, "SW") == 0){
			res = '9';
		}else if(strcmp(line.operand1, "B") == 0){
			res = '3';
		}else if(strcmp(line.operand1, "S") == 0){
			res = '4';
		}else if(strcmp(line.operand1, "T") == 0){
			res = '5';
		}else if(strcmp(line.operand1, "F") == 0){
			res = '6';
		}else
			res = '0';
		ojc[2] = res;


		if(strcmp(line.operand2, "A") == 0){
			res = '0';
		}else if(strcmp(line.operand2, "X") == 0){
			res = '1';
		}else if(strcmp(line.operand2, "L") == 0){
			res = '2';
		}else if(strcmp(line.operand2, "PC") == 0){
			res = '8';
		}else if(strcmp(line.operand2, "SW") == 0){
			res = '9';
		}else if(strcmp(line.operand2, "B") == 0){
			res = '3';
		}else if(strcmp(line.operand2, "S") == 0){
			res = '4';
		}else if(strcmp(line.operand2, "T") == 0){
			res = '5';
		}else if(strcmp(line.operand2, "F") == 0){
			res = '6';
		}else
			res = '0';
		ojc[3] = res;
		printf("%s     %d\n",ojc,line_idx);
	}else{
		
		int ojc_idx_1 = 0;
		int ojc_idx_2 = 0;
		ojc[6] = '\0';

		ojc[0] = set[line.code / 16];
		
		ojc_idx_1 = line.code % 16;

		if(line.addressing == ADDR_SIMPLE){
			ojc_idx_1 += 3;
			ojc_idx_2 += 2;
			// ojc[6] = '1';
            // ojc[7] = '1';
           // ojc[8] = '0';
			// ojc[9] = '0';
			// ojc[10] = '1';
		}else if(line.addressing == ADDR_IMMEDIATE){
			ojc_idx_1 += 1;
			ojc_idx_2 += 0;
            // ojc[6] = '0';
            // ojc[7] = '1';
            // ojc[8] = '0';
			// ojc[9] = '0';
			// ojc[10] = '1';
        }else if(line.addressing == ADDR_INDIRECT){
			ojc_idx_1 += 2;
			ojc_idx_2 += 2;
            // ojc[6] = '1';
            // ojc[7] = '0';
            // ojc[8] = '0';
			// ojc[9] = '0';
			// ojc[10] = '1';
        }else if(line.addressing >= ADDR_INDEX){
			ojc_idx_1 += 3;
			ojc_idx_2 += 10;
            // ojc[6] = '1';
            // ojc[7] = '1';
            // ojc[8] = '1';
			// ojc[9] = '0';
			// ojc[10] = '1';
        }else{
			ojc_idx_1 += 0;
			ojc_idx_2 += 2;
            // ojc[6] = '0';
            // ojc[7] = '0';
            // ojc[8] = '0';
			// ojc[9] = '0';
			// ojc[10] = '1';
        }
		if(line.fmt == FMT4){
			ojc_idx_2 += 1;
		}//n i x b p e
		
		if(line.fmt == FMT4){
			if(line.addressing == ADDR_SIMPLE){
				unsigned target = 0;
				unsigned temp;
				for(int i = 0 ; i < symtab_index; i++){
					if(strcmp(SYMTAB[i].label, line.operand1) == 0){
						target = SYMTAB[i].locctr;
						break;
					}
				}
				temp = target;
				ojc[7] = set[temp % 16];
				temp /= 16;
				ojc[6] = set[temp % 16];
				temp /= 16;
				ojc[5] = set[temp % 16];
				temp /= 16;
				ojc[4] = set[temp % 16];
				temp /= 16;
				ojc[3] = set[temp % 16];

				ojc[1] = set[ojc_idx_1];

				ojc_idx_2 -= 2;
				ojc[2] = set[ojc_idx_2];
				ojc[8] = '\0';
				printf("%s %d \n",ojc, line_idx);
			}else if(line.addressing == ADDR_IMMEDIATE){
				unsigned target = strtol(line.operand1,NULL,10);
				unsigned temp = target;

				ojc[7] = set[temp % 16];
				temp /= 16;
				ojc[6] = set[temp % 16];
				temp /= 16;
				ojc[5] = set[temp % 16];
				temp /= 16;
				ojc[4] = set[temp % 16];
				temp /= 16;
				ojc[3] = set[temp % 16];

				// ojc_idx_2 -= 2;
				ojc[2] = set[ojc_idx_2];
				ojc[1] = set[ojc_idx_1];
				printf("%s %d \n",ojc, line_idx);
			}
			
		}else if(line.addressing == ADDR_SIMPLE && line.fmt == FMT3){
			
			unsigned pc = locctr_store[line_idx + 1];
			unsigned target = 0;
			for(int i = 0 ; i < symtab_index; i++){
				if(strcmp(SYMTAB[i].label, line.operand1) == 0){
					target = SYMTAB[i].locctr;
					break;
				}
			}
			unsigned disp = target - pc;
			unsigned temp = disp;
			unsigned b_disp = target - b_add;
			// printf("%d %d\n",disp,line_idx);
			if (line.code == 0x4C){
				ojc_idx_1 = line.code % 16 + 3;
				temp = 0;
				ojc_idx_2 = 0;
			}else if(-2048 >= disp && disp >= 2047){
				ojc_idx_2 += 2;
				temp = b_disp;
			} 

			ojc[1] = set[ojc_idx_1];
			ojc[2] = set[ojc_idx_2];
			ojc[5] = set[temp % 16];
			temp /= 16;
			ojc[4] = set[temp % 16];
			temp /= 16;
			ojc[3] = set[temp % 16];

			printf("%s   %d\n",ojc, line_idx);
			// printf("%s %x %x %x %d %d\n",ojc,target,pc,target - pc,disp,line_idx);
		}else if(line.addressing == ADDR_IMMEDIATE){
			if(line.code == 0x68){
				unsigned target;
				ojc_idx_2 = 2;
				unsigned pc = locctr_store[line_idx + 2];
				unsigned disp = b_add - pc;
				unsigned temp = disp;

				ojc[5] = set[temp % 16];
				temp /= 16;
				ojc[4] = set[temp % 16];
				temp /= 16;
				ojc[3] = set[temp % 16];
				ojc[6] = '\0';
				ojc[1] = set[ojc_idx_1];
				ojc[2] = set[ojc_idx_2];
				printf("%s   %d\n",ojc, line_idx);
			}else{
				ojc[1] = set[ojc_idx_1];
				ojc[2] = set[ojc_idx_2];
				unsigned temp = strtol(line.operand1,NULL,10);
				ojc[5] = set[temp % 16];
				temp /= 16;
				ojc[4] = set[temp % 16];
				temp /= 16;
				ojc[3] = set[temp % 16];
				printf("%s   %d\n",ojc,line_idx);
			}	
		}else if(line.addressing == ADDR_INDIRECT){
			unsigned pc = locctr_store[line_idx + 1];
			unsigned target = 0;
			for(int i = 0 ; i < symtab_index; i++){
				if(strcmp(SYMTAB[i].label, line.operand1) == 0){
					target = SYMTAB[i].locctr;
					break;
				}
			}
			unsigned disp = target - pc;
			unsigned temp = disp;
			unsigned b_disp = target - b_add;
			// printf("%d %d\n",disp,line_idx);
			if (line.code == 0x4C){
				ojc_idx_1 = line.code % 16 + 3;
				temp = 0;
				ojc_idx_2 = 0;
			}else if(-2048 >= disp && disp >= 2047){
				ojc_idx_2 += 2;
				temp = b_disp;
			} 

			ojc[1] = set[ojc_idx_1];
			ojc[2] = set[ojc_idx_2];
			ojc[5] = set[temp % 16];
			temp /= 16;
			ojc[4] = set[temp % 16];
			temp /= 16;
			ojc[3] = set[temp % 16];

			printf("%s   %d\n",ojc, line_idx);
		}else if(line.addressing >= ADDR_INDEX){
			unsigned pc = locctr_store[line_idx + 1];
			unsigned target = 0;
			for(int i = 0 ; i < symtab_index; i++){
				if(strcmp(SYMTAB[i].label, line.operand1) == 0){
					target = SYMTAB[i].locctr;
					break;
				}
			}
			unsigned disp = target - pc;
			unsigned temp = disp;
			unsigned b_disp = target - b_add;
			if(-2048 >= disp && disp >= 2047){
				ojc_idx_2 += 2;
				temp = b_disp;
			}
			ojc[1] = set[ojc_idx_1];
			ojc[2] = set[ojc_idx_2];
			ojc[5] = set[temp % 16];
			temp /= 16;
			ojc[4] = set[temp % 16];
			temp /= 16;
			ojc[3] = set[temp % 16];
			printf("%s   %d\n",ojc,line_idx);
		}
			
		
		
		
    }
}



int main(int argc, char *argv[])
{
	int			i, c, line_count;
	char		buf[LEN_SYMBOL];
	LINE		line;
	LINE 		last_line;
	int locctr = 0;
	unsigned start;
	unsigned end;
	int locctr_store_len;

	if(argc < 2)
	{
		printf("Usage: %s fname.asm\n", argv[0]);
	}
	else
	{
		if(ASM_open(argv[1]) == NULL)
			printf("File not found!!\n");
		else
		{	unsigned check_start;
			for(line_count = 1 ; (c = process_line(&line)) != LINE_EOF; line_count++)
			{	
                
				if(line.code == OP_START){
					locctr = strtol(line.operand1, NULL, 16);
				}
				if(check_start != OP_START)
					locctr = LOCCTR(last_line, locctr,c);

				locctr_store[line_count] = locctr;
				// printf("%06X      ",locctr);
				// printf("\n");

				// objectcode(line, line_count);
				// if(line.fmt == FMT4){
				// 	char temp[20];
				// 	strcpy(temp, "+");
				// 	strcat(temp,line.op);
				// 	strcpy(line.op, temp);
				// }

				// if(line.addressing == ADDR_IMMEDIATE){
				// 	char temp[20];
				// 	strcpy(temp, "#");
				// 	strcat(temp,line.operand1);
				// 	strcpy(line.operand1, temp);
				// }

				// if(line.addressing == ADDR_INDIRECT){
				// 	char temp[20];
				// 	strcpy(temp, "@");
				// 	strcat(temp,line.operand1);
				// 	strcpy(line.operand1, temp);
				// }

				// if(c == LINE_ERROR)
				// 	printf("%03d : 	 Error\n", line_count);
				// else if(c == LINE_COMMENT)
				// 	printf("%-03d :  Comment line\n", line_count);
				// else if(line.code == OP_END)
				// 	printf("%-03d :         %-12s %-12s %-12s %X %d\n", line_count, line.symbol, line.op, line.operand1,line.code,line.code);
				// else if (strlen(line.operand2) == 0)
				// 	printf("%-03d :  %06X %-12s %-12s %-12s %X %d\n", line_count,locctr, line.symbol, line.op, line.operand1,line.code,line.code);
				// else{
				// 	strcat(line.operand1,",");
				// 	strcat(line.operand1,line.operand2);
				// 	printf("%-03d :  %06X %-12s %-12s %-12s %X %d\n", line_count,locctr, line.symbol, line.op, line.operand1,line.code,line.code);
				// }
				
					// line_store[line_count].addressing = line.addressing;
					// line_store[line_count].code = line.code;
					// line_store[line_count].fmt = line.fmt;
					// strcpy(line_store[line_count].op, line.op);
					// strcpy(line_store[line_count].operand1, line.operand1);
					// strcpy(line_store[line_count].operand2, line.operand2);
					// strcpy(line_store[line_count].symbol, line.symbol);
					// line_store_idx++;

				if(line.code == OP_START){
					start = locctr;
				}else if(line.code == OP_END){
					end = locctr;
				}
				if(strlen(line.symbol) > 1 && c == LINE_CORRECT){
					strcpy(SYMTAB[symtab_index].label, line.symbol);
					SYMTAB[symtab_index].locctr = locctr;
					symtab_index++;
				}
				check_start = line.code;
				last_line = line;

				locctr_store_len = line_count;
				// printf("%d\n",line_count);
				// printf(" %10s %02x %03d\n",line.op,line.fmt ,line_count);
			}
			// printf("%s %d\n", line_store[2].op,line_store_idx);
			// printf(".\n");
			// printf(".\n");
			// printf("Program length = %06x\n",end - start);
			// for(int i = 0 ; i < symtab_index; i++){
			// 	printf("%12s %06X\n",SYMTAB[i].label,SYMTAB[i].locctr);
			// }

			// for(int i = 1; i <= locctr_store_len; i++){
			// 	printf("%06X\n",locctr_store[i]);
			// }
			// for(int line_count = 1; line_count < line_store_idx; line_count++)
			// {	
			// 	// printf("%d\n",line_count);
			// 	objectcode(line_store[line_count], line_count);
			// 	// printf("pass2 %d\n",line_count);
			// 	// printf("%-03d :  %06X %-12s %-12s %-12s %X\n",line_count,locctr_store[line_count],
			// 	// line_store[line_count].op,line_store[line_count].operand1 ,line_store[line_count].operand2
			// 	// ,line_store[line_count].addressing);
			// }
			ASM_open(argv[1]);
			for(line_count = 1 ; (c = process_line(&line)) != LINE_EOF; line_count++){
				if(c == LINE_CORRECT)
					objectcode(line, line_count);
			}
			ASM_close();
			
		}
	}
}

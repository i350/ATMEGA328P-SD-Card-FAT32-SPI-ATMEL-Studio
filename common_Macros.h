#define SET_BIT(REG,BIT_NO) {REG|=(1<<BIT_NO);}
#define CLR_BIT(REG,BIT_NO) {REG&=~(1<<BIT_NO);}
#define BIT_IS_SET(REG,BIT_NO) {(1&(REG>>BIT_NO));}
#define BIT_IS_CLR(REG,BIT_NO) {!(1&(REG>>BIT_NO));}
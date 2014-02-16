#pragma  once 


//
//
//词根还原算法
//该算法会把过去式形式的词,名字形式的词还原为原型
//比如:"walked"经过该算法处理之后结果为walk
//"loyalty"还原后为loyal
//
void stemword( char* word,int len);
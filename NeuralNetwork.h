#ifndef NEURAL_NETWORK_H
#define NEURAL_NETWORK_H

#define MaxNode 100000

unsigned char NetGetAlloc(void);
unsigned char NetGetStats(void);
unsigned char NetGetLearn(void);

unsigned char NetInput(unsigned char Input);
unsigned char NetTarget(unsigned char Input);
unsigned char NetAlloc(unsigned int Alloc);
unsigned char NetRandomizeData(void);
unsigned char NetTrigg(void);

#endif

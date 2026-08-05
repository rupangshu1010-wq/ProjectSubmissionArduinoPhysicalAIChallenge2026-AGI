#include "NeuralNetwork.h"

#ifndef NULL
#define NULL ((void *)0)
#endif

static unsigned short memory_pool[102400];
static unsigned short *baseNode = NULL;
static unsigned int maxNode = 0;
static unsigned char Target = 0;

static unsigned char ReadNetAlloc = 0;
static unsigned char ReadNetStats = 0;
static unsigned char ReadNetLearn = 0;

unsigned char NetGetAlloc(void) { return ReadNetAlloc; }
unsigned char NetGetStats(void) { return ReadNetStats; }
unsigned char NetGetLearn(void) { return ReadNetLearn; }

unsigned char NetInput(unsigned char Input) {
    if (baseNode == NULL) return 0;

    if (Input < 2) {
        ReadNetAlloc = 0; 
        ReadNetStats = 0; 
        ReadNetLearn = 0;
        
        if (Input) {
            *baseNode |= (1 << 4);
        } else {
            *baseNode &= ~(1 << 4);
        }
        return 1;
    } else {
        return 0;
    }
}

unsigned char NetTarget(unsigned char Input) {
    if (Input < 2) {
        Target = Input;
        return 1;
    } else {
        return 0;
    }
}

unsigned char NetAlloc(unsigned int Alloc) { 
    static unsigned int allocation_index = 0;

    if (Alloc > 0 && Alloc <= MaxNode) { 
        if (allocation_index + Alloc > MaxNode) { 
            return 0; 
        } 
        
        baseNode = &memory_pool[allocation_index]; 
        maxNode = Alloc;
        allocation_index += Alloc; 
        
        ReadNetAlloc = 1; 
        return 1; 
    } else { 
        return 0; 
    } 
}

/* Deterministic node initializer to replace randomization */
unsigned char NetInitData(void) {
    if (baseNode == NULL || maxNode == 0) return 0;

    unsigned short *nodePtr = baseNode;

    for (unsigned int node = 0; node < maxNode; node++) {
        // Sets active routing flags & low initial thresholds (0)
        // Ensures valid forward signal propagation across the network
        *nodePtr = 0x3700;
        nodePtr++;
    }

    return 1;
}

unsigned char NetTrigg(void) {
    if (baseNode == NULL || maxNode == 0) return 0;

    unsigned short *nodePtrA = baseNode;

    for (unsigned int node = 0; node < maxNode; node++) {
        unsigned short nodeAdr = *nodePtrA;

        if ((nodeAdr & 0x3000) != 0) { 
            unsigned char countA = 0, countB = 0, countC = 0, countD = 0; 

            if ((nodeAdr & 0x1000) != 0) { 
                countA += 1; 
                countB += 1; 
            } 
            if ((nodeAdr & 0x2000) != 0) { 
                countB += 1; 
                countC += 1; 
                countD += 1; 
            } 

            if ((countA > 0) && (((nodeAdr & 0x0003) >> 0) <= countA) && ((nodeAdr & 0x0100) != 0)) { 
                countB += 1; 
            } 

            if ((countB > 0) && (((nodeAdr & 0x000C) >> 2) <= countB) && ((nodeAdr & 0x0200) != 0)) { 
                countC += 1; 
                if (node < (maxNode - 1)) {
                    *(nodePtrA + 1) |= 0x1000; 
                }
            } else if (node < (maxNode - 1)) {
                *(nodePtrA + 1) &= ~0x1000;
            }

            if (node > 0) {
                unsigned short bit13Moved = (*(nodePtrA - 1) << 1) & 0x4000;
                *(nodePtrA - 1) &= ~0x4000;
                *(nodePtrA - 1) |= bit13Moved;
            }

            if ((countC > 0) && (((nodeAdr & 0x0030) >> 4) <= countC) && ((nodeAdr & 0x0400) != 0)) { 
                if (node > 0) {
                    countD += 1; 
                    if ((*(nodePtrA - 1) & 0x8000) != 0) {
                        *(nodePtrA - 1) |= 0x2000; 
                    }
                }
            } else if (node > 0) {
                *(nodePtrA - 1) &= ~0x2000;
            }

            if ((countD > 0) && (((nodeAdr & 0x00C0) >> 6) <= countD) && ((nodeAdr & 0x0800) != 0)) { 
                nodeAdr |= 0x8000; 
            } else {
                nodeAdr &= ~0x8000;
            } 
        } else {
            nodeAdr &= ~0x8000;
        }
        
        *nodePtrA = nodeAdr;
        nodePtrA++; 
    }

    unsigned char NetworkResult = 0;
    unsigned short lastNodeValue = *(baseNode + (maxNode - 1));
    
    if (((lastNodeValue & 0x8000) != 0) == Target) {
        NetworkResult = 1;
        ReadNetStats = 1;
    } else {
        NetworkResult = 0;
        ReadNetStats = 0;
    }

    unsigned short *nodePtrB = baseNode;

    for (unsigned int node = 0; node < maxNode; node++) {
        unsigned short nodeAdr = *nodePtrB;

        if ((nodeAdr & 0x1000) != 0) {
            unsigned short ThresA = (nodeAdr & 0x0003) >> 0;
            if (NetworkResult > 0) {
                nodeAdr |= 0x0100;
                if (ThresA > 0) {
                    ThresA--;
                }
            } else {
                nodeAdr &= ~0x0100;
                if (ThresA < 3) {
                    ThresA++;
                }
            }
            nodeAdr &= ~0x0003;
            nodeAdr |= (ThresA << 0);

            unsigned short ThresB = (nodeAdr & 0x000C) >> 2;
            if (NetworkResult > 0) {
                nodeAdr |= 0x0200;
                if (ThresB > 0) {
                    ThresB--;
                }
            } else {
                nodeAdr &= ~0x0200;
                if (ThresB < 3) {
                    ThresB++;
                }
            }
            nodeAdr &= ~0x000C;
            nodeAdr |= (ThresB << 2);
            ReadNetLearn = 1;
        }

        if ((nodeAdr & 0x2000) != 0) {
            unsigned short ThresC = (nodeAdr & 0x0030) >> 4;
            if (NetworkResult > 0) {
                nodeAdr |= 0x0400;
                if (ThresC > 0) {
                    ThresC--;
                }
            } else {
                nodeAdr &= ~0x0400;
                if (ThresC < 3) {
                    ThresC++;
                }
            }
            nodeAdr &= ~0x0030;
            nodeAdr |= (ThresC << 4);

            unsigned short ThresD = (nodeAdr & 0x00C0) >> 6;
            if (NetworkResult > 0) {
                nodeAdr |= 0x0800;
                if (ThresD > 0) {
                    ThresD--;
                }
            } else {
                nodeAdr &= ~0x0800;
                if (ThresD < 3) {
                    ThresD++;
                }
            }
            nodeAdr &= ~0x00C0;
            nodeAdr |= (ThresD << 6);
            ReadNetLearn = 1;
        }

        *nodePtrB = nodeAdr;
        nodePtrB++;
    }

    return ReadNetStats;
}

#include <stdio.h>
#include "NeuralNetwork.h"

static unsigned int current_alloc_count = 0;
static unsigned int lcg_seed = 987654321;
static unsigned char display_memory = 1; // T/F toggle (1 = True, 0 = False)

/* Helper PRNG for external memory randomization in example test bench */
static unsigned short generate_random_16bit(void) {
    lcg_seed = lcg_seed * 1103515245 + 12345;
    return (unsigned short)((lcg_seed / 65536) % 65536);
}

void print_memory_bytes(unsigned int node_count) {
    unsigned int total_bytes = node_count * sizeof(unsigned short);
    unsigned short *memory = NetGetMemory();

    if (memory == NULL || node_count == 0) {
        printf("\n[MEM] No memory allocated.\n");
        return;
    }

    if (total_bytes >= 40) {
        printf("\n[MEM] Size (%u bytes) >= 40 bytes. Skipping byte view.\n", total_bytes);
        return;
    }

    printf("\n--- BYTE MEMORY VIEW (%u bytes) ---\n", total_bytes);

    unsigned char *byte_ptr = (unsigned char *)memory;

    for (unsigned int i = 0; i < total_bytes; i += 2) {
        for (int bit = 7; bit >= 0; bit--) {
            printf("%d", (byte_ptr[i] >> bit) & 1);
        }

        printf(" ");

        if (i + 1 < total_bytes) {
            for (int bit = 7; bit >= 0; bit--) {
                printf("%d", (byte_ptr[i + 1] >> bit) & 1);
            }
        } else {
            printf("00000000");
        }

        printf("\n\n");
    }
    printf("----------------------------------\n");
}

void auto_display_memory_if_enabled(void) {
    if (display_memory) {
        print_memory_bytes(current_alloc_count);
    }
}

int main(void) {
    int choice = 0;
    unsigned int val = 0;
    unsigned short hex_val = 0;

    while (1) {
        printf("\n====================================\n");
        printf("   INTERACTIVE NETWORK TEST BENCH   \n");
        printf("====================================\n");
        printf(" 1. Allocate Nodes (NetAlloc)\n");
        printf(" 2. Load Preset: 20-Node Microcircuit (40 Bytes)\n");
        printf(" 3. Set Input Bit (NetInput)\n");
        printf(" 4. Set Target Bit (NetTarget)\n");
        printf(" 5. Initialize Node Data (NetInitData)\n");
        printf(" 6. Randomize Memory Bits (via NetWriteMemory)\n");
        printf(" 7. Step Trigger Once (NetTrigg)\n");
        printf(" 8. Continuous Run Loop\n");
        printf(" 9. Direct Memory Write (NetWriteMemory)\n");
        printf("10. Read Raw Memory Bytes (NetGetMemory)\n");
        printf("11. Toggle Live Memory View [Status: %s]\n", display_memory ? "TRUE" : "FALSE");
        printf("12. Show System Telemetry Status\n");
        printf(" 0. Exit\n");
        printf("====================================\n");
        printf("Select Option: ");

        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n'); 
            continue;
        }

        switch (choice) {
            case 1:
                printf("Enter number of nodes to allocate (1-%d): ", MaxNode);
                if (scanf("%u", &val) == 1) {
                    if (NetAlloc(val)) {
                        current_alloc_count = val;
                        printf("-> Success: Allocated %u nodes.\n", val);
                        auto_display_memory_if_enabled();
                    } else {
                        printf("-> Error: Allocation failed.\n");
                    }
                }
                break;

            case 2:
                if (NetAlloc(20)) {
                    current_alloc_count = 20;
                    NetInitData();
                    printf("-> Success: Loaded 20-Node Microcircuit Preset (40 Bytes).\n");
                    auto_display_memory_if_enabled();
                } else {
                    printf("-> Error: Failed to allocate preset.\n");
                }
                break;

            case 3:
                printf("Enter Input value (0 or 1): ");
                if (scanf("%u", &val) == 1) {
                    if (NetInput((unsigned char)val)) {
                        printf("-> Success: Set input to %u.\n", val);
                        auto_display_memory_if_enabled();
                    } else {
                        printf("-> Error: Invalid input value.\n");
                    }
                }
                break;

            case 4:
                printf("Enter Target value (0 or 1): ");
                if (scanf("%u", &val) == 1) {
                    if (NetTarget((unsigned char)val)) {
                        printf("-> Success: Set target to %u.\n", val);
                        auto_display_memory_if_enabled();
                    } else {
                        printf("-> Error: Invalid target value.\n");
                    }
                }
                break;

            case 5:
                if (NetInitData()) {
                    printf("-> Success: Data initialized to baseline.\n");
                    auto_display_memory_if_enabled();
                } else {
                    printf("-> Error: Failed to initialize data.\n");
                }
                break;

            case 6:
                if (current_alloc_count == 0) {
                    printf("-> Error: Allocate nodes first.\n");
                } else {
                    for (unsigned int i = 0; i < current_alloc_count; i++) {
                        NetWriteMemory(i, generate_random_16bit());
                    }
                    printf("-> Success: Randomized memory bits for %u nodes.\n", current_alloc_count);
                    auto_display_memory_if_enabled();
                }
                break;

            case 7: {
                unsigned char status = NetTrigg();
                printf("-> Trigger Executed. Target Met Status: %d\n", status);
                auto_display_memory_if_enabled();
                break;
            }

            case 8: {
                unsigned int limit = 0;
                printf("Enter max cycle limit (e.g., 100): ");
                if (scanf("%u", &limit) == 1) {
                    unsigned int cycle = 0;
                    unsigned char status = 0;

                    printf("Running continuous loop...\n");
                    for (cycle = 1; cycle <= limit; cycle++) {
                        status = NetTrigg();
                        if (display_memory) {
                            printf("\n[Cycle %u]", cycle);
                            print_memory_bytes(current_alloc_count);
                        }
                        if (status == 1) {
                            printf("-> TARGET MET at cycle %u!\n", cycle);
                            break;
                        }
                    }

                    if (status == 0) {
                        printf("-> Limit reached (%u cycles). Target not met.\n", limit);
                    }
                }
                break;
            }

            case 9: {
                unsigned int index = 0;
                printf("Enter Node Index (0 to %u): ", current_alloc_count > 0 ? current_alloc_count - 1 : 0);
                if (scanf("%u", &index) == 1) {
                    printf("Enter 16-bit Hex Value (e.g., 3705 for 0x3705): ");
                    if (scanf("%hx", &hex_val) == 1) {
                        if (NetWriteMemory(index, hex_val)) {
                            printf("-> Success: Wrote 0x%04X to Node %u.\n", hex_val, index);
                            auto_display_memory_if_enabled();
                        } else {
                            printf("-> Error: Write failed.\n");
                        }
                    }
                }
                break;
            }

            case 10:
                print_memory_bytes(current_alloc_count);
                break;

            case 11:
                display_memory = !display_memory;
                printf("-> Live Memory Display is now %s.\n", display_memory ? "ENABLED (TRUE)" : "DISABLED (FALSE)");
                break;

            case 12:
                printf("\n--- SYSTEM TELEMETRY ---\n");
                printf("Alloc State (NetGetAlloc) : %d\n", NetGetAlloc());
                printf("Stats State (NetGetStats) : %d\n", NetGetStats());
                printf("Learn State (NetGetLearn) : %d\n", NetGetLearn());
                printf("Allocated Node Count     : %u\n", current_alloc_count);
                printf("------------------------\n");
                break;

            case 0:
                printf("Exiting test bench.\n");
                return 0;

            default:
                printf("-> Invalid option.\n");
                break;
        }
    }

    return 0;
}

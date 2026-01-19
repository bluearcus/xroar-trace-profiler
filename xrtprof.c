/*
 * XRoar Trace Profiler - Filter
 * 
 * Processes XRoar instruction traces into JSON profiles for cycle analysis
 * 
 * Copyright (c) 2025 Mike Miller
 * 
 * This work is licensed under a Creative Commons 
 * Attribution-NonCommercial-ShareAlike 4.0 International License.
 * 
 * See https://creativecommons.org/licenses/by-nc-sa/4.0/
 */
 
 /* V0.1 19 Jan 2026 - Initial release /*

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>

typedef struct {
    char line[128];
    unsigned int instruction_addresscount;
    unsigned long instruction_cycles;
    double address_ticks;
    unsigned long sum_ticks;
} ProfileEntry;

static ProfileEntry AddressSpaceProfile[65536];

int main(int argc, char *argv[]) {
    FILE *infile = stdin;
    FILE *outfile = stdout;

    if (argc >= 2) {
        infile = fopen(argv[1], "r");
        if (!infile) return 1;
    }
    if (argc >= 3) {
        outfile = fopen(argv[2], "w");
        if (!outfile) return 1;
    }

    char line[1024];
    int first_line = 1;
    memset(AddressSpaceProfile, 0, sizeof(AddressSpaceProfile));

    while (fgets(line, sizeof(line), infile)) {
        // Restore your original robust check
        if (line[0] != '\0' && line[4] == '|') {
            unsigned int pc;
            if (sscanf(line, "%4x", &pc) != 1) continue;

            if (strstr(line, "[RESET]")) continue;

            // Extract dt= value
            int64_t ticks = -1;
            char *dt_ptr = strstr(line, "dt=");
            if (dt_ptr) {
                ticks = strtoll(dt_ptr + 3, NULL, 10);
            }

            // Requirement: Disregard ONLY the initial cycle count if excessive
            if (first_line && ticks > 512) ticks = 16;
            first_line = 0;

            unsigned long cycles = (ticks > 0) ? (unsigned long)(ticks / 16) : 0;
            if (cycles == 0 && ticks > 0) cycles = 1;

            AddressSpaceProfile[pc].sum_ticks += cycles;

            if (AddressSpaceProfile[pc].line[0] == '\0') {
                // Record the count of addresses (from your old code)
                char *hex_end = strchr(line + 6, ' ');
                if (hex_end) {
                    AddressSpaceProfile[pc].instruction_addresscount = (hex_end - (line + 6)) / 2;
                } else {
                    AddressSpaceProfile[pc].instruction_addresscount = 1;
                }
                
                AddressSpaceProfile[pc].instruction_cycles = cycles;

                // Capture disassembly from col 18 until register state (cc=) or dt=
                char *src = line + 18;
                char *dst = AddressSpaceProfile[pc].line;
                int i = 0;
                while (src[i] != '\0' && src[i] != '\n' && src[i] != '\r' && i < 64) {
                    // Stop when we hit register state (starts with "  cc=") or dt= for interrupts
                    if ((i >= 2 && src[i-2] == ' ' && src[i-1] == ' ' && src[i] == 'c' && src[i+1] == 'c' && src[i+2] == '=') ||
                        (i >= 2 && src[i-2] == ' ' && src[i-1] == ' ' && src[i] == 'd' && src[i+1] == 't' && src[i+2] == '=')) {
                        i -= 2; // Back up to before the two spaces
                        break;
                    }
                    dst[i] = src[i];
                    i++;
                }
                dst[i] = '\0';

                // Trim trailing spaces
                while (i > 0 && dst[i-1] == ' ') dst[--i] = '\0';

                // Requirement: Pad to column 24 and add cycles
                if (i < 24) {
                    for (int j = i; j < 24; j++) dst[j] = ' ';
                    i = 24;
                }
                sprintf(dst + i, " ; (%lu cycles)", cycles);
            }
        }
    }

    // Normalise cycle counts across memory locations (your original ac logic)
    for (int pc = 0; pc < 65536; pc++) {
        if (AddressSpaceProfile[pc].sum_ticks > 0) {
            int count = AddressSpaceProfile[pc].instruction_addresscount;
            if (count == 0) count = 1;
            double spread = (double)AddressSpaceProfile[pc].sum_ticks / count;
            for (int ins_bytes = 0; ins_bytes < count; ins_bytes++) {
                if (pc + ins_bytes < 65536) {
                    AddressSpaceProfile[pc + ins_bytes].address_ticks += spread;
                }
            }
        }
    }

    // Requirement: JSON output
    fprintf(outfile, "[\n");
    int first_out = 1;
    for (int i = 0; i < 65536; i++) {
        if (AddressSpaceProfile[i].sum_ticks > 0 || AddressSpaceProfile[i].address_ticks > 0) {
            if (!first_out) fprintf(outfile, ",\n");
            fprintf(outfile, "  {\"a\":%d,\"ic\":%lu,\"ac\":%.2f,\"ins\":\"%s\"}", 
                    i, AddressSpaceProfile[i].sum_ticks, AddressSpaceProfile[i].address_ticks, 
                    AddressSpaceProfile[i].line);
            first_out = 0;
        }
    }
    fprintf(outfile, "\n]\n");

    if (infile != stdin) fclose(infile);
    if (outfile != stdout) fclose(outfile);
    return 0;
}
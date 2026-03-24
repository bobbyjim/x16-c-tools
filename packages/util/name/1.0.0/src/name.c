#include <stdlib.h>
#include <stdio.h>
#include "name.h"

char *adjective[] = {
    "arthritic",
    "banal",
    "campy",
    "deathly",
    "english",
    "figgy",
    "grungy",
    "hapless",
    "insolent",
    "junk",
    "klepto",
	"lumpy",
    "maniacal",
    "nearsighted",
    "outre",
    "painful",
    "quacking",
	"rancid",
    "slithy",
    "turfal",
    "unisex",
    "verbal",
    "woeful",
    "xanthic",
    "yonder",
    "zippy"
};

char *noun[] = {
    "ankle",
    "bat",
    "cryptid",
    "dunghill",
    "ent",
    "fink",
    "gruel",
    "harpy",
    "ink",
    "job",
    "kerbal",
    "lunk",
    "mud",
    "nerf",
    "oaf",
    "punk",
    "qagga",
    "rennet",
    "sinkhole",
    "tv",
    "unguent",
    "vat",
    "whip",
    "xerxes",
    "yak",
    "zebra"
};

#define NAME_ADJECTIVE_COUNT 26
#define NAME_NOUN_COUNT 26

char* generateName(char* out)
{
    sprintf(out, "%s_%s", 
       adjective[ rand() % NAME_ADJECTIVE_COUNT ],
       noun[ rand() % NAME_NOUN_COUNT ]
    );

    return out;
}
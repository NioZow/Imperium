#ifndef CRYPTO_HASH_H
#define CRYPTO_HASH_H

#include <common/Imperium.h>

#define SEED 5
#define RANDOM_KEY 5381

/*
consteval ULONG RandomKey() {
    return ( '0' * -40271 + __TIME__[ 7 ] * 1 + __TIME__[ 6 ] * 10 + __TIME__[ 4 ] * 60 +
             __TIME__[ 3 ] * 600 + __TIME__[ 1 ] * 3600 + __TIME__[ 0 ] * 36000 );
}*/

#endif //CRYPTO_HASH_H

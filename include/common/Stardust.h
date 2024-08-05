#ifndef COMMON_STARDUST_H
#define COMMON_STARDUST_H

//
// instance related macros
//
#define InstanceOffset()  ( U_PTR( & __Instance_offset ) )
#define InstancePtr()     ( ( PINSTANCE ) C_DEF( C_PTR( U_PTR( StRipStart() ) + InstanceOffset() ) ) )
#define STARDUST_INSTANCE PINSTANCE __LocalInstance = InstancePtr();
#define Self              ( ( PINSTANCE ) ( __LocalInstance ) )

//
// utils macros
//
#define D_API( x )  __typeof__( x ) * x
#define D_SEC( x )  __attribute__( ( section( ".text$" #x "" ) ) )
#define FUNC        D_SEC( B )
#define ST_GLOBAL   __attribute__( ( section( ".global" ) ) )
#define ST_READONLY __attribute__( ( section( ".rdata" ) ) )

#endif //COMMON_STARDUST_H

#ifndef _TYPE_H_
#define _TYPE_H_



//#define offsetof(TYPE, MEMBER) ((unsigned long) &((TYPE *)0)->MEMBER)


/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:    the pointer to the member.
 * @type:   the type of the container struct this is embedded in.
 * @member: the name of the member within the struct.
 *
 */
#define container_of(ptr, type, member) ({          \
    const typeof(((type *)0)->member) * __mptr = (ptr); \
    (type *)((char *)__mptr - offsetof(type, member)); })


#define __always_inline inline __attribute__((__always_inline__))

#ifndef NULL
#define NULL ((void *)0)
#endif


typedef unsigned long sector_t;

typedef unsigned char   __u8;
typedef unsigned short  __u16;
typedef unsigned int    __u32;
typedef unsigned long   __u64;


#endif

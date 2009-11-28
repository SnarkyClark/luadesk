#ifndef HTON_H_INCLUDED
#define HTON_H_INCLUDED

#if defined(BIG_ENDIAN)
  #define htons(A) (A)
  #define htonl(A) (A)
  #define ntohs(A) (A)
  #define ntohl(A) (A)
#else
  #define htons(A) ((((uint16_t)(A) & 0xff00) >> 8) | \
                    (((uint16_t)(A) & 0x00ff) << 8))
  #define htonl(A) ((((uint32_t)(A) & 0xff000000) >> 24) | \
                    (((uint32_t)(A) & 0x00ff0000) >> 8)  | \
                    (((uint32_t)(A) & 0x0000ff00) << 8)  | \
                    (((uint32_t)(A) & 0x000000ff) << 24))
  #define ntohs  htons
  #define ntohl  htonl
#endif

#endif // HTON_H_INCLUDED

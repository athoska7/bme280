/* 
 * File:   base64.h
 * Author: Viktor
 *
 * Created on 23 August 2016, 21:31
 */

#ifndef BASE64_H
#define	BASE64_H

#ifdef	__cplusplus
extern "C" {
#endif

char* base64_encode(unsigned char *data,
                    int input_length,
                    int *output_length);


#ifdef	__cplusplus
}
#endif

#endif	/* BASE64_H */


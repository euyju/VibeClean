#ifndef EDGE_AI_WRAPPER_H_
#define EDGE_AI_WRAPPER_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Classification result structure (대문자 시작: BE와 동일)
typedef struct {
    float Hard;
    float Carpet;
    float Dusty;
} surface_classification_t;

// Function prototypes
int edge_ai_init(void);
int edge_ai_classify(float *input_buffer, int buffer_size, surface_classification_t *result);

#ifdef __cplusplus
}
#endif

#endif /* EDGE_AI_WRAPPER_H_ */

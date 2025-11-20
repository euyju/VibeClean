#include "edge_ai_wrapper.h"
#include "../../Edge-AI/edge-impulse-sdk/classifier/ei_run_classifier.h"
#include "../../Edge-AI/model-parameters/model_metadata.h"

/**
 * @brief  Edge Impulse 분류기 초기화
 * @retval 0: 성공, -1: 실패
 */
int edge_ai_init(void)
{
    // Edge Impulse SDK는 별도 초기화 불필요
    // 모델 메타데이터 검증
    if (EI_CLASSIFIER_RAW_SAMPLE_COUNT != 200 ||
        EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME != 3) {
        return -1;  // 모델 설정 불일치
    }

    return 0;
}

/**
 * @brief  Edge Impulse 분류기 실행
 * @param  input_buffer: 3축 가속도 데이터 (200샘플 x 3축 = 600 floats)
 * @param  buffer_size: 버퍼 크기 (600)
 * @param  result: 분류 결과 저장 구조체
 * @retval 0: 성공, -1: 실패
 */
int edge_ai_classify(float *input_buffer, int buffer_size, surface_classification_t *result)
{
    ei_impulse_result_t ei_result = {0};
    signal_t signal;

    // 버퍼 크기 검증
    if (buffer_size != EI_CLASSIFIER_RAW_SAMPLE_COUNT * EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME) {
        return -1;
    }

    // Signal 구조체 설정 (입력 데이터)
    signal.total_length = buffer_size;
    signal.get_data = [](size_t offset, size_t length, float *out_ptr, void *data) {
        float *buffer = (float *)data;
        for (size_t i = 0; i < length; i++) {
            out_ptr[i] = buffer[offset + i];
        }
        return 0;
    };

    // Edge Impulse 분류기 실행
    EI_IMPULSE_ERROR res = run_classifier(&signal, &ei_result, false);
    if (res != EI_IMPULSE_OK) {
        return -1;
    }

    // 결과 복사 (Hard, Carpet, Dusty 순서)
    if (ei_result.classification[0].label) {
        result->Hard = 0.0f;
        result->Carpet = 0.0f;
        result->Dusty = 0.0f;

        // 라벨 매칭
        for (uint8_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
            const char *label = ei_result.classification[i].label;
            float value = ei_result.classification[i].value;

            if (strcmp(label, "Hard") == 0) {
                result->Hard = value;
            } else if (strcmp(label, "Carpet") == 0) {
                result->Carpet = value;
            } else if (strcmp(label, "Dusty") == 0) {
                result->Dusty = value;
            }
        }
    }

    return 0;
}

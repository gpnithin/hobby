#include "capi_v2.h"

/* Example Qualcomm CAPI module for simple volume control */

typedef struct {
    capi_v2_t vtbl; /* callback table provided by the framework */
    float gain;     /* gain factor applied in process */
} capi_volume_t;

/* Forward declarations of CAPI callbacks */
static capi_v2_err_t capi_volume_init(capi_v2_t* _pif,
                                      capi_v2_proplist_t* init_set_properties);
static capi_v2_err_t capi_volume_process(capi_v2_t* _pif,
                                         capi_v2_stream_data_t* input[],
                                         capi_v2_stream_data_t* output[]);
static capi_v2_err_t capi_volume_end(capi_v2_t* _pif);
static capi_v2_err_t capi_volume_set_param(capi_v2_t* _pif,
                                           uint32_t param_id,
                                           const int8_t* payload,
                                           uint32_t payload_size);
static capi_v2_err_t capi_volume_get_param(capi_v2_t* _pif,
                                           uint32_t param_id,
                                           int8_t* payload,
                                           uint32_t payload_size);
static capi_v2_err_t capi_volume_set_properties(capi_v2_t* _pif,
                                                capi_v2_proplist_t* props);
static capi_v2_err_t capi_volume_get_properties(capi_v2_t* _pif,
                                                capi_v2_proplist_t* props);
static capi_v2_err_t capi_volume_get_static_properties(
    capi_v2_proplist_t* init_set_properties,
    capi_v2_proplist_t* static_properties);

/* Implementation of the callbacks */

static capi_v2_err_t capi_volume_init(capi_v2_t* _pif, capi_v2_proplist_t* init_set_properties)
{
    capi_volume_t* me = (capi_volume_t*)_pif;
    me->gain = 1.0f; /* default unity gain */
    /* TODO: handle init_set_properties if required */
    return CAPI_V2_EOK;
}

static capi_v2_err_t capi_volume_process(capi_v2_t* _pif,
                                         capi_v2_stream_data_t* input[],
                                         capi_v2_stream_data_t* output[])
{
    capi_volume_t* me = (capi_volume_t*)_pif;
    int16_t* in  = (int16_t*)input[0]->buf_ptr;
    int16_t* out = (int16_t*)output[0]->buf_ptr;

    for (uint32_t i = 0; i < input[0]->bufs_num; ++i) {
        out[i] = (int16_t)(me->gain * in[i]);
    }
    return CAPI_V2_EOK;
}

static capi_v2_err_t capi_volume_end(capi_v2_t* _pif)
{
    /* Clean up any resources if necessary */
    return CAPI_V2_EOK;
}

static capi_v2_err_t capi_volume_set_param(capi_v2_t* _pif,
                                           uint32_t param_id,
                                           const int8_t* payload,
                                           uint32_t payload_size)
{
    capi_volume_t* me = (capi_volume_t*)_pif;
    if (param_id == SOME_GAIN_PARAM_ID && payload_size == sizeof(float)) {
        me->gain = *(float*)payload;
    }
    return CAPI_V2_EOK;
}

static capi_v2_err_t capi_volume_get_param(capi_v2_t* _pif,
                                           uint32_t param_id,
                                           int8_t* payload,
                                           uint32_t payload_size)
{
    capi_volume_t* me = (capi_volume_t*)_pif;
    if (param_id == SOME_GAIN_PARAM_ID && payload_size == sizeof(float)) {
        *(float*)payload = me->gain;
    }
    return CAPI_V2_EOK;
}

static capi_v2_err_t capi_volume_set_properties(capi_v2_t* _pif,
                                                capi_v2_proplist_t* props)
{
    /* Handle property updates (e.g., port info, media format, etc.) */
    return CAPI_V2_EOK;
}

static capi_v2_err_t capi_volume_get_properties(capi_v2_t* _pif,
                                                capi_v2_proplist_t* props)
{
    /* Return properties such as algorithm version, port details, etc. */
    return CAPI_V2_EOK;
}

static capi_v2_err_t capi_volume_get_static_properties(
    capi_v2_proplist_t* init_set_properties,
    capi_v2_proplist_t* static_properties)
{
    (void)init_set_properties; /* Unused in this simple example */

    if (!static_properties) {
        return CAPI_V2_EBADPARAM;
    }

    for (uint32_t i = 0; i < static_properties->props_num; ++i) {
        capi_v2_prop_t* prop = &static_properties->prop_ptr[i];

        switch (prop->id) {
        case CAPI_V2_PORT_NUM_INFO: {
            capi_v2_port_num_info_t* port_info =
                (capi_v2_port_num_info_t*)prop->payload.data_ptr;
            port_info->num_input_ports = 1;
            port_info->num_output_ports = 1;
            prop->payload.actual_data_len = sizeof(capi_v2_port_num_info_t);
            break;
        }
        case CAPI_V2_STACK_SIZE: {
            capi_v2_stack_size_t* stack =
                (capi_v2_stack_size_t*)prop->payload.data_ptr;
            stack->size_in_bytes = 0;
            prop->payload.actual_data_len = sizeof(capi_v2_stack_size_t);
            break;
        }
        default:
            /* For properties we don't handle, leave them untouched */
            break;
        }
    }

    return CAPI_V2_EOK;
}


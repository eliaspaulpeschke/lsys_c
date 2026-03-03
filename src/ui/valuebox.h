#ifndef VALUEBOX_H
#define VALUEBOX_H
#include "button.h"
#include "common.h"
#include "inputbox.h"
#include "module.h"
#include <stdbool.h>

typedef enum {
    VALUEBOX_TYPE_none
  , VALUEBOX_TYPE_int
  , VALUEBOX_TYPE_float
  , VALUEBOX_TYPE_double
} VALUEBOX_TYPE;

typedef struct {
    int value;
    int max;
    int min;
    int step;
    Module int_in;
} ValueBoxIntValue;

typedef struct {
    float value;
    float max;
    float min;
    float step;
    Module float_in;
} ValueBoxFloatValue;

typedef struct {
    double value;
    double max;
    double min;
    double step;
    Module double_in;
} ValueBoxDoubleValue;

typedef struct {
    bool error;
    bool active;
    bool wrap;
    bool changed;
    VALUEBOX_TYPE type;
    unsigned int clay_id_num;
    union {
        ValueBoxIntValue int_value;
        ValueBoxFloatValue float_value;
        ValueBoxDoubleValue double_value;
    };
    ButtonElem btn_plus;
    ButtonElem btn_minus;
    Inputbox inputbox;
} ValueBox;

#define ERR_VALUEBOX (ValueBox) {true, VALUEBOX_TYPE_none, 0}
void free_valuebox(ValueBox vb);
ValueBox mk_valuebox(VALUEBOX_TYPE type, bool wrap);
UpdateReturnValue update_valuebox(ValueBox * vb);
void layout_valuebox(ValueBox vb, Font * fonts, char * label);

#endif

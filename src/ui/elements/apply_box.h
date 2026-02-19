#ifndef APPLY_BOX_H
#define APPLY_BOX_H
#include "../valuebox.h"
#include "../button.h"
#include "../module.h"
#include "../move_container.h"
#include <stdbool.h>

typedef struct {
    bool error;
    unsigned int times;
    ValueBox valuebox;
    ButtonElem btn_apply;
    Module lstring_in;
    Module rules_in;
    Module lstring_out;
    Move_container container;
} ApplyBox;

#define ERR_APPLYBOX (ApplyBox) {true, 0, ERR_VALUEBOX, ERR_BUTTON, ERR_MODULE, ERR_MODULE, ERR_MODULE, 0}

ApplyBox mk_applybox();
void free_applybox(ApplyBox ab);

bool update_applybox(ApplyBox * ab);
void layout_applybox(ApplyBox ab, Font * fonts);

#endif

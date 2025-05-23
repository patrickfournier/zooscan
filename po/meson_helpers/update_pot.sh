#!/bin/bash

src_dir=$1
output=$2

shift 2

xgettext "$@" \
        --from-code=UTF-8 \
        --add-comments \
        --keyword=_ \
        --keyword=N_ \
        --keyword=C_:1c,2 \
        --keyword=NC_:1c,2 \
        --keyword=g_dcgettext:2 \
        --keyword=g_dngettext:2,3 \
        --keyword=g_dpgettext2:2c,3 \
        --flag=N_:1:pass-c-format \
        --flag=C_:2:pass-c-format \
        --flag=NC_:2:pass-c-format \
        --flag=g_dngettext:2:pass-c-format \
        --flag=g_strdup_printf:1:c-format \
        --flag=g_string_printf:2:c-format \
        --flag=g_string_append_printf:2:c-format \
        --flag=g_error_new:3:c-format \
        --flag=g_set_error:4:c-format \
        --flag=g_markup_printf_escaped:1:c-format \
        --flag=g_log:3:c-format \
        --flag=g_print:1:c-format \
        --flag=g_printerr:1:c-format \
        --flag=g_printf:1:c-format \
        --flag=g_fprintf:2:c-format \
        --flag=g_sprintf:2:c-format \
        --flag=g_snprintf:3:c-format \
        --output="${output}"

output_file=$(basename "$output")
cp "${output}" "${src_dir}/${output_file}"

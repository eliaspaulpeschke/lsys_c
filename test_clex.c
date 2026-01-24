#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#define STB_C_LEXER_IMPLEMENTATION
#include "lib/stb_c_lexer.h"


char * untoken(int token){
    switch (token) {
        case CLEX_eof:
            return "CLEX_eof";
        case CLEX_parse_error:
            return "CLEX_parse_error";
        case CLEX_intlit:
            return "CLEX_intlit";
        case CLEX_floatlit:
            return "CLEX_floatlit";
        case CLEX_id:
            return "CLEX_id";
        case CLEX_dqstring:
            return "CLEX_dqstring";
        case CLEX_sqstring:
            return "CLEX_sqstring";
        case CLEX_charlit:
            return "CLEX_charlit";
        case CLEX_eq:
            return "CLEX_eq";
        case CLEX_noteq:
            return "CLEX_noteq";
        case CLEX_lesseq:
            return "CLEX_lesseq";
        case CLEX_greatereq:
            return "CLEX_greatereq";
        case CLEX_andand:
            return "CLEX_andand";
        case CLEX_oror:
            return "CLEX_oror";
        case CLEX_shl:
            return "CLEX_shl";
        case CLEX_shr:
            return "CLEX_shr";
        case CLEX_plusplus:
            return "CLEX_plusplus";
        case CLEX_minusminus:
            return "CLEX_minusminus";
        case CLEX_pluseq:
            return "CLEX_pluseq";
        case CLEX_minuseq:
            return "CLEX_minuseq";
        case CLEX_muleq:
            return "CLEX_muleq";
        case CLEX_diveq:
            return "CLEX_diveq";
        case CLEX_modeq:
            return "CLEX_modeq";
        case CLEX_andeq:
            return "CLEX_andeq";
        case CLEX_oreq:
            return "CLEX_oreq";
        case CLEX_xoreq:
            return "CLEX_xoreq";
        case CLEX_arrow:
            return "CLEX_arrow";
        case CLEX_eqarrow:
            return "CLEX_eqarrow";
        case CLEX_shleq:
            return "CLEX_shleq";
        case CLEX_shreq:
            return "CLEX_shreq";
        case CLEX_first_unused_token:
            return "CLEX_first_unused_token";
        default: ;
            char * chr = malloc(32);
            sprintf(chr, "%c", token);
            return chr;
    }
}

int main() {
    FILE *the_file = fopen("test", "r");
    struct stat sb;
    stat("test", &sb);
    char *the_content = malloc(sb.st_size);
    fread(the_content, 1, sb.st_size, the_file);
    char *chrstore = malloc(256);
    stb_lexer the_lexer;
    stb_c_lexer_init(&the_lexer, the_content, the_content + (sb.st_size -1), chrstore, 256);

    while(stb_c_lexer_get_token(&the_lexer) != 0){
        printf("%s ", untoken(the_lexer.token));
    } 
}


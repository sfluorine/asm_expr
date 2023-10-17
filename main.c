#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct expression_t {
    int64_t op;
    struct expression_t* lhs;
    struct expression_t* rhs;
} expression_t;

expression_t* expression_make(int64_t op) {
    expression_t* expr = malloc(sizeof(expression_t));
    expr->op = op;
    expr->lhs = NULL;
    expr->rhs = NULL;

    return expr;
}

typedef struct {
    char last_used_reg;
} compiler_t;

void compiler_init(compiler_t* compiler) {
    compiler->last_used_reg = 0;
}

bool is_value(expression_t* expr) {
    return expr->op != '+' && expr->op != '-' && expr->op != '*' && expr->op != '/';
}

char get_reg_from_index(int index) {
    switch (index) {
        case 1:
            return 'a';
        case 2:
            return 'd';
        case 3:
            return 'c';
        default:
            return 0;
    }
}

int get_index_from_reg(char reg) {
    switch (reg) {
        case 'a':
            return 1;
        case 'd':
            return 2;
        case 'c':
            return 3;
        default:
            return 0;
    }
}

const char* get_reg_from_char(char ch) {
    switch (ch) {
        case 'a':
            return "rax";
        case 'd':
            return "rdx";
        case 'c':
            return "rcx";
        default:
            return "";
    }
}

#define CODEGEN_OP(op) \
    const char* prev_reg = get_reg_from_char(get_reg_from_index(get_index_from_reg(compiler->last_used_reg) - 1));\
    const char* current_reg = get_reg_from_char(get_reg_from_index(get_index_from_reg(compiler->last_used_reg)));\
    printf("%s %s, %s\n", op, prev_reg, current_reg);\
    compiler->last_used_reg = get_reg_from_index(get_index_from_reg(compiler->last_used_reg) - 1);\

void codegen_expr(compiler_t* compiler, expression_t* expr) {
    if (!expr) {
        return;
    }

    switch (expr->op) {
        case '+': {
            if (is_value(expr->lhs) && !is_value(expr->rhs)) {
                codegen_expr(compiler, expr->rhs);
                codegen_expr(compiler, expr->lhs);

                const char* flu = get_reg_from_char(get_reg_from_index(get_index_from_reg(compiler->last_used_reg) - 1));
                const char* lu  = get_reg_from_char(get_reg_from_index(get_index_from_reg(compiler->last_used_reg)));

                printf("xchg %s, %s\n", get_reg_from_char(get_reg_from_index(get_index_from_reg(compiler->last_used_reg) - 1)), get_reg_from_char(get_reg_from_index(compiler->last_used_reg)));

            } else {
                codegen_expr(compiler, expr->lhs);
                codegen_expr(compiler, expr->rhs);
            }

            CODEGEN_OP("add");
            break;
        }
        case '-': {
            if (is_value(expr->lhs) && !is_value(expr->rhs)) {
                codegen_expr(compiler, expr->rhs);
                codegen_expr(compiler, expr->lhs);

                const char* flu = get_reg_from_char(get_reg_from_index(get_index_from_reg(compiler->last_used_reg) - 1));
                const char* lu  = get_reg_from_char(get_reg_from_index(get_index_from_reg(compiler->last_used_reg)));

                printf("xchg %s, %s\n", flu, lu);
            } else {
                codegen_expr(compiler, expr->lhs);
                codegen_expr(compiler, expr->rhs);
            }

            CODEGEN_OP("sub");
            break;
        }
        case '*': {
            if (is_value(expr->lhs) && !is_value(expr->rhs)) {
                codegen_expr(compiler, expr->rhs);
                codegen_expr(compiler, expr->lhs);

                const char* flu = get_reg_from_char(get_reg_from_index(get_index_from_reg(compiler->last_used_reg) - 1));
                const char* lu  = get_reg_from_char(get_reg_from_index(get_index_from_reg(compiler->last_used_reg)));

                printf("xchg %s, %s\n", flu, lu);
            } else {
                codegen_expr(compiler, expr->lhs);
                codegen_expr(compiler, expr->rhs);
            }

            CODEGEN_OP("mul");
            break;
        }
        case '/': {
            if (is_value(expr->lhs) && !is_value(expr->rhs)) {
                codegen_expr(compiler, expr->rhs);
                codegen_expr(compiler, expr->lhs);

                const char* flu = get_reg_from_char(get_reg_from_index(get_index_from_reg(compiler->last_used_reg) - 1));
                const char* lu  = get_reg_from_char(get_reg_from_index(get_index_from_reg(compiler->last_used_reg)));

                printf("xchg %s, %s\n", flu, lu);
            } else {
                codegen_expr(compiler, expr->lhs);
                codegen_expr(compiler, expr->rhs);
            }

            if (get_index_from_reg(compiler->last_used_reg) > 2) {
                printf("push rax\n");
                CODEGEN_OP("div");
                printf("mov rdx, rax\n");
                printf("pop rax\n");
            } else {
                CODEGEN_OP("div");
            }

            break;
        }
        default: {
            if (compiler->last_used_reg == 'a') {
                printf("mov rdx, %ld\n", expr->op);
                compiler->last_used_reg = 'd';
            } else if (compiler->last_used_reg == 'd') {
                printf("mov rcx, %ld\n", expr->op);
                compiler->last_used_reg = 'c';
            } else {
                printf("mov rax, %ld\n", expr->op);
                compiler->last_used_reg = 'a';
            }
        }
    }
}

int main() {
    compiler_t compiler;
    compiler_init(&compiler);

    expression_t* root = expression_make('-');

    expression_t* lhs = expression_make('+');

    expression_t* lhs_inner_lhs = expression_make('*');
    lhs_inner_lhs->lhs = expression_make(6);
    lhs_inner_lhs->rhs = expression_make(4);

    expression_t* rhs_inner_lhs = expression_make(10);

    lhs->lhs = lhs_inner_lhs;
    lhs->rhs = rhs_inner_lhs;

    expression_t* rhs = expression_make('/');
    rhs->lhs = expression_make(6);

    expression_t* rhs_inner_rhs = expression_make('+');
    rhs_inner_rhs->lhs = expression_make(1);
    rhs_inner_rhs->rhs = expression_make(2);

    rhs->rhs = rhs_inner_rhs;

    root->lhs = lhs;
    root->rhs = rhs;

    codegen_expr(&compiler, root);
}

/***************************************************************************************
* Copyright (c) 2014-2024 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ,
  TK_NUM,
  /* TODO: Add more token types */

};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},		// plus
  {"\\-", '-'},
  {"\\*", '*'},
  {"\\/", '/'},
  {"\\(", '('},
  {"\\)", ')'},
  {"[0-9]+", TK_NUM},  
  {"==", TK_EQ},        // equal
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool check_parentheses(int p, int q){
  if (tokens[p].type != '(' || tokens[q].type != ')'){
    return false;
  }
  int cnt = 0;
  for (int i = p; i <= q; i++){
    if (tokens[i].type == '('){
      cnt++;
    } else if (tokens[i].type == ')'){
      cnt--;
    }
    if (cnt == 0 && i < q){
      return false;
    }
  }
  return cnt == 0;
}

static int find_main_op(int p, int q){
  int op = -1;
  int in_parentheses = 0;
  int op_priority = 100;

  for (int i = p; i <= q; i++){
    if (tokens[i].type == '('){
      in_parentheses++;
      continue;
    } else if (tokens[i].type == ')'){
      in_parentheses--;
      continue;
    }
    if (in_parentheses > 0){
      continue;
    }
    int current_priority = 0;
    if (tokens[i].type == '+' || tokens[i].type == '-'){
      current_priority = 1;
    } else if (tokens[i].type == '*' || tokens[i].type == '/'){
      current_priority = 2;
    } else {
      continue;
    }
    if (current_priority <= op_priority){
      op_priority = current_priority;
      op = i;
    }
  }
  return op;
}

word_t eval(int p, int q){
  if (p > q) {
    assert(0);
  } else if (p == q){
    int num;
    sscanf(tokens[p].str, "%d", &num);
    return num;
  } else if (check_parentheses(p, q) == true){
    return eval(p + 1, q - 1);
  } else {
    int op = find_main_op(p, q);
    word_t val1 = eval(p, op - 1);
    word_t val2 = eval(op + 1, q);
    switch (tokens[op].type){
      case '+': return val1 + val2;
      case '-': return val1 - val2;
      case '*': return val1 * val2;
      case '/': return val1 / val2;
      default: assert(0);
    }
  }
}

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
	  case TK_NOTYPE:
	    break;
  	  case TK_NUM:
	    if (substr_len > 31){
	      printf("Error: Number is too long!\n");
	      assert(0);
	    }
	    strncpy(tokens[nr_token].str, substr_start, substr_len);
	    tokens[nr_token].str[substr_len] = '\0';
	    tokens[nr_token].type = TK_NUM;
	    nr_token++;
	    break;
          default: 
	    tokens[nr_token].type = rules[i].token_type;
	    nr_token++;
	    break;
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
  for(int i = 0; i < nr_token; i++){
    if (tokens[i].type == TK_NUM){
      printf("Token %d: type = TK_NUM, str = %s\n", i, tokens[i].str);
    } else {
      printf("Token %d: type = %c\n", i, tokens[i].type);
    }
  }
}


word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  *success = true;
  return eval(0, nr_token - 1);
}
